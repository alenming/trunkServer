#ifndef __TEAM_SERVICE_H__
#define __TEAM_SERVICE_H__

#include "KXServer.h"

class CTeamService
{
public:
	// 队伍服务
	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 设置队伍
	static void ProcessSetTeam(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__TEAM_SERVICE_H__
