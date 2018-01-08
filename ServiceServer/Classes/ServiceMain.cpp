#include "ServiceServer.h"

int main(int argc,char **argv)
{
	CServiceServer *pServer = CServiceServer::getInstance();
	if (pServer->initServerInfo(argc, argv))
	{
		pServer->startServer();
	}
	CServiceServer::destroy();
	return 0;
}
