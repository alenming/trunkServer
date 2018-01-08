#include "ServerService.h"
#include "ServerProtocol.h"
#include "GameUserManager.h"

#include "NewMatchManager.h"

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
	CNewMatchManager::getInstance()->removeFromMatchList(uid);
	CNewMatchManager::getInstance()->removeFromRobotList(uid);
	CGameUserManager::getInstance()->removeGameUser(uid);
}
