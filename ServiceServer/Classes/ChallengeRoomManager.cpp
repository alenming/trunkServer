#include "ChallengeRoomManager.h"
#include "ChallengeRoom.h"
#include "ConfHall.h"
#include "ConfStage.h"
#include "ConfGameSetting.h"
#include "StageProtocol.h"

#include "GameUserManager.h"
#include "ModelHelper.h"
#include "GameDef.h"
#include "CommStructs.h"
#include "TimeCalcTool.h"
#include "KxCommManager.h"
#include "Protocol.h"
#include "ErrorCodeProtocol.h"

CChallengeRoomManager::CChallengeRoomManager()
{
}

CChallengeRoomManager::~CChallengeRoomManager()
{
	for (std::map<int, CChallengeRoom*>::iterator iter = m_mapRooms.begin();
		iter != m_mapRooms.end(); ++iter)
	{
		delete iter->second;
	}
	m_mapRooms.clear();
}

CChallengeRoomManager *CChallengeRoomManager::m_pInstance = NULL;
CChallengeRoomManager *CChallengeRoomManager::getInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CChallengeRoomManager;
	}
	return m_pInstance;
}

void CChallengeRoomManager::destroy()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CChallengeRoomManager::init(KxServer::KxTimerManager *pTimerManager)
{
	m_TimeManager = pTimerManager;

	if (m_TimeManager == NULL)
	{
		return false;
	}

	//无限次数
    if (!m_TimeManager->addTimer(this, CHECK_TIME_INTERVAL, KXREPEAT_FOREVER))
	{
		return false;
	}

	return true;
}

void CChallengeRoomManager::onTimer(const KxServer::kxTimeVal& now)
{
	int nCurTime = (int)time(NULL);

	for (std::list<SDelayDelData>::iterator ator = m_DelUserList.begin(); ator != m_DelUserList.end();)
	{
		if (nCurTime > ator->nDelayTime)
		{
			removeRoom(ator->nUid);
			m_MapDelUserList.erase(ator->nUid);
			m_DelUserList.erase(ator++);
		}
		else
		{
			break;
		}
	}
}

bool CChallengeRoomManager::isStageEnough(int uid, int chapterId, int stageId, int times)
{
	//在进行其它挑战的, 不允许
	if (m_mapRooms.find(uid) != m_mapRooms.end())
	{
		KXLOGDEBUG("%s m_mapRooms.find(uid) != m_mapRooms.end()", __FUNCTION__);
		//这里表示玩家已经进入到其他的战斗中，可以发送Tips
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_BATTLE_EXIST;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return false;
	}

	return CModelHelper::canChallengeStage(uid, chapterId, stageId, times);
}

bool CChallengeRoomManager::isInstanceEnough(int uid, int activityId, int times)
{
	//在进行其它挑战的, 不允许
	if (m_mapRooms.find(uid) != m_mapRooms.end())
	{
		KXLOGDEBUG("%s m_mapRooms.find(uid) != m_mapRooms.end()", __FUNCTION__);
		//这里表示玩家已经进入到其他的战斗中，可以发送Tips
		//这里表示玩家已经进入到其他的战斗中，可以发送Tips
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_BATTLE_EXIST;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return false;
	}

	return true;

	//int needEnergy = 3 * times;
	//int haveEnergy = 0;
	////活动副本配置
	//CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	//haveEnergy = CModelHelper::getEnergy(uid);
	//return haveEnergy >= needEnergy;
}

