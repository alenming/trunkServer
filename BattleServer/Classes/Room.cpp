#include "Room.h"
#include "Player.h"
#include "BattleScene.h"
#include "Director.h"
#include "Protocol.h"
#include "GameDef.h"
#include "EventManager.h"
#include "BattleServer.h"
#include "CommStructs.h"
#include "BufferData.h"
#include "RoleComm.h"
#include "KxMemPool.h"
#include "RoomManager.h"
#include "BattleServer.h"
#include "CommonHelper.h"
#include "ModelHelper.h"
#include "PvpProtocol.h"
#include "GameUserManager.h"
#include "ActiveHelper.h"
#include "KxCommManager.h"
#include "PvpSettleResult.h"
#include "ConfGameSetting.h"
#include "ConfArena.h"
#include "Role.h"
#include "BattleDataCache.h"

using namespace KxServer;
using namespace std;

IRoom::IRoom() 
	: m_nRoomType(0)
{
}

IRoom::~IRoom() 
{
}

CPvpRoom::CPvpRoom(void)
    :m_bClose(false)
    ,m_nStartStamp(0)
    ,m_nCurStamp(0)
	,m_nPrevStamp(0)
	,m_nFPS(0)
    ,m_nStageId(0)
	,m_nBattleId(0)
    ,m_nRoomState(0)
    ,m_nRoomCapacity(2)
	, m_nRoomTick(0)
	,m_fPrepareFightTick(0.0f)
	,m_nWinnerId(0)
	,m_nLoserId(0)
	,m_nChallengeResult(0)
    ,m_pDirector(NULL)
	,m_pBattleScene(NULL)
    ,m_pMaster(NULL)
	,m_pTimerManager(NULL)
    ,m_pEventManager(NULL)
{
}

CPvpRoom::~CPvpRoom(void)
{
}

bool CPvpRoom::initRoom(int battleId, int stageId, int pvpType)
{
	//初始化房间状态为加载前
	m_nRoomState = ROOMSTATE_PREPARELOAD;
    // 初始化状态 只初始化变量和参数
	m_nRoomType = pvpType;
	m_nBattleId = battleId;
    m_nStageId = stageId;
	m_nRoomCapacity = 2;
	m_nFPS = 10;
	//5秒战斗预备
	m_fPrepareFightTick = 5.0f;
    // 初始化事件管理
    m_pEventManager = new CEventManager<int>;
	// 计时器
	m_pTimerManager = CBattleServer::getInstance()->getTimerManager();
    // 初始化director
    m_pDirector = new Director;
    m_pDirector->init();
	// 前置销毁计时, 每次1s
	m_nRoomTick = PVPROOM_PREPARE_EXPIRE;
	m_pTimerManager->addTimer(this, 1, KXREPEAT_FOREVER);
    return true;
}

void CPvpRoom::loadingRoom()
{
	if (m_nRoomState == ROOMSTATE_FIGHTING)
	{
		KXLOGERROR("Start PVP Room but m_nRoomState == ROOMSTATE_FIGHTING !!!");
		return;
	}
	m_nRoomState = ROOMSTATE_LOADING;
	//根据玩家段位获得关卡id
	setStageId();
	//最长loading时间计时
	m_nRoomTick = PVPROOM_LOADING_EXPIRE;
}

bool CPvpRoom::startRoom()
{
    if (m_nRoomState >= ROOMSTATE_IDLETIME)
	{
        KXLOGERROR("start pvp twice pvp battleId=%d!!!", m_nBattleId);
		return false;
	}

	//开始游戏, 进入预备状态
	m_nRoomState = ROOMSTATE_IDLETIME;
    // 创建场景, 不能在initroom中, 因为这里需要完整的房间信息
    m_pBattleScene = new CBattleLayer();
	if (!m_pBattleScene->init(this, m_pEventManager))
    {
		KXLOGERROR("Start PVP Room  m_pBattleScene init error !!!");
        return false;
    }

    //所有时间戳设置初始值
	m_nPrevStamp = m_nCurStamp = m_nStartStamp = m_pTimerManager->getTimestamp();
    m_LastTime = m_pTimerManager->getNow();
    //执行战斗逻辑
    m_pDirector->runWithScene(m_pBattleScene);

	// 要求精确时间，每隔0秒 + 0.1秒执行一次
	stop();
	setDelay(0, MILLION / m_nFPS);
	setRepeat(KXREPEAT_FOREVER);
	m_pTimerManager->addTimer(this);

	// 通知玩家游戏开始
	PvpStartSC startSC;
	startSC.startStamp = static_cast<int>(time(NULL));
	sendRoom(CMD_PVP, CMD_PVP_IDLETIME_SC,
		reinterpret_cast<char*>(&startSC), sizeof(startSC));
    return true;
}

