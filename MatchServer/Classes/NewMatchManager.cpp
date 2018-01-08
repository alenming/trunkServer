#include "NewMatchManager.h"
#include "GameDef.h"
#include "ConfArena.h"
#include "KxCommManager.h"
#include "ModelHelper.h"
#include "PvpModel.h"
#include "Protocol.h"
#include "PvpProtocol.h"
#include "ServerProtocol.h"
#include "KxLog.h"
#include "MatchServer.h"

enum RobotLevel
{
	ROBOTLV_PRIMARY,		//初级
	ROBOTLV_NORMAL,			//普通
	ROBOTLV_ADVANCE			//高级
};

CNewMatchManager::CNewMatchManager()
: m_nBattleId(0)
, m_nMatcherCount(0)
, m_nTotalStamp(0)
{
}

CNewMatchManager::~CNewMatchManager()
{
}

CNewMatchManager *CNewMatchManager::m_pInstance = NULL;
CNewMatchManager *CNewMatchManager::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CNewMatchManager;
		m_pInstance->init();
	}
	return m_pInstance;
}

void CNewMatchManager::destroy()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CNewMatchManager::init()
{
	m_pTimerManager = CMatchServer::getInstance()->getTimerManager();
	m_pTimerManager->addTimer(this, 1, KXREPEAT_FOREVER);

	for (int i = DAN_NEWBIE; i <= DAN_KING; ++i)
	{
		NewMatcherList matcherList;
		matcherList.prevMatchStamp = 0;
		matcherList.reverse = false;
		matcherList.head = NULL;
		matcherList.tail = NULL;
		m_mapMatcherList[i] = matcherList;
	}
	//////////////////////////////////////////////////////////////////////////
// 	
// 	for (int i = 1; i < 100; ++i)
// 	{
// 		NewMatcher *matcher = new NewMatcher;
// 		matcher->uid = i;
// 		matcher->startStamp = 1;
// 		matcher->curLevelStamp = 2;
// 		matcher->userLv = i+i;
// 		matcher->integral = 1;
// 		matcher->matchLevel = (4+i)%7 + 1;
// 		matcher->next = NULL;
// 		matcher->prev = NULL;
// 		insert(matcher->matchLevel, matcher);
// 	}

	//////////////////////////////////////////////////////////////////////////
	return true;
}

bool CNewMatchManager::insertRobot(NewMatcher *matcher)
{
	if (isExsit(matcher->uid))
	{
		return false;
	}
	m_mapRobotMatchers[matcher->uid] = matcher;
	return true;
}

bool CNewMatchManager::insert(int level, NewMatcher *matcher)
{
	// 1. 权值: 等级高者高; 等级相同积分多者高;
	// 2. 列表按权值高到底排序
	if (level < DAN_NEWBIE || level > DAN_KING)
	{
		return false;
	}

	NewMatcherList &mlist = m_mapMatcherList[level];
	if (mlist.head == NULL)
	{
		mlist.head = mlist.tail = matcher;
	}
	else
	{
		NewMatcher *tmp = mlist.head;
		while (tmp != NULL)
		{
			if (tmp->userLv < matcher->userLv 
				|| (tmp->userLv == matcher->userLv && tmp->integral < matcher->integral))
			{
				// 插到查找的对象前面
				// 如果查找的对象为第一个, 替换新的head
				if (tmp == mlist.head)
				{
					matcher->next = mlist.head;
					mlist.head->prev = matcher;
					mlist.head = matcher;
				}
				else
				{
					// 正常插到查找对象前面
					tmp->prev->next = matcher;
					matcher->prev = tmp->prev;
					matcher->next = tmp;
					tmp->prev = matcher;
				}
				break;
			}
			else if ((tmp->userLv == matcher->userLv && tmp->integral > matcher->integral) 
				|| tmp == mlist.tail)
			{
				// 插到查找的对象后面
				// 如果查找的对象为最后一个, 替换新的tail
				if (tmp == mlist.tail)
				{
					mlist.tail->next = matcher;
					matcher->prev = mlist.tail;
					mlist.tail = matcher;
				}
				else
				{
					// 正常插入查找对象后面
					tmp->next->prev = matcher;
					matcher->next = tmp->next;
					matcher->prev = tmp;
					tmp->next = matcher;
				}
				break;
			}
			tmp = tmp->next;
		}
	}
	m_nMatcherCount++;
	m_mapMatchers[matcher->uid] = matcher;
	return true;
}

