#include "CheckListen.h"
#include "CheckServer.h"

using namespace KxServer;


CCheckListen::CCheckListen()
{
}


CCheckListen::~CCheckListen()
{
}

KxTCPClienter* CCheckListen::onAccept(KXCOMMID client)
{
	KxTCPClienter* tcpClient = new KxTCPClienter();
	if (tcpClient->init(client))
	{
		CCheckServer::getInstance()->getMainPoller(
			)->addCommObject(tcpClient, tcpClient->getPollType());

      /*  CCheckServer::getInstance()->getMainPoller(
            )->addCommObject(this,getPollType());*/

		return tcpClient;
	}
	else
	{
		tcpClient->release();
		return NULL;
	}
}