void CPvpRoom::fightingRoom()
{
	m_nRoomState = ROOMSTATE_FIGHTING;
}

void CPvpRoom::exitRoom()
{
    if (NULL != m_pDirector)
    {
        m_pDirector->end();
        delete m_pDirector;
        m_pDirector = NULL;
    }

    if (NULL != m_pEventManager)
    {
        delete m_pEventManager;
        m_pEventManager = NULL;
    }

	for (map<int, KxServer::IKxComm *>::iterator ator = m_PlayerCommu.begin(); 
		ator != m_PlayerCommu.end(); ++ator)
	{
		CKxCommManager::getInstance()->delKxComm(ator->first);
	}

	m_PlayerCommu.clear();
	//停止计时器
	stop();
	//房间管理器清理房间
	CRoomManager::getInstance()->removeWithBattleId(m_nBattleId);
}

void CPvpRoom::onTimer(const kxTimeVal& now)
{
	if (m_nRoomState != ROOMSTATE_IDLETIME && m_nRoomState != ROOMSTATE_FIGHTING)
	{
		m_nRoomTick -= 1;
		if (m_nRoomTick != 0)
		{
			return;
		}
		// 房间一直等待2位玩家加入
		if (m_nRoomState == ROOMSTATE_PREPARELOAD)
		{
			KXLOGDEBUG("PVP Room prepare loading time over, send user rematch!! user count %d", m_Players.size());
			//发送重新匹配
			sendRoom(CMD_MATCH, CMD_MATCH_REMATCH_SC, NULL, 0);
			//清理房间
			exitRoom();
		}
		// 如果加载事件多于指定时间, 强行开始游戏(防止玩家一直不断线, 而一直等待玩家加入)
		else if (m_nRoomState == ROOMSTATE_LOADING)
		{
			// 如果双方都断线, 移除房间
			if (isAllDisconnect())
			{
				//清理房间
				exitRoom();
			}
			else
			{
				startRoom();
			}
		}
	}
	else
	{
		// 计算出偏差时间
		kxTimeVal tv = now - m_LastTime;
		m_LastTime = now;
		float dt = tv.getfloat();
		// 战斗预备
		if (m_nRoomState == ROOMSTATE_IDLETIME)
		{
			m_fPrepareFightTick -= dt;
			if (m_fPrepareFightTick <= 0.0f)
			{
				//进入战斗状态
				fightingRoom();
				//通知前端正式开始游戏
				sendRoom(CMD_PVP, CMD_PVP_START_SC, NULL, 0);
			}
		}
		// 正常游戏
		else if (m_nRoomState == ROOMSTATE_FIGHTING)
		{
			executeGame(dt);
		}
	}
}

void CPvpRoom::executeGame(float dt)
{
    //LOG("game tick %d", m_pBattleScene->getBattleHelper()->GameTick);
	if (!m_pBattleScene->getBattleHelper()->IsBattleOver)
    {
        // 执行至第n帧, 内部进行帧数计算
		m_pDirector->loopDelta(dt);
		// 设置上一个时间戳
		m_nPrevStamp = m_nCurStamp;
    }
	else
	{
		//发送玩家消息, 处理玩家个人信息
		overGame();
		//清理房间
		exitRoom();
	}
}

void CPvpRoom::onProcessGame(int cmd, int subcmd, void *data, int len)
{
    // 上层偏移了头部，这里需要完整的头，所以偏回来
	int respLen = len + sizeof(Head);
    char *respData = reinterpret_cast<char *>(data) - sizeof(Head);
    Head* head = reinterpret_cast<Head*>(respData);
    head->cmd = MakeCommand(cmd, subcmd);
    if (NULL != m_pBattleScene)
    {
		m_pBattleScene->onResponse(respData, respLen);
    }
}

