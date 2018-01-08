#include "ActiveHelper.h"
#include "ConfManager.h"
#include "ConfActive.h"
#include "ConfHall.h"
#include "ActiveTask.h"
#include "GameUserManager.h"
#include "ModelHelper.h"
#include "CommonHelper.h"
#include "TimeCalcTool.h"

using namespace std;
using namespace KxServer;


CActiveHelper * CActiveHelper::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CActiveHelper();
	}

	return m_pInstance;
}

void CActiveHelper::destroy()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CActiveHelper::init(KxServer::KxBaseServer *pServer)
{
	CHECK_RETURN(pServer != NULL);
	m_pServer = pServer;
	int CurTime = (int)time(NULL);

	m_MapActiveTimeObject.clear();

	//活动总表
	CConfActiveTime *conf = dynamic_cast<CConfActiveTime*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TIME));
	CHECK_RETURN(conf != NULL);
	
	map<int, void*> MapData = conf->getDatas();
	int nInterval = 0;
	bool bUseFull = false;

	for (map<int, void*>::iterator ator = MapData.begin(); ator != MapData.end(); ++ator)
	{
		SConfActiveTime *pActiveTime = static_cast<SConfActiveTime *>(ator->second);

		switch (pActiveTime->nTimeType)
		{
			case NONE_TIMEACTIVE_TYPE:
			case ACTOR_ACTIVETIME_TYPE:
			{
				addForeverActiveObject(ator->first);
			}
			break;
			case SERVER_TIMEACTIVE_TYPE:
			{
				CHECK_CONTINUE(CurTime <= pActiveTime->nEndTime);

				if (CurTime < pActiveTime->nStartTime)
				{
					nInterval = pActiveTime->nStartTime - CurTime;
					bUseFull = false;
				}
				else
				{
					nInterval = pActiveTime->nEndTime - CurTime;
					bUseFull = true;
				}

				addActiveTimeObject(ator->first, nInterval, bUseFull);
			}
			break;
			default:
			break;
		}
	}

	//初始计算活动加成
	activeAddAccount();
	return true;
}

//活动加成计算
bool CActiveHelper::activeAddAccount()
{
	//活动掉落加成表
	CConfActiveExtraAdd *conf = dynamic_cast<CConfActiveExtraAdd*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_DROP));
	CHECK_RETURN(conf != NULL);

	//掉落表数据
	CConfDropProp *pDropProp = dynamic_cast<CConfDropProp*>(
		CConfManager::getInstance()->getConf(CONF_ITEMDROP));
	CHECK_RETURN(pDropProp != NULL);

	//重置数据
	pDropProp->ReSetShowDropPropItem();

	for (map<int, CActiveTimeObject*>::iterator iter = m_MapActiveTimeObject.begin(); iter != m_MapActiveTimeObject.end(); ++iter)
	{
		CActiveTimeObject* pTimeObject = iter->second;
		CHECK_CONTINUE(pTimeObject != NULL);
		int nActiveID = iter->first;
		CHECK_CONTINUE(pTimeObject->isUseFull());
		const SConfActiveTime *pActiveTime = queryActiveTimeData(nActiveID);
		CHECK_CONTINUE(pActiveTime != NULL);
		CHECK_CONTINUE(pActiveTime->nTimeType != ACTOR_ACTIVETIME_TYPE);
		SConfActiveExtraAdd *pDropData = static_cast<SConfActiveExtraAdd *>(conf->getData(nActiveID));
		CHECK_CONTINUE(pDropData != NULL);
		CHECK_CONTINUE(propDropData(pDropData));
	}

	return true;
}

//添加活动计时器对象
bool CActiveHelper::addActiveTimeObject(int nActiveID, int sec, bool bUseFull)
{
	map<int, CActiveTimeObject*>::iterator ator = m_MapActiveTimeObject.find(nActiveID);
	CHECK_RETURN(ator == m_MapActiveTimeObject.end());
	
	KxTimerManager *pTimerManager = m_pServer->getTimerManager();
	CHECK_RETURN(pTimerManager != NULL);

	CActiveTimeObject *pActiveObject = new CActiveTimeObject(nActiveID);
	pActiveObject->setTimer(pTimerManager, sec, bUseFull);

	m_MapActiveTimeObject[nActiveID] = pActiveObject;
	pActiveObject->retain();

	return true;
}

