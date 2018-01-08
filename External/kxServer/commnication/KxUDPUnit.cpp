#include "KxUDPUnit.h"
#include "KxMemPool.h"

// 单个UDP包的最大长度
#define MAX_UDP_PKG_LEN 2048

using namespace KxServer;

namespace KxServer {

KxUDPUnit::KxUDPUnit()
: m_Socket(NULL)
, m_RecvBuffer(NULL)
{
}

KxUDPUnit::~KxUDPUnit()
{
    if (NULL != m_RecvBuffer)
    {
        kxMemMgrRecycle(m_RecvBuffer, MAX_UDP_PKG_LEN);
    }
    m_Socket->release();
}

bool KxUDPUnit::init()
{
    m_Socket = new KxSock();
    if (m_Socket->init(KXSOCK_UDP))
    {
        return false;
    }
    
    m_RecvBuffer = static_cast<char*>(kxMemMgrAlocate(MAX_UDP_PKG_LEN));
    return m_RecvBuffer != NULL;
}

int KxUDPUnit::sendData(const char* buffer, unsigned int len)
{
    int ret = m_Socket->send(buffer, len);
    if (ret < 0 && !m_Socket->isSockBlockError())
    {
        onError();
    }
    return ret;
}

int KxUDPUnit::sendData(const char* buffer, unsigned int len, char* ip, int port)
{
    setSendToAddr(ip, port);
    return sendData(buffer, len);
}

int KxUDPUnit::recvData(char* buffer, unsigned int len)
{
	int ret = m_Socket->recv(buffer, len);
    if (ret < 0 && !m_Socket->isSockBlockError())
    {
        onError();
    }
    return ret;
}

int KxUDPUnit::onRecv()
{
    int ret = m_Socket->recv(m_RecvBuffer, MAX_UDP_PKG_LEN);
    if (ret < 0 && !m_Socket->isSockBlockError())
    {
        return -1;
    }
    else if (NULL != m_ProcessModule)
    {
        m_ProcessModule->processLogic(m_RecvBuffer, ret, this);
    }
	return ret;
}

int KxUDPUnit::onSend()
{
	return 0;
}

void KxUDPUnit::close()
{
    m_Socket->close();
}

int KxUDPUnit::bind(char* ip, int port)
{
	return m_Socket->bind(ip, port);
}

int KxUDPUnit::setSendToAddr(char* ip, int port)
{
	m_Socket->setSockAddr(ip, port);
	return 0;
}

}