void CPvpRoom::overGame()
{
    if ( m_nWinnerId == 0
        || m_nLoserId == 0
        || m_nChallengeResult == CHALLENGE_CANCEL)
    {
        return;
    }

    CBattleHelper *pBattleHepler = m_pBattleScene->getBattleHelper();
    if (pBattleHepler == NULL)
    {
        return;
    }

    CPlayer* winner = getRoomPlayer(m_nWinnerId);
    CPlayer* loser = getRoomPlayer(m_nLoserId);
    if (winner == NULL || loser == NULL)
    {
        return;
    }

    int nWinCrycalLv = 0;
    int nLoseCryCalLv = 0;
    int nTime = 0;

    CRole *pRole = pBattleHepler->getMainRole(pBattleHepler->getCampWithUid(m_nWinnerId));
    nWinCrycalLv = pRole->getIntAttribute(EHeroCrystalLevel);
    pRole = pBattleHepler->getMainRole(pBattleHepler->getCampWithUid(m_nLoserId));
    nLoseCryCalLv = pRole->getIntAttribute(EHeroCrystalLevel);
    nTime = pBattleHepler->GameTick / 10;
	// pvp setting
	const ArenaSettingItem *pArenaConf = queryArenaSetting();

    //定级赛数值与分差
    int nWinnerGNum = 0, nWinnerGDval = 0;
    //各自的MMR
    int nWinnerMMR = winner->getMMR();
    int nLoserMMR = loser->getMMR();
    //积分
    int nWinnerIntegral = winner->getIntegral();
    int nLoserIntegral = loser->getIntegral();
    //变化系数(连胜次数)
    int nKW = winner->getCoutinueWin();
    int nKL = loser->getCoutinueWin();

    //计算MMR
    int nWChangeMMR = 0;
    int nLChangeMMR = 0;
    int nWChangeIntegral = 0;
    int nLChangeIntegral = 0;
    if (m_nChallengeResult == CHALLENGE_DRAW)
    {
        nWChangeMMR = CPvpSettleResult::calcMMR(nWinnerMMR, nLoserMMR, nKW, -1);
        nLChangeMMR = CPvpSettleResult::calcMMR(nLoserMMR, nWinnerMMR, nKL, -1);
    }
    else
    {
        //计算MMR
        nWChangeMMR = CPvpSettleResult::calcMMR(nWinnerMMR, nLoserMMR, nKW, 1);
        nLChangeMMR = CPvpSettleResult::calcMMR(nLoserMMR, nWinnerMMR, nKL, 0);
        //计算积分
        nWChangeIntegral = CPvpSettleResult::calcIntegral(nWinnerIntegral, nLoserIntegral, 1);
        //积分大于一定积分才扣除
		if (nLoserIntegral > pArenaConf->CriticalPoint)
        {
            nLChangeIntegral = CPvpSettleResult::calcIntegral(nLoserIntegral, nWinnerIntegral, 0);
        }
        //胜利者战斗结束现有多少积分
        nWinnerIntegral += nWChangeIntegral;
        // 失败者结算后有多少积分
        nLoserIntegral += nLChangeIntegral;
    }

    processPlayer(m_nChallengeResult == CHALLENGE_DRAW ? CHALLENGE_DRAW : CHALLENGE_WON,
		winner, loser->getMainRoleId(), nWChangeMMR, nWChangeIntegral, nWinnerIntegral, nWinCrycalLv, nTime);
    processPlayer(m_nChallengeResult == CHALLENGE_DRAW ? CHALLENGE_DRAW : CHALLENGE_FAILED,
        loser,winner->getMainRoleId(), nLChangeMMR, nLChangeIntegral, nLoserIntegral, nLoseCryCalLv, nTime);
}

