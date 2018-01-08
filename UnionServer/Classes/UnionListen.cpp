#include "UnionListen.h"
#include "UnionServer.h"
#include "GateManager.h"

using namespace KxServer;


CUnionListen::CUnionListen()
{
}


CUnionListen::~CUnionListen()
{
}

// Accept时回调，根据KXCOMMID创建KxTCPClienter
KxTCPClienter* CUnionListen::onAccept(KXCOMMID client)
{
    KxTCPClienter* tcpClient = new KxTCPClienter();
    if (tcpClient->init(client))
    {

        CUnionServer::getInstance()->getMainPoller(
            )->addCommObject(tcpClient, tcpClient->getPollType());

        //目前连接只能有一个
        CGateManager::getInstance()->SetGate(tcpClient);

        return tcpClient;
    }
    else
    {
        tcpClient->release();
        return NULL;
    }
}