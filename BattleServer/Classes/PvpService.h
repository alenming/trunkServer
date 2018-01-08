#ifndef __PVPSERVICE_H__
#define __PVPSERVICE_H__

#include "KxCommInterfaces.h"

class CPvpService
{
public:

    static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processEnterRoomSS(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processEnterRoom(int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processLoading(int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processReady(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processReconnect(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processReconnectRoomData(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif 
