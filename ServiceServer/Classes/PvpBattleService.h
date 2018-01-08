#ifndef __PVPBATTLE_SERVICE_H__
#define __PVPBATTLE_SERVICE_H__

#include "KxCommInterfaces.h"

class CPvpBattleService
{
public:
	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 处理进入PVP房间
    static void processPvpEnterRoomSSServices(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 处理PVP结算
    static void processPvpFinishSSServices(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 处理进入机器人PVP房间
    static void processEnterRobotRoom(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 处理机器人结算
    static void processFinishRobotRoom(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__PVPINFO_SERVICE_H__

