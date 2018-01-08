#include "ActiveTask.h"
#include "GameUserManager.h"
#include "ActiveHelper.h"
#include "ConfActive.h"
#include "KxMemPool.h"
#include "ActiveProtocol.h"
#include "Protocol.h"
#include "KxCommManager.h"
#include "ModelHelper.h"
#include "ConfRole.h"
#include "CommonHelper.h"
#include "GameDef.h"

using namespace std;
using namespace KxServer;

CActiveTask::CActiveTask()
{
}


CActiveTask::~CActiveTask()
{
}

bool CActiveTask::init(int uid, int sActiveID, int nTaskID)
{
	m_uid = uid;
	m_ActiveID = sActiveID;
	m_TaskID = nTaskID;
	return true;
}

bool CActiveTask::onAction(int actID, void *data, int len)
{
	CHECK_RETURN(CActiveHelper::getInstance()->isUseFull(m_uid, m_ActiveID));
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(m_uid);
	CHECK_RETURN(NULL != pGameUser)
	CUserModel *pUserModel = (CUserModel*)pGameUser->getModel(MODELTYPE_USER);
	CHECK_RETURN(pUserModel != NULL)
	//活动总表
	CConfActiveTime *conf = dynamic_cast<CConfActiveTime*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TIME));
	CHECK_RETURN(conf != NULL);
	SConfActiveTime *pActiveTime = static_cast<SConfActiveTime *>(conf->getData(m_ActiveID));
	if (pActiveTime != NULL)
	{
		int nUserLevel = 0;
		pUserModel->GetUserFieldVal(USR_FD_USERLV, nUserLevel, true);
		CHECK_RETURN(nUserLevel >= (int)pActiveTime->LvLimit);

		if (pActiveTime->nTimeType == ACTOR_ACTIVETIME_TYPE)
		{
			int nCreatTime = 0;
			int nCurTime = static_cast<int>(time(NULL));
			pUserModel->GetUserFieldVal(USR_FD_CREATETIME, nCreatTime);
			nCreatTime = CTimeCalcTool::curTimeZero(static_cast<time_t>(nCreatTime));
			int nStartTime = nCreatTime + pActiveTime->nStartTime;
			int nEndTime = nCreatTime + pActiveTime->nEndTime;
			CHECK_RETURN(nCurTime >= nStartTime && nCurTime < nEndTime);
		}
	}

	return processActiveTaskData(actID, data, len);
}

//检测是否当前活动的所有任务都完成
bool CActiveTask::checkAllTaskIsFinish()
{
	//活动任务表
	CConfActiveTask *conf = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));
	CHECK_RETURN(conf != NULL);
	SConfActiveTaskData* pActiveTaskData = conf->GetTaskActiveTaskData(m_ActiveID, m_TaskID);
	CHECK_RETURN(pActiveTaskData != NULL);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(m_uid);
	CHECK_RETURN(NULL != pGameUser);
	CUserActiveModel *pUserActiveModel = (CUserActiveModel *)pGameUser->getModel(MODELTYPE_ACTIVE);
	if (pUserActiveModel == NULL)
	{
		return false;
	}

	int nValue = 0;
	pUserActiveModel->GetTaskActiveValue(m_ActiveID, m_TaskID, nValue, true);
	if (nValue < pActiveTaskData->nConditionParam[0])
	{
		return false;
	}

	//标识所有任务完成
	return true;
}

