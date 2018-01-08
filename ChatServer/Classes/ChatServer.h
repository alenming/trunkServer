/*
聊天服务器
1、并行于session服务器
2、连接验证服务器
*/

#ifndef _CHAT_SERVER_H__
#define _CHAT_SERVER_H__

#include "KxBaseServer.h"
#include "ServerConfig.h"
#include <vector>
#include <string>

// 连接服务器配置
struct ConnectServerConf
{
    int serverGroup;
    int port;
    std::string host;
};

// chat本地配置
struct ChatServerConf
{
    int ipVer;
    int group;
    int serverGroup;
    int serverId;
    int port;
    std::string host;
    std::vector<ConnectServerConf> connectors;
};

class CChatServer : public KxServer::KxBaseServer
{
public:
    CChatServer();
    ~CChatServer();

    static CChatServer* getInstance();

	static void destroy();

    virtual bool initServerInfo(int argc, char ** argv);

    virtual void onServerUpdate();

	virtual bool onServerInit();

    CServerConfig * getServerCoinfig() { return &m_ServerConfig; }

private:

    static CChatServer *    m_pInstance;
    CServerConfig			m_ServerConfig;	      //服务器数读取配置表
    ChatServerConf          m_ServerInfo;	      //服务器信息
};


#endif //_CHAT_SERVER_H__
