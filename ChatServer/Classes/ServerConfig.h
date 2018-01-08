///////////////////////////////////////////////////////
//服务器配置读取
//ServerConfig.h
//黄城
///////////////////////////////////////////////////////

#ifndef _SERVER_CONFIG_H__
#define _SERVER_CONFIG_H__

#include <map>
#include <vector>
#include <set>
#include "ServerData.h"

class CServerConfig
{
	typedef std::map<int, std::vector<ServerConfigData*> >	ServerMap;

public:
	CServerConfig();
	~CServerConfig();

	bool create();
	void clean();

	bool updateConfig();

	ServerConfigData* getLocalServerInfo() { return &m_LocalServer; }
	std::map<int, ServerConfigData>* getConnServer() { return &m_MapConnSByServerID; }
	ServerRouteConfigData* getRouteConfigData(int nMainCmd, int nSubCmd);
	std::set<int>* getClosingWhiteCmd(){ return &m_ServerCloseWhiteCmd; }

protected:
	bool loadLocalServer();
	bool loadConnServer();
	bool loadRouteServer();
	bool loadClosingWhiteCmd();
	bool loadNewConnServer();
	bool loadNewRouteServer();

private:
	ServerConfigData                        m_LocalServer;
    ServerMap                               m_MapConnServer;			// 按组做连接服务器列表
    std::map<int, ServerConfigData>         m_MapConnSByServerID;		// 按服务器ID做服务器列表
    std::map<int, ServerRouteConfigData>    m_MapRouteMain;				// 按主命令获取路由规则
    std::map<int, ServerRouteConfigData>    m_MapRouteCmd;				// 按主命令+子命令获取路由规则
	std::set<int>							m_ServerCloseWhiteCmd;		// 服务器平滑关闭时允许通过的命令
};

#endif //_SERVER_CONFIG_H__
