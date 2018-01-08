#include "MatchManager.h"
#include "GameDef.h"
#include "ModelHelper.h"
#include "GameUserManager.h"
#include "ConfGameSetting.h"
#include "MatchServer.h"
#include "Protocol.h"
#include "PvpProtocol.h"
#include "ServerProtocol.h"
#include "CommonHelper.h"
#include "KxCommManager.h"
#include "CommOssHelper.h"
#include "ConfHall.h"
#include "ConfArena.h"

#include <time.h>

#define MATCH_TIME		1			//匹配时间
#define MAX_LEVEL_LIMIT 10           //最大等级差

CMatchManager::CMatchManager()
: m_bFairInvert(false)
, m_bCPNIntert(false)
, m_nBattleId(0)
{
	m_uFairSeconds = 0;
	m_uCPNSeconds = 0;
	m_uTotalSeconds = 0;
}

CMatchManager::~CMatchManager()
{
}

CMatchManager *CMatchManager::m_pInstance = NULL;
CMatchManager *CMatchManager::getInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CMatchManager;
		m_pInstance->m_pTimerManager = CMatchServer::getInstance()->getTimerManager();
		m_pInstance->m_pTimerManager->addTimer(m_pInstance, MATCH_TIME, KXREPEAT_FOREVER);
	}
	return m_pInstance;
}

void CMatchManager::destroy()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool matchRobotCondition(int integral, int winTimes, int robotTimes)
{
    // 策划说, 匹配小于5场必定匹配机器人
    if (robotTimes < 5)
    {
        return true;
    }

    int dan = CCommonHelper::getUserIntegralDan(integral);
    // 连败为负数, 白银以下为连败2场
    if (dan <= DAN_SILVER && winTimes <= -2)
    {
        return true;
    }
    // 白银以上为连败4场
    if (dan > DAN_SILVER && winTimes <= -4)
    {
        return true;
    }

    return false;
}

bool CMatchManager::addToMatch(int matchType, int uid, KxServer::IKxComm *target)
{
    if (find(uid))
    {
        return false;
    }
    CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
    CPvpModel *pPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(uid, MODELTYPE_PVP));
    if (NULL == pPvpModel || NULL == pUserModel)
    {
        return false;
    }
    // 刷新pvp数据， 需要最新连胜场数， 最新积分
    pPvpModel->Refresh();

    int usrLevel = 0;
    if (!pUserModel->GetUserFieldVal(USR_FD_USERLV, usrLevel))
    {
        KXLOGERROR("get user %d field error when add to match list!", uid);
        return false;
    }

    if (matchType == MATCH_CPN)
    {
        int integral = 0;
        if (!pPvpModel->GetPvpField(CPN_FD_INTEGRAL, integral))
        {
            return false;
        }
        if (insert(matchType, uid, integral, usrLevel))
        {
            m_setMatcherUids.insert(uid);
            CKxCommManager::getInstance()->addKxComm(uid, target);
            return true;
        }
    }
    else
    {
        //获得integral
        int winTimes = 0;
        int integral = 0;
        int robotTimes = 0;
        if (!pPvpModel->GetPvpField(PVP_FD_INTEGRAL, integral)
            || !pPvpModel->GetPvpField(PVP_FD_CONTINUSWINTIMES, winTimes)
            || !pPvpModel->GetPvpField(PVPCOMM_FD_ROBOTTIMES, robotTimes))
        {
            return false;
        }

        if (matchRobotCondition(integral, winTimes, robotTimes))
        {
            NewMatch matcher;
            matcher.integral = integral;
            matcher.uid = uid;
            matcher.nStartTime = m_uTotalSeconds;
            m_listNewGuy.push_back(matcher);
            m_setMatcherUids.insert(uid);
            CKxCommManager::getInstance()->addKxComm(uid, target);
            return true;
        }

        if (insert(matchType, uid, integral, usrLevel))
        {
            m_setMatcherUids.insert(uid);
            CKxCommManager::getInstance()->addKxComm(uid, target);
            return true;
        }
    }

	return false;
}

bool CMatchManager::cancelMatch(int uid)
{
	if (!find(uid))
	{
		return false;
	}

	NewMatch Match;
	Match.uid = uid;
	std::list<NewMatch>::iterator ator = ::find(m_listCpn.begin(), m_listCpn.end(), Match);
	if (ator != m_listCpn.end())
	{
		int nInterval = m_uTotalSeconds - ator->nStartTime;
		CCommOssHelper::UserMatchOss(uid, enChamopinMatchType, nInterval, 0);
	}
	else
	{
		ator = ::find(m_listFairPvp.begin(), m_listFairPvp.end(), Match);
		if (ator != m_listFairPvp.end())
		{
			int nInterval = m_uTotalSeconds - ator->nStartTime;
			CCommOssHelper::UserMatchOss(uid, enNormalMathType, nInterval, 0);
		}
	}
	

	return remove(uid);
}

