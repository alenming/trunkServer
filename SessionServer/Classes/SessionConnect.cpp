#include "SessionConnect.h"
#include "SessionServer.h"
#include "KXServer.h"
#include<string.h>

using namespace KxServer;

CSessionConnector::CSessionConnector()
: m_nGroupID(0)
, m_Port(0)
, m_TimerCallBack(NULL)
{
    memset(m_strIP, 0, sizeof(m_strIP));
}

CSessionConnector::~CSessionConnector()
{
    if (m_TimerCallBack != NULL)
    {
        m_TimerCallBack->stop();
        KXSAFE_RELEASE(m_TimerCallBack);
    }
}

// 连接指定的IP和端口
bool CSessionConnector::connect(const char* addr, int port, int nGroupID, bool nonblock)
{
    if (addr == NULL)
    {
        return false;
    }

    memcpy(m_strIP, addr, sizeof(m_strIP));
    m_Port = port;
	m_nGroupID = nGroupID;
    return KxTCPConnector::connect(m_strIP, m_Port, nonblock);
}

void CSessionConnector::onConnected(bool success)
{
    KxTCPConnector::onConnected(success);
    if (success)
    {
        KXLOGDEBUG("CSessionConnect::onConnected() success IP:%s,port:%d", m_strIP, m_Port);
    }
    else
    {
        KXLOGDEBUG("CSessionConnect::onConnected() faile IP:%s,port:%d", m_strIP, m_Port);
    }
}

// 发生错误时回调，由IKxCommPoller调用
int CSessionConnector::onError()
{
    int nError = KxTCPConnector::onError();
    KXLOGDEBUG("CSessionConnect::onError() IP:%s,port:%d", m_strIP, m_Port);
    setTimer(RECONNECT_TIME_INTERVAL);
    return nError;
}

// 设置定时重连	
void CSessionConnector::setTimer(int nSec)
{
    if (m_TimerCallBack == NULL)
    {
        m_TimerCallBack = new KxServer::KxTimerCallback<CSessionConnector>();
    }
    m_TimerCallBack->setCallback(this, &CSessionConnector::onTimer);
    CSessionServer::getInstance()->getTimerManager()->addTimer(
        m_TimerCallBack, nSec, 0);
}

// 定时触发
void CSessionConnector::onTimer()
{
    reconnect();
    KXSAFE_RELEASE(m_TimerCallBack);
}

// 重连
bool CSessionConnector::reconnect()
{
    close();
    if (!init() || !connect(m_strIP, m_Port))
    {
        return false;
    }

    IKxCommPoller* pPoller = CSessionServer::getInstance()->getMainPoller();
    pPoller->addCommObject(this, getPollType());
    return true;
}
