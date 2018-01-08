#ifndef __USER_SERVICE_H__
#define __USER_SERVICE_H__

#include "KxCommInterfaces.h"

class CUserService
{
public:

	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processBuyService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	
	static void processRenameService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processHeadIconChange(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//处理玩家签到
	static void processUserSignService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//处理玩家礼包Key
	static void processUserGiftKeyService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//首冲礼包领取
	static void processUserFistPayService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//基金消息处理
	static void processUserFundService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 数据转发
    static void processUserForwardDataService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//修改玩家账号对应密码
	static void processUsrPasswordModifyService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__USER_SERVICE_H__
