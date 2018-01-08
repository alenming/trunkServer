#include "RankHelper.h"
#include "TimeCalcTool.h"
#include "ConfGameSetting.h"
#include "KxTimerManager.h"
#include "PvpRankModel.h"
#include "RankModel.h"

using namespace KxServer;
using namespace std;

CRankHelper* CRankHelper::m_pInstance = NULL;

CRankHelper::CRankHelper()
{
}


CRankHelper::~CRankHelper()
{
}

CRankHelper* CRankHelper::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CRankHelper();
	}

	return m_pInstance;
}

void CRankHelper::destroy()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CRankHelper::init(KxServer::KxBaseServer *pBaseServer)
{
	m_pServer = pBaseServer;
	int CurTime = (int)time(NULL);

	const TimeRecoverItem *pRecoverItem = queryConfTimeRecoverSetting();

	if (pRecoverItem == NULL)
	{
		return false;
	}

	//int nIterval = CTimeCalcTool::nextTimeStamp(CurTime, 0, pRecoverItem->AllTimeReset) - CurTime;
	//setTimer(nIterval, KXREPEAT_FOREVER);
	////备份前一天数据
	//CPvpRankModel *pPvpRankModel = CPvpRankModel::getInstance();
	//pPvpRankModel->bakPvpData();
	int nCurTime = static_cast<int>(time(NULL));
	SetTowerRankTimer(nCurTime);
	return true;
}

void CRankHelper::onTimer(const KxServer::kxTimeVal& now)
{
	//CPvpRankModel *pPvpRankModel = CPvpRankModel::getInstance();

	////备份7天以内的Key,删除7天以外的Key
	//pPvpRankModel->bakPvpData();

	//time_t CurTime = time(NULL) + 60;
	//tm NowTm;
	//stlocaltime(&CurTime, &NowTm);
	//if (NowTm.tm_wday == 1)
	//{
	//	pPvpRankModel->bakChampionRank();
	//}

	//const TimeRecoverItem *pRecoverItem = queryConfTimeRecoverSetting();
	//if (pRecoverItem == NULL)
	//{
	//	return;
	//}

	CRankModel *pRankModel = CRankModel::getInstance();
	pRankModel->DelRankData(TOWER_RANK_TYPE);

	stop();
	int nCurTime = static_cast<int>(time(NULL)) + 60;
	SetTowerRankTimer(nCurTime);
	/*CurTime = time(NULL);
	int nIterval = CTimeCalcTool::nextTimeStamp(CurTime, 0, pRecoverItem->AllTimeReset) - CurTime;
	if (nIterval < 60)
	{
	nIterval = nIterval + 24 * 3600;
	}
	setTimer(nIterval, KXREPEAT_FOREVER);*/
	KXLOGDEBUG("CPvpRankModel::onTimer CurTimes Is %d", nCurTime);
}

void CRankHelper::setTimer(int sec, int nNum)
{
	if (m_pServer == NULL)
	{
		return;
	}

	KxTimerManager *pTimerManager = m_pServer->getTimerManager();

	if (pTimerManager == NULL)
	{
		return;
	}

	pTimerManager->addTimer(this, sec, nNum);
}

void CRankHelper::SetTowerRankTimer(int nCurTime)
{
	const TowerTestSettingItem * pSettingConf = queryConfTowerSetting();
	CHECK_RETURN_VOID(NULL != pSettingConf);
	int nStartTime = CTimeCalcTool::curDayTimeZero(pSettingConf->nStartWeekDay);
	int nEndTime = nStartTime + pSettingConf->nInterval * 3600;
	int nInterval = 0;
	if (nCurTime < nStartTime)
	{
		nInterval = nStartTime - nCurTime;
	}
	else
	{
		nInterval = nStartTime + WEEKSECOND - nCurTime;
	}

	setTimer(nInterval, KXREPEAT_FOREVER);
}


