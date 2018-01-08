#include "SessionModule.h"
#include "KxCore.h"
#include "SessionClient.h"
#include "SessionConnect.h"
#include "NetworkManager.h"

#include "Protocol.h"
#include "LoginProtocol.h"
#include "ServerData.h"
#include "SessionServer.h"
#include "ServerProtocol.h"
#include <set>
#include "SessionGMService.h"

using namespace KxServer;
using namespace ServerProtocol;

CSessionModule::CSessionModule(void)
{
}

CSessionModule::~CSessionModule(void)
{
}

void CSessionModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
    CSessionClient* pClient = dynamic_cast<CSessionClient*>(target);
	CHECK_RETURN_DEBUG_VOID(NULL != pClient && sizeof(ServerProtocol::Head) <= len);
	CServerConfig * pServerConfig = CSessionServer::getInstance()->getServerCoinfig();
	CHECK_RETURN_DEBUG_VOID(pServerConfig != NULL);
	// 直接转发给游戏服务器
	ServerProtocol::Head* head = reinterpret_cast<ServerProtocol::Head*>(buffer);
	CHECK_RETURN_DEBUG_VOID(head != NULL);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();

	if (CSessionServer::getInstance()->getClosingFlag())//判断服务器是否处于平滑关闭状态，以决定命令是否可以转发
	{
		std::set<int>* cmdSet = pServerConfig->getClosingWhiteCmd();
		std::set<int>::iterator it = cmdSet->find(head->MainCommand());
		if (it == cmdSet->end())
		{
			CSessionServer::getInstance()->getGameEvent()->onUserEvent(EventClosed, target);//服务器平滑关闭时对客户端的处理
			return;
		}
	}
	
	ServerRouteConfigData *pRouteConifgData = pServerConfig->getRouteConfigData(nMainCmd,nSubCmd);
	CHECK_RETURN_DEBUG_VOID(pRouteConifgData != NULL);
    // 如果权限允许
	int cmdID = head->id;//路由到Session本地时，包头id对应为本地处理的方法
    if (pRouteConifgData->Permission <= (unsigned int)pClient->getPermission())
    {
        if (pClient->getPermission() == 0)
        {
            head->id = pClient->getGuestId();
        }
        else
        {
            head->id = pClient->getUserId();
        }

        switch (pRouteConifgData->RouteType)
        {
		case RouteSingle:
            pClient->sendDataToServer(pRouteConifgData->GroupID, pRouteConifgData->RouteKey, buffer, len);
            break;
		case RouteBroadcast:
            pClient->sendDataToGroupServer(pRouteConifgData->GroupID, buffer, len);
            break;
		case RouteReturn:
            pClient->sendData(buffer, len);
            break;
		case SesssionProc:
			//CSessionServer::getInstance()->getGameEvent()->onUserProc(cmdID);
			CSessionGMService::processGMLogic(buffer, len, pRouteConifgData->GroupID,pRouteConifgData->RouteKey, target);
			break;
        default:
            break;
        }
    }
}

void CSessionModule::processError(IKxComm *target)
{
    // 玩家断线
    this->userDisconnect(target);
}

// 1. 告诉所有服务器，玩家XXX掉线了
// 2. 从ClientManager中移除该玩家
void CSessionModule::userDisconnect(IKxComm *target)
{
    CSessionClient *pClient = dynamic_cast<CSessionClient*>(target);
    if (pClient == NULL)
    {
        return;
    }

    // 设置下线消息
    ServerProtocol::Head head;
    head.MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubPlayerOffLine);
    head.length = sizeof(head);
    head.id = pClient->getUserId();

    // 发送数据到后端告知角色下线
    // 发送下线消息给后端指定的服务器
    pClient->sendDataToAllServer(reinterpret_cast<char*>(&head), sizeof(head));
    // 关闭socket、从NetWorkManager中和clean移除
    pClient->clean();
}
