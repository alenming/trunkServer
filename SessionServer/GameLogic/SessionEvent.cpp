#include "SessionEvent.h"
#include "SessionProtocol.h"
#include "SessionClient.h"
#include "Protocol.h"
#include "LoginProtocol.h"


CSessionEvent::CSessionEvent()
{
}

CSessionEvent::~CSessionEvent()
{
}

bool CSessionEvent::onUserEvent(int nType, KxServer::IKxComm* target) //Session本地路由时根据nType对客户端进行不同的响应
{
	CSessionClient *pClient = static_cast<CSessionClient *>(target);
	if (pClient != NULL)
	{
		switch (nType)
		{
		case EventClosed:
			sendDataToActor(pClient, CMD_LOGIN, CMD_LOGIN_TICK_SC, NULL, 0);
			break;
		default:
			break;
		}
	}
	return true;
}

bool CSessionEvent::onUserProc(unsigned int id)//Session本地路由时根据命令ID执行操作
{
	switch (id) //路由到SessionServer时，id表示执行不同的命令
	{
	case Closing:
		CSessionServer::getInstance()->setSessionServerClosing();
		break;
	case Close:
		CSessionServer::getInstance()->setSessionServerClose();
		break;
	case DynamicUpdate:
		CSessionServer::getInstance()->dynamicUpdate();
		break;
	default:
		break;
	}
	
	return true;
}

bool CSessionEvent::sendDataToActor(CSessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen)
{
	unsigned int buffSize = sizeof(Head);
	bool ret = false;
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

	//设置头部
	Head* head = reinterpret_cast<Head*>(buff);
	head->MakeCommand(nMainCmd, nSubCmd);
	head->length = buffSize + nLen;
	head->id = pClient->getUserId();
	ret = pClient->sendData(buff, buffSize) >= 0;
	if (ret && NULL != pszContext && nLen > 0)
	{
		ret = pClient->sendData(pszContext, nLen) >= 0;
	}
	kxMemMgrRecycle(buff, buffSize);
	return ret;
}