void CPvpRoom::processPlayer(int result, CPlayer* player, int nEnemySummerID, int mmr,
    int integral, int newintegral, int crylv, int dt)
{
	int nMySummonerID = player->getMainRoleId();
	std::vector<CSoldierModel*> vectHero = player->getSoldierCards();

    CBufferData* bufferData = new CBufferData();
    bufferData->init(256);
    bufferData->writeData(m_nRoomType);
    bufferData->writeData(result);
    bufferData->writeData(newintegral);
    bufferData->writeData(integral);

    // 转发到Service服务器，进行结算后的处理
    CBufferData finishSSBuffer;
    finishSSBuffer.init(256);
    finishSSBuffer.writeData(m_nRoomType);  // 房间类型
    finishSSBuffer.writeData(result);       // 结果
    finishSSBuffer.writeData(newintegral);  // 计算后的积分
    finishSSBuffer.writeData(integral);     // 改变的积分
    finishSSBuffer.writeData(mmr);          // 改变的MMR
    finishSSBuffer.writeData(crylv);        // 水晶等级
    finishSSBuffer.writeData(dt);           // 耗时（秒）
	finishSSBuffer.writeData(nMySummonerID);  // 召唤师ID
	for (unsigned int i = 0; i < MAX_HERO_COUNT; i++)
	{
		if (i < vectHero.size())
		{
			finishSSBuffer.writeData(vectHero[i]->getSoldId());
		}
		else
		{
			finishSSBuffer.writeData((int)0);
		}
	}
	finishSSBuffer.writeData(nEnemySummerID);

    if (CHALLENGE_WON == result)
    {
        // 胜利掉落
        std::vector<DropItemInfo> vecDropItems;
        int winnerDropId = CCommonHelper::getArenaDrop(player->getUserLv(), player->getDayWinTimes());
        if (winnerDropId > 0)
        {
            CItemDrop::Drop(winnerDropId, vecDropItems);
        }

        bufferData->writeData(static_cast<int>(vecDropItems.size()));
        finishSSBuffer.writeData(static_cast<int>(vecDropItems.size()));
        for (std::vector<DropItemInfo>::iterator iter = vecDropItems.begin();
            iter != vecDropItems.end(); ++iter)
        {
            bufferData->writeData(*iter);
            finishSSBuffer.writeData(*iter);
        }
    }
    else
    {
        bufferData->writeData(0);
        finishSSBuffer.writeData(0);
    }

    // 缓存结算数据，确保由于断线丢失了该数据的玩家可以得到结算界面，CBattleDataCache会自动清理 delete
    CBattleDataCache::getInstance()->pushBackBattleData(player->getUserId(), bufferData);
    // 告诉客户端结果
    CKxCommManager::getInstance()->sendData(player->getUserId(),
        CMD_PVP, CMD_PVP_RESULT_SC, bufferData->getBuffer(), bufferData->getDataLength());
    // 告诉Service结果，统计、更新排行榜、更新数据等等
    CKxCommManager::getInstance()->forwardData(player->getUserId(),
        CMD_PVP, CMD_PVP_FINISH_SS, finishSSBuffer.getBuffer(), finishSSBuffer.getDataLength());
}

int CPvpRoom::getRoomState()
{
    return m_nRoomState;
}

bool CPvpRoom::isFull()
{
    return static_cast<int>(m_Players.size()) >= m_nRoomCapacity;
}

bool CPvpRoom::isAllReady()
{
	if ((int)m_PlayerProgress.size() < m_nRoomCapacity)
	{
		return false;
	}

    for (std::map<int, int>::iterator iter = m_PlayerProgress.begin(); 
        iter != m_PlayerProgress.end(); ++iter)
    {
        if (iter->second < 100)
        {
            return false;
        }
    }

    return true;
}

void CPvpRoom::playerLoading(int uid, int progress)
{
    if (m_PlayerProgress.empty())
    {
        for (std::map<int, CPlayer*>::iterator iter = m_Players.begin(); 
            iter != m_Players.end(); ++iter)
        {
            m_PlayerProgress[iter->first] = 0;
        }
    }

    m_PlayerProgress[uid] = progress;
}

int CPvpRoom::getPlayerProgress(int uid)
{
	if (m_PlayerProgress.find(uid) != m_PlayerProgress.end())
	{
		return m_PlayerProgress[uid];
	}
	return 0;
}

void CPvpRoom::playerReady(int uid)
{
	KXLOGDEBUG("PVP Room user %d is ready!!!", uid);
	// 加载完成
	playerLoading(uid, 100);

	PvpReadySC pvpReadySC;
	pvpReadySC.uid = uid;
	sendRoom(CMD_PVP, CMD_PVP_READY_SC, reinterpret_cast<char*>(&pvpReadySC), sizeof(pvpReadySC));
    // 房间还未开始
    if (m_nRoomState <= ROOMSTATE_LOADING)
    {
        if (isAllReady())
        {
            //开始游戏
            startRoom();
        }
        else
        {
            //如果对手断线, 等待n秒进入游戏
            if (isPlayerDisconnect(getOpponentUid(uid)))
            {
                m_nRoomTick = 10;
            }
        }
    }
    else if (m_nRoomState == ROOMSTATE_IDLETIME)
    {
        // 如果收到Ready时房间处于准备开始状态，要让玩家切换到战斗场景
        PvpStartSC startSC;
        startSC.startStamp = m_nStartStamp;
        CKxCommManager::getInstance()->sendData(uid, CMD_PVP, CMD_PVP_IDLETIME_SC,
            reinterpret_cast<char*>(&startSC), sizeof(startSC));
    }
    else
    {
        // 如果收到Ready时房间处于战斗状态，要让玩家刷新数据，并切换到战斗场景
        PvpReconnectSC reconnectSC;
        reconnectSC.roomState = m_nRoomState;
        reconnectSC.roomType = getRoomType();
        CKxCommManager::getInstance()->sendData(uid, CMD_PVP, CMD_PVP_RECONNECT_SC,
            reinterpret_cast<char *>(&reconnectSC), sizeof (reconnectSC));

        PvpStartSC startSC;
        startSC.startStamp = m_nStartStamp;
        CKxCommManager::getInstance()->sendData(uid, CMD_PVP, CMD_PVP_IDLETIME_SC,
            reinterpret_cast<char*>(&startSC), sizeof(startSC));
    }
}

