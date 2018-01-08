#ifndef __SUMMONER_SERVER_H__
#define __SUMMONER_SERVER_H__

#include <string>
#include "KxBaseServer.h"

// session本地配置
struct SummonerServerConf
{
	int ipVer;
	int group;
	int serverGroup;
	int serverId;
	int port;
	std::string host;
};

class CSummonerServer : public KxServer::KxBaseServer
{
public:
	CSummonerServer();
	virtual ~CSummonerServer();

public:
	// 提供给统计使用
	static CSummonerServer *getNewInstance();
	// 初始化服务器信息
	virtual bool initServerInfo(int argc, char **argv);
	// 设置服务器id
	virtual void setServerId(int serverId) { m_ServerInfo.serverId = serverId; }
	virtual int getServerId() { return m_ServerInfo.serverId; }
	// 设置服务器组属
	virtual void setGroup(int group) { m_ServerInfo.group = group; }
	virtual int getGroup() { return m_ServerInfo.group; }
	// 设置端口号
	virtual void setPort(int port) { m_ServerInfo.port = port; }
	virtual int getPort() { return m_ServerInfo.port; }
	// 设置ip
	virtual void setHost(std::string &ip) { m_ServerInfo.host = ip; }
	virtual std::string getHost() { return m_ServerInfo.host; }
	
protected:

	SummonerServerConf      m_ServerInfo;	 //服务器信息
	static CSummonerServer *m_pNewInstance;	 //子类对象
};

#endif //__SUMMONER_SERVER_H__
