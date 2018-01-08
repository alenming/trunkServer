#include "ChatListen.h"
#include "ChatServer.h"
#include "NetworkManager.h"

using namespace KxServer;


CChatListen::CChatListen()
{
}


CChatListen::~CChatListen()
{
}

// Accept时回调，根据KXCOMMID创建KxTCPClienter
KxTCPClienter* CChatListen::onAccept(KXCOMMID client)
{
    CChatClient* tcpClient = new CChatClient();
    CNetWorkManager *pNetWorkManager = CNetWorkManager::getInstance();
    if (tcpClient->init(client))
    {
        // 连接时置为游客，并为其生成游客ID
        unsigned int guestId = pNetWorkManager->getGuestId();
        tcpClient->setGuestId(guestId);
        if (!pNetWorkManager->addGuest(guestId, tcpClient))
        {
            KXLOGERROR("add guest error id %d", guestId);
            tcpClient->release();
            return NULL;
        }

        CChatServer::getInstance()->getMainPoller(
            )->addCommObject(tcpClient, tcpClient->getPollType());

        return tcpClient;
    }
    else
    {
        tcpClient->release();
        return NULL;
    }
}