bool CActiveTask::processActiveTaskData(int nActionID, void *data, int len)
{
	switch (nActionID)
	{
		case ELA_PASS_STAGE:
		case ELA_PVP:                          // 完成N次竞技
		case ELA_CHAMPION:					  // 完成N次竞标赛
		{
			if (data == NULL || len != sizeof(ActionCommonInfo))
			{
				return false;
			}

			ActionCommonInfo *pInfo = (ActionCommonInfo *)data;
			return processTimesEventFinish(nActionID,pInfo->param1, pInfo->param2);
		}
		break;
		case ELA_HERO_LEVEL_UP:
		case ELA_HERO_UPGRADE_STAR:
		case ELA_HERO_UPGRADE_SKILL:           // 为任意英雄技能升级
		{
			if (data == NULL || len != sizeof(int))
			{
				return false;
			}

			int *pInfo = (int *)data;

			return processLvUpEventFinish(nActionID, *pInfo);
		}
		break;
		case ELA_DRAW_CARD:
		case ELA_DIAMONDCOSTNUM:
		case ELA_GOLDCOSTNUM:
		case ELA_PVPCOINCOSTNUM:
		case ELA_TOWERCOINCOSTNUM:
		{
			if (data == NULL || len != sizeof(int))
			{
				return false;
			}

			int *pInfo = (int *)data;

			return processNumEventFinish(nActionID, 0, *pInfo);
		}
		break;
		//金币购买次数
		case ELA_BUY_GOLD_TIMES:
		case ELA_GOLD_TEST:                    // 完成N次金币试炼
		case ELA_ENERGY_BUYTIMES:
		case ELA_TOTALLOGIN:
		{
			return processTimesEventFinish(nActionID, 0, 0);
		}
		break;
		case ELA_USER_LEVEL_UP:
		case ELA_CONTINUELOGIN:
		case ELA_OWN_SUMMONER_COUNT:			//召唤师个数
		case ELA_OWN_DIFFEREN_HERO:
		case ELA_STAGE_STAR:
		{
			return processParamEvent(nActionID);
		}
		break;
		case ELA_HAVE_HERO_X_STAR:
		{
			return processHeroStarEvent(nActionID);
		}
		break;
		case ELA_HERO_TEST:                   // 完成N次英雄试炼
		case ELA_TOWER_TEST_FLOOR:             // 完成N层爬塔试炼
		case ELA_FINISHACTIVETASK:
		{
			if (data == NULL || len != sizeof(int))
			{
				return false;
			}

			int *pInfo = (int *)data;

			return processTimesEventFinish(nActionID, (*pInfo), 0);
		}
		break;
		case ELA_BUY_ITEM_COUNT:			//商店购买次数
		case ELA_SHOPFRESHTIMES:			//商店刷新次数
		case ELA_EQUIPBUILD:
		{
			if (data == NULL || len != sizeof(ActionCommonInfo))
			{
				return false;
			}

			ActionCommonInfo *pInfo = (ActionCommonInfo *)data;
			return processNumEventFinish(nActionID, pInfo->param1, pInfo->param2);
		}
		break;
		case ELA_FUND:
		{
			return processFund(nActionID);
		}
		break;
		case ELA_RMVBCOST:					  //人民币消费
		{
			if (data == NULL || len != sizeof(int))
			{
				return false;
			}

			int *pInfo = (int *)data;

			return processUserPay(nActionID, *pInfo);
		}
		break;
		case ELA_OLTIME:
		{
			return processOLTimeEvent(nActionID);
		}
		break;
		case ELA_PAYDIAMOND:
		case ELA_ACTIVELIFTCOST:					//体力消耗
		{
			if (data == NULL || len != sizeof(int))
			{
				return false;
			}

			int *pInfo = (int *)data;

			return processNumEventFinish(nActionID, 0, (*pInfo));
		}
		break;
		case ELA_OWNCOLORHERO:
		{
			return processHeroColorEvent(nActionID);
		}
		break;
		case ELA_OWNLEVELHERO:
		{
			return processHeroLevEvent(nActionID);
		}
		break;
	default:
		break;
	}

	return true;
}

//处理次数增加事件
bool CActiveTask::processTimesEventFinish(int actID, int Param1, int Param2)
{
	//活动任务表
	CConfActiveTask *conf = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));
	CHECK_RETURN(conf != NULL);
	SConfActiveTaskData* pActiveTaskData = conf->GetTaskActiveTaskData(m_ActiveID, m_TaskID);
	CHECK_RETURN(pActiveTaskData != NULL);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(m_uid);
	CHECK_RETURN(NULL != pGameUser);
	CUserActiveModel *pUserActiveModel = (CUserActiveModel *)pGameUser->getModel(MODELTYPE_ACTIVE);
	CHECK_RETURN(pUserActiveModel != NULL);
	CUserModel *pUserModel = (CUserModel*)pGameUser->getModel(MODELTYPE_USER);
	CHECK_RETURN(pUserModel != NULL);
	
	int nValue = 0;
	int nFlag = 0;
	pUserActiveModel->GetTaskActiveIndex(m_ActiveID, m_TaskID, nFlag, true);
	if (nFlag != 0)
	{
		//这时候删除任务对象
		return true;
	}

	pUserActiveModel->GetTaskActiveValue(m_ActiveID, m_TaskID, nValue, true);
	if (pActiveTaskData->nConditionParam[1] != 0)
	{
		if (pActiveTaskData->nConditionParam[1] != Param1)
		{
			return false;
		}
	}

	char *pSendData = reinterpret_cast<char *>(kxMemMgrAlocate(4096));
	SActiveTaskUpdateSC *pActiveTaskUpdateSC = (SActiveTaskUpdateSC *)pSendData;
	pActiveTaskUpdateSC->sCount = 0;
	pActiveTaskUpdateSC->sActiveID = m_ActiveID;
	int nLen = sizeof(SActiveTaskUpdateSC);
	SActiveTaskUpdateData *pActiveTaskUpdateData = (SActiveTaskUpdateData *)(pActiveTaskUpdateSC + 1);
	int nInterval = CActiveHelper::getInstance()->getActiveTimeInterval(m_uid, m_ActiveID);
	//计算通关次数累加
	nValue = nValue + 1;
	pActiveTaskUpdateSC->sCount++;
	pUserActiveModel->SetTaskActiveValue(m_ActiveID,nInterval, m_TaskID, nValue);
	pActiveTaskUpdateData->cID = m_TaskID;
	pActiveTaskUpdateData->nParam = nValue;
	pActiveTaskUpdateData++;
	nLen += sizeof(SActiveTaskUpdateData);

    CKxCommManager::getInstance()->sendData(m_uid, CMD_ACTIVE, CMD_ACTIVE_TASKUPDATE_SC, pSendData, nLen);
	KxServer::kxMemMgrRecycle(pSendData, 4096);
	return false;
}