bool CNewMatchManager::isExsit(int uid)
{
	return m_mapMatchers.find(uid) != m_mapMatchers.end()
		|| m_mapRobotMatchers.find(uid) != m_mapRobotMatchers.end();
}

int CNewMatchManager::getCurrentStamp()
{
	return m_nTotalStamp;
}

NewMatcher* CNewMatchManager::removeFromMatchList(int uid, bool del)
{
	std::map<int, NewMatcher *>::iterator iter = m_mapMatchers.find(uid);
	if (iter == m_mapMatchers.end())
	{
		return NULL;
	}

	NewMatcher *matcher = iter->second;
	NewMatcherList &mlist = m_mapMatcherList[matcher->matchLevel];
	// 1. 列表只有一个对象
	if (matcher == mlist.head && matcher == mlist.tail)
	{
		mlist.head = NULL;
		mlist.tail = NULL;
	}
	// 2. 链表对象多个, 替换头
	else if (matcher == mlist.head)
	{
		mlist.head = mlist.head->next;
		mlist.head->prev = NULL;
		
	}
	// 3. 链表对象多个, 替换尾
	else if (matcher == mlist.tail)
	{
		mlist.tail = matcher->prev;
		mlist.tail->next = NULL;
	}
	// 4. 链表对象多个, 正常移除
	else
	{
		matcher->prev->next = matcher->next;
		matcher->next->prev = matcher->prev;
	}
	matcher->prev = NULL;
	matcher->next = NULL;
	m_mapMatchers.erase(iter);
	if (del)
	{
		deleteMatcher(matcher);
		matcher = NULL;
	}
	m_nMatcherCount--;
	return matcher;
}

bool CNewMatchManager::removeFromRobotList(int uid)
{
	std::map<int, NewMatcher *>::iterator iter = m_mapRobotMatchers.find(uid);
	if (iter == m_mapRobotMatchers.end())
	{
		return false;
	}

	deleteMatcher(iter->second);
	m_mapRobotMatchers.erase(iter);
	return true;
}

void CNewMatchManager::onTimer(const KxServer::kxTimeVal& now)
{
	m_nTotalStamp += 1;
	matchUpdate();
}

void CNewMatchManager::matchUpdate()
{
	int matchTime = 0;
	int timeDiff = 0;
	for (int i = DAN_NEWBIE; i <= DAN_KING; ++i)
	{
		//不同T级列表需要不同时间段匹配一次
		matchTime = getMatchTime(i);
		timeDiff = m_nTotalStamp - m_mapMatcherList[i].prevMatchStamp;
		if (timeDiff >= matchTime)
		{
			m_mapMatcherList[i].prevMatchStamp = m_nTotalStamp;
			if (i == DAN_NEWBIE)
			{
				matchLevel1List(m_mapMatcherList[i]);
			}
			else
			{
				matchUpdateList(i, m_mapMatcherList[i]);
			}
		}
	}

    const NewPlayerItem* newPlayer = queryConfNewPlayerItem(1);

	// 匹配机器人列表, 玩家都匹配到机器人
	for (std::map<int ,NewMatcher*>::iterator iter = m_mapRobotMatchers.begin();
		iter != m_mapRobotMatchers.end(); ++iter)
	{
		NewMatcher *pRobotMatcher = iter->second;
		switch (pRobotMatcher->matchLevel)
		{
		case DAN_NEWBIE:
            // 如果是前N场，匹配固定的机器人
            if (pRobotMatcher->totalWin >= 0 &&
                pRobotMatcher->totalWin < newPlayer->RobotIds.size())
            {
                matchGuideRobotSuccess(pRobotMatcher,
                    newPlayer->RobotIds[pRobotMatcher->totalWin]);
                break;
            }
		case DAN_BRONZE:
			matchRobotSuccess(pRobotMatcher, ROBOTLV_PRIMARY);
			break;
		case DAN_SILVER:
		case DAN_GOLD:
			matchRobotSuccess(pRobotMatcher, ROBOTLV_NORMAL);
			break;
		case DAN_PLATINUM:
		case DAN_DIAMOND:
		case DAN_KING:
			matchRobotSuccess(pRobotMatcher, ROBOTLV_ADVANCE);
			break;
		default:
			break;
		}
		deleteMatcher(pRobotMatcher);
	}
	m_mapRobotMatchers.clear();
}

