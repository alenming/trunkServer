#include "KXServer.h"
#include "MatchServer.h"

int main(int argc, char **argv)
{
	CMatchServer * pMatchServer = CMatchServer::getInstance();
	if (pMatchServer->initServerInfo(argc, argv))
	{
		pMatchServer->startServer();
	}
	CMatchServer::destroy();
	return 0;
}
