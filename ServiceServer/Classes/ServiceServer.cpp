#include "ServiceServer.h"
#include "KXServer.h"
#include "LogConsoleHandler.h"
#include "GateModule.h"
#include "ServiceModule.h"
#include "StorageManager.h"
#include "ConfManager.h"
#include "MailModel.h"
#include "LogFileHandler.h"
#include "GameUserManager.h"
#include "RankModel.h"
#include "PvpRankModel.h"
#include "StatModel.h"
#include "GlobalMailModel.h"
#include "ActiveHelper.h"
#include "ChallengeRoomManager.h"
#include "HttpHelper.h"
#include "BillnoModel.h"
#include "GiftModel.h"

using namespace KxServer;

CServiceServer* CServiceServer::m_Instance = NULL;
CServiceServer::CServiceServer(void)
{
}

CServiceServer::~CServiceServer(void)
{
}

void CServiceServer::AddModelType()
{
	//初始化
	CGameUserManager::getInstance()->init(getTimerManager());
	//加载该服务器创建所需模型
	CGameUserManager::getInstance()->addModelType(MODELTYPE_USER);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_BAG);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_EQUIP);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_HERO);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_SUMMONER);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_STAGE);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_TEAM);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_TASK);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_ACHIEVEMENT);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_GUIDE);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_MAIL);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_INSTANCE);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_HEROTEST);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_GOLDTEST);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_TOWERTEST);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_PVP);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_SHOP);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_ACTIVE);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_HEAD);
    CGameUserManager::getInstance()->addModelType(MODELTYPE_USERUNION);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_MERCENARY);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_PVPCHEST);
	CGameUserManager::getInstance()->addModelType(MODELTYPE_BDACTIVE);
	CBillnoModel::getInstance()->init();
}

CServiceServer* CServiceServer::getInstance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new CServiceServer();
	}
	return m_Instance;
}

void CServiceServer::destroy()
{
	if (NULL != m_Instance)
	{
		CRankModel::destroy();
		CPvpRankModel::destroy();
		CStatModel::destroy();
		CActiveHelper::destroy();
		CChallengeRoomManager::destroy();
		CGiftModel::destroy();
		delete m_Instance;
		m_Instance = NULL;
	}
}

void CServiceServer::onServerUpdate()
{
}

bool CServiceServer::onServerInit()
{
    KxLogger::getInstance()->setShowTime(true);
	KxLogger::getInstance()->setShowDate(true);
	KxLogger::getInstance()->addHandler(1, new KxLogConsoleHandler());
    KxLogFileHandler* pFileHandle = new KxLogFileHandler();
	char szLogPath[128] = {};
	snprintf(szLogPath, sizeof(szLogPath), "SUM_Service_%d", m_ServerInfo.serverId);
	pFileHandle->setFileName(szLogPath);
    pFileHandle->setFastModel(false);
    KxLogger::getInstance()->addHandler(2, pFileHandle);

	if (KxBaseServer::onServerInit())
    {
		KXLOGDEBUG("==================================================");
		KXLOGDEBUG("===> ServiceServer ServerID = %d Binding on IP %s Port=%d ...", m_ServerInfo.serverId, m_ServerInfo.host.c_str(), m_ServerInfo.port);

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
		CServiceModule* serviceModule = new CServiceModule();
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

		AddModelType();

		CRankModel::getInstance()->init();
		CPvpRankModel::getInstance()->init(this);
		CStatModel::getInstance()->init();
		CGlobalMailModel::getInstance()->init();
		CGiftModel::getInstance()->init(m_ServerInfo.group);

		//初始化活动
		CActiveHelper::getInstance()->init(this);

		CChallengeRoomManager::getInstance()->init(getTimerManager());

        //创建公共Web邮件
        //CMailModel::CreateWebEmail();

		KXLOGDEBUG("===> ServiceServer Launching ...");
		return true;
	}
	
	return false;
}