void CNewMatchManager::matchLevel1List(NewMatcherList &list)
{
	NewMatcher *matcher = list.head;
	if (matcher == NULL)
	{
		return;
	}

	while (matcher)
	{
		NewMatcher *temp1 = matcher;
		matcher = matcher->next;

		matchRobotSuccess(temp1, ROBOTLV_PRIMARY);
		removeFromMatchList(temp1->uid);
	}
}

void CNewMatchManager::matchUpdateList(int level, NewMatcherList &list)
{
	const ArenaSettingItem *pArenaSetting = queryArenaSetting();
	// 玩家之间等级差
	int levelDiff = 0;
	// 固定等级差限制
	int levelLimit = 3;
	// 尽量成对匹配玩家 
	if (list.reverse)
	{
		NewMatcher *matcher1 = list.tail;
		if (matcher1 == NULL)
		{
			return;
		}
		NewMatcher *matcher2 = matcher1->prev;
		while (matcher1 != NULL && matcher2 != NULL)
		{
			levelDiff = abs(matcher1->userLv - matcher2->userLv);
			if (levelDiff <= levelLimit)
			{
				NewMatcher *temp1 = matcher1;
				NewMatcher *temp2 = matcher2;
				matcher1 = matcher2->prev;
				// 有可能是最后一个了
				if (matcher1 != NULL)
				{
					matcher2 = matcher1->prev;
				}
				// 发送匹配成功消息
				matchSuccess(temp1, temp2);
				//从列表中移除
				removeFromMatchList(temp1->uid);
				removeFromMatchList(temp2->uid);
			}
			else
			{
				matcher1 = matcher2;
				matcher2 = matcher2->prev;
			}
		}
	}
	else
	{
		NewMatcher *matcher1 = list.head;
		if (matcher1 == NULL)
		{
			return;
		}
		NewMatcher *matcher2 = matcher1->next;
		while (matcher1 != NULL && matcher2 != NULL)
		{
			levelDiff = abs(matcher1->userLv - matcher2->userLv);
			if (levelDiff <= levelLimit)
			{
				NewMatcher *temp1 = matcher1;
				NewMatcher *temp2 = matcher2;
				matcher1 = matcher2->next;
				// 有可能是最后一个了
				if (matcher1 != NULL)
				{
					matcher2 = matcher1->next;
				}
				// 发送匹配成功消息
				matchSuccess(temp1, temp2);
				//从列表中移除
				removeFromMatchList(temp1->uid);
				removeFromMatchList(temp2->uid);
			}
			else
			{
				matcher1 = matcher2;
				matcher2 = matcher2->next;
			}
		}
	}
	list.reverse = !list.reverse;
	// 最后剩下的:
	// 1. 到了极限等待时间, 匹配高级电脑
	// 2. 到了最大等待时间, 移到下个等级匹配组
	int timeLimit = pArenaSetting->LimitWaitingTime;
	int maxTime = getWaitingTime(level);
	NewMatcher *timeMatcher = list.head;
	while (timeMatcher)
	{
		NewMatcher *temp = timeMatcher;
		timeMatcher = timeMatcher->next;

		int matchTime = m_nTotalStamp - temp->curLevelStamp;
		if (matchTime >= timeLimit)
		{
			matchRobotSuccess(temp, ROBOTLV_ADVANCE);
			removeFromMatchList(temp->uid);
		}
		else if (matchTime >= maxTime)
		{
			// 重新开始计时
			removeFromMatchList(temp->uid, false);
			level -= 1;
			if (level < DAN_NEWBIE)
			{
				level = DAN_NEWBIE;
			}
			temp->matchLevel = level;
			temp->curLevelStamp = m_nTotalStamp;
			insert(level, temp);
		}
	}
}

