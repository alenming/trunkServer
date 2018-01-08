#ifndef __LOOK_SERVICE_H__
#define __LOOK_SERVICE_H__

#include "KXServer.h"

class CLookService
{
public:
	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 查看某人英雄
	static void ProcessLookHero(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 查看某人装备
    static void ProcessLookEquip(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__LOOK_SERVICE_H__
