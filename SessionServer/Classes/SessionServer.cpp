#include "SessionServer.h"
#include "SessionListener.h"
#include "SessionClient.h"
#include "ConnectModule.h"
#include "SessionModule.h"
#include "NetworkManager.h"
#include "LogConsoleHandler.h"
#include "SessionConnect.h"
#include "LogFileHandler.h"
#include <string>
#include "SummonerEvent.h"
#include "NetworkManager.h"
#include "KxPlatform.h"
#include "SessionEvent.h"

using namespace std;
using namespace KxServer;

CSessionServer* CSessionServer::m_Instance = NULL;
CSessionServer::CSessionServer(void)
:m_IsClosing(false)
{
	m_GameEvent = new CSessionEvent;
}

CSessionServer::~CSessionServer(void)
{
	KXSAFE_RELEASE(m_Poller);
	m_ServerConfig.clean();
	delete m_GameEvent;
}

CSessionServer* CSessionServer::getInstance()
{
    if (NULL == m_Instance)
    {
        m_Instance = new CSessionServer();
    }
    return m_Instance;
}

void CSessionServer::destroy()
{
    if (NULL != m_Instance)
    {
        delete m_Instance;
        m_Instance = NULL;
    }
}

bool CSessionServer::onServerInit()
{
	KXLOGDEBUG("==================================================");
	KxBaseServer::onServerInit();

    KxLogger::getInstance()->setShowTime(true);
    KxLogger::getInstance()->setShowDate(true);
    KxLogger::getInstance()->addHandler(1, new KxLogConsoleHandler());
    KxLogFileHandler* pFileHandle = new KxLogFileHandler();
    pFileHandle->setFileName("SUM_Session");
    pFileHandle->setFastModel(false);
    KxLogger::getInstance()->addHandler(2, pFileHandle);

	//1.初始化轮询器
#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)
	m_Poller = new KxEpoller();
#else
	m_Poller = new KxSelectPoller();
#endif
	// 服务器配置
	if (!m_ServerConfig.create())
	{
		return false;
	}

    CSessionListener* listener = new CSessionListener();
	if (!listener->init())
    {
        return false;
    }

	char *ip = NULL;
	if (m_ServerInfo.host != "0" && m_ServerInfo.host != "")
	{
		ip = (char *)m_ServerInfo.host.c_str();
	}
	if (!listener->listen(m_ServerInfo.port, ip))
	{
		return false;
	}

	CSessionModule *pSessionModel = new CSessionModule();
	listener->setClientModule(pSessionModel);
    m_Poller->addCommObject(listener, listener->getPollType());
	
	CConnectModule *pConnectModule = new CConnectModule();
	CSummonerEvent *pGameEvent = new CSummonerEvent();
	pConnectModule->init(pGameEvent);


	for (std::vector<ConnectServerConf>::iterator iter = m_ServerInfo.connectors.begin();
		iter != m_ServerInfo.connectors.end(); ++iter)
	{
		KXLOGDEBUG("===> SessionServer Connect to Server: IP=%s, Port=%d", iter->host.c_str(), iter->port);
		CSessionConnector *pConnector = new CSessionConnector();
		if (!pConnector->init() || !pConnector->connect((char *)iter->host.c_str(), iter->port,iter->serverGroup,true))
		{
			KXLOGERROR("===> SessionServer Connect to Server Failed!");
			return false;
		}
		pConnector->setModule(pConnectModule);
		m_Poller->addCommObject(pConnector, pConnector->getPollType());
		CNetWorkManager::getInstance()->addServer(iter->serverGroup, pConnector);
		KXSAFE_RELEASE(pConnector);
	}

	KXSAFE_RELEASE(pConnectModule);
	KXSAFE_RELEASE(listener);
	KXSAFE_RELEASE(pSessionModel);

	KXLOGDEBUG("===> SessionServer Launching IP=%s Port=%d", m_ServerInfo.host.c_str(), m_ServerInfo.port);

	return true;
}

