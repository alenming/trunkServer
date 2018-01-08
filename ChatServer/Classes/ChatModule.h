#ifndef _CHAT_MODULE_H__
#define _CHAT_MODULE_H__

#include "BaseModule.h"

class CChatModule : public CBaseModule
{
public:
    CChatModule();
    ~CChatModule();

	// IKxComm对象接收到完整数据后，调用的回调
	virtual void processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target);
	// IKxComm对象发生错误时，调用的回调
	virtual void processError(KxServer::IKxComm *target);
	// IKxComm对象内部发生特殊事件时触发的回调——如TCP非阻塞连接成功
	virtual void processEvent(int eventId, KxServer::IKxComm* target) {}

protected:
private:
    //玩家断线
    void userDisconnect(KxServer::IKxComm *target);
};

#endif //_CHAT_MODULE_H__