//添加永久活动对象
bool CActiveHelper::addForeverActiveObject(int nActiveID)
{
	map<int, CActiveTimeObject*>::iterator ator = m_MapActiveTimeObject.find(nActiveID);
	CHECK_RETURN(ator == m_MapActiveTimeObject.end());
	CActiveTimeObject *pActiveObject = new CActiveTimeObject(nActiveID);
	pActiveObject->setUseFull(true);
	m_MapActiveTimeObject[nActiveID] = pActiveObject;
	pActiveObject->retain();
	return true;
}

//删除活动计时器对象
bool CActiveHelper::delActiveTimeObject(int nActiveID)
{
	map<int, CActiveTimeObject*>::iterator ator = m_MapActiveTimeObject.find(nActiveID);
	CHECK_RETURN(ator != m_MapActiveTimeObject.end());
	
	CActiveTimeObject* pTimeObject = ator->second;
	if (pTimeObject != NULL)
	{
		pTimeObject->stop();
		pTimeObject->release();
	}

	m_MapActiveTimeObject.erase(nActiveID);

	return true;
}

//初始化玩家的活动任务对象
bool CActiveHelper::initActiveTask(int uid)
{
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN(pGameUser != NULL);
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CHECK_RETURN(pUserModel != NULL);
	
	//活动任务表
	CConfActiveTask *conf = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));
	CHECK_RETURN(conf != NULL);

	for (map<int, CActiveTimeObject*>::iterator iter = m_MapActiveTimeObject.begin(); iter != m_MapActiveTimeObject.end(); ++iter)
	{
		CActiveTimeObject* pTimeObject = iter->second;
		int nActiveID = iter->first;
		CHECK_CONTINUE(pTimeObject != NULL);
		//校验活动时限
		const SConfActiveTime *pActiveTime = queryActiveTimeData(nActiveID);
		CHECK_CONTINUE(pActiveTime != NULL);
		if (pActiveTime->nTimeType == ACTOR_ACTIVETIME_TYPE)
		{
			int nCreatTime = 0;
			int nCurTime = static_cast<int>(time(NULL));
			pUserModel->GetUserFieldVal(USR_FD_CREATETIME, nCreatTime);
			nCreatTime = CTimeCalcTool::curTimeZero(static_cast<time_t>(nCreatTime));
			int nStartTime = nCreatTime + pActiveTime->nStartTime;
			int nEndTime = nCreatTime + pActiveTime->nEndTime;
			CHECK_CONTINUE(nCurTime >= nStartTime && nCurTime < nEndTime);
		}

		std::map<int, SConfActiveTaskData>* pMapActiveTask = conf->GetTaskActiveTaskDataByActiveID(nActiveID);
		CHECK_CONTINUE(pMapActiveTask != NULL);
		for (std::map<int, SConfActiveTaskData>::iterator ator = pMapActiveTask->begin(); ator != pMapActiveTask->end(); ++ator)
		{
			SConfActiveTaskData& TaskData = ator->second;
			// 注册监听活动任务
			CActiveTask *pActiveTask = new CActiveTask();
			pActiveTask->init(uid, nActiveID,ator->first);
			if (pActiveTask->checkAllTaskIsFinish())
			{
				//该系列任务全部完成，不需要监听了
				delete pActiveTask;
				continue;
			}

			pGameUser->getUserActionManager()->AddActionListener(TaskData.nFinishCondition, pActiveTask);
		}
	}

	init7DayActiveTask(uid);
	return true;
}

