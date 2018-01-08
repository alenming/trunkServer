#ifndef __PVPCHEST_SERVICE_H__
#define __PVPCHEST_SERVICE_H__

#include "KxCommInterfaces.h"

class CPvpChestService
{
public:

	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processRefreshChest(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processBuyChest(int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processOpenChest(int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processOpenChestAtOnce(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__PVPCHEST_SERVICE_H__

