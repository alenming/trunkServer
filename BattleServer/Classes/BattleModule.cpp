#include "BattleModule.h"
#include "EventManager.h"
#include "GameDef.h"
#include "GameUserManager.h"

#include "Protocol.h"
#include "ServerProtocol.h"
#include "KxLog.h"

#include "BattleService.h"
#include "PvpService.h"
#include "ServerService.h"

using namespace KxServer;

CBattleModule::CBattleModule(void)
{
}

CBattleModule::~CBattleModule(void)
{
}

int CBattleModule::processLength(char* buffer, unsigned int len)
{
    if (len < sizeof(Head))
    {
        return sizeof(Head);
    }
    // 返回一个包的长度, 由包头解析, 解析的长度包括包头
    return reinterpret_cast<Head*>(buffer)->length;
}

void CBattleModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
    Head *pHead = reinterpret_cast<Head*>(buffer);
    int uid = pHead->id;
    int cmd = pHead->MainCommand();
    int subcmd = pHead->SubCommand();
    len -= sizeof(Head);

    //KXLOGDEBUG("BattleServer maincmd %d, subcmd %d, len %d, uid %d", cmd, subcmd, len, pHead->id);
    switch(cmd)
    {
    case CMD_BATTLE:
        CBattleService::processService(subcmd, uid, static_cast<char *>(pHead->data()), len, target);
        break;
    case CMD_PVP:
		CPvpService::processService(subcmd, uid, static_cast<char *>(pHead->data()), len, target);
        break;
	case ServerProtocol::ServerMain:
		CServerService::processService(subcmd, uid, static_cast<char *>(pHead->data()), len, target);
		break;
    default:
        break;
    }
}

void CBattleModule::processError(IKxComm *target)
{
}
