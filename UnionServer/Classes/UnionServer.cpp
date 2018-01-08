#include "UnionServer.h"
#include "LogConsoleHandler.h"
#include "UnionClient.h"
#include "UnionListen.h"
#include "UnionModule.h"
#include "ConfManager.h"
#include "StorageManager.h"
#include "UnionModel.h"
#include "UnionManager.h"
#include <string>
#include "GameUserManager.h"
#include "RankModel.h"
#include "LogFileHandler.h"

using namespace KxServer;

CUnionServer * CUnionServer::m_pInstance = NULL;

CUnionServer::CUnionServer()
{
}

CUnionServer::~CUnionServer()
{
}

void CUnionServer::AddModelType()
{
	//初始化
	CGameUserManager::getInstance()->init(getTimerManager());
	//加载该服务器创建所需模型
	CGameUserManager::getInstance()->addModelType(MODELTYPE_USER);
    CGameUserManager::getInstance()->addModelType(MODELTYPE_EQUIP);
    CGameUserManager::getInstance()->addModelType(MODELTYPE_HERO);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_USERUNION);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_MERCENARY);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_SUMMONER);
}

// 服务器初始化
bool CUnionServer::onServerInit()
{
    if (!KxBaseServer::onServerInit())
    {
        return false;
    }

    KxLogger::getInstance()->setShowTime(true);
    KxLogger::getInstance()->setShowDate(true);
    KxLogger::getInstance()->addHandler(1, new KxLogConsoleHandler());
	KxLogFileHandler* pFileHandle = new KxLogFileHandler();
	char szLogPath[128] = {};
	snprintf(szLogPath, sizeof(szLogPath), "SUM_Union_%d", m_ServerInfo.serverId);
	pFileHandle->setFileName(szLogPath);
	pFileHandle->setFastModel(false);
	KxLogger::getInstance()->addHandler(2, pFileHandle);

    //1.初始化轮询器
    m_Poller = new KxSelectPoller();

    //2.读取配置文件创建Server，配置客户端连接处理的Module
    CUnionListen* listener = new CUnionListen();
	if (!listener->init() 
		|| !listener->listen(m_ServerInfo.port, (char*)m_ServerInfo.host.c_str()))
    {
        return false;
    }

    CUnionModule *pUnionModule= new CUnionModule();
    listener->setClientModule(pUnionModule);
    m_Poller->addCommObject(listener, listener->getPollType());
	KXSAFE_RELEASE(listener);
	KXSAFE_RELEASE(pUnionModule);

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

	AddModelType();

	CRankModel::getInstance()->init();
    CUnionManager::getInstane()->init();

	KXLOGDEBUG("==========Start UnionServer serverid %d ip %s, port %d", m_ServerInfo.serverId, m_ServerInfo.host.c_str(), m_ServerInfo.port);
    return true;
}

CUnionServer * CUnionServer::getInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new CUnionServer();
    }

    return m_pInstance;
}

void CUnionServer::destroy()
{
    if (m_pInstance != NULL)
    {
        CUnionManager::destroy();
        delete m_pInstance;
        m_pInstance = NULL;
    }
}
