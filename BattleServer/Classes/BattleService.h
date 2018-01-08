#ifndef __BATTLESERVICE_H__
#define __BATTLESERVICE_H__

#include "KxCommInterfaces.h"

class CPvpRoom;
class CBattleService
{
public:
    
    static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processBattleCommand(int uid, CPvpRoom *room, char *buffer, int len, KxServer::IKxComm *commun);

    static void processBattleUpdate(int uid, CPvpRoom *room, char *buff, int len, KxServer::IKxComm *commun);
};

#endif 
