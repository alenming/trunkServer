#ifndef __SERVER_SERVICE_H__
#define __SERVER_SERVICE_H__

#include "KxCommInterfaces.h"

class CServerService
{
public:

	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processPlayerOffLine(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processPlayerDisConnect(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__SERVER_SERVICE_H__
