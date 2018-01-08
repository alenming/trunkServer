#include "SProtocolService.h"
#include "ServerProtocol.h"
#include "KxCommManager.h"
#include "GameUserManager.h"

using namespace ServerProtocol;


CSProtocolService::CSProtocolService()
{
}


CSProtocolService::~CSProtocolService()
{
}

void CSProtocolService::ProcessSProtocolService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    switch (subcmd)
    {
        //处理用户掉线
        case ServerSubPlayerOffLine:
            {
                CSProtocolService::ProcessUserOffLine(uid);
            }
        break;
    }
}

void CSProtocolService::ProcessUserOffLine(int uid)
{
	CKxCommManager::getInstance()->delKxComm(uid);
	CGameUserManager::getInstance()->removeGameUser(uid);
}