//处理数值增加事件
bool CActiveTask::processNumEventFinish(int actID, int Param1, int Param2)
{
	//活动任务表
	CConfActiveTask *conf = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));
	CHECK_RETURN(conf != NULL && Param2 != 0)
	SConfActiveTaskData* pActiveTaskData = conf->GetTaskActiveTaskData(m_ActiveID, m_TaskID);
	CHECK_RETURN(pActiveTaskData != NULL);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(m_uid);
	CHECK_RETURN(NULL != pGameUser);
	CUserActiveModel *pUserActiveModel = (CUserActiveModel *)pGameUser->getModel(MODELTYPE_ACTIVE);
	CHECK_RETURN(pUserActiveModel != NULL);
	CUserModel *pUserModel = (CUserModel*)pGameUser->getModel(MODELTYPE_USER);
	CHECK_RETURN(pUserModel != NULL);
	
	int nValue = 0;
	//已经领取了
	int nFlag = 0;
	pUserActiveModel->GetTaskActiveIndex(m_ActiveID, m_TaskID, nFlag, true);
	if (nFlag != 0)
	{
		return true;
	}

	pUserActiveModel->GetTaskActiveValue(m_ActiveID, m_TaskID, nValue, true);
	if (pActiveTaskData->nConditionParam[1] != 0)
	{
		if (pActiveTaskData->nConditionParam[1] != Param1)
		{
			return false;
		}
	}

	char *pSendData = reinterpret_cast<char *>(kxMemMgrAlocate(4096));
	SActiveTaskUpdateSC *pActiveTaskUpdateSC = (SActiveTaskUpdateSC *)pSendData;
	pActiveTaskUpdateSC->sCount = 0;
	pActiveTaskUpdateSC->sActiveID = m_ActiveID;
	int nLen = sizeof(SActiveTaskUpdateSC);
	SActiveTaskUpdateData *pActiveTaskUpdateData = (SActiveTaskUpdateData *)(pActiveTaskUpdateSC + 1);
	int nInterval = CActiveHelper::getInstance()->getActiveTimeInterval(m_uid, m_ActiveID);
	//计算通关次数累加
	nValue = nValue + Param2;
	pActiveTaskUpdateSC->sCount++;
	pUserActiveModel->SetTaskActiveValue(m_ActiveID, nInterval, m_TaskID, nValue);
	pActiveTaskUpdateData->cID = m_TaskID;
	pActiveTaskUpdateData->nParam = nValue;
	pActiveTaskUpdateData++;
	nLen += sizeof(SActiveTaskUpdateData);

	CKxCommManager::getInstance()->sendData(m_uid, CMD_ACTIVE, CMD_ACTIVE_TASKUPDATE_SC, pSendData, nLen);
	KxServer::kxMemMgrRecycle(pSendData, 4096);
	return false;
}

//处理升星等级条件
bool CActiveTask::processLvUpEventFinish(int actID, int Param1)
{
	//活动任务表
	CConfActiveTask *conf = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));
	CHECK_RETURN(conf != NULL);
	SConfActiveTaskData* pActiveTaskData = conf->GetTaskActiveTaskData(m_ActiveID, m_TaskID);
	CHECK_RETURN(pActiveTaskData != NULL);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(m_uid);
	CHECK_RETURN(NULL != pGameUser);
	CUserActiveModel *pUserActiveModel = (CUserActiveModel *)pGameUser->getModel(MODELTYPE_ACTIVE);
	CHECK_RETURN(pUserActiveModel != NULL)
	CUserModel *pUserModel = (CUserModel*)pGameUser->getModel(MODELTYPE_USER);
	CHECK_RETURN(pUserModel != NULL)
	
	int nValue = 0;
	//已经领取了
	int nFlag = 0;
	pUserActiveModel->GetTaskActiveIndex(m_ActiveID, m_TaskID, nFlag, true);
	if (nFlag != 0)
	{
		return true;
	}

	pUserActiveModel->GetTaskActiveValue(m_ActiveID, m_TaskID, nValue, true);
	if (pActiveTaskData->nConditionParam[1] != 0)
	{
		if (pActiveTaskData->nConditionParam[1] > Param1)
		{
			return false;
		}
	}

	char *pSendData = reinterpret_cast<char *>(kxMemMgrAlocate(4096));
	SActiveTaskUpdateSC *pActiveTaskUpdateSC = (SActiveTaskUpdateSC *)pSendData;
	pActiveTaskUpdateSC->sCount = 0;
	pActiveTaskUpdateSC->sActiveID = m_ActiveID;
	int nLen = sizeof(SActiveTaskUpdateSC);
	SActiveTaskUpdateData *pActiveTaskUpdateData = (SActiveTaskUpdateData *)(pActiveTaskUpdateSC + 1);
	int nInterval = CActiveHelper::getInstance()->getActiveTimeInterval(m_uid, m_ActiveID);
	//计算通关次数累加
	nValue = nValue + 1;
	pActiveTaskUpdateSC->sCount++;
	pUserActiveModel->SetTaskActiveValue(m_ActiveID, nInterval, m_TaskID, nValue);
	pActiveTaskUpdateData->cID = m_TaskID;
	pActiveTaskUpdateData->nParam = nValue;
	pActiveTaskUpdateData++;
	nLen += sizeof(SActiveTaskUpdateData);
	nValue = 0;

    CKxCommManager::getInstance()->sendData(m_uid, CMD_ACTIVE, CMD_ACTIVE_TASKUPDATE_SC, pSendData, nLen);
	KxServer::kxMemMgrRecycle(pSendData, 4096);
	return false;
}

