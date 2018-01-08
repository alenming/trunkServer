#include "RankService.h"
#include "RankProtocol.h"
#include "Protocol.h"
#include "PvpRankModel.h"
#include "RankModel.h"
#include "KxMemPool.h"
#include "KxCommManager.h"

using namespace std;
using namespace KxServer;

int CRankService::m_LevelExpireTime = 0;	//排行榜数据过期时间
int CRankService::m_PvpExpireTime = 0;
int CRankService::m_UnionExpireTime = 0;
int CRankService::m_TowerExpireTime = 0;
int CRankService::m_ChampionExpireTime = 0;

CRankService::CRankService()
{
}


CRankService::~CRankService()
{
}

void CRankService::ProcessService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
	case CMD_RANKNFO_UI_CS:
		ProcessRankService(uid, buffer, len, commun);
		break;
	default:
		break;
	}
}

void CRankService::ProcessRankService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	if (buffer == NULL || len != sizeof(SRankInfoUI_CS))
	{
		return;
	}

	SRankInfoUI_CS *pMsg = (SRankInfoUI_CS *)buffer;

	time_t CurTime = time(NULL);
	switch (pMsg->nType)
	{
		case PVPRANK_TYPE:
		{
			if (m_PvpExpireTime < (int)CurTime)
			{
				CPvpRankModel::getInstance()->BuildPvpRank();
				m_PvpExpireTime = (int)CurTime + 5 * 60;
			}
			ProcessPvpRankService(uid, commun);
		}
		break;
		case LEVELRANK_TYPE:				//等级排行榜
		{
			if (m_LevelExpireTime < (int)CurTime)
			{
				CRankModel::getInstance()->BuildLevelRank();
				m_LevelExpireTime = (int)CurTime + 5 * 60;
			}
			ProcessLevelRankService(uid, commun);
		}
		break;
		case UNIONRANK_TYPE:				//公会排行榜
		{
			if (m_UnionExpireTime < (int)CurTime)
			{
				CRankModel::getInstance()->BuildUnionRank();
				m_UnionExpireTime = (int)CurTime + 5 * 60;
			}
			ProcessUnionRankService(uid, commun);
		}
		break;
		case TOWERRANK_TYPE:				//爬塔排行榜
		{
			if (m_TowerExpireTime < (int)CurTime)
			{
				CRankModel::getInstance()->BuildTowerRank();
				m_TowerExpireTime = (int)CurTime + 5 * 60;
			}
			ProcessTowerRankService(uid, commun);
		}
		break;
		case CHAMPIONRANK_TYPE:
		{
			if (m_ChampionExpireTime < (int)CurTime)
			{
				CPvpRankModel::getInstance()->BuildChampionRank();
				m_ChampionExpireTime = (int)CurTime + 5 * 60;
			}
			processChampionRankService(uid, commun);
		}
		break;
	default:
		break;
	}
}

//处理PVP排行榜
void CRankService::ProcessPvpRankService(int uid, KxServer::IKxComm *commun)
{
	map<SPvpRankKey, SPvpRankData>& PvpRankMap = CPvpRankModel::getInstance()->GetShowPvpRankMap();

	int nBuffSize = sizeof(SRankInfoUI_SC)+PvpRankMap.size() * sizeof(SRankShowPvpData);
	char *buff = reinterpret_cast<char*>(kxMemMgrAlocate(nBuffSize));

	int nIndex = 1;
	SRankInfoUI_SC *pRankInfoUISC = (SRankInfoUI_SC *)buff;
	SRankShowPvpData *pShowPvpData = (SRankShowPvpData *)(pRankInfoUISC + 1);

	pRankInfoUISC->nIndex = 0;
	if (!CPvpRankModel::getInstance()->GetPvpRank(uid, pRankInfoUISC->nIndex))
	{
		pRankInfoUISC->nIndex = 0;
	}

	pRankInfoUISC->nNum = PvpRankMap.size();
	pRankInfoUISC->nType = PVPRANK_TYPE;

	for (map<SPvpRankKey, SPvpRankData>::iterator ator = PvpRankMap.begin(); ator != PvpRankMap.end(); ++ator)
	{
		const SPvpRankKey& PvpRankKey = ator->first;
		const SPvpRankData& PvpRakData = ator->second;

		//保证排行榜数据一致
		if (uid == PvpRankKey.nUid)
		{
			pRankInfoUISC->nIndex = nIndex;
		}

		pShowPvpData->nIndex = nIndex;
		pShowPvpData->nHeadID = PvpRakData.nHeadID;
		pShowPvpData->nScore = PvpRankKey.nScore;
		pShowPvpData->nLevel = PvpRakData.nLevel;
		pShowPvpData->cBDType = PvpRakData.cBDType;
		pShowPvpData->cBDLev = PvpRakData.cBDLev;
		strncpys(pShowPvpData->szName, PvpRakData.UserName.c_str(), sizeof(pShowPvpData->szName));
		strncpys(pShowPvpData->szUnionName, PvpRakData.UnionName.c_str(), sizeof(pShowPvpData->szUnionName));
		nIndex++;
		pShowPvpData++;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_RANK, CMD_RANKNFO_UI_SC, buff, nBuffSize,commun);
	kxMemMgrRecycle(buff, nBuffSize);
}

