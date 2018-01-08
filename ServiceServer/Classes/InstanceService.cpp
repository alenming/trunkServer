#include "InstanceService.h"
#include "ChallengeRoomManager.h"
#include "ModelHelper.h"
#include "GameDef.h"
#include "ConfHall.h"
#include "ConfGameSetting.h"
#include "GameUserManager.h"
#include "CommStructs.h"
#include "InstanceProtocol.h"
#include "StageProtocol.h"
#include "Protocol.h"
#include "KxCommManager.h"


void CInstanceService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
	case CMD_INSTANCE_CHALLENGE_CS:
		ProcessChanllege(uid, buffer, len, commun);
		break;
	case CMD_INSTANCE_FINISH_CS:
		ProcessFinish(uid, buffer, len, commun);
		break;
	case CMD_INSTANCE_BUYTIMES_CS:
		ProcessBuyTimes(uid, buffer, len, commun);
		break;
	default:
		break;
	}
}

void CInstanceService::ProcessChanllege(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{

}

void CInstanceService::ProcessFinish(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{

}

void CInstanceService::ProcessBuyTimes(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{

}
