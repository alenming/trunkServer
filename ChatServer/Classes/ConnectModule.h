#ifndef __CHAT_CONNECT_MODULE_H__
#define __CHAT_CONNECT_MODULE_H__

#include "BaseModule.h"

class CConnectModule : public CBaseModule
{
public:
    CConnectModule(void);
    virtual ~CConnectModule(void);

    //游戏服务器返回服务器协议，转发给指定的前端
    virtual void processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target);

    //与游戏服务器断开连接，重新连接
    //（必须重新new一个，通讯ID变化，由框架清理相关资源）
    virtual void processError(KxServer::IKxComm *target);
    virtual void processEvent(int eventId, KxServer::IKxComm* target);
};

#endif