//初始化监听7天活动任务事件
bool CActiveHelper::init7DayActiveTask(int uid)
{
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN(pGameUser != NULL);
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
	CHECK_RETURN(pUserModel != NULL);
	int nCreatTime = 0;
	int nCurTime = static_cast<int>(time(NULL));
	pUserModel->GetUserFieldVal(USR_FD_CREATETIME, nCreatTime);
	int nCreateDay = CCommonHelper::getTotalDay(nCreatTime);
	int nCurDay = CCommonHelper::getTotalDay();
	int nDay = nCurDay - nCreateDay;

	//初始化7天活动
	CConf7DayActive *pConf7DayActive = dynamic_cast<CConf7DayActive*>(
		CConfManager::getInstance()->getConf(CONF_7DAY_ACTIVE));
	CHECK_RETURN(pConf7DayActive != NULL);

	S7DayActive *pDayActive = static_cast<S7DayActive *>(pConf7DayActive->getData(nDay + 1));
	CHECK_RETURN(pDayActive != NULL);

	//活动任务表
	CConfActiveTask *conf = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));
	CHECK_RETURN(conf != NULL);

	for (int i = 0; i < pDayActive->VectActive.size(); i++)
	{
		if (pDayActive->VectActive[i].nActiveType == TASK_ACTIVE_TYPE)
		{
			std::map<int, SConfActiveTaskData>* pMapActiveTask = conf->GetTaskActiveTaskDataByActiveID(pDayActive->VectActive[i].nActiveID);
			CHECK_CONTINUE(pMapActiveTask != NULL);

			for (std::map<int, SConfActiveTaskData>::iterator ator = pMapActiveTask->begin(); ator != pMapActiveTask->end(); ++ator)
			{
				SConfActiveTaskData& TaskData = ator->second;
				// 注册监听活动任务
				CActiveTask *pActiveTask = new CActiveTask();
				pActiveTask->init(uid, pDayActive->VectActive[i].nActiveID, ator->first);
				if (pActiveTask->checkAllTaskIsFinish())
				{
					//该系列任务全部完成，不需要监听了
					delete pActiveTask;
					continue;
				}

				pGameUser->getUserActionManager()->AddActionListener(TaskData.nFinishCondition, pActiveTask);
			}
		}
	}

	return true;
}

//检测启用的活动是否生效
bool CActiveHelper::isUseFull(int uid, int nActiveID)
{
	map<int, CActiveTimeObject*>::iterator ator = m_MapActiveTimeObject.find(nActiveID);

	if (ator == m_MapActiveTimeObject.end())
	{

		return is7DayUseFull(uid,nActiveID);
	}

	CActiveTimeObject* pActiveTimeObject = ator->second;

	if (pActiveTimeObject == NULL)
	{
		return false;
	}

	return pActiveTimeObject->isUseFull();
}

//检验7天活动是否生效
bool CActiveHelper::is7DayUseFull(int uid,int nActiveID)
{
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN(pGameUser != NULL);
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
	CHECK_RETURN(pUserModel != NULL);
	int nCreatTime = 0;
	int nCurTime = static_cast<int>(time(NULL));
	pUserModel->GetUserFieldVal(USR_FD_CREATETIME, nCreatTime);
	int nCreateDay = CCommonHelper::getTotalDay(nCreatTime);
	int nCurDay = CCommonHelper::getTotalDay();
	int nDay = nCurDay - nCreateDay;

	//初始化7天活动
	CConf7DayActive *pConf7DayActive = dynamic_cast<CConf7DayActive*>(
		CConfManager::getInstance()->getConf(CONF_7DAY_ACTIVE));
	CHECK_RETURN(pConf7DayActive != NULL);

	S7DayActive *pDayActive = static_cast<S7DayActive *>(pConf7DayActive->getData(nDay + 1));
	CHECK_RETURN(pDayActive != NULL);

	bool bFind = false;
	unsigned int nIndex = 0;
	for (; nIndex < pDayActive->VectActive.size(); nIndex++)
	{
		if (nActiveID == pDayActive->VectActive[nIndex].nActiveID)
		{
			bFind = true;
			break;
		}
	}

	return bFind;
}