void CNewMatchManager::matchSuccess(NewMatcher *matcher1, NewMatcher *matcher2)
{
	// 生成战斗id
	int battleId = getBattleId();
	// 发送服务器内部协议, 命令session 设置battleid
	CKxCommManager::getInstance()->setSessionRoute(matcher1->uid, emRouteBattleKey, battleId);
	CKxCommManager::getInstance()->setSessionRoute(matcher2->uid, emRouteBattleKey, battleId);

	// 设置pvp battle信息
	CPvpModel *pMatcher1PvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(matcher1->uid, MODELTYPE_PVP));
	CPvpModel *pMatcher2PvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(matcher2->uid, MODELTYPE_PVP));
	if (NULL == pMatcher2PvpModel || NULL == pMatcher1PvpModel)
	{
		return;
	}
	pMatcher1PvpModel->SetPvpBattleId(battleId, 0);
	pMatcher2PvpModel->SetPvpBattleId(battleId, 0);
	// 对2个玩家进行单播, 告知匹配消息
	MatchMatchSuccessSC matchSC;
	matchSC.battleId = battleId;
	matchSC.robotId = 0;

	CKxCommManager::getInstance()->sendData(matcher1->uid, CMD_MATCH, CMD_MATCH_MATCHSUCCESS_SC,
		reinterpret_cast<char *>(&matchSC), sizeof(matchSC));
	CKxCommManager::getInstance()->sendData(matcher2->uid, CMD_MATCH, CMD_MATCH_MATCHSUCCESS_SC,
		reinterpret_cast<char *>(&matchSC), sizeof(matchSC));

	CKxCommManager::getInstance()->delKxComm(matcher1->uid);
	CKxCommManager::getInstance()->delKxComm(matcher2->uid);
}

void CNewMatchManager::matchRobotSuccess(NewMatcher *matcher, int robotLevel)
{
	CPvpModel *pMatcherPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(matcher->uid, MODELTYPE_PVP));
	if (NULL == pMatcherPvpModel)
	{
		KXLOGERROR("%d match robot but get pvp model is NULL!", matcher->uid);
		return;
	}

	// 不同等级机器人需要不同的机器人次序, 用来依次获得机器人
	int robotIndexField = PVPCOMM_FD_ROBOTINDEX1;
	if (robotLevel == ROBOTLV_NORMAL)
	{
		robotIndexField = PVPCOMM_FD_ROBOTINDEX2;
	}
	else if (robotLevel == ROBOTLV_ADVANCE)
	{
		robotIndexField = PVPCOMM_FD_ROBOTINDEX3;
	}

	int robotTimes = 0, robotIndex = 0;
	if (!pMatcherPvpModel->GetPvpField(PVPCOMM_FD_ROBOTTIMES, robotTimes)
		|| !pMatcherPvpModel->GetPvpField(robotIndexField, robotIndex))
	{
		KXLOGERROR("%d match robot but get pvp model robot times error!", matcher->uid);
		return;
	}
	// 生成战斗id
	int battleId = getBattleId();
	int robotId = 0;
	CConfArenaRobot *pConf = dynamic_cast<CConfArenaRobot*>(
		CConfManager::getInstance()->getConf(CONF_ARENA_ROBOT));

	if (robotLevel == ROBOTLV_PRIMARY)
	{
		robotIndex = robotIndex % pConf->m_RobotEasy.size();
		robotId = pConf->m_RobotEasy[robotIndex]->index;	
	}
	else if (robotLevel == ROBOTLV_NORMAL)
	{
		robotIndex = robotIndex % pConf->m_RobotNormal.size();
		robotId = pConf->m_RobotNormal[robotIndex]->index;
	}
	else
	{
		robotIndex = robotIndex % pConf->m_RobotAdvance.size();
		robotId = pConf->m_RobotAdvance[robotIndex]->index;
	}

	robotIndex++;
	robotTimes++;
	// 根据机器人等级, 随机获得机器人
	if (!pMatcherPvpModel->SetPvpField(PVPCOMM_FD_ROBOTTIMES, robotTimes)
		|| !pMatcherPvpModel->SetPvpField(robotIndexField, robotIndex)
		|| !pMatcherPvpModel->SetPvpBattleId(battleId, robotId))
	{
		KXLOGERROR("%d match robot but set pvp model robot times error!", matcher->uid);
		return;
	}

	// 发送机器人回应包
	MatchMatchSuccessSC matchSC;
	matchSC.battleId = battleId;
	matchSC.robotId = robotId;

	CKxCommManager::getInstance()->sendData(matcher->uid, CMD_MATCH, CMD_MATCH_MATCHSUCCESS_SC,
		reinterpret_cast<char *>(&matchSC), sizeof(matchSC));

	CKxCommManager::getInstance()->delKxComm(matcher->uid);
}

