#include "ChatServer.h"
#include "LogConsoleHandler.h"
#include "ChatModule.h"
#include "LogFileHandler.h"
#include <string>
#include "ChatListen.h"
#include "ChatConnect.h"
#include "ConnectModule.h"
#include "NetworkManager.h"
#include "KxEpoller.h"

using namespace std;
using namespace KxServer;

CChatServer* CChatServer::m_pInstance = NULL;

CChatServer::CChatServer(void)
{
}

CChatServer::~CChatServer(void)
{
	KXSAFE_RELEASE(m_Poller);
}

CChatServer* CChatServer::getInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CChatServer();
	}
	return m_pInstance;
}

void CChatServer::destroy()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}


bool CChatServer::initServerInfo(int argc, char ** argv)
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
        // 本身由session连接,并连接验证服务器
        else if (argc >= 2 && !strcmp(argv[0], "-checksg")) {
            argv++; argc--;
            ConnectServerConf csc;
            csc.serverGroup = atoi(argv[0]);
            csc.host = "127.0.0.1";
            argv++; argc--;
            if (argc >= 2 && !strcmp(argv[0], "-checkp")) {
                argv++; argc--;
				csc.port = atoi(argv[0]);
				m_ServerInfo.connectors.push_back(csc);
            }
            else
            {
                KXLOGERROR("error parameter, should be input \"-sc count!\" after \"-sg\"");
                return false;
            }
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

bool CChatServer::onServerInit()
{
    if (!KxBaseServer::onServerInit())
    {
        return false;
    }

    // 服务器配置
    if (!m_ServerConfig.create())
    {
        return false;
    }

    KxLogger::getInstance()->setShowTime(true);
    KxLogger::getInstance()->setShowDate(true);
    KxLogger::getInstance()->addHandler(1, new KxLogConsoleHandler());
    KxLogFileHandler* pFileHandle = new KxLogFileHandler();
    char szLogPath[128] = {};
    snprintf(szLogPath, sizeof(szLogPath), "SUM_Chat_%d", m_ServerInfo.serverId);
    pFileHandle->setFileName(szLogPath);
    pFileHandle->setFastModel(false);
    KxLogger::getInstance()->addHandler(2, pFileHandle);

#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)
    m_Poller = new KxEpoller();
#else
    m_Poller = new KxSelectPoller();
#endif
    CChatListen *pListener = new CChatListen();
	if (!pListener->init())
	{
		pListener->release();
		return false;
	}
	char *ip = NULL;
	if (m_ServerInfo.host != "0" && m_ServerInfo.host != "")
	{
		ip = (char *)m_ServerInfo.host.c_str();
	}
	if (!pListener->listen(m_ServerInfo.port, ip))
    {
        KXLOGDEBUG("===> init Or Bind Error");
        pListener->release();
        return false;
    }

    CChatModule* pChatModule = new CChatModule();
    pListener->setClientModule(pChatModule);
    m_Poller->addCommObject(pListener, pListener->getPollType());

    CConnectModule *pConnectModule = new CConnectModule();
    // 连接其他服务器
    for (std::vector<ConnectServerConf>::iterator iter = m_ServerInfo.connectors.begin();
        iter != m_ServerInfo.connectors.end(); ++iter)
    {
        KXLOGDEBUG("===> ChatServer Connect to Server: IP=%s, Port=%d", iter->host.c_str(), iter->port);
        CChatConnector *pConnector = new CChatConnector();
        if (!pConnector->init() || !pConnector->connect((char *)iter->host.c_str(), iter->port, true))
        {
            KXLOGERROR("===> ChatServer Connect to Server Failed!");
            return false;
        }

        pConnector->setModule(pConnectModule);
        m_Poller->addCommObject(pConnector, pConnector->getPollType());
        CNetWorkManager::getInstance()->addServer(iter->serverGroup, pConnector);
        KXSAFE_RELEASE(pConnector);
    }

    KXSAFE_RELEASE(pListener);
    KXSAFE_RELEASE(pChatModule);

	KXLOGDEBUG("===> ChatServer ServerID %d IP %s Port %d Launching ... ", m_ServerInfo.serverId, m_ServerInfo.host.c_str(), m_ServerInfo.port);
	return true;
}

void CChatServer::onServerUpdate()
{
}