void CMatchManager::matchUpdate()
{
	unsigned int cpnInterval = 0;
	unsigned int fairInterval = 0;
	m_uTotalSeconds++;

	CConfPvpMatchTime * pPvpMatchConf = dynamic_cast<CConfPvpMatchTime*>(
		CConfManager::getInstance()->getConf(CONF_PVP_MATCH_TIME));
	if (NULL == pPvpMatchConf)
	{
		return;
	}

	int curCpnCount = m_listCpn.size();
	int curFairCount = m_listFairPvp.size();

	//计算匹配需要的时间
	std::vector<PvpMatchTime> &matchConfs = pPvpMatchConf->GetMatchTime();
	for (std::vector<PvpMatchTime>::iterator iter = matchConfs.begin();
		iter != matchConfs.end(); ++iter)
	{
		if (iter->MatchOnlineLower <= curCpnCount && iter->MatchOnlineUpper > curCpnCount)
		{
			cpnInterval = iter->MatchTime;
		}

		if (iter->MatchOnlineLower <= curFairCount && iter->MatchOnlineUpper > curFairCount)
		{
			fairInterval = iter->MatchTime;
		}
	}

//     KXLOGDEBUG("m_tCPNSeconds %d cpnInterval %d m_tFairSeconds %d fairInterval %d", 
//         m_tCPNSeconds, cpnInterval, m_tFairSeconds, fairInterval);
	// 如果相隔时间超过需要的时间, 则开始匹配
	if (++m_uCPNSeconds >= cpnInterval)
	{
        //KXLOGDEBUG("match server update matching CPN! m_tCPNSeconds %d cpnInterval %d\n", m_tCPNSeconds, cpnInterval);
		while (m_listCpn.size() >= 2)
		{
			NewMatch match1;
			NewMatch match2;
			memset(&match1, 0, sizeof(match1));
			memset(&match2, 0, sizeof(match2));
			if (m_bCPNIntert)
			{
				// 反序
				match1 = m_listCpn.back();
				m_listCpn.pop_back();
				match2 = m_listCpn.back();
				m_listCpn.pop_back();
			}
			else
			{
				match1 = m_listCpn.front();
				m_listCpn.pop_front();
				match2 = m_listCpn.front();
				m_listCpn.pop_front();
			}

			matchSuccess(match1, match2);
			int nInterval = m_uTotalSeconds - match1.nStartTime;
			CCommOssHelper::UserMatchOss(match1.uid, enChamopinMatchType, nInterval, 1);
			nInterval = m_uTotalSeconds - match2.nStartTime;
			CCommOssHelper::UserMatchOss(match2.uid, enChamopinMatchType, nInterval, 1);
		}

		m_bCPNIntert = !m_bCPNIntert;
		m_uCPNSeconds = 0;
	}

	if (++m_uFairSeconds >= fairInterval)
	{
		NewMatch match1;
		NewMatch match2;
		memset(&match1, 0, sizeof(match1));
		memset(&match2, 0, sizeof(match2));
		// 反序 (不处理反序， 反序的遍历删除会出问题)
// 		if (m_bFairInvert)
// 		{
// 
// 		}
// 		else
		{
            /*std::list<NewMatch>::iterator iter1 = m_listFairPvp.begin();
            while (iter1 != m_listFairPvp.end())
            {
                // 注意， iter2才是第一个， iter1为第二个
                std::list<NewMatch>::iterator iter2 = iter1++;
                if (iter1 == m_listFairPvp.end())
                {
                    break;
                }
                NewMatch match1 = *iter1;
                NewMatch match2 = *iter2;
                if (abs(match1.level - match2.level) < MAX_LEVEL_LIMIT)
                {
                    matchSuccess(match1, match2);
                    int nInterval = m_uTotalSeconds - match1.nStartTime;
                    CCommOssHelper::UserMatchOss(match1.uid, enNormalMathType, nInterval, 1);
                    nInterval = m_uTotalSeconds - match2.nStartTime;
                    CCommOssHelper::UserMatchOss(match2.uid, enNormalMathType, nInterval, 1);

                    iter1 = m_listFairPvp.erase(iter1);
                    m_listFairPvp.erase(iter2); 
                }
                else
                {
                    //等级差别到达限制， iter1保留第二个， 下次循环获得第三个
                }
			}*/
            std::list<NewMatch>::iterator iter1 = m_listFairPvp.begin();
            std::list<NewMatch>::iterator iter2 = m_listFairPvp.begin();
            while (iter1 != m_listFairPvp.end())
            {
                ++iter2;
                if (iter2 == m_listFairPvp.end())
                {
                    // 找到尾部应该从下一个开始
                    ++iter1;
                    iter2 = iter1;
                }
                else
                {
                    // 对这两个玩家进行匹配
                    NewMatch match1 = *iter1;
                    NewMatch match2 = *iter2;
                    if (abs(match1.level - match2.level) < MAX_LEVEL_LIMIT)
                    {
                        matchSuccess(match1, match2);
                        int nInterval = m_uTotalSeconds - match1.nStartTime;
                        CCommOssHelper::UserMatchOss(match1.uid, enNormalMathType, nInterval, 1);
                        nInterval = m_uTotalSeconds - match2.nStartTime;
                        CCommOssHelper::UserMatchOss(match2.uid, enNormalMathType, nInterval, 1);

                        iter1 = m_listFairPvp.erase(iter1);
                        if (iter1 == iter2)
                        {
                            iter1 = m_listFairPvp.erase(iter2);
                        }
                        else
                        {
                            m_listFairPvp.erase(iter2);
                        }
                        iter2 = iter1;
                    }
                }
            }
		}

        std::list<NewMatch>::iterator iter = m_listFairPvp.begin();
        while (iter != m_listFairPvp.end())
        {
        	// 如果时间大于指定时间, 匹配机器人
            NewMatch matcher = *iter;
			int interval = m_uTotalSeconds - matcher.nStartTime;
			int dan = CCommonHelper::getUserIntegralDan(matcher.integral);
			int robotTime = CCommonHelper::getDanRobotMatchTime(dan);

			if (interval >= robotTime)
			{
				CCommOssHelper::UserMatchOss(matcher.uid, enNormalMathType, interval, 1);
				matchRobotSuccess(matcher);
                iter = m_listFairPvp.erase(iter);
			}
            else
            {
                ++iter;
            }
		}
		// 该列表的玩家, 每次时间到都全部匹配给机器人
		while (m_listNewGuy.size() > 0)
		{
			NewMatch matcher = m_listNewGuy.front();
			int interval = m_uTotalSeconds - matcher.nStartTime;

			CCommOssHelper::UserMatchOss(matcher.uid, enNormalMathType, interval, 1);
			m_listNewGuy.pop_front();
			matchRobotSuccess(matcher);
		}

		m_bFairInvert = !m_bFairInvert;
		m_uFairSeconds = 0;
	}
}

