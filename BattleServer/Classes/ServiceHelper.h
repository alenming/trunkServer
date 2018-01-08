#ifndef __SERVICEHELPER_H__
#define __SERVICEHELPER_H__

#include "KxCommInterfaces.h"
#include "Protocol.h"
#include "GateManager.h"
#include "HeroModel.h"
#include "ConfRole.h"
#include "ConfStage.h"
#include "GameDef.h"

class CServiceHelper
{
public:
	// ·¢ËÍÏûÏ¢
    static int SentMsg(int uid, int maincmd, int subcmd, char *buffer, int len, KxServer::IKxComm *commu)
	{
        return CGateManager::getInstance()->Transmit(uid, MakeCommand(maincmd, subcmd), buffer, len);
    }
};

#endif 
