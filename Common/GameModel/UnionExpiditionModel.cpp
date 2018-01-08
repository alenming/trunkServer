#include "UnionExpiditionModel.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "ConfAnalytic.h"
#include "KXServer.h"


using namespace std;


CUnionExpiditionModel::CUnionExpiditionModel()
{
}


CUnionExpiditionModel::~CUnionExpiditionModel()
{
}

bool CUnionExpiditionModel::init(int nUnionID)
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);
	if (NULL == m_pStorage)
	{
		return false;
	}

	m_nUnionID = nUnionID;
	m_MapBossHp.clear();
	m_SetFinishIndex.clear();
	m_MapValue.clear();
	m_strUnionExpiditionkey = ModelKey::UnionExpiditionKey(nUnionID);
	return Refresh();
}
bool CUnionExpiditionModel::Refresh()
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
	if (redisStorer == NULL)
	{
		return false;
	}

	for (int i = UNION_EXPIDITION_WORLDID; i <= UNION_EXPIDITION_REWARDSENDTIME; i++)
	{
		m_MapValue[i] = 0;
	}

	if (SUCCESS != redisStorer->GetHashByField(m_strUnionExpiditionkey, m_MapValue))
	{
		// 没有公会信息
		return true;
	}
	std::string StrValue;
	if (SUCCESS == redisStorer->GetHashByField(m_strUnionExpiditionkey, UNION_EXPIDITION_STAGELIST, StrValue))
	{
		StrValue = "[" + StrValue + "]";
		VecInt VectValue;
		CConfAnalytic::ToJsonInt(StrValue, VectValue);

		for (unsigned int i = 0; i < VectValue.size(); i++)
		{
			m_SetFinishIndex.insert(VectValue[i]);
		}
	}

	if (SUCCESS == redisStorer->GetHashByField(m_strUnionExpiditionkey, UNION_EXPIDITION_BOSS_HPLIST, StrValue))
	{
		StrValue = "[" + StrValue + "]";
		VecInt VectValue;
		CConfAnalytic::ToJsonInt(StrValue, VectValue);

		for (unsigned int i = 0; i < VectValue.size()/2; i++)
		{
			m_MapBossHp[VectValue[i * 2]] = VectValue[i * 2 + 1];
		}
	}

	if (SUCCESS == redisStorer->GetHashByField(m_strUnionExpiditionkey, UNION_EXPIDITION_STAGE_LIST, StrValue))
	{
		StrValue = "[" + StrValue + "]";
		std::vector<std::string> VectValue;
		CConfAnalytic::ToJsonStr(StrValue, VectValue);
		SUnionStageShowData Data;
		int nIndex = 0;
		for (unsigned int i = 0; i < VectValue.size() / 5; i++)
		{
			nIndex = atoi(VectValue[i*5].c_str());
			Data.cLevel = VectValue[i*5+1][0];
			Data.nHeadID = atoi(VectValue[i * 5 + 2].c_str());
			Data.nDamage = atoi(VectValue[i * 5 + 3].c_str());
			memcpy(Data.szName, VectValue[i * 5 + 4].c_str(), VectValue[i * 5 + 4].length() + 1);
			m_MapDamageShowData[nIndex] = Data;
		}
	}

	return true;
}
//获取当前BOSS血量
bool CUnionExpiditionModel::getCurExpiditionBossHp(int nIndex, int &BossHp)
{
	map<int, int>::iterator ator = m_MapBossHp.find(nIndex);

	if (ator == m_MapBossHp.end())
	{
		return false;
	}
	
	BossHp = ator->second;
	return true;
}
//设置BOSS血量
bool CUnionExpiditionModel::setCurExpiditionBossHp(int nIndex, int BossHp)
{
	map<int, int>::iterator ator = m_MapBossHp.find(nIndex);

	if (BossHp <= 0)
	{
		if (ator != m_MapBossHp.end())
		{
			m_MapBossHp.erase(nIndex);
		}
	}
	else
	{
		m_MapBossHp[nIndex] = BossHp;
	}

	return saveBossHpToDB();
}

//清除BOSS血量数据
bool CUnionExpiditionModel::clearCurExpiditionBossHp(int nIndex)
{
	if (nIndex != 0)
	{
		m_MapBossHp.erase(nIndex);
	}
	else
	{
		m_MapBossHp.clear();
	}
	
	return saveBossHpToDB();
}

//添加完成关卡
bool CUnionExpiditionModel::addExpiditionFinishIndex(int nIndex)
{
	m_SetFinishIndex.insert(nIndex);
	return saveExpiditionIndexToDB();
}
//校验关卡是否为完成关卡
bool CUnionExpiditionModel::checkExpiditionFinishIndex(int nIndex)
{
	set<int>::iterator ator = m_SetFinishIndex.find(nIndex);

	if (ator != m_SetFinishIndex.end())
	{
		return true;
	}

	return false;
}

//清除完成关卡
bool CUnionExpiditionModel::clearExpiditionFinishIndex()
{
	m_SetFinishIndex.clear();
	return saveExpiditionIndexToDB();
}

