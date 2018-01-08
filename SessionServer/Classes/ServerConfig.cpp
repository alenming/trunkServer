#include "ServerConfig.h"
#include "CsvLoader.h"
#include <time.h>
#include <string.h>
#include "KXServer.h"
#include "Protocol.h"

using namespace std;

#define LOCAL_SERVER_PATH "..//GameConfig//Session//LocalServerConfig.csv"
#define CONN_SERVER_PATH "..//GameConfig//Session//ServerConfig.csv"
#define ROUTE_SERVER_PATH "..//GameConfig//Session//ServerRouteConfig.csv"
#define CLOSINGWHITECMD_SERVER_PATH "..//GameConfig//Session//ServerClosingWhiteCmdConfig.csv"

CServerConfig::CServerConfig()
{
}


CServerConfig::~CServerConfig()
{
}

bool CServerConfig::create()
{
	clean();
	if (!loadLocalServer())
	{
		return false;
	}
	if (!loadConnServer())
	{
		return false;
	}
	if (!loadRouteServer())
	{
		return false;
	}
	if (!loadClosingWhiteCmd())
	{
		return false;
	}
	return true;
}

void CServerConfig::clean()
{
	memset(&m_LocalServer, 0, sizeof(m_LocalServer));
	m_MapConnServer.clear();
	m_MapConnSByServerID.clear();
	m_MapRouteMain.clear();
    m_MapRouteCmd.clear();
}

ServerRouteConfigData* CServerConfig::getRouteConfigData(int nMainCmd, int nSubCmd)
{
    std::map<int, ServerRouteConfigData>::iterator iter;

    iter = m_MapRouteCmd.find(MakeCommand(nMainCmd, nSubCmd));
    if (iter != m_MapRouteCmd.end())
    {
        return &(iter->second);
    }
    else
    {
        iter = m_MapRouteMain.find(nMainCmd);
        if (iter != m_MapRouteMain.end())
        {
            return &(iter->second);
        }
    }
	return NULL;
}

bool CServerConfig::loadLocalServer()
{
    CCsvLoader pLoader;
    if (!pLoader.LoadCSV(LOCAL_SERVER_PATH))
    {
        return false;
    }

    // 如果有数据,只读一行
    pLoader.NextLine();
    m_LocalServer.GroupID = pLoader.NextInt();
    m_LocalServer.ServerID = pLoader.NextInt();
    pLoader.NextStr();	// 这一列不需要解析
    strncpys(m_LocalServer.IP, pLoader.NextStr().c_str(), sizeof(m_LocalServer.IP));
    m_LocalServer.Port = pLoader.NextInt();
    return true;
}

bool CServerConfig::loadConnServer()
{
	CCsvLoader pLoader;
    if (!pLoader.LoadCSV(CONN_SERVER_PATH))
    {
        return false;
    }

	while (pLoader.NextLine())
	{
		int groupId = pLoader.NextInt();
		int serverId = pLoader.NextInt();
        pLoader.NextStr();	// 这一列不需要解析
        ServerConfigData& serverData = m_MapConnSByServerID[serverId];
        serverData.GroupID = groupId;
        serverData.ServerID = serverId;
        strncpys(serverData.IP, pLoader.NextStr().c_str(), sizeof(serverData.IP));
		serverData.Port = pLoader.NextInt();
        ServerMap::iterator ator = m_MapConnServer.find(groupId);
		if (ator == m_MapConnServer.end())
		{
			vector<ServerConfigData*> vectData;
            vectData.push_back(&serverData);
            m_MapConnServer[groupId] = vectData;
		}
		else
		{
			vector<ServerConfigData*> &VectData = ator->second;
            VectData.push_back(&serverData);
		}
	}
	return true;
}

bool CServerConfig::loadRouteServer()
{
	CCsvLoader pLoader;
    if (!pLoader.LoadCSV(ROUTE_SERVER_PATH))
    {
        return false;
    }

	//如果有数据
	while (pLoader.NextLine())
	{
		ServerRouteConfigData Data;
		int mainCmd = pLoader.NextInt();
		int subCmd = pLoader.NextInt();
		Data.GroupID  = pLoader.NextInt();
		Data.RouteKey = pLoader.NextInt();
		Data.Permission = pLoader.NextInt();
        Data.RouteType = pLoader.NextInt();
        // 设置通用规则和特殊规则
        if (subCmd == 0)
        {
            m_MapRouteMain[mainCmd] = Data;
        }
        else
        {
            m_MapRouteCmd[MakeCommand(mainCmd, subCmd)] = Data;
        }
	}
	return true;
}


bool CServerConfig::loadClosingWhiteCmd()
{
	CCsvLoader pLoader;
	if (!pLoader.LoadCSV(CLOSINGWHITECMD_SERVER_PATH))
	{
		return false;
	}

	while (pLoader.NextLine())
	{
		m_ServerCloseWhiteCmd.insert(pLoader.NextInt());
	}

	return true;
}



bool CServerConfig::updateConfig()
{
	m_MapConnServer.clear();
	m_MapConnSByServerID.clear(); 
	m_MapRouteMain.clear();
	m_MapRouteCmd.clear();

	if (!loadConnServer())
	{
		return false;
	}
	
	
	if (!loadRouteServer())
	{
		return false;
	}
	
	return true;
}