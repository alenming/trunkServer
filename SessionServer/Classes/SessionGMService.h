#ifndef _SESSION_GM_SERVICE_H__
#define _SESSION_GM_SERVICE_H__

#include "KXServer.h"

class CSessionGMService
{
public:
	static void processGMLogic(char* buffer, unsigned int len,int nGroupID,int nKey,KxServer::IKxComm *target);

protected:
	//处理GM 获取玩家人数请求
	static void processGetServerNum(KxServer::IKxComm *target);
	//处理GM 命令平滑关服请求
	static void processCloseServer(KxServer::IKxComm *target);
	//处理GM 命令热更新服务器请求
	static void processDynUpdateServer(char *pData,int nLen,KxServer::IKxComm *target);
	//处理GM 命令数据转发聊天服 跑马灯
	static void processGMRouteChatServer(char *pData, int nLen,int nGroupID,int nKey,KxServer::IKxComm *target);

};

#endif //_SESSION_GM_SERVICE_H__

