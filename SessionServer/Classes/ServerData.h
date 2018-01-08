///////////////////////////////////////////////////////
//服务器读取配置结构
//ServerData.h
//黄城
///////////////////////////////////////////////////////
#ifndef _SERVER_DATA_H__
#define _SERVER_DATA_H__

#include <string.h>

#pragma pack(1)

//服务器信息
struct ServerConfigData
{
	char			IP[16];
	unsigned int	Port;
	unsigned int	ServerID;				//服务器ID
	unsigned int	GroupID;				//服务器组ID
};

enum ERouteType
{
    RouteSingle,                    // 转发给单个服务器
    RouteBroadcast,                 // 转发给一组服务器
    RouteReturn,                    // 直接返回
	SesssionProc		//SessionServer事件处理
};

struct ServerRouteConfigData
{
    unsigned int GroupID;			// 服务器组ID
    unsigned int RouteKey;			// 路由Key
    unsigned int Permission;		// 权限ID
    unsigned int RouteType;         // 路由类型，0-单播，1-广播，2-直接返回

	ServerRouteConfigData()
	{
		memset(this, 0, sizeof(*this));
	}
};

#pragma pack()

#endif //_SERVER_DATA_H__