//处理等级排行榜
void CRankService::ProcessLevelRankService(int uid, KxServer::IKxComm *commun)
{
	map<SLevelRankKey, SLevelRankData>& LevelRankMap = CRankModel::getInstance()->GetLevelRankData();

	int nBuffSize = sizeof(SRankInfoUI_SC)+LevelRankMap.size() * sizeof(SRankShowLevelData);
	char *buff = reinterpret_cast<char*>(kxMemMgrAlocate(nBuffSize));

	int nIndex = 1;
	SRankInfoUI_SC *pRankInfoUISC = (SRankInfoUI_SC *)buff;
	SRankShowLevelData *pShowLevelData = (SRankShowLevelData *)(pRankInfoUISC + 1);

	if (!CRankModel::getInstance()->GetRankData(LEVEL_RANK_TYPE, uid, pRankInfoUISC->nIndex))
	{
		pRankInfoUISC->nIndex = 0;
	}
	pRankInfoUISC->nNum = LevelRankMap.size();
	pRankInfoUISC->nType = LEVELRANK_TYPE;

	for (map<SLevelRankKey, SLevelRankData>::iterator ator = LevelRankMap.begin(); ator != LevelRankMap.end(); ++ator)
	{
		const SLevelRankKey& LevelRankKey = ator->first;
		const SLevelRankData& LevelRakData = ator->second;

		//保证排行榜数据一致
		if (uid == LevelRankKey.nUid)
		{
			pRankInfoUISC->nIndex = nIndex;
		}

		pShowLevelData->nIndex = nIndex;
		pShowLevelData->nHeadID = LevelRakData.nHeadID;
		pShowLevelData->nLevel = LevelRankKey.nLevel;
		pShowLevelData->cBDType = LevelRakData.cBDType;
		pShowLevelData->cBDLev = LevelRakData.cBDLev;
		strncpys(pShowLevelData->szName, LevelRakData.UserName.c_str(), sizeof(pShowLevelData->szName));
		strncpys(pShowLevelData->szUnionName, LevelRakData.UnionName.c_str(), sizeof(pShowLevelData->szUnionName));
		nIndex++;
		pShowLevelData++;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_RANK, CMD_RANKNFO_UI_SC, buff, nBuffSize, commun);
	kxMemMgrRecycle(buff, nBuffSize);
}

//处理公会排行榜
void CRankService::ProcessUnionRankService(int uid, KxServer::IKxComm *commun)
{
	map<SUnionRankKey, SUnionRankData>& UnionRankMap = CRankModel::getInstance()->GetUnionRankData();
	int nBuffSize = sizeof(SRankInfoUI_SC)+UnionRankMap.size() * sizeof(SRankShowUnionData);
	char *buff = reinterpret_cast<char*>(kxMemMgrAlocate(nBuffSize));

	int nIndex = 1;
	int nOffset = 0;
	SRankInfoUI_SC *pRankInfoUISC = (SRankInfoUI_SC *)buff;
	nOffset += sizeof(SRankInfoUI_SC);
	SRankShowUnionData *pShowUnionData = (SRankShowUnionData *)(pRankInfoUISC + 1);

	int nUnionID = 0;
	CRankModel::getInstance()->getUserRankID(uid, nUnionID);
	if (!CRankModel::getInstance()->GetRankData(UNION_RANK_TYPE, nUnionID, pRankInfoUISC->nIndex))
	{
		pRankInfoUISC->nIndex = 0;
	}

	pRankInfoUISC->nNum = UnionRankMap.size();
	pRankInfoUISC->nType = UNIONRANK_TYPE;

	for (map<SUnionRankKey, SUnionRankData>::iterator ator = UnionRankMap.begin(); ator != UnionRankMap.end(); ++ator)
	{
		const SUnionRankKey& UnionRankKey = ator->first;
		const SUnionRankData& UnionRakData = ator->second;

		if (nIndex >= MAX_RANK_SHOW_NUM+1)
		{
			pRankInfoUISC->nNum = MAX_RANK_SHOW_NUM;
			break;
		}

		//保证排行榜数据一致
		if (nUnionID == UnionRankKey.nUnionID)
		{
			pRankInfoUISC->nIndex = nIndex;
		}

		pShowUnionData->nIndex = nIndex;
		pShowUnionData->nUnionLevel = UnionRankKey.nUnionLevel;
		pShowUnionData->nUnionNum = UnionRakData.nUnionNum;
		pShowUnionData->nEmblemID = UnionRakData.nEmblemID;
		strncpys(pShowUnionData->szUnionName, UnionRakData.UnionName.c_str(), sizeof(pShowUnionData->szUnionName));
		nIndex++;
		pShowUnionData++;
		nOffset += sizeof(SRankShowUnionData);
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_RANK, CMD_RANKNFO_UI_SC, buff, nOffset, commun);
	kxMemMgrRecycle(buff, nBuffSize);
}

