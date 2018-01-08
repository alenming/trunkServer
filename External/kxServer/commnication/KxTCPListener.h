/*
 * TCP Socket 监听者
 * Client Module 用于给客户端配置对应的处理模块
 * Process Module 用于对客户端的连接进行处理，例如防火墙验证，IP过滤等等
 *
 *  2013-04-15 By 宝爷
 *  
 */
#ifndef __TCPLISTENER_H__
#define __TCPLISTENER_H__

#include "KxCore.h"
#include "KxCommInterfaces.h"
#include "KxTCPClienter.h"
#include "KxSock.h"

namespace KxServer {

class KxTCPListener : public IKxComm
{
public:
	KxTCPListener();
	virtual ~KxTCPListener();

    // 初始化Sock
    bool init();
    // 绑定到端口并监听
    bool listen(int port, char* addr = NULL, bool nonblock = true, int listenQ = 128);
    // Accept时回调，根据KXCOMMID创建KxTCPClienter
    virtual KxTCPClienter* onAccept(KXCOMMID client);

    // 接收到数据时触发的回调，由IKxCommPoller调用
    virtual int onRecv();
    // 数据可被发送时触发的回调，由IKxCommPoller调用
    virtual int onSend();

    // 获取通讯ID（通常是socket fd）
    virtual KXCOMMID getCommId() { return m_Socket->getSockFd(); }
    // 关闭该通讯对象
    virtual void close() { m_Socket->close(); }

    // 设置和获取客户端处理模块
    void setClientModule(IKxModule* module);
    inline IKxModule* getClientModule()	{ return m_ClientModule; }

    // 获取Sock
    inline KxSock* getSock() { return m_Socket; }

protected:
    // 发送数据
    virtual int sendData(const char* buffer, unsigned int len);
    // 接收数据
    virtual int recvData(char* buffer, unsigned int len);

protected:
	KxSock* m_Socket;
	IKxModule* m_ClientModule;
};

}

#endif
