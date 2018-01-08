#include "MatchService.h"
#include "Protocol.h"
#include "PvpProtocol.h"
#include "MatchManager.h"
#include "NewMatchManager.h"
#include "KxCommManager.h"
#include "ConfGameSetting.h"
#include "ConfArena.h"
#include "CommonHelper.h"
#include "ModelHelper.h"
#include "UserModel.h"
#include "PvpModel.h"
#include "KxLog.h"

void CMatchService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
	case CMD_MATCH_MATCHING_CS:
		processMatchService(uid, buffer, len, commun);
		break;
	case CMD_MATCH_CANCEL_CS:
		processCancelService(uid, buffer, len, commun);
		break;
	default:
		break;
	}
}

void CMatchService::processMatchService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	if (len != sizeof(MatchMatchingCS))
	{
		return;
	}

	if (CNewMatchManager::getInstance()->isExsit(uid))
	{
		KXLOGERROR("user %d already in matching!", uid);
		return;
	}

	MatchMatchingCS *pMatchingCS = reinterpret_cast<MatchMatchingCS*>(buffer);

	CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(uid, MODELTYPE_PVP));
	// 获得最新pvp数据, 连胜场数, 竞技积分.
	pPvpModel->Refresh();
	if (NULL == pUserModel || NULL == pPvpModel)
	{
		//玩家模型获取失败
		KXLOGERROR("uid=%d get model error!", uid);
		return;
	}
	
	int nUserLevel = 0, nIntegral = 0, winTimes = 0, totalWin = 0;
	if (!pUserModel->GetUserFieldVal(USR_FD_USERLV, nUserLevel)
		|| !pPvpModel->GetPvpField(PVP_FD_INTEGRAL, nIntegral)
        || !pPvpModel->GetPvpField(PVP_FD_CONTINUSWINTIMES, winTimes)
        || !pPvpModel->GetPvpField(PVP_FD_TOTALWINTIMES, totalWin))
	{
		KXLOGERROR("uid=%d get model field value error!", uid);
		return;
	}

//	CConfArenaLevel *pLevelConf = dynamic_cast<CConfArenaLevel*>(
//		CConfManager::getInstance()->getConf(CONF_ARENA_LEVEL));
//	std::map<int, int> mapLevels = pLevelConf->getArenaLevels();
//
//	//锦标赛是否处于开启时间
// 	if (pMatchingCS->matchType == MATCH_CPN)
// 	{
// 		if (mapLevels[3] > nUserLevel)
// 		{
// 			//用户等级不足
// 			return;
// 		}
// 
// 		CConfPvpSetting *pPvpSetting = dynamic_cast<CConfPvpSetting*>(
// 			CConfManager::getInstance()->getConf(CONF_PVP_SETTING));
// 		if (NULL == pPvpSetting)
// 		{
// 			KXLOGERROR("uid=%d pvp setting is NULL!", uid);
// 			return;
// 		}
// 		
// 		if (pPvpSetting->m_PvpSetting.ArenaDay.size() < 2 || 
// 			pPvpSetting->m_PvpSetting.ArenaTime.size() < 4)
// 		{
// 			KXLOGERROR("uid=%d pvp setting param is error!", uid);
// 			return;
// 		}
// 
// 		time_t timeNow = time(NULL);
// 		tm *pTm = localtime(&timeNow);
// 		int day = pTm->tm_wday;
// 		day = day == 0 ? 7 : day;
// 
// 		if (day < pPvpSetting->m_PvpSetting.ArenaDay[0] 
// 			|| day > pPvpSetting->m_PvpSetting.ArenaDay[1])
// 		{
// 			//错误码未开启
// 			KXLOGERROR("uid=%d today don't open cpn pvp!!!", uid);
// 			return;
// 		}
// 
// 		int nowMin = pTm->tm_hour * 60 + pTm->tm_min;
// 		int beginMin = pPvpSetting->m_PvpSetting.ArenaTime[0] * 60
// 			+ pPvpSetting->m_PvpSetting.ArenaTime[1];
// 		int endMin = pPvpSetting->m_PvpSetting.ArenaTime[2] * 60
// 			+ pPvpSetting->m_PvpSetting.ArenaTime[3];
// 	
// 		if (nowMin < beginMin || nowMin >= endMin)
// 		{
// 			//错误码 时间未到
// 			KXLOGERROR("uid=%d open later!!!", uid);
// 			return;
// 		}
// 	}
//	else if (pMatchingCS->matchType == MATCH_FAIRPVP)
//	{
//		if (mapLevels[2] > nUserLevel)
//		{
//			// 用户等级不足
//			KXLOGERROR("uid=%d user level not enough!!!", uid);
//			return;
//		}
//	}

	NewMatcher *pMatcher = CNewMatchManager::getInstance()->newMatcher();
	pMatcher->uid = uid;
	pMatcher->startStamp = CNewMatchManager::getInstance()->getCurrentStamp();
	pMatcher->curLevelStamp = CNewMatchManager::getInstance()->getCurrentStamp();
	pMatcher->userLv = nUserLevel;
	pMatcher->integral = nIntegral;
    pMatcher->totalWin = totalWin;
	pMatcher->matchLevel = CCommonHelper::getUserIntegralDan(nIntegral);
	pMatcher->prev = NULL;
	pMatcher->next = NULL;

	// t1会一直匹配机器人, t2输1场匹配机器人.... t7永不会匹配机器人
	int loseTimes[] = {999999, -1, -1, -2, -2, -3, -999999};
	
	int dan = pMatcher->matchLevel - 1;
	if (winTimes <= loseTimes[dan])
	{
		CNewMatchManager::getInstance()->insertRobot(pMatcher);
	}
	else
	{
		CNewMatchManager::getInstance()->insert(pMatcher->matchLevel, pMatcher);
	}

	CKxCommManager::getInstance()->addKxComm(uid, commun);
	CKxCommManager::getInstance()->sendData(uid, CMD_MATCH, CMD_MATCH_MATCHING_SC, NULL, 0);
}

void CMatchService::processCancelService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	//取消成功, 发送
	CKxCommManager::getInstance()->sendData(uid, CMD_MATCH, CMD_MATCH_CANCEL_SC, NULL, 0);

	CNewMatchManager::getInstance()->removeFromMatchList(uid);
	CNewMatchManager::getInstance()->removeFromRobotList(uid);
}
