#ifndef __SERVER_PROTOCOL_H__
#define __SERVER_PROTOCOL_H__

enum
{
    emRouteLoginKey = 1,                    //登陆验证Key
    emRouteUnionKey,                        //公会验证Key
	emRouteBattleKey,                       //战斗验证Key
    emRouteChatKey,                         //聊天验证Key
};

namespace ServerProtocol
{
	// 服务器内部主命令 —— 不得与业务主命令重复
	enum ServerMainCmd
	{
        ServerMain,
	};

	enum ServerSubCmd
	{
        // 用户相关
        ServerSubSetPermission,		// 设置权限-SProSetPermission包体
        ServerSubDisconnect,		// 断开用户
        ServerSubPlayerOffLine,     // 用户已掉线
        ServerSubSetRoute,          // 设置路由ID以及对应的值-SProSetRoute包体
        ServerSubTransmit,          // 传输数据
        ServerSubBoardCast,         // 给所有用户传输数据
        ServerSubInit,              // 组合命令，Route+权限+数据-SProInit包体

        // 服务器内部相关
        ServerSubQueryLoad = 100,   // 查询当前服务的用户量
        ServerSubResponseLoad,      // 返回当前服务的用户量-SProLoad包体
        ServerSubCloseServer,       // 关闭服务器
        ServerSubForward,           // 服务器内部转发数据
	};

#pragma pack(1)
	// 服务器内部通用包头
	struct Head
	{
		int length;
		int cmd;
		int id;

		inline void MakeCommand(int main, int sub)
		{
			cmd = (main << 16) | (sub & 0x0000ffff);
		}

		inline int SubCommand()
		{
			return cmd & 0x0000ffff;
		}

		inline int MainCommand()
		{
			return cmd >> 16;
		}

		inline void* data()
		{
			return this + 1;
		}
	};

    //设置某个用户的权限数据
    struct SProSetPermission
    {
        int     Permission;
    };

    //设置某个玩家的路由数据
    struct SProSetRoute
    {
        int nRouteKey;
        int nRoteValue;         //路由值
    };

	//ServerSubDisconnect
	struct SProDisConect
	{
		int nDisConectuid;		//被断开的用户ID
	};

    //ServerSubInit
    struct SProInitSCC
    {
        unsigned int nGuestId;
        unsigned int nRouteKey;
        unsigned int nRouteValue;
        unsigned int nPermssion;			//权限ID

        SProInitSCC()
        {
            nGuestId = 0;
            nRouteKey = 0;
            nRouteValue = 0;
            nPermssion = 0;
        }
    };

    //ServerSubQueryLoad
    struct SPServerLoad
    {
        unsigned int ClientNum;             //客户端连接数
    };

#pragma pack()
}

#endif
