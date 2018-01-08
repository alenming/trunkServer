#include "SessionListener.h"
#include "SessionClient.h"
#include "SessionServer.h"
#include "NetworkManager.h"
#include "KxLog.h"

using namespace KxServer;

CSessionListener::CSessionListener()
{
}

CSessionListener::~CSessionListener(void)
{
}

KxTCPClienter* CSessionListener::onAccept(KXCOMMID client)
{
	CNetWorkManager *pNetWorkManager = CNetWorkManager::getInstance();
    CSessionClient* tcpClient = new CSessionClient();
    if (tcpClient->init(client))
    {
        // 连接时置为游客，并为其生成游客ID
        unsigned int guestId = pNetWorkManager->genGuestId();
        tcpClient->setGuestId(guestId);
        if (!pNetWorkManager->addGuest(guestId, tcpClient))
        {
            KXLOGERROR("add guest error id %d", guestId);
            tcpClient->release();
            return NULL;
        }

        CSessionServer::getInstance()->getMainPoller(
            )->addCommObject(tcpClient, tcpClient->getPollType());
        return tcpClient;
    }
    else
    {
        tcpClient->release();
        return NULL;
    }
}
