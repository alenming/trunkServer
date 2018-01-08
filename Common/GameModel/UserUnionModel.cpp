#include "UserUnionModel.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "ConfAnalytic.h"
#include "KXServer.h"

using namespace std;

CUserUnionModel::CUserUnionModel()
{
}


CUserUnionModel::~CUserUnionModel()
{
}

bool CUserUnionModel::init(int nUid)
{
	m_nUid = nUid;
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);
	if (NULL == m_pStorage)
	{
		return false;
	}
	m_strUserUnionkey = ModelKey::UserUnionKey(nUid);
	m_MapValue.clear();
	m_VectReardValue.clear();
	return Refresh();
}

bool CUserUnionModel::Refresh()
{
	m_MapValue.clear();
	m_VectReardValue.clear();
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (redisStorer == NULL)
	{
		return false;
	}

    for (int i = USER_UNION_EXPIDITION_TIME; i <= USER_UNION_MAX; i++)
	{
		m_MapValue[i] = 0;
	}

	if (SUCCESS != redisStorer->GetHashByField(m_strUserUnionkey, m_MapValue))
	{
		// 没有公会信息
		return true;
	}

	std::string StrValue;
	if (SUCCESS == redisStorer->GetHashByField(m_strUserUnionkey, USER_UNION_EXPIDITION_REWARD, StrValue))
	{
		StrValue = "[" + StrValue + "]";
		CConfAnalytic::ToJsonInt(StrValue, m_VectReardValue);
	}

    m_MapApplyInfo.clear();
    if (SUCCESS != redisStorer->GetHash(ModelKey::ApplyKey(m_nUid), m_MapApplyInfo))
    {
        return false;
    }

	return true;
}

bool CUserUnionModel::setUserUnionValue(int nField, int nValue)
{
	map<int, int>::iterator ator = m_MapValue.find(nField);
	if (nField <= USER_UNION_EXPIDITION_REWARD || nField >= USER_UNION_MAX)
	{
		return false;
	}
	m_MapValue[nField] = nValue;

	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (redisStorer == NULL)
	{
		return false;
	}
	if (SUCCESS != redisStorer->SetHashByField(m_strUserUnionkey, nField, nValue))
	{
		return false;
	}

	return true;
}

bool CUserUnionModel::setUserUnionValues(std::map<int, int> mapValue)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL == pStorer)
    {
        return false;
    }

    if (SUCCESS == pStorer->SetHash(m_strUserUnionkey, mapValue))
    {
        std::map<int, int>::iterator iter = mapValue.begin();
        for (; iter != mapValue.end(); ++iter)
        {
            m_MapValue[iter->first] = iter->second;
        }

        return true;
    }

    return false;
}

bool CUserUnionModel::getUserUnionValue(int nField, int &nValue)
{
    map<int, int>::iterator ator = m_MapValue.find(nField);
	if (ator == m_MapValue.end())
	{
		return false;
	}
	nValue = ator->second;
	return true;
}

const std::vector<int>& CUserUnionModel::getUserExpiditionValue()
{
	return m_VectReardValue;
}

bool CUserUnionModel::setUserExpiditionValue(std::vector<int> RewardValue)
{
	m_VectReardValue.clear();
	m_VectReardValue.assign(RewardValue.begin(), RewardValue.end());
	return saveUserRewardData();
}

bool CUserUnionModel::addUserUnionValue(int nField, int nValue /*= 1*/)
{
    if (nField != USER_UNION_DAYAPPLAYCOUNT
        && nField != USER_UNION_CONTRIBUTION)
    {
        return false;
    }

    map<int, int>::iterator ator = m_MapValue.find(nField);
    if (ator == m_MapValue.end())
    {
        return false;
    }

    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL != redisStorer)
    {
        if (SUCCESS == redisStorer->SetHashByField(m_strUserUnionkey, nField, ator->second + nValue))
        {
            ator->second += nValue;
            return true;
        }
    }
    
    return false;
}

bool CUserUnionModel::addApplyInfo(int unionID, int liveTime)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL != pStorer)
    {
        std::string key = ModelKey::ApplyKey(m_nUid);
        if (SUCCESS == pStorer->SetHashByField(key, unionID, liveTime))
        {
            m_MapApplyInfo[unionID] = liveTime;
            return true;
        }
    }
    return false;
}

bool CUserUnionModel::removeApplyInfo(int unionID)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL != pStorer)
    {
        std::map<int, int>::iterator iter = m_MapApplyInfo.find(unionID);
        if (iter != m_MapApplyInfo.end())
        {
            std::string key = ModelKey::ApplyKey(m_nUid);
            if (SUCCESS == pStorer->DelHashByField(key, unionID))
            {
                m_MapApplyInfo.erase(iter);
                return true;
            }
        }
    }
    return false;
}

