#include "KxSock.h"
#include "KxLog.h"

namespace KxServer {

bool KxSock::s_IsInit = false;

bool KxSock::initSock()
{
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    
    wVersionRequested = MAKEWORD(2, 2);
    
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        return false;
    }
    
    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2)
    {
        WSACleanup();
        return false;
    }
#endif
    s_IsInit = true;
    return true;
}

void KxSock::uninitSock()
{
#ifdef WIN32
    WSACleanup();
#endif
    s_IsInit = false;
}

KxSock::KxSock()
: m_SockType(KXSOCK_UNKNOWN)
, m_Sock(KXINVALID_COMMID)
{
	if (!s_IsInit)
	{
		initSock();
	}
}

KxSock::~KxSock()
{
	close();
}

bool KxSock::init(KXSOCK_TYPE type)
{
    if (KXINVALID_COMMID != m_Sock)
    {
        return false;
    }

    m_SockType = type;
    switch (m_SockType)
    {
        case KXSOCK_TCP:
            m_Sock = socket(AF_INET, SOCK_STREAM, 0);
            break;

        case KXSOCK_UDP:
            m_Sock = socket(AF_INET, SOCK_DGRAM, 0);
            break;

        default:
            break;
    }
    if (m_Sock == KXINVALID_COMMID)
    {
        echoSockError("init");
        return false;
    }
	return true;
}

bool KxSock::init(KXCOMMID fd)
{
    if (KXINVALID_COMMID != m_Sock)
    {
        KXLOGERROR("error: KxSock::init sock %d init with %d faile", m_Sock, fd);
        return false;
    }
    
    m_Sock = fd;
    m_SockType = KXSOCK_TCP;
    return m_Sock != KXINVALID_COMMID;
}

int KxSock::listen(int maxListenQueue)
{
    int ret = ::listen(m_Sock, maxListenQueue);
    if (ret < 0)
    {
        echoSockError("listen");
    }
    return ret;
}

int KxSock::connect(const char* addr, int port)
{
    KXLOGDEBUG("debug: KxSock::connect %s : %d socket %d", addr, port, m_Sock);
	sockaddr_in name;
	sockInitAddr(name, port, addr);
    return ::connect(m_Sock, (sockaddr*)&name, sizeof(sockaddr));
}

int KxSock::bind(const char* addr, int port)
{
	sockaddr_in name;
	sockInitAddr(name, port, addr);	
    int ret = ::bind(m_Sock, (sockaddr*)&name, sizeof(sockaddr));
    if (ret < 0)
    {
        echoSockError("bind");
    }
    return ret;
}

KXCOMMID KxSock::accept()
{
	sockaddr_in name;
	int len = sizeof(sockaddr);
    KXCOMMID ret = ::accept(m_Sock, (sockaddr*)&name, (kxSockLen*)&len);
    if (ret != KXINVALID_COMMID)
    {
        KXLOGDEBUG("debug: KxSock::accept ip %s prot %d socketId %d", inet_ntoa(name.sin_addr), name.sin_port, ret);
    }
    else
    {
        echoSockError("accept");
    }
    return ret;
}

int KxSock::send(const char* buffer, int size)
{
    switch (m_SockType)
    {
        case KXSOCK_TCP:
            return (int)::send(m_Sock, buffer, size, 0);
    
        case KXSOCK_UDP:
            return (int)::sendto(m_Sock, buffer, size, 0, (sockaddr*)&m_SockAddr, sizeof(m_SockAddr));
    
        default:
            return KXSOCK_ERRORTYPE;
    }
}

int KxSock::recv(char* buffer, int size)
{
    switch (m_SockType)
    {
        case KXSOCK_TCP:
            return (int)::recv(m_Sock, buffer, size, 0);

        case KXSOCK_UDP:
            {
                int len = sizeof(sockaddr);
                return (int)::recvfrom(m_Sock, buffer, size, 0, (sockaddr*)&m_SockAddr, (kxSockLen*)&len);
            }

        default:
            return KXSOCK_ERRORTYPE;
    }
}

void KxSock::close()
{
    KXLOGDEBUG("debug: KxSock::close %d close", m_Sock);
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    ::closesocket(m_Sock);
#else
	::close(m_Sock);
#endif
    m_Sock = KXINVALID_COMMID;
}

void KxSock::setSockAddr(kxSocketAddr &name)
{
	m_SockAddr = name;
}

void KxSock::setSockAddr(const char* ip, int port)
{
	sockInitAddr(m_SockAddr, port, ip);
}

void KxSock::setSockNonblock()
{
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    u_long nonblocking = 1;
	if (ioctlsocket(m_Sock, FIONBIO, &nonblocking) == SOCKET_ERROR)
    {
        echoSockError("setSockNonblock");
    }
#else
    int flags;
	if ((flags = fcntl(m_Sock, F_GETFL, NULL)) < 0) 
    {
        echoSockError("setSockNonblock fcntl F_GETFL");
        return;
    }
	if (fcntl(m_Sock, F_SETFL, flags | O_NONBLOCK) == -1) 
    {
        echoSockError("setSockNonblock fcntl F_SETFL");
        return;
    }
#endif
}

void KxSock::setSockKeepAlive()
{
    int on = 1;
	if (setsockopt(m_Sock, SOL_SOCKET, SO_KEEPALIVE, (char*)&on, sizeof(on))<0)
    {
        echoSockError("setSockKeepAlive setsockopt SO_KEEPALIVE");
    }
}

void KxSock::setSockNondelay()
{
	int on = 1;
	if (setsockopt(m_Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on)) < 0)
    {
        echoSockError("setSockNondelay");
    }
}

void KxSock::setSockAddrReuse()
{
    int on = 1;
	if (setsockopt(m_Sock, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0)
    {
        echoSockError("setSockAddrReuse");
    }
}

void KxSock::sockInitAddr(kxSocketAddr &name, int port, const char* ip)
{
	name.sin_family		= AF_INET;
	name.sin_port		= htons(port);
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    name.sin_addr.S_un.S_addr = (NULL == ip) ? htonl(INADDR_ANY) : inet_addr(ip);
#else
    name.sin_addr.s_addr = (NULL == ip) ? htonl(INADDR_ANY) : inet_addr(ip);
#endif
}

bool KxSock::isSockBlockError()
{
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    int errorCode = WSAGetLastError();
    if (WSAEWOULDBLOCK == errorCode || WSAEINPROGRESS == errorCode
        || WSAEINTR == errorCode)
    {
        return true;
    }
    KXLOGERROR("error: sock %d isSockBlockError %d ", m_Sock, errorCode);
#else
    if (errno == EWOULDBLOCK || errno == EAGAIN 
        || errno == EINPROGRESS || errno == EINTR)
    {
        return true;
    }
    KXLOGERROR("error: sock %d isSockBlockError %d %s", m_Sock, errno, strerror(errno));
#endif
    return false;
}

int KxSock::getSockError()
{
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    return WSAGetLastError();
#else
    return errno;
#endif
}

void KxSock::echoSockError(const char* msg)
{
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    int errorCode = WSAGetLastError();
    KXLOGERROR("error: KxSock::echoSockError sock %d msg %s SockError %d ", m_Sock, msg, errorCode);
#else
    KXLOGERROR("error: KxSock::echoSockError sock %d msg %s SockError %d %s", m_Sock, msg, errno, strerror(errno));
#endif
}

}
