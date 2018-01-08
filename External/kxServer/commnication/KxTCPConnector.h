/*
 * BufferList 缓冲区列表
 * 用于缓存数据块
 *
 *  2013-04-20 By 宝爷
 *  
 */
#ifndef __TCPCONNECTOR_H__
#define __TCPCONNECTOR_H__

#include "KxCore.h"
#include "KxTCPUnit.h"

namespace KxServer {

enum KXEVENT_CONNECTER
{
    KXEVENT_CONNECT_SUCCESS,
    KXEVENT_CONNECT_FAILE,
};

class KxTCPConnector : public KxTCPUnit
{
public:
	KxTCPConnector();
	virtual ~KxTCPConnector();

    // 连接指定的IP和端口
    virtual bool connect(const char* addr, int port, bool nonblock = true);

    // 接收到数据时触发的回调，由IKxCommPoller调用
    virtual int onSend();
    virtual int onRecv();
    
    virtual void onConnected(bool success);

protected:
    bool m_IsConnecting;    // 是否正在连接
};

}

#endif
