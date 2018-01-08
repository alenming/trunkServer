#include "ExpiditonRankModel.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "ConfAnalytic.h"
#include "KXServer.h"

using namespace std;

CExpiditonRankModel::CExpiditonRankModel()
{
}


CExpiditonRankModel::~CExpiditonRankModel()
{
}

bool CExpiditonRankModel::init(int nUnionID)
{
	m_nUnionID = nUnionID;
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);
	if (NULL == m_pStorage)
	{
		return false;
	}

	m_strExpiditionRankkey = ModelKey::UnionExpiditionDamageKey(nUnionID);
	m_strExpiditionRankDataKey = ModelKey::UnionExpiditionDamageDataKey(nUnionID);
	m_MapShowRankData.clear();

	return Refresh();
}
bool CExpiditonRankModel::Refresh()
{
	return buildDamageRank();
}

//添加数据到远征伤害排行榜中
bool CExpiditonRankModel::addDamageData(int nUid, int &nDamage, SExpiditionRankDBData &DBData)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
	if (redisStorer == NULL)
	{
		return false;
	}

	if (SUCCESS != redisStorer->Zadd(m_strExpiditionRankkey, nUid, nDamage))
	{
		return false;
	}

	if (SUCCESS != redisStorer->SetHashByField(m_strExpiditionRankDataKey, nUid, (char*)&DBData, sizeof(SExpiditionRankDBData)))
	{
		return false;
	}

	return true;
}

//获取某个远征排行榜数据
bool CExpiditonRankModel::getDamageData(int nUid, SDamageRankShowData &ShowData)
{
	std::map<int, SDamageRankShowData>::iterator ator = m_MapShowRankData.find(nUid);

	if (ator != m_MapShowRankData.end())
	{
		memcpy(&ShowData, &ator->second, sizeof(ShowData));
		return true;
	}
	else
	{
		CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
		if (redisStorer == NULL)
		{
			return false;
		}

		if (SUCCESS != redisStorer->ZRank(m_strExpiditionRankkey, nUid, ShowData.nIndex))
		{
			ShowData.nIndex = 0;
			return false;
		}

		if (SUCCESS != redisStorer->ZScore(m_strExpiditionRankkey, nUid, ShowData.nDamage))
		{
			ShowData.nDamage = 0;
			return false;
		}

		SExpiditionRankDBData DBData;
		int nLen = sizeof(SExpiditionRankDBData);
		if (SUCCESS != redisStorer->GetHashByField(m_strExpiditionRankDataKey, nUid, (char*)&DBData,nLen))
		{
			return false;
		}
		memcpy(ShowData.nHeroID, DBData.nHeroID, sizeof(DBData.nHeroID));
		memcpy(ShowData.nStartID, DBData.nStartID, sizeof(DBData.nStartID));
		ShowData.nSummerID = DBData.nSummerID;
		memcpy(ShowData.szName, DBData.szName, sizeof(DBData.szName));
	}

	return true;
}

//获取排名
bool CExpiditonRankModel::getDamageRank(int nUid, int &nRank)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
	if (redisStorer == NULL)
	{
		return false;
	}

	if (SUCCESS != redisStorer->ZRank(m_strExpiditionRankkey, nUid,nRank))
	{
		nRank = 0;
		return false;
	}

	return true;
}

//设置地图ID
bool CExpiditonRankModel::setMapID(int nMapID)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
	if (redisStorer == NULL)
	{
		return false;
	}

	if (SUCCESS != redisStorer->SetHashByField(m_strExpiditionRankDataKey,0,nMapID))
	{
		return false;
	}

	m_nMapID = nMapID;
	return true;
}

//清除远征伤害排行榜数据
bool CExpiditonRankModel::clearDamageRankData()
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
	if (redisStorer == NULL)
	{
		return false;
	}
	m_nMapID = 0;
	m_MapShowRankData.clear();
	if (SUCCESS != redisStorer->DelKey(m_strExpiditionRankkey))
	{
		return false;
	}

	if (SUCCESS != redisStorer->DelKey(m_strExpiditionRankDataKey))
	{
		return false;
	}

	return true;
}

bool CExpiditonRankModel::buildDamageRank()
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
	if (redisStorer == NULL)
	{
		return false;
	}

	m_MapShowRankData.clear();
	m_nMapID = 0;
	map<int, RedisBinaryData> MapData;
	vector<RankData> DamageRankData;
	if (SUCCESS != redisStorer->ZRange(m_strExpiditionRankkey, DamageRankData, 0, 9))
	{
		return false;
	}

	redisStorer->GetHashByField(m_strExpiditionRankDataKey, 0, m_nMapID);

	if (SUCCESS != redisStorer->GetHash(m_strExpiditionRankDataKey, MapData))
	{
		return false;
	}

	int nIndex = 0;
	for (vector<RankData>::iterator ator = DamageRankData.begin(); ator != DamageRankData.end(); ++ator)
	{
		nIndex++;
		map<int, RedisBinaryData>::iterator iter = MapData.find(ator->id);

		if (iter != MapData.end())
		{
			SExpiditionRankDBData *pDBData = (SExpiditionRankDBData*)iter->second.data;
			SDamageRankShowData ShowData;
			ShowData.nDamage = ator->source;
			ShowData.nIndex = nIndex;
			memcpy(ShowData.nHeroID, pDBData->nHeroID, sizeof(pDBData->nHeroID));
			memcpy(ShowData.nStartID, pDBData->nStartID, sizeof(pDBData->nStartID));
			ShowData.nSummerID = pDBData->nSummerID;
			memcpy(ShowData.szName, pDBData->szName, sizeof(pDBData->szName));
			ShowData.cBDType = pDBData->cBDType;
			ShowData.cBDLev = pDBData->cBDLev;
			m_MapShowRankData[ator->id] = ShowData;
		}
	}

	return true;
}