void CMatchManager::onTimer(const KxServer::kxTimeVal& now)
{
	matchUpdate();
}

void CMatchManager::clearMatchers()
{

	m_listFairPvp.clear();
	m_listCpn.clear();
}

bool CMatchManager::insert(int matchType, int uid, int integral, int level)
{
	NewMatch match;
	match.uid = uid;
	match.integral = integral;
    match.level = level;

	if (matchType == MATCH_CPN)
	{
		match.nStartTime = m_uTotalSeconds;
		if (m_listCpn.size() == 0 || m_listCpn.back().integral >= integral)
		{
			m_listCpn.push_back(match);
		}
		else
		{
			// 从大到小排序
			for (std::list<NewMatch>::iterator iter = m_listCpn.begin();
				iter != m_listCpn.end(); ++iter)
			{
				if (iter->uid == uid)
				{
					return false;
				}

				if (iter->integral < integral)
				{
					m_listCpn.insert(iter, match);
					break;
				}
			}
		}
	}
	else
	{
		match.nStartTime = m_uTotalSeconds;
		if (m_listFairPvp.size() == 0 || m_listFairPvp.back().integral >= integral)
		{
			m_listFairPvp.push_back(match);
		}
		else
		{
			// 从大到小排序
			for (std::list<NewMatch>::iterator iter = m_listFairPvp.begin();
				iter != m_listFairPvp.end(); ++iter)
			{
				if (iter->uid == uid)
				{
					return false;
				}

				if (iter->integral < integral)
				{
					m_listFairPvp.insert(iter, match);
					break;
				}
			}
		}

	}

	return true;
}

bool CMatchManager::remove(int uid)
{
	// 从大到小排序
	NewMatch match;
	match.uid = uid;
	match.integral = 0;
	// 遍历两个列表, 此处如果有性能问题可以优化
	m_listCpn.remove(match);
	m_listFairPvp.remove(match);
	m_listNewGuy.remove(match);
	return true;
}

bool CMatchManager::find(int uid)
{
	return m_setMatcherUids.find(uid) != m_setMatcherUids.end();
}

int CMatchManager::makeBattleId()
{
	return m_nBattleId += 1;
}

