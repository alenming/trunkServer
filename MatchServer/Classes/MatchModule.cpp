#include "MatchModule.h"
#include "Protocol.h"
#include "ServerProtocol.h"

#include "MatchService.h"
#include "ServerService.h"
#include "NewMatchManager.h"

using namespace KxServer;

CMatchModule::CMatchModule()
{
}

CMatchModule::~CMatchModule()
{
}

int CMatchModule::processLength(char* buffer, unsigned int len)
{
	// 返回一个包的长度, 由包头解析, 解析的长度包括包头
	int ret = reinterpret_cast<Head*>(buffer)->length;
	return ret;
}

void CMatchModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
	Head *pHead = reinterpret_cast<Head*>(buffer);
	int cmd = pHead->MainCommand();
	int subCmd = pHead->SubCommand();
	
	char *pData = reinterpret_cast<char*>(pHead->data());
	int uid = pHead->id;
	int length = len - sizeof(Head);

    //KXLOGDEBUG("MatchServer maincmd %d, subcmd %d, len %d, uid %d", cmd, subCmd, len, uid);
	switch (cmd)
	{
	case CMD_MATCH:
		CMatchService::processService(subCmd, uid, pData, length, target);
		break;
	case ServerProtocol::ServerMain:
		CServerService::processService(subCmd, uid, pData, length, target);
		break;
	default:
		break;
	}
}

void CMatchModule::processError(IKxComm *target)
{
	CNewMatchManager::getInstance()->clearMatchers();
}
