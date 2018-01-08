#include "ChatClient.h"
#include "Protocol.h"
#include "ChatServer.h"
#include "NetworkManager.h"

using namespace KxServer;
using namespace std;

#define HEARTBEATTIME                   (24*60*60)       //心跳时间 s

CChatClient::CChatClient()
: m_Permission(0)
, m_GuestId(0)
, m_UserId(0)
{
    // 设置客户端心跳超时
    m_TimerCallBack = new KxServer::KxTimerCallback<CChatClient>();
    setTimer();
}

CChatClient::~CChatClient()
{
    m_TimerCallBack->stop();
    KXSAFE_RELEASE(m_TimerCallBack);
}

int CChatClient::onRecv()
{
    m_TimerCallBack->stop();
    CChatServer::getInstance()->getTimerManager()->addTimer(
        m_TimerCallBack, HEARTBEATTIME, 0);
    return KxTCPClienter::onRecv();
}

bool CChatClient::setConServer(int nKey, int nValue)
{
    if (nKey == 0)
    {
        return false;
    }
    m_MapConKeyValue[nKey] = nValue;
    return true;
}

bool CChatClient::sendDataToServer(int nGroupID, int nKey, char *pszContext, int nLen)
{
    KxTCPConnector* pTcpConnector = NULL;
    pTcpConnector = static_cast<KxTCPConnector*>(CNetWorkManager::getInstance(
        )->getServer(nGroupID, getRouteValue(nKey)));
    if (pTcpConnector == NULL)
    {
        return false;
    }
    return pTcpConnector->sendData(pszContext, nLen) >= 0;
}

bool CChatClient::sendDataToGroupServer(int nGroupID, char *pszContext, int nLen)
{
    vector<KxServer::IKxComm*>* pVectConnector = CNetWorkManager::getInstance()->getGroupServer(nGroupID);
    if (pVectConnector == NULL)
    {
        return false;
    }

    for (vector<KxServer::IKxComm*>::iterator ator = pVectConnector->begin();
        ator != pVectConnector->end(); ++ator)
    {
        (*ator)->sendData(pszContext, nLen);
    }
    return true;
}

bool CChatClient::sendDataToAllServer(char *pszContext, int nLen)
{
    map<int, vector<KxServer::IKxComm*> >& allServer = CNetWorkManager::getInstance()->getAllServer();
    for (map<int, vector<KxServer::IKxComm*> >::iterator ator = allServer.begin();
        ator != allServer.end(); ++ator)
    {
        vector<KxServer::IKxComm*>& groupServer = ator->second;
        for (vector<KxServer::IKxComm*>::iterator iter = groupServer.begin();
            iter != groupServer.end(); ++iter)
        {
            // ps. 如果send失败触发onError，在onError中从NetWorkManager中移除，会导致崩溃
            (*iter)->sendData(pszContext, nLen);
        }
    }
    return true;
}

int CChatClient::getRouteValue(int nKey)
{
    map<int, int>::iterator ator = m_MapConKeyValue.find(nKey);
    if (ator == m_MapConKeyValue.end())
    {
        return 0;
    }
    return ator->second;
}

void CChatClient::setTimer()
{
    m_TimerCallBack->setCallback(this, &CChatClient::onTimer);
    CChatServer::getInstance()->getTimerManager()->addTimer(
        m_TimerCallBack, HEARTBEATTIME, 0);
    KXLOGDEBUG("CChatClient::setTimer()");
}

void CChatClient::onTimer()
{
    // 时间到了
    clean();
    KXLOGDEBUG("CChatClient::onTimer()");
}

void CChatClient::clean()
{
    if (m_Permission == 0)
    {
        CNetWorkManager::getInstance()->removeGuest(m_GuestId);
    }
    else
    {
        CNetWorkManager::getInstance()->removeUser(m_UserId);
    }

    m_TimerCallBack->stop();
    m_TimerCallBack->clean();
    if (m_Poller != NULL)
    {
        m_Poller->removeCommObject(this);
    }
}