bool CChallengeRoomManager::isGoldTestEnough(int uid, int wday)
{
	//在进行其它挑战的, 不允许
	if (m_mapRooms.find(uid) != m_mapRooms.end())
	{
		KXLOGDEBUG("%s m_mapRooms.find(uid) != m_mapRooms.end()", __FUNCTION__);
		//这里表示玩家已经进入到其他的战斗中，可以发送Tips
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_BATTLE_EXIST;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return false;
	}
	//先刷新一下金币试练信息
	CModelHelper::updateGoldTestInfo(uid);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CGoldTestModel *pGoldTestModel = dynamic_cast<CGoldTestModel*>(pGameUser->getModel(MODELTYPE_GOLDTEST));
	//金币试练配置
	const GoldTestConfItem *pConf = queryConfGoldTest(wday);
	CHECK_RETURN(NULL != pConf);
	//次数判断
	int nUseTimes = 0;
	CHECK_RETURN(pGoldTestModel->GetFieldNum(GT_FD_USETIMES, nUseTimes));

	if (nUseTimes >= pConf->Frequency)
	{
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_SHILIANTIMES_NOTENOUGH;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return false;
	}

	return true;
}

bool CChallengeRoomManager::isHeroTestEnough(int uid, int nInstanceId, int nDiff)
{
	//在进行其它挑战的, 不允许
	if (m_mapRooms.find(uid) != m_mapRooms.end())
	{
		KXLOGDEBUG("%s m_mapRooms.find(uid) != m_mapRooms.end()", __FUNCTION__);
		//这里表示玩家已经进入到其他的战斗中，可以发送Tips
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_BATTLE_EXIST;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return false;
	}
	//刷新英雄试练
	CModelHelper::updateHeroTestInfo(uid);

	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN(NULL != pGameUser);
	CHeroTestModel *pHeroTestModel = dynamic_cast<CHeroTestModel*>(pGameUser->getModel(MODELTYPE_HEROTEST));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CHECK_RETURN(NULL != pHeroTestModel && NULL != pUserModel);

	const TimeRecoverItem *pRecoverConf = queryConfTimeRecoverSetting();
    const HeroTestItem* pHeroTestItem = queryConfHeroTest(nInstanceId);
    CHECK_RETURN(NULL != pHeroTestItem && NULL != pRecoverConf);

	// 指定的英雄试练今日是否开启
	bool bOpening = false;
	time_t nowTime = time(NULL);
	tm *pCurTm = localtime(&nowTime);
	int nCurWDay = pCurTm->tm_wday == 0 ? 7 : pCurTm->tm_wday;

	int hour = pRecoverConf->AllTimeReset / 60;
	int min = pRecoverConf->AllTimeReset % 60;
  
    for (unsigned int i = 0; i < pHeroTestItem->Time.size(); ++i)
    {
		//比如5点开启, 大于今天5点并小于明天的5点, 24小时内
		int nConfWDay = pHeroTestItem->Time[i];
		int nConfNextWDay = pHeroTestItem->Time[i] == 7 ? 1 : pHeroTestItem->Time[i] + 1;
		
		if ((nCurWDay == nConfWDay && pCurTm->tm_hour >= hour && pCurTm->tm_min >= min)
			|| (nCurWDay == nConfNextWDay && pCurTm->tm_hour < hour && pCurTm->tm_min < min))
        {
            bOpening = true;
            break;
        }
    }

    CHECK_RETURN(bOpening);
	//等级限制
    CHECK_RETURN(nDiff > 0 && nDiff <= (int)pHeroTestItem->Diff.size());
    int nUserLv = 0;
    CHECK_RETURN(pUserModel->GetUserFieldVal(USR_FD_USERLV, nUserLv));
    CHECK_RETURN(nUserLv >= pHeroTestItem->Diff[nDiff - 1].UnlockLevel);

	if (pHeroTestItem->Times <= pHeroTestModel->GetChallengeTimes(nInstanceId))
	{
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_SHILIANTIMES_NOTENOUGH;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return false;
	}

	return true;
}