//处理玩家属性事件
bool CActiveTask::processParamEvent(int actID)
{
	//活动任务表
	CConfActiveTask *conf = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));
	CHECK_RETURN(conf != NULL);
	SConfActiveTaskData* pActiveTaskData = conf->GetTaskActiveTaskData(m_ActiveID, m_TaskID);
	CHECK_RETURN(pActiveTaskData != NULL);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(m_uid);
	CHECK_RETURN(NULL != pGameUser);
	CUserActiveModel *pUserActiveModel = (CUserActiveModel *)pGameUser->getModel(MODELTYPE_ACTIVE);
	CHECK_RETURN(pUserActiveModel != NULL);
	CUserModel *pUserModel = (CUserModel*)pGameUser->getModel(MODELTYPE_USER);
	CHECK_RETURN(pUserModel != NULL);
	//已经领取了
	int nFlag = 0;
	pUserActiveModel->GetTaskActiveIndex(m_ActiveID, m_TaskID, nFlag, true);
	if (nFlag != 0)
	{
		return true;
	}

	int nValue = 0;
	switch (actID)
	{
		case ELA_USER_LEVEL_UP:
		{
			pUserModel->GetUserFieldVal(USR_FD_USERLV, nValue);
		}
		break;
		case ELA_CONTINUELOGIN:
		{
			pUserModel->GetUserFieldVal(USR_FC_CONLOGIN_DAY, nValue);
		}
		break;
		case ELA_TOTALLOGIN:
		{
			pUserModel->GetUserFieldVal(USR_FD_LOGIN_DAY, nValue);
		}
		break;
		case ELA_OWN_SUMMONER_COUNT:
		{
			CSummonModel *pSummonModel = (CSummonModel*)pGameUser->getModel(MODELTYPE_SUMMONER);
			CHECK_RETURN(pSummonModel != NULL)
			std::vector<int> SummerSet;
			SummerSet = pSummonModel->GetAllSummon();
			nValue = SummerSet.size();
		}
		break;
		case ELA_OWN_DIFFEREN_HERO:
		{
			CHeroModel *pHeroModel = (CHeroModel*)pGameUser->getModel(MODELTYPE_HERO);
			CHECK_RETURN(pHeroModel != NULL)
			std::map<int, DBHeroAttr> HeroMap = pHeroModel->GetHeros();
			nValue = HeroMap.size();
		}
		break;
		case ELA_STAGE_STAR:
		{
			CStageModel *pStageModel = dynamic_cast<CStageModel*>(pGameUser->getModel(MODELTYPE_STAGE));
			CHECK_RETURN(pStageModel != NULL)
			std::map<int, int>& mapNormalChapter = pStageModel->GetChapterStatusMap();
			std::map<int, int>::iterator iterNormalChapter = mapNormalChapter.begin();
			for (; iterNormalChapter != mapNormalChapter.end(); ++iterNormalChapter)
			{
				if (iterNormalChapter->second >= CHAPTERSTATUS_UNLOCK)
				{
					nValue += CModelHelper::getChapterRewardStar(m_uid, iterNormalChapter->first);
				}
			}
		}
		break;
	default:
		break;
	}

	char *pSendData = reinterpret_cast<char *>(kxMemMgrAlocate(4096));
	SActiveTaskUpdateSC *pActiveTaskUpdateSC = (SActiveTaskUpdateSC *)pSendData;
	pActiveTaskUpdateSC->sCount = 0;
	pActiveTaskUpdateSC->sActiveID = m_ActiveID;
	int nLen = sizeof(SActiveTaskUpdateSC);
	SActiveTaskUpdateData *pActiveTaskUpdateData = (SActiveTaskUpdateData *)(pActiveTaskUpdateSC + 1);
	CHECK_RETURN(nValue != 0);
	int nTempValue;
	pUserActiveModel->GetTaskActiveValue(m_ActiveID, m_TaskID, nTempValue, true);
	CHECK_RETURN(nValue != nTempValue)
	int nInterval = CActiveHelper::getInstance()->getActiveTimeInterval(m_uid, m_ActiveID);
	//计算通关次数累加
	pActiveTaskUpdateSC->sCount++;
	pUserActiveModel->SetTaskActiveValue(m_ActiveID, nInterval, m_TaskID, nValue);
	pActiveTaskUpdateData->cID = m_TaskID;
	pActiveTaskUpdateData->nParam = nValue;
	pActiveTaskUpdateData++;
	nLen += sizeof(SActiveTaskUpdateData);

    CKxCommManager::getInstance()->sendData(m_uid, CMD_ACTIVE, CMD_ACTIVE_TASKUPDATE_SC, pSendData, nLen);
	KxServer::kxMemMgrRecycle(pSendData, 4096);
	return  false;
}

