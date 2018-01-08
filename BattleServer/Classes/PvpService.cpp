#include "PvpService.h"
#include "Protocol.h"
#include "PvpProtocol.h"
#include "ErrorCodeProtocol.h"
#include "KxCore.h"
#include "KxMemPool.h"
#include "GameDef.h"
#include "Room.h"
#include "RoomManager.h"
#include "Player.h"
#include "CommStructs.h"
#include "ModelHelper.h"
#include "ActiveHelper.h"
#include "KxCommManager.h"
#include "CommOssHelper.h"
#include "BattleDataCache.h"
#include "BattleServer.h"
#include "ConfStage.h"

#include "PvpRankModel.h"

void CPvpService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    switch(subcmd)
    {
    case CMD_PVP_ENTERROOM_SS:
        processEnterRoomSS(uid, buffer, len, commun);
        break;

	case CMD_PVP_ENTERROOM_CS:
		//processEnterRoom(uid, buffer, len, commun);
		break;

    case CMD_PVP_LOADING_CS:
        processLoading(uid, buffer, len, commun);
        break;

    case CMD_PVP_READY_CS:
        processReady(uid, buffer, len, commun);
        break;

	case CMD_PVP_RECONNECT_CS:
		processReconnect(uid, buffer, len, commun);
		break;

	case CMD_PVP_RECONNECTROOMDATA_CS:
		processReconnectRoomData(uid, buffer, len, commun);
		break;
	}
}

void sendOpponentData(CPvpRoom * room, int uid)
{
	// 严谨的写不应该直接认为有2个玩家, 只是游戏确实只有2个玩家.
    std::map<int, CPlayer*> &players = room->getRoomPlayers();
    CPlayer * pPlayer1 = players[uid];
	CPlayer * pPlayer2 = NULL;
	for (std::map<int, CPlayer*>::iterator iter = players.begin();
		iter != players.end(); ++iter)
	{
        if (iter->second != pPlayer1)
		{
			pPlayer2 = iter->second;
			break;
		}
	}

	CHECK_RETURN_VOID(pPlayer1 != NULL);
	CHECK_RETURN_VOID(pPlayer2 != NULL);
	
	//先告诉玩家匹配到的对手信息
	//告知玩家1, 玩家2的信息
	PvpEnterRoomSC opponentInfo;
	memset(&opponentInfo, 0, sizeof(opponentInfo));
	
    // 匹配结果
	opponentInfo.result = 1;
	// 自己的阵营
    opponentInfo.camp = pPlayer1->getCamp();
	// 对手的信息
    opponentInfo.progress = room->getPlayerProgress(pPlayer2->getUserId());
    opponentInfo.uid = pPlayer2->getUserId();
    opponentInfo.summonerId = pPlayer2->getMainRoleId();
    opponentInfo.integral = pPlayer2->getIntegral();
    opponentInfo.usrLv = pPlayer2->getUserLv();
    opponentInfo.extend = pPlayer2->getIdentity();
	strncpys(opponentInfo.usrName, pPlayer2->getUserName().c_str(), sizeof(opponentInfo.usrName));

    // 通知指定玩家
    room->sendPlayer(uid, CMD_PVP, CMD_PVP_ENTERROOM_SC,
		reinterpret_cast<char*>(&opponentInfo), sizeof(opponentInfo));
}