bool CChallengeRoomManager::isTowerTestEnough(int uid)
{
	//在进行其它挑战的, 不允许
	if (m_mapRooms.find(uid) != m_mapRooms.end())
	{
		KXLOGDEBUG("%s m_mapRooms.find(uid) != m_mapRooms.end()", __FUNCTION__);
		//这里表示玩家已经进入到其他的战斗中，可以发送Tips
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_BATTLE_EXIST;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return false;
	}

	CGameUser * pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	if (pGameUser == NULL)
	{
		KXLOGDEBUG("%s pGameUser == NULL", __FUNCTION__);
		return false;
	}
	//如果次数不够
	//更新玩家爬塔
	CModelHelper::updateTowerTestInfo(uid);
	CTowerTestModel *pTowerTestModel = dynamic_cast<CTowerTestModel*>(pGameUser->getModel(MODELTYPE_TOWERTEST));
	if (pTowerTestModel->GetTowerTestField(TOWER_FD_TIMES) <= 0)
	{
		KXLOGDEBUG("%s pTowerTestModel->GetTowerTestField(TOWER_FD_TIMES) <= 0", __FUNCTION__);
		//发送次数不足的Tips
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_BATTLE_TIMESNOTENOUGH;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return false;
	}

	int floor = pTowerTestModel->GetTowerTestField(TOWER_FD_FLOOR);
	int state = pTowerTestModel->GetTowerTestField(TOWER_FD_FLOORSTATE);
	int maxFloor = queryMaxTowerFloor();

	// 如果不处于挑战状态不允许挑战
	if (state != FLOORSTATE_FIGHTING)
	{
		KXLOGDEBUG("%s state != FLOORSTATE_FIGHTING", __FUNCTION__);
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_BATTLE_STATEERROR;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return false;
	}
	// 当前楼层最高
	if (floor > maxFloor)
	{
		KXLOGDEBUG("%s floor > maxFloor", __FUNCTION__);
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_BATTLE_MAXTOWER;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return false;
	}
	return true;
}

CChallengeRoom *CChallengeRoomManager::getRoom(int uid)
{
	if (m_mapRooms.find(uid) != m_mapRooms.end())
	{
		return m_mapRooms[uid];
	}
	return NULL;
}

CChallengeRoom *CChallengeRoomManager::createRoom(int uid, int battleType)
{
	CChallengeRoom *pRoom = NULL;
	if (m_mapRooms.find(uid) != m_mapRooms.end())
	{
		removeRoom(uid);
	}

	switch (battleType)
	{
	case EBATTLE_INSTANCE:
		pRoom = new CInstanceRoom;
		break;
	default:
		return NULL;
		break;
	}

	m_mapRooms[uid] = pRoom;
	return pRoom;
}

bool CChallengeRoomManager::haveRoom(int uid)
{
	return m_mapRooms.find(uid) != m_mapRooms.end();
}

void CChallengeRoomManager::removeRoom(int uid)
{
	CChallengeRoom *pRoom = NULL;
	std::map<int, CChallengeRoom*>::iterator iter = m_mapRooms.find(uid);
	if (iter != m_mapRooms.end())
	{
		pRoom = iter->second;
		delete pRoom;
		m_mapRooms.erase(iter);
	}
}

void CChallengeRoomManager::PushDelRoom(int uid)
{
	if (m_TimeManager == NULL)
	{
		return;
	}

	std::map<int, CChallengeRoom*>::iterator iter = m_mapRooms.find(uid);
	if (iter == m_mapRooms.end())
	{
		return;
	}

	std::list<SDelayDelData>::iterator ator;
	SDelayDelData DelData;

	DelData.nDelayTime = (int)time(NULL) + DELAY_TIME_NUM;
	DelData.nUid = uid;
	ator = m_DelUserList.insert(m_DelUserList.end(), DelData);
	m_MapDelUserList[uid] = ator;
}

//去除延迟删除数据
void CChallengeRoomManager::ReSetDelRoom(int uid)
{
	std::map<int, std::list<SDelayDelData>::iterator>::iterator ator = m_MapDelUserList.find(uid);

	if (ator != m_MapDelUserList.end())
	{
		std::list<SDelayDelData>::iterator iter = ator->second;
		m_DelUserList.erase(iter);
		m_MapDelUserList.erase(ator);
	}
}
