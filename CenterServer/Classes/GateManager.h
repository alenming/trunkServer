/*
 * 入口管理器，用于管理唯一的Session入口
 * 
 * 2013-11-1 By 宝爷
 */

#ifndef __GATE_MANAGER_H__
#define __GATE_MANAGER_H__

#include "KXServer.h"
#include <vector>

class CGateManager
{
private:
    CGateManager(void);
    virtual ~CGateManager(void);

public:
    
    static CGateManager* getInstance();
    static void destroy();

    //发送给Session, 内容不限定, 功能自己定制.
    int Send(char* buffer, unsigned int len);
    //单发个指定用户消息
    int Transmit(int uid, int cmd, char* data, unsigned int len);
    //广播给多个用户消息
    int Broadcast(std::vector<int>& uids, int cmd, char* data, unsigned int len);
    //设置指定的网关
    void SetGate(KxServer::IKxComm* gate)
    {
        m_Gate = gate;
    }
    //获得指定网关
    KxServer::IKxComm* GetGate()
    {
        return m_Gate;
    }

private:

    KxServer::IKxComm* m_Gate;
    static CGateManager* m_Instance;
};

#endif