void CPvpService::processEnterRoomSS(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    // 服务器转发过来的，咱信任你，可别坑我啊
    PvpEnterRoomSS* enterRoomSS = reinterpret_cast<PvpEnterRoomSS*>(buffer);
    // 哎，我还是太严谨了，要判断一下
    if (sizeof(PvpEnterRoomSS) >= len)
    {
        return;
    }
    
    CPvpRoom *pRoom = CRoomManager::getInstance()->enterRoom(uid, enterRoomSS->battleId, enterRoomSS->roomType);
    if (NULL != pRoom)
    {
        //设置session连接
        pRoom->setPlayerCommunication(uid, commun);
        if (!pRoom->playerJoin(uid, buffer, len))
        {
            KXLOGERROR("PVP room user %d! join false, maybe have joined!", uid);
            // 加入失败
            return;
        }

        // KXLOGDEBUG("PVP room user %d, enter room success", uid);
        if (pRoom->isFull())
        {
            // 进入加载状态
            pRoom->loadingRoom();
            //满员,通知前端开始加载,先下方房间信息
            CBufferData bufferData;
            bufferData.init(10240);
            pRoom->getRoomData(bufferData);
            pRoom->sendRoom(CMD_PVP, CMD_PVP_PREPARE_SC, bufferData.getBuffer(), bufferData.getDataLength());
            // 发送对手信息
            std::map<int, CPlayer*>& players = pRoom->getRoomPlayers();
            for (std::map<int, CPlayer*>::iterator iter = players.begin();
                iter != players.end(); ++iter)
            {
                sendOpponentData(pRoom, iter->first);
                // KXLOGDEBUG("PVP room full, notify user %d prepare ready go!", iter->first);
            }
        }
    }
}

void CPvpService::processEnterRoom(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	/* PvpEnterRoomCS *pEnterRoomCS = reinterpret_cast<PvpEnterRoomCS *>(buffer);
	//进入房间时先刷新数据
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	// 此时刷新一下玩家pvp数据 
	pGameUser->refreshModel(MODELTYPE_PVP);

	int battleId = pEnterRoomCS->battleId;
	int roomType = pEnterRoomCS->roomType;
	int summonerId = pEnterRoomCS->summonerId;
	// 房间类型不对
	if (roomType != PVPROOMTYPE_PVP && roomType != PVPROOMTYPE_CHAMPIONSHIP)
	{
		KXLOGERROR("ProcessEnterRoom uid %d root type illegal!", uid);
		return;
	}

	std::vector<int> heroIds;
	int *pHeroId = reinterpret_cast<int *>(pEnterRoomCS + 1);
	for (int i = 0; i < pEnterRoomCS->heroCount; ++i)
	{
		heroIds.push_back(*pHeroId);
		pHeroId += 1;
	}

	if (!CModelHelper::haveSummoner(uid, summonerId))
	{
		// 召唤师未拥有
		KXLOGERROR("ProcessEnterRoom uid %d summoner id not exist!", uid);
		return;
	}

	if (!CModelHelper::checkHeros(uid, heroIds))
	{
		// 英雄数据不合法
		KXLOGERROR("ProcessEnterRoom uid %d some hero id not exist!", uid);
		return;
	}

	CPvpRoom *pRoom = CRoomManager::getInstance()->enterRoom(uid, battleId, roomType);
	if (NULL != pRoom)
	{
		//设置session连接
		pRoom->setPlayerCommunication(uid, commun);
		if (!pRoom->playerJoin(uid, summonerId, heroIds))
		{
			KXLOGERROR("PVP room user %d! join false, maybe have joined!", uid);
			// 加入失败
			return;
		}
		KXLOGDEBUG("PVP room user %d, enter room success", uid);
		if (pRoom->isFull())
		{
			// 进入加载状态
			pRoom->loadingRoom();
			//满员,通知前端开始加载,先下方房间信息
            int roomDataLen = 0;
            int alocateLen = 10240;
            char *pRoomData = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(alocateLen));
			pRoom->getRoomData(pRoomData, roomDataLen);
			pRoom->sendRoom(CMD_PVP, CMD_PVP_PREPARE_SC, pRoomData, roomDataLen);
            KxServer::kxMemMgrRecycle(pRoomData, alocateLen);
			// 发送对手信息
			std::map<int, CPlayer*>& players = pRoom->getRoomPlayers();
			for (std::map<int, CPlayer*>::iterator iter = players.begin();
				iter != players.end(); ++iter)
			{
				sendOpponentData(pRoom, iter->first);
				KXLOGDEBUG("PVP room full, notify user %d prepare ready go!", iter->first);
			}
		}
	}

	CCommOssHelper::ServerStatusOss();*/
}

