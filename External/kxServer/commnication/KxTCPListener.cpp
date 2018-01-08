#include "KxTCPListener.h"
#include "KxLog.h"

namespace KxServer {

KxTCPListener::KxTCPListener()
: m_Socket(NULL)
, m_ClientModule(NULL)
{
}

KxTCPListener::~KxTCPListener()
{
    KXSAFE_RELEASE(m_Socket);
    KXSAFE_RELEASE(m_ClientModule);
}

// 初始化Sock
bool KxTCPListener::init()
{
    changePollType(KXPOLLTYPE_IN);
    m_Socket = new KxSock();
    return m_Socket->init(KXSOCK_TCP);
}

// 绑定到端口并监听
bool KxTCPListener::listen(int port, char* addr, bool nonblock, int listenQ)
{
    if (nonblock)
    {
        m_Socket->setSockNonblock();
    }
    m_Socket->setSockAddrReuse();
    m_Socket->setSockKeepAlive();
    if (0 > m_Socket->bind(addr, port)
        || 0 > m_Socket->listen(listenQ))
    {
        return false;
    }
    return true;
}

// Accept时回调
KxTCPClienter* KxTCPListener::onAccept(KXCOMMID client)
{
    KxTCPClienter* tcpClient = new KxTCPClienter();
    if (tcpClient->init(client))
    {
        return tcpClient;
    }
    else
    {
        KXLOGERROR("error: KxTCPListener::onAccept tcpClient %d init faile", client);
        tcpClient->release();
        return NULL;
    }
}

// 接收到数据时触发的回调，由IKxCommPoller调用
int KxTCPListener::onRecv()
{
    KxTCPClienter* client = NULL;
    while (true)
    {
        KXCOMMID fd = m_Socket->accept();
        if (KXINVALID_COMMID == fd
            || fd < 0)
        {
            // 检查错误
            int errorNo = m_Socket->getSockError();
			if (!m_Socket->isSockBlockError())
            {
                KXLOGERROR("error: KxTCPListener::onRecv accept error fd %d, errno %d", fd, errorNo);
            }
            break;
        }

        client = onAccept(fd);
        if (NULL != client)
        {
            client->setModule(m_ClientModule);
            if (NULL != m_ProcessModule)
            {
                // 需要进行retain
                m_ProcessModule->processLogic(NULL, 0, client);
            }
            client->release();
        }
    }
    return 0;
}

// 数据可被发送时触发的回调，由IKxCommPoller调用
int KxTCPListener::onSend()
{
    return 0;
}

void KxTCPListener::setClientModule(IKxModule* module)
{
    KXSAFE_RELEASE(m_ClientModule);
    m_ClientModule = module; 
    KXSAFE_RETAIN(m_ClientModule);
}

// 发送数据
int KxTCPListener::sendData(const char* buffer, unsigned int len)
{
    return 0;
}

// 接收数据
int KxTCPListener::recvData(char* buffer, unsigned int len)
{
    return 0;
}

}
