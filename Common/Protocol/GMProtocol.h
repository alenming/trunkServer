#ifndef __GM_PROTOCOL_H__
#define __GM_PROTOCOL_H__

#pragma pack(1)

enum PAYROTOCOL
{
	CMD_GM_CSBEGIN,
	CMD_GM_LOGIN_CS,					//GM账号登陆
	CMD_GM_CLOSE_CS,					//服务器关闭请求
	CMD_GM_SERVERNUM_CS,				//获取服务器人数请求
	CMD_GM_DYNUPDATE_CS,				//热更新服务器，动态切换服务器组
	CMD_GM_ROUTE_CS,					//GM数据转发聊天服务器
	CMD_GM_CSEND,

	CMD_GM_SCBEGIN = 100,
	CMD_GM_LOGIN_SC,					//GM账号登陆
	CMD_GM_CLOSE_SC,					//服务器关闭
	CMD_GM_SERVERNUM_SC,
	CMD_GM_DYNUPDATE_SC,				//热更新服务器，动态切换服务器组
	CMD_GM_SCEND,
};

//////////////////////////////////////////////////////////////////////////

//CMD_GM_LOGIN_CS
struct SGMLoginCS
{
	char szAdimin[32];			//账号 32个字符
	char szPassword[32];		//密码 32个字符
};

struct SGMLoginSC
{
	unsigned int nUid;
};


//CMD_GM_SERVERNUM_CS
struct SGMServerNumSC
{
	unsigned int nNum;			//获取服务器人数
};

struct SGMFlagSC
{
	unsigned int nFlag;			//是否成功 0-失败，1-成功
};

//CMD_GM_CLOSE_CS
//SGMFlagSC

//CMD_GM_DYNUPDATE_CS
//备份服务器组和主服务器组是同一台服务器
struct SGMDynUpdateCS
{
	unsigned int nGroupID;		//服务器内组ID
	unsigned int nNum;			//服务器个数
	unsigned int nStartPort;	//服务器起始端口
};

struct SGMRouteChatCS
{
	int nLen;
	//字符串长度
};

#pragma pack()

#endif