void CPvpRoom::playerReconnect(int uid)
{
	CPlayer *pPlayer = getRoomPlayer(uid);
	if (NULL == pPlayer)
	{
		KXLOGERROR("PVP room user %d is NULL!", uid);
		return;
	}
	pPlayer->setIsConnecting(true);

	int opponentUid = getOpponentUid(uid);
	sendPlayer(opponentUid, CMD_PVP, CMD_PVP_OPPRECONNECT_SC, NULL, 0);
}

void CPvpRoom::playerDisconnect(int uid)
{
	CPlayer *pPlayer = getRoomPlayer(uid);
	if (NULL == pPlayer)
	{
		KXLOGERROR("playerDisconnect PVP room user %d is NULL!", uid);
		return;
	}
	pPlayer->setIsConnecting(false);
	KXLOGINFO("PVP room user %d is disconnect!");

    // 加载中断线不理会, 会等待玩家重连
    // 	if (m_nRoomState == ROOMSTATE_LOADING)
    // 	{
    // 		PvpLoadingSC loadingSC;
    // 		loadingSC.uid = uid;
    // 		loadingSC.progress = 100;
    // 
    // 		sendRoom(CMD_PVP, CMD_PVP_LOADING_SC,
    // 			reinterpret_cast<char*>(&loadingSC), sizeof(loadingSC));
    // 		// 断线玩家准备就绪
    // 		playerReady(uid);
    // 	}

	// 通知对手玩家断线
	int opponentUid = getOpponentUid(uid);
	sendPlayer(opponentUid, CMD_PVP, CMD_PVP_OPPDISCONNECT_SC, NULL, 0);
}

int CPvpRoom::getOpponentUid(int uid)
{
	for (std::map<int, CPlayer*>::iterator iter = m_Players.begin();
		iter != m_Players.end(); ++iter)
	{
		if (iter->first != uid)
		{
			return iter->first;
		}
	}
	return 0;
}

void CPvpRoom::setRoomTick(int tick)
{
	m_nRoomTick = tick;
}

int CPvpRoom::setStageId()
{
	m_nStageId = 1;
	if (static_cast<int>(m_Players.size()) >= m_nRoomCapacity)
	{
		int finalDan = 0;
		for (std::map<int, CPlayer*>::iterator iter = m_Players.begin(); 
			iter != m_Players.end(); ++iter)
		{
            // 获取玩家MMR对应的段位
            int dan = CCommonHelper::getUserMMRDan(iter->second->getMMR());
			if (dan > finalDan)
			{
				finalDan = dan;
			}
		}

		m_nStageId = qureyRandomPvpStage(finalDan);
	}
	return m_nStageId;
}

int CPvpRoom::getStageId()
{
    return m_nStageId;
}

int CPvpRoom::getBattleId()
{
	return m_nBattleId;
}

int CPvpRoom::getCurrentTick()
{
    if (m_pBattleScene != NULL)
    {
        return m_pBattleScene->getBattleHelper()->GameTick;
    }
    return 0;
}

int CPvpRoom::getRoomTick()
{
	return m_nRoomTick;
}

float CPvpRoom::getPrepareTick()
{
	return m_fPrepareFightTick;
}

CPlayer *CPvpRoom::getRoomPlayer(int uid)
{
    std::map<int, CPlayer*>::iterator iter = m_Players.find(uid);
    if (iter != m_Players.end())
    {
        return iter->second;
    }

    return NULL;
}

