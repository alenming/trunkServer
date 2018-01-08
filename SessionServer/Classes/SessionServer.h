#ifndef __SESSIONSERVER_H__
#define __SESSIONSERVER_H__

#include "KXServer.h"
#include "ServerConfig.h"
#include "GameInterface.h"

// 连接服务器配置
struct ConnectServerConf
{
	int serverGroup;
	int port;
	std::string host;
};

// session本地配置
struct SessionServerConf
{
	int ipVer;
	int group;
	int serverGroup;
	int serverId;
	int port;
	std::string host;
	std::vector<ConnectServerConf> connectors;
};

class CSessionServer : public KxServer::KxBaseServer
{
private:
    CSessionServer(void);
    virtual ~CSessionServer(void);

public:
    static CSessionServer* getInstance();
    static void destroy();

	//动态更新服务器
	bool dynamicUpdate();											
	//服务器信息初始化
	virtual bool initServerInfo(int argc, char ** argv);
	//服务器模块初始化
    virtual bool onServerInit();
	//设置服务器立刻关闭 
	void setSessionServerClose();							
	//设置服务器平滑开始标志
	void setSessionServerClosing();							
	//得到平滑关闭标志位
	bool getClosingFlag(){ return m_IsClosing; }			
	CServerConfig * getServerCoinfig() { return &m_ServerConfig; }
	IGameEvent*	getGameEvent(){ return m_GameEvent; }

private:

	SessionServerConf       m_ServerInfo;	//服务器信息
	bool					m_IsClosing;	//开启平滑关闭时的标志
	IGameEvent*				m_GameEvent;	//SessionServe本地事件处理回调接口
	CServerConfig			m_ServerConfig;	//服务器数读取配置表
	static CSessionServer*	m_Instance;		//单例
};

#endif //__SESSIONSERVER_H__
