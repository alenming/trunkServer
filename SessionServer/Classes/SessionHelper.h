#ifndef __SessionHelper_H__
#define __SessionHelper_H__
#include "ConnectModule.h"
#include "NetworkManager.h"
#include "SessionClient.h"
#include "SessionServer.h"
#include "GameInterface.h"


using namespace KxServer;
using namespace ServerProtocol;


class CSessionHelper
{
public:
	CSessionHelper();
	~CSessionHelper();

	// 为用户设置权限
	static void ServerSubSetPermission(char* buffer, unsigned int len, IKxComm *target, Head* head);
	// 断开用户,控制台发出消息
	static void ServerSubDisconnect(char* buffer, unsigned int len, IKxComm *target, Head* head, CConnectModule* connectModule);
	// 设置某个玩家的路由值
	static void ServerSubSetRoute(char* buffer, unsigned int len, Head* head);
	// 单播命令
	static void ServerSubTransmit(char* buffer, unsigned int len, Head* head);
	// 广播给所有玩家
	static void ServerSubBoardCast(char* buffer, unsigned int len);
	// 验证命令
	static void ServerSubInit(char* buffer, unsigned int len, CConnectModule* connectModule);
	// 查询当前连接玩家数
	static void ServerSubQueryLoad(char* buffer, unsigned int len, Head* head);
    // 转发服务器内部数据
    static void ServerSubForward(char* buffer, unsigned int len, Head* head);
private:

};

#endif