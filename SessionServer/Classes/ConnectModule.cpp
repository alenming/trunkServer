#include "ConnectModule.h"
#include "ServerProtocol.h"
#include "NetworkManager.h"
#include "SessionClient.h"
#include "SessionServer.h"
#include "GameInterface.h"
#include "SessionHelper.h"


using namespace KxServer;
using namespace ServerProtocol;

CConnectModule::CConnectModule(void)
: m_pSink(NULL)
{
}

CConnectModule::~CConnectModule(void)
{
    KXSAFE_DELETE(m_pSink);
}

bool CConnectModule::init(IGameEvent *pSink)
{
	m_pSink = pSink;
	return true;
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
		CSessionHelper::ServerSubSetPermission(buffer, len, target, head);
        break;
        // 断开用户,控制台发出消息
    case ServerSubDisconnect:
		CSessionHelper::ServerSubDisconnect(buffer, len, target, head, this);
        break;
        // 设置某个玩家的路由值
    case ServerSubSetRoute:
		CSessionHelper::ServerSubSetRoute(buffer, len, head);
        break;
        // 单播命令
    case ServerSubTransmit:
		CSessionHelper::ServerSubTransmit(buffer, len, head);
        break;
        // 广播给所有玩家
    case ServerSubBoardCast:
		CSessionHelper::ServerSubBoardCast(buffer, len);
        break;
		// 验证命令
    case ServerSubInit:
		CSessionHelper::ServerSubInit(buffer, len, this);
		break;
        // 查询当前连接玩家数
    case ServerSubQueryLoad:
		CSessionHelper::ServerSubQueryLoad(buffer, len, head);
        break;
    case ServerSubForward:
        CSessionHelper::ServerSubForward(buffer, len, head);
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

bool CConnectModule::processUserEvent(int nType, KxServer::IKxComm* target)
{
	if (m_pSink != NULL)
	{
        m_pSink->onUserEvent(nType, target);
	}
	return true;
}
