#include "ConnectModule.h"
#include "ServerProtocol.h"
#include "NetworkManager.h"
#include "ChatClient.h"
#include "ChatServer.h"
#include "ChatHelper.h"

using namespace KxServer;
using namespace ServerProtocol;

CConnectModule::CConnectModule(void)
{
}

CConnectModule::~CConnectModule(void)
{
}

void CConnectModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
    // 发给指定的前端
    Head* head = reinterpret_cast<Head*>(buffer);
    if (head->MainCommand() != ServerMain || len < sizeof(Head))
    {
        return;
    }
    switch (head->SubCommand())
    {
        // 为用户设置权限
    case ServerSubSetPermission:
		CChatHelper::ServerSubSetPermission(buffer, len, target, head);
        break;
        // 断开用户,控制台发出消息
    case ServerSubDisconnect:
		CChatHelper::ServerSubDisconnect(buffer, len, target, head, this);
        break;
        // 设置某个玩家的路由值
    case ServerSubSetRoute:
		CChatHelper::ServerSubSetRoute(buffer, len, head);
        break;
        // 单播命令
    case ServerSubTransmit:
		CChatHelper::ServerSubTransmit(buffer, len, head);
        break;
        // 广播给所有玩家
    case ServerSubBoardCast:
		CChatHelper::ServerSubBoardCast(buffer, len);
        break;
		// 验证命令
    case ServerSubInit:
		CChatHelper::ServerSubInit(buffer, len, this);
		break;
        // 查询当前连接玩家数
    case ServerSubQueryLoad:
		CChatHelper::ServerSubQueryLoad(buffer, len, head);
        break;
    case ServerSubForward:
        CChatHelper::ServerSubForward(buffer, len, head);
        break;
    default:
        break;
    }
}

void CConnectModule::processError(IKxComm *target)
{
	// 后端连接断开
    KXLOGDEBUG("Socket Connect To Server Failed");
}

void CConnectModule::processEvent(int eventId, IKxComm* target)
{
    if (eventId == KXEVENT_CONNECT_FAILE)
    {
        KXLOGDEBUG("Connect Server Failed");
    }
    else if (eventId == KXEVENT_CONNECT_SUCCESS)
    {
        KXLOGDEBUG("Connect Server Success");
    }
}
