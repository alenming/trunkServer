#include "TeamService.h"
#include "TeamProtocol.h"
#include "Protocol.h"
#include "KxCommManager.h"
#include "GameUserManager.h"

void CTeamService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
	case CMD_TEAM_SETTEAM_CS:
		ProcessSetTeam(uid, buffer, len, commun);
		break;
	}
}

void CTeamService::ProcessSetTeam(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(len == sizeof(TeamSetCS));
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN_VOID(pGameUser != NULL);

	CTeamModel *pTeamModel = dynamic_cast<CTeamModel*>(pGameUser->getModel(MODELTYPE_TEAM));
    CHECK_RETURN_VOID(pTeamModel != NULL);

	TeamInfo info;
	memset(&info, 0, sizeof(info));
	memcpy(&info, buffer, sizeof(TeamSetCS));

    TeamSetCS *teamCS = reinterpret_cast<TeamSetCS *>(buffer);
	if (!pTeamModel->SetTeamInfo(teamCS->teamType, info))
	{
        KXLOGERROR("ProcessSetTeam SetTeamInfo is error! uid%d", uid);
		return; 
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_TEAM, CMD_TEAM_SETTEAM_SC, NULL, 0);
}