//处理爬塔排行榜
void CRankService::ProcessTowerRankService(int uid, KxServer::IKxComm *commun)
{
	map<STowerRankKey, STowerRankData>& TowerRankMap = CRankModel::getInstance()->GetTowerRankData();

	int nBuffSize = sizeof(SRankInfoUI_SC)+TowerRankMap.size() * sizeof(SRankShowTowerData);
	char *buff = reinterpret_cast<char*>(kxMemMgrAlocate(nBuffSize));

	int nIndex = 1;
	SRankInfoUI_SC *pRankInfoUISC = (SRankInfoUI_SC *)buff;
	SRankShowTowerData *pShowTowerData = (SRankShowTowerData *)(pRankInfoUISC + 1);

	if (!CRankModel::getInstance()->GetRankData(TOWER_RANK_TYPE, uid, pRankInfoUISC->nIndex))
	{
		pRankInfoUISC->nIndex = 0;
	}

	pRankInfoUISC->nNum = TowerRankMap.size();
	pRankInfoUISC->nType = TOWERRANK_TYPE;

	for (map<STowerRankKey, STowerRankData>::iterator ator = TowerRankMap.begin(); ator != TowerRankMap.end(); ++ator)
	{
		const STowerRankKey& TowerRankKey = ator->first;
		const STowerRankData& TowerRakData = ator->second;

		//保证排行榜数据一致
		if (uid == TowerRankKey.nUid)
		{
			pRankInfoUISC->nIndex = nIndex;
		}

		pShowTowerData->nIndex = nIndex;
		pShowTowerData->nHeadID = TowerRakData.nHeadID;
		pShowTowerData->nMaxTowerLevel = TowerRankKey.nMaxTowerLevel;
		pShowTowerData->nScore = TowerRankKey.nScore;
		pShowTowerData->nLevel = TowerRakData.nLevel;
		pShowTowerData->cBDType = TowerRakData.cBDType;
		pShowTowerData->cBDLev = TowerRakData.cBDLev;
		strncpys(pShowTowerData->szName, TowerRakData.UserName.c_str(), sizeof(pShowTowerData->szName));
		strncpys(pShowTowerData->szUnionName, TowerRakData.UnionName.c_str(), sizeof(pShowTowerData->szUnionName));
		nIndex++;
		pShowTowerData++;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_RANK, CMD_RANKNFO_UI_SC, buff, nBuffSize, commun);
	kxMemMgrRecycle(buff, nBuffSize);
}

//处理锦标赛排行榜
void CRankService::processChampionRankService(int uid, KxServer::IKxComm *commun)
{
	map<SPvpRankKey, SPvpRankData>& championMap = CPvpRankModel::getInstance()->getShowChampionMap();

	int nBuffSize = sizeof(SRankInfoUI_SC)+championMap.size() * sizeof(SRankShowPvpData);
	char *buff = reinterpret_cast<char*>(kxMemMgrAlocate(nBuffSize));

	int nIndex = 1;
	SRankInfoUI_SC *pRankInfoUISC = (SRankInfoUI_SC *)buff;
	SRankShowPvpData *pShowPvpData = (SRankShowPvpData *)(pRankInfoUISC + 1);

	pRankInfoUISC->nIndex = 0;
	if (!CPvpRankModel::getInstance()->getChampionRank(uid, pRankInfoUISC->nIndex))
	{
		pRankInfoUISC->nIndex = 0;
	}

	pRankInfoUISC->nNum = championMap.size();
	pRankInfoUISC->nType = CHAMPIONRANK_TYPE;

	for (map<SPvpRankKey, SPvpRankData>::iterator ator = championMap.begin(); ator != championMap.end(); ++ator)
	{
		const SPvpRankKey& PvpRankKey = ator->first;
		const SPvpRankData& PvpRakData = ator->second;

		//保证排行榜数据一致
		if (uid == PvpRankKey.nUid)
		{
			pRankInfoUISC->nIndex = nIndex;
		}

		pShowPvpData->nIndex = nIndex;
		pShowPvpData->nHeadID = PvpRakData.nHeadID;
		pShowPvpData->nScore = PvpRankKey.nScore;
		pShowPvpData->nLevel = PvpRakData.nLevel;
		pShowPvpData->cBDType = PvpRakData.cBDType;
		pShowPvpData->cBDLev = PvpRakData.cBDLev;
		strncpys(pShowPvpData->szName, PvpRakData.UserName.c_str(), sizeof(pShowPvpData->szName));
		strncpys(pShowPvpData->szUnionName, PvpRakData.UnionName.c_str(), sizeof(pShowPvpData->szUnionName));
		nIndex++;
		pShowPvpData++;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_RANK, CMD_RANKNFO_UI_SC, buff, nBuffSize, commun);
	kxMemMgrRecycle(buff, nBuffSize);
}

	

