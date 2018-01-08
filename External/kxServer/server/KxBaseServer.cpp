#include "KxBaseServer.h"
#include "KxTimerManager.h"
#include "LogConsoleHandler.h"
#include "KxPlatform.h"
#include "KxMemPool.h"
#include <signal.h>

namespace KxServer {

#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
    bool ctrlhandler(DWORD ev)
    {
        switch (ev)
        {
        case CTRL_C_EVENT:          // - 当用户按下了CTRL + C, 或者由GenerateConsoleCtrlEvent API发出.
        case CTRL_BREAK_EVENT:      // - 用户按下CTRL + BREAK, 或者由GenerateConsoleCtrlEvent API发出.
        case CTRL_CLOSE_EVENT:      // - 当试图关闭控制台程序，系统发送关闭消息。
        case CTRL_LOGOFF_EVENT:     // - 用户退出时，但是不能决定是哪个用户.
        case CTRL_SHUTDOWN_EVENT:   // - 当系统被关闭时.
            KxBaseServer::getInstance()->stopServer();
        default:
            break;
        }
        return true;
    }
#endif

void processSingal(int nSingal)
{
	switch (nSingal)
    {
#if KX_TARGET_PLATFORM == KX_PLATFORM_LINUX
	case SIGQUIT:
    case SIGKILL:
#endif
    case SIGINT:
    case SIGABRT:
	case SIGTERM:
	{
		KxBaseServer::getInstance()->stopServer();
	}
	break;
	default:
		break;
	}
}

KxBaseServer* KxBaseServer::m_Server = NULL;

KxBaseServer::KxBaseServer()
: m_Poller(NULL)
, m_TimerMgr(NULL)
, m_IsRunning(false)
{
    m_Server = this;
	m_Tick = 0;
}

KxBaseServer::~KxBaseServer()
{
}

void KxBaseServer::startServer()
{
#if KX_TARGET_PLATFORM == KX_PLATFORM_LINUX
	signal(SIGPIPE, SIG_IGN);
	signal(SIGQUIT, processSingal);
	signal(SIGKILL, processSingal);
#endif
#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrlhandler, true);
#endif
    signal(SIGINT, processSingal);
    signal(SIGABRT, processSingal);
    signal(SIGTERM, processSingal);

	if (!onServerInit())
	{
		return;
	}

	runServer();
	onServerUninit();
}

//server run
void KxBaseServer::runServer()
{
    m_IsRunning = true;
	while(m_IsRunning)
	{
		if (NULL != m_Poller)
		{
			int nRet = m_Poller->poll();

			if (nRet == 0)
			{
				m_Tick++;
				if (m_Tick >= 100)
				{
#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)
					usleep(1000);
#else
					Sleep(10);
#endif
				}
			}
			else
			{
				m_Tick = 1;
			}
		}

        if (NULL != m_TimerMgr)
        {
            m_TimerMgr->updateTimers();
        }

        onServerUpdate();
	}
}

void KxBaseServer::onServerUpdate()
{

}

bool KxBaseServer::onServerInit()
{
    if (NULL == m_TimerMgr)
    {
        m_TimerMgr = new KxTimerManager();
    }

	return true;
}

//server uninit
void KxBaseServer::onServerUninit()
{
	KXLOGDEBUG("Server Stop !!");
	m_IsRunning = false;
    KXSAFE_RELEASE(m_Poller);
    KXSAFE_RELEASE(m_TimerMgr);
    KxMemManager::destroy();
    KxLogger::destroy();
}

}
