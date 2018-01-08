#ifndef __PAY_SERVICE_H__
#define __PAY_SERVICE_H__

#include "KxCommInterfaces.h"

class CPayService
{
public:
	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processPayService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__USER_SERVICE_H__
