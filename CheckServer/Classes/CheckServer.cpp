#include "CheckServer.h"
#include "LogConsoleHandler.h"
#include "CheckListen.h"
#include "CheckModule.h"
#include "ConfManager.h"
#include "StorageManager.h"
#include "LogFileHandler.h"
#include <string>
#include "HttpHelper.h"
#include "HttpCheckModel.h"

using namespace std;
using namespace KxServer;

CCheckServer* CCheckServer::m_pInstance = NULL;

CCheckServer::CCheckServer(void)
{
}

CCheckServer::~CCheckServer(void)
{
	KXSAFE_RELEASE(m_Poller);
}

CCheckServer* CCheckServer::getInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CCheckServer();
	}
	return m_pInstance;
}

void CCheckServer::destroy()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CCheckServer::onServerInit()
{
	KxBaseServer::onServerInit();

    KxLogger::getInstance()->setShowTime(true);
    KxLogger::getInstance()->setShowDate(true);
	KxLogger::getInstance()->addHandler(1, new KxLogConsoleHandler());
    KxLogFileHandler* pFileHandle = new KxLogFileHandler();
	char szLogPath[128] = {};
	snprintf(szLogPath, sizeof(szLogPath), "SUM_Check_%d", m_ServerInfo.serverId);
	pFileHandle->setFileName(szLogPath);
    pFileHandle->setFastModel(false);
    KxLogger::getInstance()->addHandler(2, pFileHandle);

	KXLOGDEBUG("==================================================");

	//1.初始化轮询器
	m_Poller = new KxSelectPoller();

	//2.读取配置文件创建Server，配置客户端连接处理的Module
	CCheckListen* listener = new CCheckListen();
	if (!listener->init() 
		|| !listener->listen(m_ServerInfo.port, (char*)m_ServerInfo.host.c_str()))
	{
		return false;
	}

	CCheckModule *pCheckModel = new CCheckModule();
	listener->setClientModule(pCheckModel);
	m_Poller->addCommObject(listener, listener->getPollType());

	// 初始化数据库
    if (!StorageManager::getInstance()->InitWithXML(STORAGE_XML_FILE))
	{
		return false;
	}

    if (!CUidModelHelper::getInstance()->init())
    {
        return false;
    }

	KXSAFE_RELEASE(listener);
	KXSAFE_RELEASE(pCheckModel);

	KXLOGDEBUG("===> CheckServer ServerID %d IP %s Port %d Launching ... ", m_ServerInfo.serverId, m_ServerInfo.host.c_str(), m_ServerInfo.port);
	return true;
}

void CCheckServer::onServerUpdate()
{
}

