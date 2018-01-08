#ifndef __SUMMONER_SERVICE_H__
#define __SUMMONER_SERVICE_H__

#include "KxCommInterfaces.h"

class CSummonerService
{
public:

	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processBuySummoner(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__SUMMONER_SERVICE_H__
