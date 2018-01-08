#ifndef __MATCH_SERVICE_H__
#define __MATCH_SERVICE_H__

#include "KxCommInterfaces.h"

class CMatchService
{
public:

	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processMatchService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processCancelService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__MATCH_SERVICE_H__
