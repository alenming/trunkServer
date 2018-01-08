#include "SummonerServer.h"
#include "KxLog.h"
#include <assert.h>

CSummonerServer::CSummonerServer()
{
	m_pNewInstance = this;

	m_ServerInfo.group = 0;
	m_ServerInfo.serverGroup = 0;
	m_ServerInfo.serverId = 0;
	m_ServerInfo.port = 6000;
	m_ServerInfo.host = "127.0.0.1";
}

CSummonerServer::~CSummonerServer()
{
}

CSummonerServer *CSummonerServer::m_pNewInstance = NULL;
CSummonerServer *CSummonerServer::getNewInstance()
{
	return m_pNewInstance;
}

bool CSummonerServer::initServerInfo(int argc, char **argv)
{
	if (argc < 2)
	{
		KXLOGERROR("usage: ./server [-g] group [-ip] ip [-p] port \
						[-sg] server group [-sc] server count");
		return false;
	}
	argv++; argc--;
	while (argc) {
		if (argc >= 2 && !strcmp(argv[0], "-ipv")) {
			argv++; argc--;
			m_ServerInfo.ipVer = atoi(argv[0]);
		}
		else if (argc >= 2 && !strcmp(argv[0], "-g")) {
			argv++; argc--;
			m_ServerInfo.group = atoi(argv[0]);
		}
		else if (argc >= 2 && !strcmp(argv[0], "-sg")) {
			argv++; argc--;
			m_ServerInfo.serverGroup = atoi(argv[0]);
		}
		else if (argc >= 2 && !strcmp(argv[0], "-si")) {
			argv++; argc--;
			m_ServerInfo.serverId = atoi(argv[0]);
		}
		else if (argc >= 2 && !strcmp(argv[0], "-ip")) {
			argv++; argc--;
			m_ServerInfo.host = argv[0];
		}
		else if (argc >= 2 && !strcmp(argv[0], "-p")) {
			argv++; argc--;
			m_ServerInfo.port = atoi(argv[0]);
		}
		else {
			fprintf(stderr, "Invalid argument: %s\n", argv[0]);
			KXLOGERROR("usage: ./server [-g] group [-ip] ip [-p] port \
						[-sg] server group [-sc] server count");
			return false;
		}
		argv++; argc--;
	}

	return true;
}
