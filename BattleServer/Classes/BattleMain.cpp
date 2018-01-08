#include "BattleServer.h"

int main(int argc, char **argv)
{
	CBattleServer *pServer = CBattleServer::getInstance();
	if (pServer->initServerInfo(argc, argv))
	{
		pServer->startServer();
	}
    CBattleServer::destroy();
    return 0;
}