bool CUserUnionModel::removeAllApplyInfo()
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL != pStorer)
    {
        std::string key = ModelKey::ApplyKey(m_nUid);
        if (SUCCESS != pStorer->ExistKey(key))
        {
            return true;
        }

        if (SUCCESS == pStorer->DelKey(key))
        {
            m_MapApplyInfo.clear();
            return true;
        }
    }
    return false;
}

bool CUserUnionModel::hasExpiditionReward()
{
    return !m_VectReardValue.empty();
}

bool CUserUnionModel::alterUserUnionValue(int nField, int nValue)
{
    std::map<int, int>::iterator iter = m_MapValue.find(nField);
    if (iter != m_MapValue.end())
    {
        iter->second = nValue;
        return true;
    }

    return false;
}

bool CUserUnionModel::alterUserUnionValueAdd(int nField, int nValue)
{
    std::map<int, int>::iterator iter = m_MapValue.find(nField);
    if (iter != m_MapValue.end())
    {
        iter->second += nValue;
        return true;
    }

    return false;
}

//角色不在线时，添加角色奖励数据
bool CUserUnionModel::setOffLineExpiditionValue(int nUid, std::vector<int> RewardValue)
{
	std::string Key = ModelKey::UserUnionKey(nUid);
	Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);
	if (NULL == pStorage)
	{
		return false;
	}
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(pStorage->GetStorer(nUid));
	if (redisStorer == NULL)
	{
		return false;
	}

	char szBuffer[256] = {};
	int nLen = 0;
	int nMaxLen = sizeof(szBuffer);
	for (vector<int>::iterator ator = RewardValue.begin(); ator != RewardValue.end(); ++ator)
	{
		if (ator == RewardValue.begin())
		{
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "%d", (*ator));
		}
		else
		{
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%d", (*ator));
		}
	}

	if (RewardValue.size() != 0)
	{
		if (SUCCESS != redisStorer->SetHashByField(Key, USER_UNION_EXPIDITION_REWARD, szBuffer, nLen))
		{
			return false;
		}
	}
	else
	{
		if (SUCCESS != redisStorer->DelHashByField(Key, USER_UNION_EXPIDITION_REWARD))
		{
			return false;
		}
	}

	return true;
}

bool CUserUnionModel::saveUserRewardData()
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (redisStorer == NULL)
	{
		return false;
	}

	char szBuffer[256] = {};
	int nLen = 0;
	int nMaxLen = sizeof(szBuffer);
	for (vector<int>::iterator ator = m_VectReardValue.begin(); ator != m_VectReardValue.end(); ++ator)
	{
		if (ator == m_VectReardValue.begin())
		{
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "%d",(*ator));
		}
		else
		{
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%d", (*ator));
		}
	}

	if (m_VectReardValue.size() != 0)
	{
		if (SUCCESS != redisStorer->SetHashByField(m_strUserUnionkey, USER_UNION_EXPIDITION_REWARD, szBuffer, nLen))
		{
			return false;
		}
	}
	else
	{
		if (SUCCESS != redisStorer->DelHashByField(m_strUserUnionkey, USER_UNION_EXPIDITION_REWARD))
		{
			return false;
		}
	}

	return true;
}

bool CUserUnionModel::getOffLineUserUnionValue(int nUid, int nField, int &nValue)
{
    Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);
    if (NULL == pStorage)
    {
        return false;
    }

    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(pStorage->GetStorer(nUid));
    if (redisStorer == NULL)
    {
        return false;
    }

    if (SUCCESS != redisStorer->GetHashByField(ModelKey::UserUnionKey(nUid), nField, nValue))
    {
        return false;
    }

    return true;
}

bool CUserUnionModel::setOffLineUserUnionValue(int nUid, int nField, int nValue)
{
    Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);
    if (NULL == pStorage)
    {
        return false;
    }

    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(pStorage->GetStorer(nUid));
    if (redisStorer == NULL)
    {
        return false;
    }

    if (SUCCESS != redisStorer->ExistKey(ModelKey::UserUnionKey(nUid))
        || SUCCESS != redisStorer->SetHashByField(ModelKey::UserUnionKey(nUid), nField, nValue))
    {
        return false;
    }

    return true;
}

bool CUserUnionModel::removeOffLineAllApplyInfo(int nUid)
{
    Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);
    if (NULL == pStorage)
    {
        return false;
    }

    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(pStorage->GetStorer(nUid));
    if (redisStorer == NULL)
    {
        return false;
    }

    if (SUCCESS != redisStorer->DelKey(ModelKey::ApplyKey(nUid)))
    {
        return false;
    }

    return true;
}

bool CUserUnionModel::removeOffLineApplyInfo(int nUid, int nUnionId)
{
    Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);
    if (NULL == pStorage)
    {
        return false;
    }

    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(pStorage->GetStorer(nUid));
    if (redisStorer == NULL)
    {
        return false;
    }

    if (SUCCESS != redisStorer->DelHashByField(ModelKey::ApplyKey(nUid), nUnionId))
    {
        return false;
    }

    return true;
}