CPlayer *CPvpRoom::getMasterPlayer()
{
    return m_pMaster;
}

CPlayer *CPvpRoom::getOtherPlayer()
{
    return m_pOther;
}

void CPvpRoom::getRoomData(CBufferData& bufferData)
{
    RoomData roomData;
    roomData.BattleType = EBATTLE_PVP;
	roomData.StageLevel = 1;
	roomData.StageId = m_nStageId;
    roomData.PlayerCount = m_Players.size();
	roomData.Ext1 = 0;
	roomData.Ext2 = 0;
	roomData.BuffCount = 0;
    bufferData.writeData(&roomData, sizeof(roomData));

    for (std::map<int, CPlayer*>::iterator iter = m_Players.begin(); 
        iter != m_Players.end(); ++iter)
    {
        iter->second->fillBufferData(bufferData);
    }
}

void CPvpRoom::getBattleData(CBufferData &bufferData)
{
    if (NULL != m_pBattleScene)
    {
        m_pBattleScene->getBattleHelper()->serialize(bufferData);
    }
}

std::map<int, CPlayer*> &CPvpRoom::getRoomPlayers()
{
    return m_Players;
}

bool CPvpRoom::isRoomPlayer(int uid)
{
    return m_Players.find(uid) != m_Players.end(); 
}

bool CPvpRoom::isAllDisconnect()
{
	for (std::map<int, CPlayer*>::iterator iter = m_Players.begin();
		iter != m_Players.end(); ++iter)
	{
		if (iter->second->isConnecting())
		{
			return false;
		}
	}
	return true;
}

bool CPvpRoom::isPlayerDisconnect(int uid)
{
	std::map<int, CPlayer*>::iterator iter = m_Players.find(uid);
	return iter == m_Players.end() || !iter->second->isConnecting();
}

bool CPvpRoom::playerJoin(int uid, char* playerData, int len)
{
    if (static_cast<int>(m_Players.size()) >= m_nRoomCapacity
        || m_Players.find(uid) != m_Players.end())
    {
        KXLOGERROR("PVP room user %d! already in room!", uid);
        return false;
    }

    CPlayer *pPlayer = new CPlayer();
    if (!pPlayer->initWithData(playerData, len))
    {
        KXLOGERROR("PVP room user %d! initWithData faile!", uid);
        delete pPlayer;
        return false;
    }

    m_Players[uid] = pPlayer;
    if (m_Players.size() == 1)
    {
        m_pMaster = pPlayer;
        pPlayer->setCamp(ECamp_Blue);
    }
    else
    {
        m_pOther = pPlayer;
        pPlayer->setCamp(ECamp_Red);
    }
    return true;
}

bool CPvpRoom::playerQuit(int uid)
{
	KXLOGDEBUG("PVP room uid=%d quit room!", uid);
    map<int, CPlayer*>::iterator iter = m_Players.find(uid);
    if (iter != m_Players.end())
    {
        CPlayer *pPlayer = iter->second;
        if (NULL != pPlayer)
        {
            delete pPlayer;
        }
        m_Players.erase(iter);
    }

	CKxCommManager::getInstance()->delKxComm(uid);
    return true;
}

void CPvpRoom::setWinnerId(int uid)
{
	m_nWinnerId = uid;
}

void CPvpRoom::setLoserId(int uid)
{
	m_nLoserId = uid;
}

void CPvpRoom::setPlayerCommunication(int uid, KxServer::IKxComm *target)
{
	m_PlayerCommu[uid] = target;
	CKxCommManager::getInstance()->addKxComm(uid, target);
}

int CPvpRoom::sendRoom(int maincmd, int subcmd, char *data, unsigned int len)
{
	for (std::map<int, KxServer::IKxComm *>::iterator iter = m_PlayerCommu.begin();
		iter != m_PlayerCommu.end(); ++iter)
	{
        CKxCommManager::getInstance()->sendData(
            iter->first, maincmd, subcmd, data, len);
	}
	return 0;
}

int CPvpRoom::sendPlayer(int uid, int maincmd, int subcmd, char *data, unsigned int len)
{
	if (m_PlayerCommu.find(uid) == m_PlayerCommu.end())
    {
        //不允许向不同房间的人发送消息(聊天或者其它战斗无关消息除外)
        return 0;
    }

	return CKxCommManager::getInstance()->sendData(uid,maincmd, subcmd, data,len);
}
