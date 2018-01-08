#ifndef __CHAT_HELPER_H__
#define __CHAT_HELPER_H__

#include <string>
#include "ConnectModule.h"
#include "NetworkManager.h"
#include "ChatClient.h"
#include "ChatServer.h"
#include "ServerProtocol.h"

class CChatHelper
{
public:
    CChatHelper();
    ~CChatHelper();

	// 为用户设置权限
    static void ServerSubSetPermission(char* buffer, unsigned int len, KxServer::IKxComm *target, ServerProtocol::Head* head);
	// 断开用户,控制台发出消息
    static void ServerSubDisconnect(char* buffer, unsigned int len, KxServer::IKxComm *target, ServerProtocol::Head* head, CConnectModule* connectModule);
	// 设置某个玩家的路由值
    static void ServerSubSetRoute(char* buffer, unsigned int len, ServerProtocol::Head* head);
	// 单播命令
    static void ServerSubTransmit(char* buffer, unsigned int len, ServerProtocol::Head* head);
	// 广播给所有玩家
	static void ServerSubBoardCast(char* buffer, unsigned int len);
	// 验证命令
	static void ServerSubInit(char* buffer, unsigned int len, CConnectModule* connectModule);
	// 查询当前连接玩家数
    static void ServerSubQueryLoad(char* buffer, unsigned int len, ServerProtocol::Head* head);
    // 转发服务器内部数据
    static void ServerSubForward(char* buffer, unsigned int len, ServerProtocol::Head* head);
    // 内容是否合法
    static bool isContentLegal(int chatType, const std::string& str);
    // 玩家是否合法
    static bool isPlayerLegal(int uid);
private:

};

#endif