#include "PvpModel.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"

CPvpModel::CPvpModel()
: m_nUid(0)
, m_pStorage(NULL)
{
}

CPvpModel::~CPvpModel()
{
}

bool CPvpModel::init(int uid)
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_PVP);
	if (NULL == m_pStorage)
	{
		return false;
	}

	m_nUid = uid;
	m_strPvpKey	= ModelKey::PvpKey(m_nUid);
	m_strPvpBattleKey = ModelKey::PvpBattleIdKey(m_nUid);

	Refresh();
	return true;
}

bool CPvpModel::Refresh()
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (pRedisStorer != NULL)
	{
		//ËùÓÐpvp¼üÖµ
		m_MapPvpValues.clear();
		for (int i = PVPCOMM_FD_DAYRESETSTAMP; i < PVP_FD_MAX; ++i)
		{
			m_MapPvpValues[i] = 0;
		}

		if (SUCCESS != pRedisStorer->GetHash(m_strPvpKey, m_MapPvpValues))
		{
			return false;
		}
		return true;
	}
	return false;
}

bool CPvpModel::GetPvpBattleId(int &battleId)
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (pRedisStorer != NULL)
	{
        if (SUCCESS == pRedisStorer->GetHashByField(m_strPvpBattleKey, PVPVERIFY_FD_BATTLEID, battleId))
        {
            return true;
        }
	}
    battleId = 0;
	return false;
}

bool CPvpModel::GetPvpRobotId(int &robotId)
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (pRedisStorer != NULL)
	{
        if (SUCCESS == pRedisStorer->GetHashByField(m_strPvpBattleKey, PVPVERIFY_FD_ROBOTID, robotId))
        {
            return true;
        }
	}
    robotId = 0;
	return false;
}

bool CPvpModel::SetPvpBattleId(int battleId, int robotId)
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (pRedisStorer != NULL)
	{
		std::map<int, int> mapValues;
		mapValues[PVPVERIFY_FD_BATTLEID] = battleId;
		mapValues[PVPVERIFY_FD_ROBOTID] = robotId;
		
		if (SUCCESS != pRedisStorer->SetHash(m_strPvpBattleKey, mapValues))
		{
			return false;
		}

		if (robotId != 0)
		{
            return true;
		}
		else
		{
			return SUCCESS == pRedisStorer->Expire(m_strPvpBattleKey, 30);
		}
	}
	return false;
}

bool CPvpModel::PersistPvpBattleKey()
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (pRedisStorer != NULL)
	{
		return SUCCESS == pRedisStorer->Persist(m_strPvpBattleKey);
	}

	return false;
}

bool CPvpModel::DeletePvpBattleKey()
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	return SUCCESS == pRedisStorer->DelKey(m_strPvpBattleKey);
}

bool CPvpModel::SetPvpField(int field, int value)
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (pRedisStorer != NULL)
	{
		m_MapPvpValues[field] = value;
		return SUCCESS == pRedisStorer->SetHashByField(m_strPvpKey, field, value);
	}
	return false;
}

bool CPvpModel::SetPvpField(std::map<int, int> &mapValues)
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (pRedisStorer != NULL)
	{
		for (std::map<int, int>::iterator iter = mapValues.begin();
			iter != mapValues.end(); ++iter)
		{
			m_MapPvpValues[iter->first] = iter->second;
		}
		return SUCCESS == pRedisStorer->SetHash(m_strPvpKey, mapValues);
	}
	return false;
}

bool CPvpModel::GetPvpField(int field, int &value, bool bNew)
{
	std::map<int, int>::iterator iter = m_MapPvpValues.find(field);
	if (iter != m_MapPvpValues.end())
	{
		if (bNew)
		{
			if (!GetRealFieldFromDB(field, value))
			{
				return false;
			}

			iter->second = value;
		}

		value = iter->second;
		return true;
	}
	else
	{
		if (!GetRealFieldFromDB(field, value))
		{
			return false;
		}

		m_MapPvpValues[field] = value;
		return true;
	}

	return false;
}

bool CPvpModel::GetPvpField(std::map<int, int> &mapValues, bool bNew)
{
    for (std::map<int, int>::iterator iter = mapValues.begin();
        iter != mapValues.end(); ++iter)
    {
        iter->second = m_MapPvpValues[iter->first];
    }
	return true;
}

bool CPvpModel::GetRealFieldFromDB(int field, int &value)
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	if (SUCCESS != pRedisStorer->GetHashByField(m_strPvpKey,field,value))
	{
		return false;
	}

	return true;
}
