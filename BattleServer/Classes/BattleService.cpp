#include "BattleService.h"
#include "Protocol.h"
#include "BattleProtocol.h"

#include "GameDef.h"
#include "Player.h"
#include "Room.h"
#include "RoomManager.h"
#include "KxLog.h"

#include "BufferData.h"
#include "BattleDataCache.h"
#include "KxCommManager.h"

void CBattleService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CPvpRoom *pRoom = dynamic_cast<CPvpRoom*>(CRoomManager::getInstance()->getRoomWithUid(uid));
    switch(subcmd)
    {
    case CMD_BAT_PVPCOMMANDCS:
		processBattleCommand(uid, pRoom, buffer, len, commun);
        break;

    case CMD_BAT_PVPUPDATECS:
		processBattleUpdate(uid, pRoom, buffer, len, commun);
        break;
    }
}

void CBattleService::processBattleCommand(int uid, CPvpRoom *room, char *buffer, int len, KxServer::IKxComm *commun)
{
    if (len != sizeof(BattleCommandInfo) || NULL == room)
    {
        return;
    }
    BattleCommandInfo * battleInfo = reinterpret_cast<BattleCommandInfo*>(buffer);
    // 重置为+5帧之后的值
    battleInfo->Tick = room->getCurrentTick() + 5;
    // 插入指令
    room->onProcessGame(CMD_BATTLE, CMD_BAT_PVPCOMMANDSC, buffer, len);
    room->sendRoom(CMD_BATTLE, CMD_BAT_PVPCOMMANDSC, buffer, len);
}

void CBattleService::processBattleUpdate(int uid, CPvpRoom *room, char *buff, int len, KxServer::IKxComm *commun)
{
    if (len != 0)
    {
        return;
    }

    if (NULL != room)
    {
        // 先执行游戏逻辑
        //room->ExecuteGame();
        // 封游戏逻辑包
        CBufferData bufferData;
        //10kb的容量
        bufferData.init(10240);
        room->getBattleData(bufferData);
        KXLOGINFO("player %d update battle data length %d", uid, bufferData.getDataLength());
        room->sendPlayer(uid, CMD_BATTLE, CMD_BAT_PVPUPDATESC, bufferData.getBuffer(), bufferData.getDataLength());
        //发送序列化游戏数据
        bufferData.clean();
    }
    else
    {
        PvpCacheData* cacheData = CBattleDataCache::getInstance()->getBattleData(uid);
        if (cacheData != NULL)
        {
            CKxCommManager::getInstance()->sendData(uid, CMD_PVP, CMD_PVP_RESULT_SC,
                cacheData->BufferData->getBuffer(), cacheData->BufferData->getDataLength(), commun);
        }
    }
}