//处理玩家充值
bool CActiveTask::processUserPay(int actID, int Param1)
{
	//活动任务表
	CConfActiveTask *conf = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));
	CHECK_RETURN(conf != NULL)
	SConfActiveTaskData* pActiveTaskData = conf->GetTaskActiveTaskData(m_ActiveID, m_TaskID);
	CHECK_RETURN(pActiveTaskData != NULL)
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(m_uid);
	CHECK_RETURN(NULL != pGameUser)
	CUserActiveModel *pUserActiveModel = (CUserActiveModel *)pGameUser->getModel(MODELTYPE_ACTIVE);
	CHECK_RETURN(pUserActiveModel != NULL)
	CUserModel *pUserModel = (CUserModel*)pGameUser->getModel(MODELTYPE_USER);
	CHECK_RETURN(pUserModel != NULL)
	
	int nValue = 0;
	pUserModel->GetUserFieldVal(USR_FD_PAYMENT, nValue,true);
	//已经领取了
	int nFlag = 0;
	pUserActiveModel->GetTaskActiveIndex(m_ActiveID, m_TaskID, nFlag, true);
	if (nFlag != 0)
	{
		return true;
	}

	char *pSendData = reinterpret_cast<char *>(kxMemMgrAlocate(4096));
	SActiveTaskUpdateSC *pActiveTaskUpdateSC = (SActiveTaskUpdateSC *)pSendData;
	pActiveTaskUpdateSC->sCount = 0;
	pActiveTaskUpdateSC->sActiveID = m_ActiveID;
	int nLen = sizeof(SActiveTaskUpdateSC);
	SActiveTaskUpdateData *pActiveTaskUpdateData = (SActiveTaskUpdateData *)(pActiveTaskUpdateSC + 1);
	int nInterval = CActiveHelper::getInstance()->getActiveTimeInterval(m_uid, m_ActiveID);
	switch (pActiveTaskData->nConditionParam[1])
	{
		case emCum_PayType:			//累计充值
		{
			pActiveTaskUpdateSC->sCount++;
			pUserActiveModel->SetTaskActiveValue(m_ActiveID, nInterval, m_TaskID, nValue);
		}
		break;
		case emActive_PayType:		//活动期间内充值
		{
			pUserActiveModel->GetTaskActiveValue(m_ActiveID, m_TaskID, nValue, true);
			nValue = nValue + Param1;
			pUserActiveModel->SetTaskActiveValue(m_ActiveID, nInterval, m_TaskID, nValue);
		}
		break;
		case emPoint_PayType:		//指定额度充值
		{
			if (Param1 == pActiveTaskData->nConditionParam[0])
			{
				pUserActiveModel->SetTaskActiveValue(m_ActiveID, nInterval, m_TaskID, Param1);
				nValue = Param1;
			}
			else
			{
				nValue = 0;
			}
		}
		break;
		default:
		break;
	}

	//计算通关次数累加
	pActiveTaskUpdateSC->sCount++;
	pActiveTaskUpdateData->cID = m_TaskID;
	pActiveTaskUpdateData->nParam = nValue;
	pActiveTaskUpdateData++;
	nLen += sizeof(SActiveTaskUpdateData);
    CKxCommManager::getInstance()->sendData(m_uid, CMD_ACTIVE, CMD_ACTIVE_TASKUPDATE_SC, pSendData, nLen);
	KxServer::kxMemMgrRecycle(pSendData, 4096);
	return false;
}

