#include "MatchServer.h"
#include "KXServer.h"
#include "KxLog.h"
#include "LogConsoleHandler.h"
#include "MatchModule.h"
#include "GateModule.h"
#include "LogFileHandler.h"
#include "ConfManager.h"
#include "StorageManager.h"
#include "GameUserManager.h"
#include "NewMatchManager.h"

using namespace KxServer;

CMatchServer::CMatchServer()
{
}

CMatchServer::~CMatchServer()
{
}

void CMatchServer::AddModelType()
{
	//初始化
	CGameUserManager::getInstance()->init(getTimerManager());
	//加载该服务器创建所需模型
	CGameUserManager::getInstance()->addModelType(MODELTYPE_PVP);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_USER);
}

CMatchServer * CMatchServer::m_pInstance = NULL;
CMatchServer * CMatchServer::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CMatchServer;
	}
	return m_pInstance;
}

void CMatchServer::destroy()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CMatchServer::onServerInit()
{
	KXLOGDEBUG("==================================================");
	KXLOGDEBUG("===> MatchServer %d Binding on IP %s Port=%d ...", m_ServerInfo.serverId, m_ServerInfo.host.c_str(), m_ServerInfo.port);

	KxBaseServer::onServerInit();
	KxLogger::getInstance()->setShowTime(true);
	KxLogger::getInstance()->setShowDate(true);
	KxLogger::getInstance()->addHandler(1, new KxLogConsoleHandler());

    KxLogFileHandler* pFileHandle = new KxLogFileHandler();

	char szLogPath[128] = {};
	snprintf(szLogPath, sizeof(szLogPath), "SUM_Match_%d", m_ServerInfo.serverId);
	pFileHandle->setFileName(szLogPath);
    pFileHandle->setFastModel(false);
    KxLogger::getInstance()->addHandler(2, pFileHandle);

	m_Poller = new KxSelectPoller();
	KxTCPListener *pListener = new KxTCPListener();
	if (!pListener->init() 
		|| !pListener->listen(m_ServerInfo.port, (char *)m_ServerInfo.host.c_str()))
	{
		KXLOGDEBUG("===> init Or Bind Error");
		pListener->release();
		return false;
	}
	
	CGateModule *pGateModule = new CGateModule();
	CMatchModule* pMatchModule = new CMatchModule();
	pListener->setModule(pGateModule);
	pListener->setClientModule(pMatchModule);
	m_Poller->addCommObject(pListener, pListener->getPollType());

	KXSAFE_RELEASE(pListener);
	KXSAFE_RELEASE(pGateModule);
	KXSAFE_RELEASE(pMatchModule);
	// 初始化数据库
    if (!StorageManager::getInstance()->InitWithXML(STORAGE_XML_FILE))
	{
		return false;
	}
	// 配置表读取
	if (!CConfManager::getInstance()->init())
	{
		return false;
	}
	//加载该服务器创建所需模型
	AddModelType();
	CNewMatchManager::getInstance();
	KXLOGDEBUG("===> MatchServer Launching ...");
	return true;
}
