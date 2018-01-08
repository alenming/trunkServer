#include "ChatModule.h"
#include "Protocol.h"
#include "ServerProtocol.h"
#include "ChatService.h"
#include "NoticeService.h"
#include "ChatClient.h"
#include "ChatServer.h"
#include "NetworkManager.h"
#include "ServerConfig.h"

using namespace KxServer;

CChatModule::CChatModule()
{
}


CChatModule::~CChatModule()
{
}

// IKxComm对象接收到完整数据后，调用的回调
void CChatModule::processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target)
{
    CChatClient* pClient = dynamic_cast<CChatClient*>(target);
    if (NULL == pClient || sizeof(ServerProtocol::Head) > len)
    {
        return;
    }

    // 直接转发给游戏服务器
    ServerProtocol::Head* head = reinterpret_cast<ServerProtocol::Head*>(buffer);
    int nMainCmd = head->MainCommand();
    int nSubCmd = head->SubCommand();
    char *data = reinterpret_cast<char *>(head->data());
    int length = head->length - sizeof(Head);

    if (nMainCmd == ServerProtocol::ServerMain)
    {
        if (nSubCmd == ServerProtocol::ServerSubQueryLoad)
        {
            int nCurNum = CNetWorkManager::getInstance()->getCurClientNum();
            CNetWorkManager::getInstance()->sendDataToClient(pClient, nMainCmd, nSubCmd, (char*)&nCurNum, sizeof(nCurNum));
            return;
        }
    }
    else if (CMD_CHAT == nMainCmd)
    {
        if (pClient->getPermission() == 1)
        {
            CChatService::processService(nSubCmd, pClient->getUserId(), data, length, target);
        }
        else
        {
            KXLOGERROR("player need check login chat server!!!");
        }

        return;
    }
    else if (CMD_NOTICE == nMainCmd)
    {
        CNoticeService::processService(nSubCmd, pClient->getUserId(), data, length, target);
        return;
    }

    CServerConfig * pServerConfig = CChatServer::getInstance()->getServerCoinfig();
    ServerRouteConfigData *pRouteConifgData = pServerConfig->getRouteConfigData(nMainCmd, nSubCmd);

    //if (CChatServer::getInstance()->getClosingFlag())//判断服务器是否处于平滑关闭状态，以决定命令是否可以转发
    //{
    //    std::set<int>* cmdSet = pServerConfig->getClosingWhiteCmd();
    //    std::set<int>::iterator it = cmdSet->find(head->MainCommand());
    //    if (it == cmdSet->end())
    //    {
    //        CChatServer::getInstance()->getGameEvent()->onUserEvent(EventClosed, target);//服务器平滑关闭时对客户端的处理
    //        return;
    //    }
    //}

    if (pRouteConifgData == NULL)
    {
        // 找不到该指令对应的转发规则配置
        return;
    }

    // 如果权限允许
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

        default:
            break;
        }
    }
}

// IKxComm对象发生错误时，调用的回调
void CChatModule::processError(KxServer::IKxComm *target)
{
    KXLOGDEBUG("CChatModule::processError");
    // 玩家断线
    this->userDisconnect(target);
}

void CChatModule::userDisconnect(KxServer::IKxComm *target)
{
    CChatClient *pClient = dynamic_cast<CChatClient*>(target);
    if (pClient == NULL)
    {
        return;
    }

    pClient->clean();
}
