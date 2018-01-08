#include "RebotManager.h"
#include "LogConsoleHandler.h"
#include "ConfManager.h"
#include "StorageManager.h"
#include "LogFileHandler.h"
#include <string>
#include "PlayerModule.h"
#include "RebotPlayer.h"
#include "RebotConfig.h"
#include "RebotOpConfig.h"
#include "RebotComm.h"

using namespace std;
using namespace KxServer;

CRebotManager* CRebotManager::m_pInstance = NULL;

CRebotManager::CRebotManager()
{
}


CRebotManager::~CRebotManager()
{
	KXSAFE_RELEASE(m_Poller);
}

CRebotManager* CRebotManager::getInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CRebotManager();
	}
	return m_pInstance;
}

void CRebotManager::destroy()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CRebotPlayer * CRebotManager::GetRebotPlayer(int nUid)
{
	map<int, CRebotPlayer*>::iterator ator = m_MapRebot.find(nUid);

	if (ator == m_MapRebot.end())
	{
		return NULL;
	}

	return ator->second;
}

bool CRebotManager::onServerInit()
{
	KxLogger::getInstance()->setShowTime(false);
	KxLogger::getInstance()->setShowDate(false);
	KxLogger::getInstance()->addHandler(1, new KxLogConsoleHandler());
	KxLogFileHandler* fileHandle = new KxLogFileHandler();
	fileHandle->setFileName("RebotDebug");
	fileHandle->setFastModel(false);
	KxLogger::getInstance()->addHandler(2, fileHandle);

	KXLOGDEBUG("==================================================");
	CRebotConfig::getInstance()->init();
	CRebotOpConfig::getInstance()->init();

	//1.初始化轮询器
	m_Poller = new KxSelectPoller();

	// 初始化数据库
	/*if (!StorageManager::getInstance()->InitWithXML("../DBConfig/module_config.xml"))
	{
		return false;
	}*/

	string szTemp;
	szTemp = CRebotConfig::getInstance()->GetRebotConfig(emRebotMinUid);
	int nMinRebotUid = atoi(szTemp.c_str());
	szTemp = CRebotConfig::getInstance()->GetRebotConfig(emRebotMaxUid);
	int nMaxRebotUid = atoi(szTemp.c_str());
	string szIp = CRebotConfig::getInstance()->GetRebotConfig(emRebotOpIp);
	szTemp = CRebotConfig::getInstance()->GetRebotConfig(emRebotOpPort);
	int port = atoi(szTemp.c_str());

	for (int i = nMinRebotUid; i <= nMaxRebotUid; i++)
	{
		KxServer::KxTCPConnector *pConnector = new KxServer::KxTCPConnector();
		CPlayerModule *pPayModel = new CPlayerModule();
		pConnector->setModule(pPayModel);
		if (!pConnector->init() || !pConnector->connect(szIp.c_str(),port,false))
		{
			return false;
		}

		CRebotPlayer *pRebot = new CRebotPlayer();
		pRebot->init(i+1);
		pRebot->SetKxComm(pConnector);
		pPayModel->SetRebotPlayer(pRebot);
		m_Poller->addCommObject(pConnector, pConnector->getPollType());
		m_MapRebot[i + 1] = pRebot;
		KXSAFE_RELEASE(pConnector);
		KXSAFE_RELEASE(pPayModel);
	}

	return KxBaseServer::onServerInit();
}

void CRebotManager::onServerUpdate()
{
	//循环发送请求
	for (map<int, CRebotPlayer*>::iterator ator = m_MapRebot.begin();ator != m_MapRebot.end();++ator)
	{
		CRebotPlayer *pPlayer = ator->second;

		if (pPlayer != NULL)
		{
			pPlayer->PlayerOperator();
		}
	}

	string szTemp = CRebotConfig::getInstance()->GetRebotConfig(emRebotOpInterval);
	Sleep(atoi(szTemp.c_str()));
}

void CRebotManager::onServerUninit()
{

}


