#ifndef __BAG_SERVICE_H__
#define __BAG_SERVICE_H__

#include "KxCommInterfaces.h"

class CBagService
{
public:

	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processUnlockService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processSaleService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processUseService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processMakeEquipmentService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processDecomposeEquipmentService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

private:
    static bool canUse(int itemID);
};

#endif //__BAG_SERVICE_H__
