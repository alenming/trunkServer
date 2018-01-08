/*
* 实现TCP协议Socket传输单元
* 主要解决TCP数据传输的问题
* 保证数据能够被完整地发送
* 
*  2015-05-04 By 宝爷
*
*/
#ifndef __TCPUNIT_H__
#define __TCPUNIT_H__

#include "KxCore.h"
#include "KxCommInterfaces.h"
#include "KxSock.h"
#include "KxBufferList.h"

namespace KxServer {

class KxTCPUnit : public IKxComm
{
public:
    KxTCPUnit();
    virtual ~KxTCPUnit();

    // 初始化
    virtual bool init();

    // 发送数据
    virtual int sendData(const char* buffer, unsigned int len);
    // 接收数据
    virtual int recvData(char* buffer, unsigned int len);
    // 关闭该通讯对象
    virtual void close() { m_Socket->close(); }

    // 接收到数据时触发的回调，由IKxCommPoller调用
    virtual int onRecv();
    // 数据可被发送时触发的回调，由IKxCommPoller调用
    virtual int onSend();

    // 获取通讯ID（通常是socket fd）
    virtual KXCOMMID getCommId() { return m_Socket->getSockFd(); }
    // 获取Sock
    inline KxSock* getSock() { return m_Socket; }

protected:
    KxSock* m_Socket;                   // Socket对象
    char* m_SendBuffer;                 // 正在发送的缓冲区
    char* m_RecvBuffer;                 // 正在接收的缓冲区
    unsigned int m_SendBufferLen;       // 正在发送的缓冲区大小
    unsigned int m_RecvBufferLen;       // 正在接收的缓冲区大小
    unsigned int m_SendBufferOffset;    // 正在发送的缓冲区偏移
    unsigned int m_RecvBufferOffset;    // 正在接收的缓冲区偏移
    KxBufferList m_BufferList;          // 待发送的缓冲区列表
    static char* s_RecvBuffer;          // 全局接收缓冲区
};

}

#endif