void CMatchManager::matchSuccess(NewMatch &matcher1, NewMatch &matcher2)
{
	KXLOGDEBUG("match success uid1=%d, uid2=%d", matcher1.uid, matcher2.uid);
	int battleId = makeBattleId();

	//发送服务器内部协议, 命令session 设置battleid
	CKxCommManager::getInstance()->setSessionRoute(matcher1.uid, emRouteBattleKey, battleId);
	CKxCommManager::getInstance()->setSessionRoute(matcher2.uid, emRouteBattleKey, battleId);

	//对2个玩家进行单播, 告知匹配消息
	CPvpModel *pMatcher2PvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(matcher2.uid, MODELTYPE_PVP));
	CPvpModel *pMatcher1PvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(matcher1.uid, MODELTYPE_PVP));
	if (NULL == pMatcher2PvpModel || NULL == pMatcher1PvpModel)
	{
		return;
	}

	pMatcher1PvpModel->SetPvpBattleId(battleId, 0);
	pMatcher2PvpModel->SetPvpBattleId(battleId, 0);

	MatchMatchSuccessSC matchSC;
	matchSC.battleId = battleId;
	matchSC.robotId = 0;

	CKxCommManager::getInstance()->sendData(matcher1.uid, CMD_MATCH, CMD_MATCH_MATCHSUCCESS_SC,
		reinterpret_cast<char *>(&matchSC), sizeof(matchSC));
	CKxCommManager::getInstance()->sendData(matcher2.uid, CMD_MATCH, CMD_MATCH_MATCHSUCCESS_SC,
		reinterpret_cast<char *>(&matchSC), sizeof(matchSC));

	//移除玩家连接管理等额外信息
	removeFromMatchServer(matcher1.uid);
	removeFromMatchServer(matcher2.uid);
}

void CMatchManager::matchRobotSuccess(NewMatch &matcher)
{
	CPvpModel *pMatcherPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(matcher.uid, MODELTYPE_PVP));
	if (NULL == pMatcherPvpModel)
	{
		KXLOGERROR("%d match robot but get pvp model is NULL!", matcher.uid);
		return;
	}

	int robotTimes = 0, robotIndex = 0;
	if (!pMatcherPvpModel->GetPvpField(PVPCOMM_FD_ROBOTTIMES, robotTimes)
        || !pMatcherPvpModel->GetPvpField(PVPCOMM_FD_ROBOTINDEX1, robotIndex))
	{
		KXLOGERROR("%d match robot but get pvp model robot times error!", matcher.uid);
		return;
	}

	CConfArenaRobot *pConf = dynamic_cast<CConfArenaRobot*>(
		CConfManager::getInstance()->getConf(CONF_ARENA_ROBOT));
    CHECK_RETURN_VOID(NULL != pConf);
    if (pConf->m_RobotEasy.empty() || pConf->m_RobotNormal.empty())
    {
        KXLOGERROR("match robot error! robot list is empty");
        return;
    }

	//根据匹配机器人次数算出该匹配哪个机器人
	int robotId = 0;
	int battleId = makeBattleId();

    if (robotTimes == 0)
    {
        robotIndex = rand() % pConf->m_RobotEasy.size();
    }
    else if (robotTimes == 5)
    {
        robotIndex = rand() % pConf->m_RobotNormal.size();
    }
    
    if (robotTimes < 5)
    {
        robotIndex++;
        robotIndex = robotIndex % pConf->m_RobotEasy.size();
        robotId = pConf->m_RobotEasy[robotIndex]->index;
	}
	else
	{
        robotIndex++;
        robotIndex = robotTimes % pConf->m_RobotNormal.size();
        robotId = pConf->m_RobotNormal[robotIndex]->index;
	}

    robotTimes += 1;
	if (!pMatcherPvpModel->SetPvpField(PVPCOMM_FD_ROBOTTIMES, robotTimes) 
		|| !pMatcherPvpModel->SetPvpBattleId(battleId, robotId)
        || !pMatcherPvpModel->SetPvpField(PVPCOMM_FD_ROBOTINDEX1, robotIndex))
	{
		KXLOGERROR("%d match robot but set pvp model robot times error!", matcher.uid);
		return;
	}

	//发送服务器内部协议, 命令session 设置battleid
	CKxCommManager::getInstance()->setSessionRoute(matcher.uid, emRouteBattleKey, battleId);

	MatchMatchSuccessSC matchSC;
	matchSC.battleId = battleId;
	matchSC.robotId = robotId;

	CKxCommManager::getInstance()->sendData(matcher.uid, CMD_MATCH, CMD_MATCH_MATCHSUCCESS_SC,
		reinterpret_cast<char *>(&matchSC), sizeof(matchSC));

	//移除玩家连接管理等额外信息
	removeFromMatchServer(matcher.uid);
}

void CMatchManager::removeFromMatchServer(int uid)
{
	std::set<int>::iterator iter = m_setMatcherUids.find(uid);
	if (m_setMatcherUids.find(uid) != m_setMatcherUids.end())
	{
		m_setMatcherUids.erase(iter);
	}

	CKxCommManager::getInstance()->delKxComm(uid);
	CGameUserManager::getInstance()->removeGameUser(uid);
}