//获取远征属性
bool CUnionExpiditionModel::getExpiditionFieldValue(int nIndex, int &nValue)
{
	nValue = 0;
	map<int, int>::iterator ator = m_MapValue.find(nIndex);
	if (nIndex <= UNION_EXPIDITION_BOSS_HPLIST || ator == m_MapValue.end())
	{
		return false;
	}
	nValue = ator->second;
	return true;
}
//设置远征属性
bool CUnionExpiditionModel::setExpiditinoFieldVale(int nIndex, int nValue)
{
	map<int, int>::iterator ator = m_MapValue.find(nIndex);
	if (nIndex <= UNION_EXPIDITION_BOSS_HPLIST || ator == m_MapValue.end())
	{
		return false;
	}

	m_MapValue[nIndex] = nValue;
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
	if (redisStorer == NULL)
	{
		return false;
	}
	if (SUCCESS != redisStorer->SetHashByField(m_strUnionExpiditionkey, nIndex,nValue))
	{
		return false;
	}

	return true;
}

//设置关卡序列显示最高伤害
bool CUnionExpiditionModel::setStageIndexShowData(int nIndex, SUnionStageShowData &Data)
{
	std::map<int, SUnionStageShowData>::iterator ator = m_MapDamageShowData.find(nIndex);

	if (ator == m_MapDamageShowData.end())
	{
		m_MapDamageShowData[nIndex] = Data;
	}
	else
	{
		memcpy(&ator->second, &Data, sizeof(Data));
	}

	return saveStageIndexShowData();
}

bool CUnionExpiditionModel::clearStageIndexShowData()
{
	m_MapDamageShowData.clear();
	return clearStageIndexDBShowData();
}

bool CUnionExpiditionModel::getStageIndexShowData(int nIndex, SUnionStageShowData &Data)
{
	std::map<int, SUnionStageShowData>::iterator ator = m_MapDamageShowData.find(nIndex);

	if (ator == m_MapDamageShowData.end())
	{
		return false;
	}

	memcpy(&Data, &ator->second, sizeof(Data));
	return true;
}

bool CUnionExpiditionModel::saveBossHpToDB()
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
	if (redisStorer == NULL)
	{
		return false;
	}

	char szBuffer[256] = {};
	int nLen = 0;
	int nMaxLen = sizeof(szBuffer);
	for (map<int, int>::iterator ator = m_MapBossHp.begin(); ator != m_MapBossHp.end(); ++ator)
	{
		if (ator == m_MapBossHp.begin())
		{
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "%d+%d",ator->first,ator->second);
		}
		else
		{
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%d+%d", ator->first, ator->second);
		}
	}

	if (m_MapBossHp.size() != 0)
	{
		if (SUCCESS != redisStorer->SetHashByField(m_strUnionExpiditionkey, UNION_EXPIDITION_BOSS_HPLIST, szBuffer, nLen))
		{
			return false;
		}
	}
	else
	{
		if (SUCCESS != redisStorer->DelHashByField(m_strUnionExpiditionkey, UNION_EXPIDITION_BOSS_HPLIST))
		{
			return false;
		}
	}

	return true;
}

//保存完成关卡数据
bool CUnionExpiditionModel::saveExpiditionIndexToDB()
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
	if (redisStorer == NULL)
	{
		return false;
	}

	char szBuffer[256] = {};
	int nLen = 0;
	int nMaxLen = sizeof(szBuffer);
	for (set<int>::iterator ator = m_SetFinishIndex.begin(); ator != m_SetFinishIndex.end(); ++ator)
	{
		if (ator == m_SetFinishIndex.begin())
		{
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "%d",(*ator));
		}
		else
		{
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%d",(*ator));
		}
	}

	if (m_SetFinishIndex.size() != 0)
	{
		if (SUCCESS != redisStorer->SetHashByField(m_strUnionExpiditionkey, UNION_EXPIDITION_STAGELIST, szBuffer, nLen))
		{
			return false;
		}
	}
	else
	{
		if (SUCCESS != redisStorer->DelHashByField(m_strUnionExpiditionkey, UNION_EXPIDITION_STAGELIST))
		{
			return false;
		}
	}

	return true;
}

//保存关卡序列显示最高伤害数据
bool CUnionExpiditionModel::saveStageIndexShowData()
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
	if (redisStorer == NULL)
	{
		return false;
	}

	char szBuffer[4096] = {};
	int nLen = 0;
	int nMaxLen = sizeof(szBuffer);
	for (map<int, SUnionStageShowData>::iterator ator = m_MapDamageShowData.begin(); ator != m_MapDamageShowData.end(); ++ator)
	{
		SUnionStageShowData &Data = ator->second;
		int nIndex = ator->first;
		if (ator == m_MapDamageShowData.begin())
		{
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "%d", nIndex);
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%c", Data.cLevel);
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%d", Data.nHeadID);
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%d", Data.nDamage);
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%s", Data.szName);
		}
		else
		{
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%d", nIndex);
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%c", Data.cLevel);
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%d", Data.nHeadID);
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%d", Data.nDamage);
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%s", Data.szName);
		}
	}

	if (SUCCESS != redisStorer->SetHashByField(m_strUnionExpiditionkey, UNION_EXPIDITION_STAGE_LIST, szBuffer, nLen))
	{
		return false;
	}

	return true;
}
//清除关卡序列显示最高伤害数据
bool CUnionExpiditionModel::clearStageIndexDBShowData()
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
	if (redisStorer == NULL)
	{
		return false;
	}

	if (SUCCESS != redisStorer->DelHashByField(m_strUnionExpiditionkey, UNION_EXPIDITION_STAGE_LIST))
	{
		return false;
	}

	return true;
}

