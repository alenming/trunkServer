#include "KXServer.h"
#include "UnionServer.h"

int main(int argc, char **argv)
{
	CUnionServer* server = CUnionServer::getInstance();
	if (server->initServerInfo(argc, argv))
	{
		server->startServer();
	}
    CUnionServer::destroy();
    return 0;
}