//获取活动时限
int CActiveHelper::getActiveTimeInterval(int uid, int nActiveID)
{
	//活动对应时限表
	CConfActiveTime *ActiveTime = dynamic_cast<CConfActiveTime*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TIME));
	if (ActiveTime == NULL)
	{
		return false;
	}

	SConfActiveTime *pActiveTime = static_cast<SConfActiveTime *>(ActiveTime->getData(nActiveID));
	int nIterval = 0;
	int CurTime = (int)time(NULL);
	if (pActiveTime != NULL)
	{
		if (pActiveTime->nTimeType == NONE_TIMEACTIVE_TYPE)
		{
			return 0;
		}
		else if (pActiveTime->nTimeType == SERVER_TIMEACTIVE_TYPE)
		{
			CHECK_RETURN_INT(pActiveTime->nStartTime <= CurTime && pActiveTime->nEndTime > CurTime)
				nIterval = pActiveTime->nEndTime - CurTime;
		}
		else
		{
			CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
			CHECK_RETURN(pGameUser != NULL);
			CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
			CHECK_RETURN(pUserModel != NULL);
			int nCreatTime = 0;
			pUserModel->GetUserFieldVal(USR_FD_CREATETIME, nCreatTime);
			nCreatTime = CTimeCalcTool::curTimeZero(static_cast<time_t>(nCreatTime));
			int nStartTime = nCreatTime + pActiveTime->nStartTime;
			int nEndTime = nCreatTime + pActiveTime->nEndTime;
			CHECK_RETURN_INT(CurTime >= nStartTime && CurTime < nEndTime);
			nIterval = nEndTime - CurTime;
		}
	}
	else
	{
		CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
		CHECK_RETURN_INT(pUserModel != NULL);
		int nCreatTime = 0;
		int nCurTime = static_cast<int>(time(NULL));
		pUserModel->GetUserFieldVal(USR_FD_CREATETIME, nCreatTime);
		int nCreateDay = CCommonHelper::getTotalDay(nCreatTime);
		int nCurDay = CCommonHelper::getTotalDay();
		int nDay = nCurDay - nCreateDay;

		//初始化7天活动
		CConf7DayActive *pConf7DayActive = dynamic_cast<CConf7DayActive*>(
			CConfManager::getInstance()->getConf(CONF_7DAY_ACTIVE));
		CHECK_RETURN_INT(pConf7DayActive != NULL);

		S7DayActive *pDayActive = static_cast<S7DayActive *>(pConf7DayActive->getData(nDay + 1));
		CHECK_RETURN_INT(pDayActive != NULL);
		CurTime = time(NULL);
		nIterval = CTimeCalcTool::nextTimeStamp(CurTime, 0, 0) - CurTime;
	}

	return nIterval;
}