//处理玩家英雄数据
bool CActiveTask::processHeroStarEvent(int actID)
{
	//活动任务表
	CConfActiveTask *conf = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));
	CHECK_RETURN(conf != NULL)
	SConfActiveTaskData* pActiveTaskData = conf->GetTaskActiveTaskData(m_ActiveID, m_TaskID);
	CHECK_RETURN(pActiveTaskData != NULL);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(m_uid);
	CHECK_RETURN(NULL != pGameUser);
	CUserActiveModel *pUserActiveModel = (CUserActiveModel *)pGameUser->getModel(MODELTYPE_ACTIVE);
	CHECK_RETURN(pUserActiveModel != NULL);
	CHeroModel *pHeroModel = (CHeroModel*)pGameUser->getModel(MODELTYPE_HERO);
	CHECK_RETURN(pHeroModel != NULL);
	int nValue = 0;
	std::map<int, DBHeroAttr> HeroMap = pHeroModel->GetHeros();
	//已经领取了
	int nFlag = 0;
	pUserActiveModel->GetTaskActiveIndex(m_ActiveID, m_TaskID, nFlag, true);
	if (nFlag != 0)
	{
		return true;
	}

	//计算星级对应的英雄个数
	for (std::map<int, DBHeroAttr>::iterator iter = HeroMap.begin(); iter != HeroMap.end(); ++iter)
	{
		DBHeroAttr &HeroInfo = iter->second;
		if (HeroInfo.star >= pActiveTaskData->nConditionParam[1])
		{
			nValue++;
		}
	}

	int nTempValue;
	pUserActiveModel->GetTaskActiveValue(m_ActiveID, m_TaskID, nTempValue, true);
	if (nValue == 0 || nValue == nTempValue)
	{
		return false;
	}

	int nInterval = CActiveHelper::getInstance()->getActiveTimeInterval(m_uid, m_ActiveID);
	char *pSendData = reinterpret_cast<char *>(kxMemMgrAlocate(4096));
	SActiveTaskUpdateSC *pActiveTaskUpdateSC = (SActiveTaskUpdateSC *)pSendData;
	pActiveTaskUpdateSC->sCount = 0;
	pActiveTaskUpdateSC->sActiveID = m_ActiveID;
	int nLen = sizeof(SActiveTaskUpdateSC);
	SActiveTaskUpdateData *pActiveTaskUpdateData = (SActiveTaskUpdateData *)(pActiveTaskUpdateSC + 1);
	//计算通关次数累加
	pActiveTaskUpdateSC->sCount++;
	pUserActiveModel->SetTaskActiveValue(m_ActiveID, nInterval, m_TaskID, nValue);
	pActiveTaskUpdateData->cID = m_TaskID;
	pActiveTaskUpdateData->nParam = nValue;
	pActiveTaskUpdateData++;
	nLen += sizeof(SActiveTaskUpdateData);

    CKxCommManager::getInstance()->sendData(m_uid, CMD_ACTIVE, CMD_ACTIVE_TASKUPDATE_SC, pSendData, nLen);
	KxServer::kxMemMgrRecycle(pSendData, 4096);
	return false;
}

//处理玩家英雄颜色数据
bool CActiveTask::processHeroColorEvent(int actID)
{
	//活动任务表
	CConfActiveTask *conf = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));
	CHECK_RETURN(conf != NULL)
	SConfActiveTaskData* pActiveTaskData = conf->GetTaskActiveTaskData(m_ActiveID, m_TaskID);
	CHECK_RETURN(pActiveTaskData != NULL);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(m_uid);
	CHECK_RETURN(NULL != pGameUser);
	CUserActiveModel *pUserActiveModel = (CUserActiveModel *)pGameUser->getModel(MODELTYPE_ACTIVE);
	CHECK_RETURN(pUserActiveModel != NULL);
	CHeroModel *pHeroModel = (CHeroModel*)pGameUser->getModel(MODELTYPE_HERO);
	CHECK_RETURN(pHeroModel != NULL);
	//已经领取了
	int nFlag = 0;
	pUserActiveModel->GetTaskActiveIndex(m_ActiveID, m_TaskID, nFlag, true);
	if (nFlag != 0)
	{
		return true;
	}

	int nValue = 0;
	std::map<int, DBHeroAttr> HeroMap = pHeroModel->GetHeros();
	//计算星级对应的颜色个数
	for (std::map<int, DBHeroAttr>::iterator iter = HeroMap.begin(); iter != HeroMap.end(); ++iter)
	{
		DBHeroAttr &HeroInfo = iter->second;
		const SoldierConfItem* pSolderConf = queryConfSoldier(iter->first, HeroInfo.star);
		if (pSolderConf == NULL)
		{
			continue;
		}

		if (pSolderConf->Rare >= pActiveTaskData->nConditionParam[1])
		{
			nValue++;
		}
	}
	CHECK_RETURN(nValue != 0)
	int nTempValue;
	pUserActiveModel->GetTaskActiveValue(m_ActiveID, m_TaskID, nTempValue, true);
	CHECK_RETURN(nValue != nTempValue)
	
	char *pSendData = reinterpret_cast<char *>(kxMemMgrAlocate(4096));
	SActiveTaskUpdateSC *pActiveTaskUpdateSC = (SActiveTaskUpdateSC *)pSendData;
	pActiveTaskUpdateSC->sCount = 0;
	pActiveTaskUpdateSC->sActiveID = m_ActiveID;
	int nLen = sizeof(SActiveTaskUpdateSC);
	SActiveTaskUpdateData *pActiveTaskUpdateData = (SActiveTaskUpdateData *)(pActiveTaskUpdateSC + 1);
	int nInterval = CActiveHelper::getInstance()->getActiveTimeInterval(m_uid, m_ActiveID);
	//计算通关次数累加
	pActiveTaskUpdateSC->sCount++;
	pUserActiveModel->SetTaskActiveValue(m_ActiveID, nInterval, m_TaskID, nValue);
	pActiveTaskUpdateData->cID = m_TaskID;
	pActiveTaskUpdateData->nParam = nValue;
	pActiveTaskUpdateData++;
	nLen += sizeof(SActiveTaskUpdateData);

	CKxCommManager::getInstance()->sendData(m_uid, CMD_ACTIVE, CMD_ACTIVE_TASKUPDATE_SC, pSendData, nLen);
	KxServer::kxMemMgrRecycle(pSendData, 4096);
	return false;
}

