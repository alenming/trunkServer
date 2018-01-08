#include "BattleServer.h"
#include "BattleModule.h"
#include "GateModule.h"
#include "KXServer.h"
#include "LogConsoleHandler.h"
#include "LogFileHandler.h"
#include "LogBattleHandler.h"
#include "ConfManager.h"
using namespace KxServer;

CBattleServer* CBattleServer::m_Instance = NULL;
CBattleServer::CBattleServer(void)
{
}

CBattleServer::~CBattleServer(void)
{
}

CBattleServer* CBattleServer::getInstance()
{
    if (NULL == m_Instance)
    {
        m_Instance = new CBattleServer();
    }
    return m_Instance;
}

void CBattleServer::destroy()
{
    if (NULL != m_Instance)
    {
        delete m_Instance;
        m_Instance = NULL;
    }
}

bool CBattleServer::onServerInit()
{
	KXLOGDEBUG("==================================================");
	KXLOGDEBUG("===> BattleServer ServerID=%d Binding on Port=%d ...", m_ServerInfo.host.c_str(), m_ServerInfo.port);

	KxBaseServer::onServerInit();
	// 初始化日志
    KxLogger::getInstance()->setShowTime(true);
    KxLogger::getInstance()->setShowDate(true);
    KxLogger::getInstance()->addHandler(1, new KxLogConsoleHandler());
    KxLogFileHandler* fileHandle = new KxLogFileHandler();
	char szLogPath[128] = {};
	snprintf(szLogPath, sizeof(szLogPath), "SUM_Battle_%d", m_ServerInfo.serverId);
	fileHandle->setFileName(szLogPath);
    fileHandle->setFastModel(false);
    KxLogger::getInstance()->addHandler(2, fileHandle);
    /*
    CLogBattleHandler* battleHandle = new CLogBattleHandler();
    battleHandle->setFileName("ConformityServer");
    battleHandle->setFastModel(false);
    battleHandle->setTag(1 << 1);
    KxLogger::getInstance()->addHandler(4, battleHandle);
    */

    m_Poller = new KxSelectPoller();
    KxTCPListener *pListener = new KxTCPListener();
	if (!pListener->init() 
		|| !pListener->listen(m_ServerInfo.port, (char*)m_ServerInfo.host.c_str()))
    {
        KXLOGDEBUG("===> init Or Bind Error");
        pListener->release();
        return false;
    }

    CGateModule* gateModule = new CGateModule();
    CBattleModule* battleModule = new CBattleModule();
    pListener->setModule(gateModule);
    pListener->setClientModule(battleModule);
    m_Poller->addCommObject(pListener, pListener->getPollType());

	KXSAFE_RELEASE(pListener);
    KXSAFE_RELEASE(gateModule);
    KXSAFE_RELEASE(battleModule);

	// 配置表读取
	if (!CConfManager::getInstance()->init())
	{
		return false;
	}
    
	KXLOGDEBUG("===> BattleServer Launching ...");
    return true;
}
