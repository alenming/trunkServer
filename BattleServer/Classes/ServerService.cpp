#include "ServerService.h"
#include "ServerProtocol.h"
#include "Room.h"
#include "RoomManager.h"
#include "KxCommManager.h"


void CServerService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
	case ServerProtocol::ServerSubPlayerOffLine:
		processPlayerOffLine(uid, buffer, len, commun);
		break;

	default:
		break;
	}
}

void CServerService::processPlayerOffLine(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	IRoom *pRoom = CRoomManager::getInstance()->getRoomWithUid(uid);
	if (NULL != pRoom)
    {
        KXLOGDEBUG("processPlayerOffLine %d room Type %d", uid, pRoom->getRoomType());
		if (pRoom->getRoomType() == PVPROOMTYPE_PVP)
		{
			//∂œœﬂ¥¶¿Ì
			dynamic_cast<CPvpRoom*>(pRoom)->playerDisconnect(uid);
		}
	}
	CKxCommManager::getInstance()->delKxComm(uid);
}
