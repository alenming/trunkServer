#include "RankServer.h"
#include "KXServer.h"
#include "LogConsoleHandler.h"
#include "GateModule.h"
#include "RankModule.h"
#include "StorageManager.h"
#include "ConfManager.h"
#include "LogFileHandler.h"
#include "GameUserManager.h"
#include "RankModel.h"
#include "PvpRankModel.h"
#include "RankHelper.h"

using namespace KxServer;

CRankServer* CRankServer::m_Instance = NULL;
CRankServer::CRankServer(void)
{
}

CRankServer::~CRankServer(void)
{
}

CRankServer* CRankServer::getInstance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new CRankServer();
	}
	return m_Instance;
}

void CRankServer::destroy()
{
	if (NULL != m_Instance)
	{
		CRankModel::destroy();
		CPvpRankModel::destroy();
		delete m_Instance;
		m_Instance = NULL;
	}
}

void CRankServer::onServerUpdate()
{

}

bool CRankServer::onServerInit()
{
    KxLogger::getInstance()->setShowTime(true);
	KxLogger::getInstance()->setShowDate(true);
	KxLogger::getInstance()->addHandler(1, new KxLogConsoleHandler());
    KxLogFileHandler* pFileHandle = new KxLogFileHandler();
	char szLogPath[128] = {};
	snprintf(szLogPath, sizeof(szLogPath), "SUM_Rank_%d", m_ServerInfo.serverId);
	pFileHandle->setFileName(szLogPath);
    pFileHandle->setFastModel(false);
    KxLogger::getInstance()->addHandler(2, pFileHandle);

	if (KxBaseServer::onServerInit())
    {
      	KXLOGDEBUG("==================================================");
		KXLOGDEBUG("===> RankServer ServerID = %d Binding on IP %s Port=%d ...", m_ServerInfo.serverId, m_ServerInfo.host.c_str(), m_ServerInfo.port);

		m_Poller = new KxSelectPoller();
		KxTCPListener *listener = new KxTCPListener();
		if (!listener->init() 
			|| !listener->listen(m_ServerInfo.port, (char*)m_ServerInfo.host.c_str()))
		{
			KXLOGDEBUG("===> init Or Bind Error");
			listener->release();
			return false;
		}

		CGateModule* gateModule = new CGateModule();
		CRankModule* serviceModule = new CRankModule();
		listener->setModule(gateModule);
		listener->setClientModule(serviceModule);
		m_Poller->addCommObject(listener, listener->getPollType());
		KXSAFE_RELEASE(listener);
		KXSAFE_RELEASE(gateModule);
		KXSAFE_RELEASE(serviceModule);

		// 初始化数据库
        if (!StorageManager::getInstance()->InitWithXML(STORAGE_XML_FILE))
		{
			return false;
		}

		// 加载配表
		if (!CConfManager::getInstance()->init())
		{
			return false;
		}

		CRankModel::getInstance()->init();
		CPvpRankModel::getInstance()->init(this);
		CRankHelper::getInstance()->init(this);
		
		KXLOGDEBUG("===> RankServer Launching ...");
		return true;
	}
	
	return false;
}