//计算对应相应掉落ID活动数据的影响
bool CActiveHelper::propDropData(SConfActiveExtraAdd *pDropData)
{
	CHECK_RETURN(pDropData != NULL);

	//掉落表数据
	CConfDropProp *pDropProp = dynamic_cast<CConfDropProp*>(
		CConfManager::getInstance()->getConf(CONF_ITEMDROP));
	CHECK_RETURN(pDropProp != NULL);

	map<int, DropPropItem> RealDropItem = pDropProp->GetShowDropPropItem();
	
	map<int, ExtraDropData>::iterator ator;

	for (int i = 0; i < (int)pDropData->VectDropID.size(); i++)
	{
		map<int, DropPropItem>::iterator iter = RealDropItem.find(pDropData->VectDropID[i]);

		CHECK_CONTINUE(iter != RealDropItem.end());
		DropPropItem& DropItem = iter->second;

		const DropPropItem *pBaseDrop = static_cast<DropPropItem *>(pDropProp->getData(pDropData->VectDropID[i]));
		CHECK_CONTINUE(pBaseDrop != NULL);

		switch (pDropData->nDropType)
		{
			case GOODSID_ACTIVE_DROP_TYPE:
			{
					for (int j = 0; j < (int)DropItem.DropIDs.size(); j++)
					{
						ator = pDropData->MapDropChange.find(DropItem.DropIDs[j].DropID);
						CHECK_CONTINUE(ator != pDropData->MapDropChange.end());
						ExtraDropData &ActiveDropData = ator->second;

						if (pBaseDrop->DropIDs[j].DropNum[0] != 0)
						{
							DropItem.DropIDs[j].DropNum[0] = (int)(pBaseDrop->DropIDs[j].DropNum[0] * (DropItem.DropIDs[j].DropNum[0] / pBaseDrop->DropIDs[j].DropNum[0] + ActiveDropData.nNumMinRate));
						}

						if (pBaseDrop->DropIDs[j].DropNum[1] != 0)
						{
							DropItem.DropIDs[j].DropNum[1] = (int)(pBaseDrop->DropIDs[j].DropNum[1] * (DropItem.DropIDs[j].DropNum[1] / pBaseDrop->DropIDs[j].DropNum[1] + ActiveDropData.nNumMaxRate));
						}
						
						if (pBaseDrop->DropIDs[j].DropRate != 0)
						{
							DropItem.DropIDs[j].DropRate = (int)(pBaseDrop->DropIDs[j].DropRate*(DropItem.DropIDs[j].DropRate / pBaseDrop->DropIDs[j].DropRate + ActiveDropData.fDropRateAdd));
						}
					}
			}
			break;

			case TYPE_ACTIVE_DROP_TYPE:
			{
				for (int j = 0; j < (int)DropItem.DropIDs.size(); j++)
				{
					const PropItem* pPropItem = queryConfProp(DropItem.DropIDs[j].DropID);
                    if (pPropItem == NULL)
                    {
                        LOG("Execute False On File %s Line %d : %d", __FILE__, __LINE__, DropItem.DropIDs[j].DropID);
                        continue;
                    }

					ator = pDropData->MapAddDropType.find(pPropItem->Type);
					CHECK_CONTINUE(ator != pDropData->MapAddDropType.end());
					
					ExtraDropData &ActiveDropData = ator->second;

					if (pBaseDrop->DropIDs[j].DropNum[0] != 0)
					{
						DropItem.DropIDs[j].DropNum[0] = (int)(pBaseDrop->DropIDs[j].DropNum[0] * ((float)DropItem.DropIDs[j].DropNum[0] / (float)pBaseDrop->DropIDs[j].DropNum[0] + ActiveDropData.nNumMinRate));
					}

					if (pBaseDrop->DropIDs[j].DropNum[1] != 0)
					{
						DropItem.DropIDs[j].DropNum[1] = (int)(pBaseDrop->DropIDs[j].DropNum[1] * ((float)DropItem.DropIDs[j].DropNum[1] / (float)pBaseDrop->DropIDs[j].DropNum[1] + ActiveDropData.nNumMaxRate));
					}

					if (pBaseDrop->DropIDs[j].DropRate != 0)
					{
						DropItem.DropIDs[j].DropRate = (int)(pBaseDrop->DropIDs[j].DropRate*((float)DropItem.DropIDs[j].DropRate / (float)pBaseDrop->DropIDs[j].DropRate + ActiveDropData.fDropRateAdd));
					}
				}
			}
			break;

		default:
			break;
		}

		//处理掉落物品添加
		for (map<int, ExtraDropData>::iterator ator = pDropData->MapAddDropExtra.begin(); ator != pDropData->MapAddDropExtra.end(); ++ator)
		{
			ExtraDropData &ActiveDropData = ator->second;
			DropIdData DropData;

			DropData.DropID = ator->first;
			DropData.DropRate = static_cast<int>(ActiveDropData.fDropRateAdd);
			DropData.DropNum.push_back(ActiveDropData.nNumMinRate);
			DropData.DropNum.push_back(ActiveDropData.nNumMaxRate);
			DropItem.DropIDs.push_back(DropData);
		}

		//处理物品掉落数修改
		if (pDropData->nDropNumMin != 0 && pDropData->nDropNumMax != 0)
		{
			DropItem.MeanwhileDropNum.clear();
			DropItem.MeanwhileDropNum.push_back(pDropData->nDropNumMin);
			DropItem.MeanwhileDropNum.push_back(pDropData->nDropNumMax);
		}

		pDropProp->SetShowDropPropItem(pDropData->VectDropID[i], DropItem);
	}

	return true;
}


CActiveHelper::CActiveHelper()
{
}


CActiveHelper::~CActiveHelper()
{
	for (map<int, CActiveTimeObject*>::iterator ator = m_MapActiveTimeObject.begin(); ator != m_MapActiveTimeObject.end(); ++ator)
	{
		CActiveTimeObject* pTimeObject = ator->second;

		if (pTimeObject != NULL)
		{
			pTimeObject->stop();
			pTimeObject->release();
		}
	}

	m_MapActiveTimeObject.clear();
	m_pServer = NULL;
}

CActiveHelper*	CActiveHelper::m_pInstance = NULL;