/*
* Gate入口模块
* 过滤前端会话服务器的连接
* 由GameServer的Listener调用
* 对于不符合要求的连接会被强制关闭
*
* 2013-11-1 By 宝爷
*
*/
#ifndef __GATE_MODULE_H__
#define __GATE_MODULE_H__

#include "KXServer.h"

class CGateModule : public KxServer::IKxModule
{
public:
	CGateModule(void);
	virtual ~CGateModule(void);

	//return the length your package need
	virtual int processLength(char* buffer, unsigned int len);

	//when you recv a msg, Process will be call, the target is the msg sender
	virtual void processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target);

	virtual void processError(KxServer::IKxComm *target) { }
};

#endif