void CPvpService::processLoading(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    PvpLoadingCS *pLoadingCS = reinterpret_cast<PvpLoadingCS*>(buffer);
    CRoomManager *pRoomMgr = CRoomManager::getInstance();
	CPvpRoom *pRoom = dynamic_cast<CPvpRoom*>(pRoomMgr->getRoomWithUid(uid));
	if (NULL != pRoom)
	{
		// KXLOGDEBUG("PVP user=%d loading progress=%d", uid, pLoadingCS->progress);
		pRoom->playerLoading(uid, pLoadingCS->progress);

		PvpLoadingSC loadingSC;
		loadingSC.uid = uid;
		loadingSC.progress = pLoadingCS->progress;

		pRoom->sendRoom(CMD_PVP, CMD_PVP_LOADING_SC,
			reinterpret_cast<char*>(&loadingSC), sizeof(loadingSC));
	}
}

void CPvpService::processReady(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CRoomManager *pRoomMgr = CRoomManager::getInstance();
	CPvpRoom *pRoom = dynamic_cast<CPvpRoom*>(pRoomMgr->getRoomWithUid(uid));
    if (NULL != pRoom)
    {
		// KXLOGDEBUG("PVP user=%d was ready!", uid);
        pRoom->playerReady(uid);
    }
    else
    {
        // 玩家加载完成后房间已经销毁，让玩家进入战斗场景，并弹出结算界面
        PvpStartSC startSC;
        startSC.startStamp = CBattleServer::getInstance()->getTimerManager()->getTimestamp();
        CKxCommManager::getInstance()->sendData(uid, CMD_PVP, CMD_PVP_IDLETIME_SC,
            reinterpret_cast<char*>(&startSC), sizeof(startSC), commun);

        // 告知结算消息
        PvpCacheData* cacheData = CBattleDataCache::getInstance()->getBattleData(uid);
        if (cacheData != NULL)
        {
            CKxCommManager::getInstance()->sendData(uid, CMD_PVP, CMD_PVP_RESULT_SC,
                cacheData->BufferData->getBuffer(), cacheData->BufferData->getDataLength());
        }
    }
}

void CPvpService::processReconnect(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CRoomManager *pRoomMgr = CRoomManager::getInstance();
	CPvpRoom *pRoom = dynamic_cast<CPvpRoom*>(pRoomMgr->getRoomWithUid(uid));
	PvpReconnectSC reconnectSC;
	if (NULL != pRoom)
	{
		KXLOGDEBUG("PVP user=%d reconnect!", uid);
		reconnectSC.roomState = pRoom->getRoomState();
		reconnectSC.roomType = pRoom->getRoomType();
		pRoom->playerReconnect(uid);
		//更新Session连接
		pRoom->setPlayerCommunication(uid, commun);
	}
	else
	{
		//容错, 如果服务器崩溃而导致的玩家pvpbattleid不为0, 清除玩家battleId
		//KXLOGDEBUG("Notice: PVP user %d reconnect, but room was destroyed! now delete store battle id key", uid);
		//CModelHelper::deletePvpBattleKey(uid);
		reconnectSC.roomState = ROOMSTATE_NOROOM;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_PVP, CMD_PVP_RECONNECT_SC,
		reinterpret_cast<char *>(&reconnectSC), sizeof (reconnectSC));
}

void CPvpService::processReconnectRoomData(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	KXLOGERROR("PVP service user %d request serialize data!", uid);
	CRoomManager *pRoomMgr = CRoomManager::getInstance();
	CPvpRoom *pRoom = dynamic_cast<CPvpRoom*>(pRoomMgr->getRoomWithUid(uid));
    if (NULL != pRoom)
    {
        //KXLOGDEBUG("PVP user %d reconnect send opponent data!", uid);
        //另发送对手信息
        sendOpponentData(pRoom, uid);
        CBufferData bufferData;
        bufferData.init(10240);
        pRoom->getRoomData(bufferData);
        pRoom->sendRoom(CMD_PVP, CMD_PVP_RECONNECTROOMDATA_SC,
            bufferData.getBuffer(), bufferData.getDataLength());
    }
}
