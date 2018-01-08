/*
 * 基础Socket通讯对象
 * 定义了非阻塞，非延迟，发送和接收接口
 * 定义Socket错误异常
 *
 *  2013-04-12 By 宝爷
 *  
 */
#ifndef __KX_SOCKET__
#define __KX_SOCKET__

#include "KxCore.h"

namespace KxServer {

enum KXSOCK_TYPE
{
    KXSOCK_UNKNOWN,
    KXSOCK_RAW,
    KXSOCK_TCP,
    KXSOCK_UDP
};

enum KXSOCK_CODE
{
    KXSOCK_ERRORUNKNOW = -9999,		    // 未知的Socket错误
    KXSOCK_ERRORFAILE,                  // Socket操作失败
    KXSOCK_ERRORTYPE,                   // Socket类型错误:非TCP和UDP，或UDP调用TCP操作

	KXSOCK_SUCCESS		= 0				// 成功返回
};

typedef sockaddr_in kxSocketAddr;

class KxSock : public KxObject
{
public:
	KxSock();
	
	virtual ~KxSock();

    static bool initSock();

    static void uninitSock();
	
	// 初始化Socket
    bool init(KXSOCK_TYPE type);
    // 根据SockFd初始化，主要用于初始化accept后的client
    bool init(KXCOMMID fd);
	
	// 监听，TCPSocket类型下可以调用
	int listen(int maxListenQueue);
	// 链接指定的IP和端口
	int connect(const char* addr, int port);
	// 绑定到指定的Ip和端口
	int bind(const char* addr, int port);
	// Accept返回一个Socket连接对象
	KXCOMMID accept();
    // 发送数据
    int send(const char* buffer, int size);
    // 接收数据
    int recv(char* buffer, int size);	
	// 关闭Socket
	void close();

	// 设置当前的SocketAddr,这个SocketAddr将会被用于UDP发送
	void setSockAddr(kxSocketAddr &name);
	void setSockAddr(const char* ip, int port);

    // 获取当前的Socket地址,这个地址在每次UDP接收以及SetAddr时会被更新
    inline const kxSocketAddr& getSockAddr() { return m_SockAddr; }

    // 设置非阻塞
	void setSockNonblock();
    // 设置keepalive
    void setSockKeepAlive();
	// 设置延迟（是否使用Ngale算法延时发送）
	void setSockNondelay();
    // 设置可重用地址
	void setSockAddrReuse();

    // 查询是否阻塞错误
    bool isSockBlockError();
    // 获取错误码
    int getSockError();
    // 打印错误信息
    void echoSockError(const char* msg = "");

    // 获取SockFD
    inline KXCOMMID getSockFd()
    {
        return m_Sock;
    }

private:
	//初始化Addr结构体，包含了不同平台的实现
	void sockInitAddr(kxSocketAddr &name, int port, const char* ip = NULL);

private:
    int             m_SockType;
    KXCOMMID        m_Sock;
    kxSocketAddr    m_SockAddr;
    static bool     s_IsInit;
};

}

#endif