void CNewMatchManager::matchGuideRobotSuccess(NewMatcher *matcher, int robotId)
{
    CPvpModel *pMatcherPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(matcher->uid, MODELTYPE_PVP));
    if (NULL == pMatcherPvpModel)
    {
        KXLOGERROR("%d match robot but get pvp model is NULL!", matcher->uid);
        return;
    }

    // 生成战斗id
    int battleId = getBattleId();
    // 根据机器人等级, 随机获得机器人
    if (!pMatcherPvpModel->SetPvpBattleId(battleId, robotId))
    {
        KXLOGERROR("%d match robot but set pvp model robot times error!", matcher->uid);
        return;
    }

    // 发送机器人回应包
    MatchMatchSuccessSC matchSC;
    matchSC.battleId = battleId;
    matchSC.robotId = robotId;

    CKxCommManager::getInstance()->sendData(matcher->uid, CMD_MATCH, CMD_MATCH_MATCHSUCCESS_SC,
        reinterpret_cast<char *>(&matchSC), sizeof(matchSC));

    CKxCommManager::getInstance()->delKxComm(matcher->uid);
}

NewMatcher *CNewMatchManager::newMatcher()
{
	return new NewMatcher;
}

void CNewMatchManager::deleteMatcher(NewMatcher* matcher)
{
	delete matcher;
	matcher = NULL;
}

void CNewMatchManager::clearMatchers()
{

	for (std::map<int, NewMatcherList>::iterator iter = m_mapMatcherList.begin();
		iter != m_mapMatcherList.end(); ++iter)
	{
		NewMatcherList &mlist = iter->second;
		NewMatcher* matcher = mlist.head;
		while (matcher)
		{
			NewMatcher *temp = matcher;
			matcher = matcher->next;
			deleteMatcher(temp);
		}
		mlist.head = NULL;
		mlist.tail = NULL;
	}

	m_mapMatchers.clear();

	for (std::map<int ,NewMatcher*>::iterator iter = m_mapRobotMatchers.begin();
		iter != m_mapRobotMatchers.end(); ++iter)
	{
		deleteMatcher(iter->second);
	}
	m_mapRobotMatchers.clear();
}

int CNewMatchManager::getMatcherStatus()
{
	const ArenaSettingItem *pArenaSetting = queryArenaSetting();
	if (m_nMatcherCount >= pArenaSetting->High[0] && m_nMatcherCount <= pArenaSetting->High[1])
	{
		return MATCHSTATUS_HIGH;
	}
	else if (m_nMatcherCount >= pArenaSetting->Mid[0] && m_nMatcherCount <= pArenaSetting->Mid[1])
	{
		return MATCHSTATUS_MID;
	}
	else
	{
		return MATCHSTATUS_LOW;
	}
}

int CNewMatchManager::getMatchTime(int matchLevel)
{
	// 枚举从1到7
	matchLevel -= 1;
	const ArenaSettingItem *pArenaSetting = queryArenaSetting();
	int status = getMatcherStatus();
	if (status == MATCHSTATUS_HIGH)
	{
		return pArenaSetting->HighTime[matchLevel];
	}
	else if (status == MATCHSTATUS_MID)
	{
		return pArenaSetting->MidTime[matchLevel];
	}
	else
	{
		return pArenaSetting->LowTime[matchLevel];
	}
}

int CNewMatchManager::getWaitingTime(int matchLevel)
{
	// 枚举从1到7
	matchLevel -= 1;
	const ArenaSettingItem *pArenaSetting = queryArenaSetting();
	int status = getMatcherStatus();
	if (status == MATCHSTATUS_HIGH)
	{
		return pArenaSetting->HighWaitinghTime[matchLevel];
	}
	else if (status == MATCHSTATUS_MID)
	{
		return pArenaSetting->MidWaitingTime[matchLevel];
	}
	else
	{
		return pArenaSetting->LowWaitingTime[matchLevel];
	}
}

int CNewMatchManager::getBattleId()
{
	return ++m_nBattleId;
}