//处理玩家英雄等级数据
bool CActiveTask::processHeroLevEvent(int actID)
{
	//活动任务表
	CConfActiveTask *conf = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));
	CHECK_RETURN(conf != NULL);
	SConfActiveTaskData* pActiveTaskData = conf->GetTaskActiveTaskData(m_ActiveID, m_TaskID);
	CHECK_RETURN(pActiveTaskData != NULL);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(m_uid);
	CHECK_RETURN(NULL != pGameUser);
	CUserActiveModel *pUserActiveModel = (CUserActiveModel *)pGameUser->getModel(MODELTYPE_ACTIVE);
	CHECK_RETURN(pUserActiveModel != NULL);
	CHeroModel *pHeroModel = (CHeroModel*)pGameUser->getModel(MODELTYPE_HERO);
	CHECK_RETURN(pHeroModel != NULL);
	//已经领取了
	int nFlag = 0;
	pUserActiveModel->GetTaskActiveIndex(m_ActiveID, m_TaskID, nFlag, true);
	if (nFlag != 0)
	{
		return true;
	}

	int nValue = 0;
	std::map<int, DBHeroAttr> HeroMap = pHeroModel->GetHeros();
	//计算星级对应的颜色个数
	for (std::map<int, DBHeroAttr>::iterator iter = HeroMap.begin(); iter != HeroMap.end(); ++iter)
	{
		DBHeroAttr &HeroInfo = iter->second;
		if (HeroInfo.level >= pActiveTaskData->nConditionParam[1])
		{
			nValue++;
		}
	}

	CHECK_RETURN(nValue != 0)
	int nTempValue;
	pUserActiveModel->GetTaskActiveValue(m_ActiveID, m_TaskID, nTempValue, true);
	CHECK_RETURN(nValue != nTempValue)
	
	char *pSendData = reinterpret_cast<char *>(kxMemMgrAlocate(4096));
	SActiveTaskUpdateSC *pActiveTaskUpdateSC = (SActiveTaskUpdateSC *)pSendData;
	pActiveTaskUpdateSC->sCount = 0;
	pActiveTaskUpdateSC->sActiveID = m_ActiveID;
	int nLen = sizeof(SActiveTaskUpdateSC);
	SActiveTaskUpdateData *pActiveTaskUpdateData = (SActiveTaskUpdateData *)(pActiveTaskUpdateSC + 1);
	int nInterval = CActiveHelper::getInstance()->getActiveTimeInterval(m_uid, m_ActiveID);
	//计算通关次数累加
	pActiveTaskUpdateSC->sCount++;
	pUserActiveModel->SetTaskActiveValue(m_ActiveID, nInterval, m_TaskID, nValue);
	pActiveTaskUpdateData->cID = m_TaskID;
	pActiveTaskUpdateData->nParam = nValue;
	pActiveTaskUpdateData++;
	nLen += sizeof(SActiveTaskUpdateData);

	CKxCommManager::getInstance()->sendData(m_uid, CMD_ACTIVE, CMD_ACTIVE_TASKUPDATE_SC, pSendData, nLen);
	KxServer::kxMemMgrRecycle(pSendData, 4096);
	return false;
}

