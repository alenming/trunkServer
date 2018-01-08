#include "RankServer.h"

int main(int argc,char **argv)
{
	CRankServer *pServer = CRankServer::getInstance();
	if (pServer->initServerInfo(argc, argv))
	{
		pServer->startServer();
	}
	CRankServer::destroy();
	return 0;
}