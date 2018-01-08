#include "ChatServer.h"

int main(int argc, char **argv)
{
    CChatServer* server = CChatServer::getInstance();
	if (server->initServerInfo(argc, argv))
	{
		server->startServer();
	}

	CChatServer::destroy();
    return 0;
}