void CSessionServer::setSessionServerClose()
{
	m_IsRunning = false;
}

void CSessionServer::setSessionServerClosing()
{
	m_IsClosing = true;
}

bool CSessionServer::dynamicUpdate()
{
	CNetWorkManager::getInstance()->closeAllServer();//释放之前的连接的sockfd，释放资源
	if (!getServerCoinfig()->updateConfig())//清空之前的配置，重新读取配置
	{
		return false;
	}

	std::map<int, ServerConfigData>* pMapConnectServer = m_ServerConfig.getConnServer();//连接后台服务器

	if (pMapConnectServer == NULL)
	{
		return false;
	}

	for (std::map<int, ServerConfigData>::iterator ator = pMapConnectServer->begin(); ator != pMapConnectServer->end(); ++ator)
	{
		ServerConfigData& ConfigData = ator->second;
		CSessionConnector *pConnector = new CSessionConnector();
		CConnectModule *pConnectModule = new CConnectModule();
		pConnector->setModule(pConnectModule);
		const char* ip = ConfigData.IP;
		if (strnlen(ip, sizeof(ConfigData.IP)) < 7)
		{
			ip = NULL;
		}
		if (!pConnector->init() || !pConnector->connect(ip, ConfigData.Port, true))
		{
			return false;
		}

		CSummonerEvent *pGameChangeClient = new CSummonerEvent();
		pConnectModule->init(pGameChangeClient);

		m_Poller->addCommObject(pConnector, pConnector->getPollType());
		CNetWorkManager::getInstance()->addServer(ConfigData.GroupID, pConnector);

		KXSAFE_RELEASE(pConnector);
		KXSAFE_RELEASE(pConnectModule);
	}
	
	return true;
}

bool CSessionServer::initServerInfo(int argc, char ** argv)
{
	int portDisp = 5000;
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
			portDisp = m_ServerInfo.port;
		}
		else if (argc >= 2 && !strcmp(argv[0], "-sg")) {
			argv++; argc--;
			m_ServerInfo.serverGroup = atoi(argv[0]);
		}
		else if (argc >= 2 && !strcmp(argv[0], "-connsg")) {
			argv++; argc--;
			ConnectServerConf csc;
			csc.serverGroup = atoi(argv[0]);
			// 其它服务器ip只能session服务器连接
			csc.host = "127.0.0.1";
			argv++; argc--;
			if (argc >= 2 && !strcmp(argv[0], "-connsc")) {
				argv++; argc--;
				int sc = atoi(argv[0]);
				for (int i = 0; i < sc; ++i)
				{
					csc.port = ++portDisp;
					m_ServerInfo.connectors.push_back(csc);
				}
			}
			else
			{
				KXLOGERROR("error parameter, should be input \"-sc count!\" after \"-sg\"");
				return false;
			}
		}
		else if (argc >= 2 && !strcmp(argv[0], "-chatsg")) {
			argv++; argc--;
			ConnectServerConf csc;
			csc.serverGroup = atoi(argv[0]);
			// 其它服务器ip只能session服务器连接
			csc.host = "127.0.0.1";
			argv++; argc--;
			if (argc >= 2 && !strcmp(argv[0], "-chatsc")) {
				argv++; argc--;
				int sc = atoi(argv[0]);
				for (int i = sc; i > 0; --i)
				{
					csc.port = m_ServerInfo.port - i;
					m_ServerInfo.connectors.push_back(csc);
				}
			}
			else
			{
				KXLOGERROR("error parameter, should be input \"-chatsc count!\" after \"-chatsg\"");
				return false;
			}
		}
		else {
			fprintf(stderr, "Invalid argument: %s\n", argv[0]);
			KXLOGERROR("usage: ./server [-g] group [-ip] ip [-p] port \
					   [-sg] server group [-sc] server count \
					   [-chatsg] chat server group [-chatsc] chat server count");
			return false;
		}
		argv++; argc--;
	}

	return true;
}