//处理玩家在线时间累计
bool CActiveTask::processOLTimeEvent(int actID)
{
	//活动任务表
	CConfActiveTask *conf = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));
	CHECK_RETURN(conf != NULL);
	SConfActiveTaskData* pActiveTaskData = conf->GetTaskActiveTaskData(m_ActiveID, m_TaskID);
	CHECK_RETURN(pActiveTaskData != NULL);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(m_uid);
	CHECK_RETURN(NULL != pGameUser);
	CUserActiveModel *pUserActiveModel = (CUserActiveModel *)pGameUser->getModel(MODELTYPE_ACTIVE);
	CHECK_RETURN(pUserActiveModel != NULL);
	CUserModel *pUserModel = (CUserModel *)pGameUser->getModel(MODELTYPE_USER);
	CHECK_RETURN(pUserModel != NULL);
	//已经领取了
	int nFlag = 0;
	pUserActiveModel->GetTaskActiveIndex(m_ActiveID, m_TaskID, nFlag, true);
	if (nFlag != 0)
	{
		return true;
	}

	int nValue = 0;
	//计算累计在线时间
	int nLoginTime = 0;
	pUserModel->GetUserFieldVal(USR_FD_LOGINTIME, nLoginTime);
	nValue = time(NULL) - nLoginTime;
	CHECK_RETURN(nValue != 0)
	int nTempValue;
	pUserActiveModel->GetTaskActiveValue(m_ActiveID, m_TaskID, nTempValue, true);
	nValue = nValue + nTempValue;
	char *pSendData = reinterpret_cast<char *>(kxMemMgrAlocate(4096));
	SActiveTaskUpdateSC *pActiveTaskUpdateSC = (SActiveTaskUpdateSC *)pSendData;
	pActiveTaskUpdateSC->sCount = 0;
	pActiveTaskUpdateSC->sActiveID = m_ActiveID;
	int nLen = sizeof(SActiveTaskUpdateSC);
	SActiveTaskUpdateData *pActiveTaskUpdateData = (SActiveTaskUpdateData *)(pActiveTaskUpdateSC + 1);
	int nInterval = CActiveHelper::getInstance()->getActiveTimeInterval(m_uid, m_ActiveID);
	//计算通关次数累加
	pActiveTaskUpdateSC->sCount++;
	pUserActiveModel->SetTaskActiveValue(m_ActiveID, nInterval, m_TaskID, nValue);
	pActiveTaskUpdateData->cID = m_TaskID;
	pActiveTaskUpdateData->nParam = nValue;
	pActiveTaskUpdateData++;
	nLen += sizeof(SActiveTaskUpdateData);
	CKxCommManager::getInstance()->sendData(m_uid, CMD_ACTIVE, CMD_ACTIVE_TASKUPDATE_SC, pSendData, nLen);
	KxServer::kxMemMgrRecycle(pSendData, 4096);
	return false;
}

//处理玩家基金数据
bool CActiveTask::processFund(int actID)
{
	//活动任务表
	CConfActiveTask *conf = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));
	CHECK_RETURN(conf != NULL);
	SConfActiveTaskData* pActiveTaskData = conf->GetTaskActiveTaskData(m_ActiveID, m_TaskID);
	CHECK_RETURN(pActiveTaskData != NULL);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(m_uid);
	CHECK_RETURN(NULL != pGameUser);
	CUserActiveModel *pUserActiveModel = (CUserActiveModel *)pGameUser->getModel(MODELTYPE_ACTIVE);
	CHECK_RETURN(pUserActiveModel != NULL);
	CUserModel *pUserModel = (CUserModel *)pGameUser->getModel(MODELTYPE_USER);
	CHECK_RETURN(pUserModel != NULL);
	//已经领取了
	int nFlag = 0;
	pUserActiveModel->GetTaskActiveIndex(m_ActiveID, m_TaskID, nFlag, true);
	if (nFlag != 0)
	{
		return true;
	}

	int nValue = 0;
	//计算玩家等级
	int nFondFlag = 0;
	pUserModel->GetUserFieldVal(USR_FD_USERLV, nValue);
	CUserActiveModel::getMonthCardValue(m_uid, FOND_PID, nFondFlag);
	if (nFondFlag == 0)
	{
		return false;
	}

	char *pSendData = reinterpret_cast<char *>(kxMemMgrAlocate(4096));
	SActiveTaskUpdateSC *pActiveTaskUpdateSC = (SActiveTaskUpdateSC *)pSendData;
	pActiveTaskUpdateSC->sCount = 0;
	pActiveTaskUpdateSC->sActiveID = m_ActiveID;
	int nLen = sizeof(SActiveTaskUpdateSC);
	SActiveTaskUpdateData *pActiveTaskUpdateData = (SActiveTaskUpdateData *)(pActiveTaskUpdateSC + 1);
	int nInterval = CActiveHelper::getInstance()->getActiveTimeInterval(m_uid, m_ActiveID);
	//计算通关次数累加
	pActiveTaskUpdateSC->sCount++;
	pUserActiveModel->SetTaskActiveValue(m_ActiveID, nInterval, m_TaskID, nValue);
	pActiveTaskUpdateData->cID = m_TaskID;
	pActiveTaskUpdateData->nParam = nValue;
	pActiveTaskUpdateData++;
	nLen += sizeof(SActiveTaskUpdateData);
	CKxCommManager::getInstance()->sendData(m_uid, CMD_ACTIVE, CMD_ACTIVE_TASKUPDATE_SC, pSendData, nLen);
	KxServer::kxMemMgrRecycle(pSendData, 4096);
	return false;
}


