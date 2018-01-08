/*
*   
*
*
*/
#ifndef __SESSION_MODULE_H__
#define __SESSION_MODULE_H__

#include "BaseModule.h"

class CSessionServer;
class CServerModel;

class CSessionModule : public CBaseModule
{
public:
    CSessionModule(void);
    virtual ~CSessionModule(void);

    //处理客户端发来消息
    virtual void processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target);
    //客户端与Session服务器断开，通知游戏服务器，并清理相关资源
    virtual void processError(KxServer::IKxComm *target);

private:
    //玩家断线
    void userDisconnect(KxServer::IKxComm *target);
};

#endif
