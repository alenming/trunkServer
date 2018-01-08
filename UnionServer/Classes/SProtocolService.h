#ifndef _SPROTOCOL_SERVICE_H__
#define _SPROTOCOL_SERVICE_H__

#include "KXServer.h"

class CSProtocolService
{
public:
    CSProtocolService();
    ~CSProtocolService();

    static void ProcessSProtocolService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

protected:

    static void ProcessUserOffLine(int uid);


};

#endif //_SPROTOCOL_SERVICE_H__

