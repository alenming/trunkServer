#include "TowerTestModel.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "KXServer.h"

CTowerTestModel::CTowerTestModel()
: m_nUid(0)
, m_pStorage(NULL)
, m_strTowerkey("")
{

}

CTowerTestModel::~CTowerTestModel()
{

}

bool CTowerTestModel::init(int uid)
{
	m_nUid = uid;
	m_strTowerkey = ModelKey::TowerTestKey(uid);
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_TOWERTEST);
	Refresh();
	return true;
}

bool CTowerTestModel::Refresh()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		std::map<int, int> tempTowerData;
		for (int i = TOWER_FD_NONE + 1; i < TOWER_FD_END; ++i)
		{
			if (i == TOWER_FD_OUTERBONUSLIST)
			{
				continue;
			}
			tempTowerData[i] = 0;
		}

		if (SUCCESS != pStorer->GetHashByField(m_strTowerkey, tempTowerData))
		{
			// 没有东西
			return false;
		}

		for (std::map<int, int>::iterator iter = tempTowerData.begin();
			iter != tempTowerData.end(); ++iter)
		{
			m_mapTowerData[iter->first] = iter->second;
		}

		m_mapOuterBonusList.clear();
		std::string outerBonusStr;
		if (SUCCESS == pStorer->GetHashByField(m_strTowerkey, TOWER_FD_OUTERBONUSLIST, outerBonusStr))
		{
			// 解析外部buffid[id1+id2+id3+...]
			std::string::size_type prev = 0;
			std::string::size_type pos = 0;
			while ((pos = outerBonusStr.find_first_of("+", prev)) != std::string::npos)
			{
				std::string strSub = outerBonusStr.substr(prev, pos);
				int outerId = atoi(strSub.c_str());
				m_mapOuterBonusList.push_back(outerId);
				prev = pos + 1; //跳过"+"
			}
			if (prev < outerBonusStr.size())
			{
				std::string lastSub = outerBonusStr.substr(prev, outerBonusStr.size());
				int lastOuterId = atoi(lastSub.c_str());
				if (lastOuterId > 0)
				{
					m_mapOuterBonusList.push_back(lastOuterId);
				}
			}
		}

		return true;
	}
	return false;
}

bool CTowerTestModel::SetTowerTestField(std::map<int, int> &values)
{
	if (values.find(TOWER_FD_OUTERBONUSLIST) != values.end())
	{
		return false;
	}

	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		for (std::map<int, int>::iterator iter = values.begin(); 
			iter != values.end(); ++iter)
		{
			m_mapTowerData[iter->first] = iter->second;
		}
		return SUCCESS == pStorer->SetHash(m_strTowerkey, values);
	}
	return false;
}

bool CTowerTestModel::SetTowerTestField(int field, int value)
{
	if (field == TOWER_FD_OUTERBONUSLIST)
	{
		return false;
	}
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL != pStorer && SUCCESS == pStorer->SetHashByField(m_strTowerkey, field, value))
	{
        m_mapTowerData[field] = value;
		return true;
	}
	return false;
}

bool CTowerTestModel::GetTowerTestField(std::map<int, int> &values)
{
	if (values.find(TOWER_FD_OUTERBONUSLIST) != values.end())
	{
		return false;
	}
	for (std::map<int, int>::iterator iter = values.begin(); 
		iter != values.end(); ++iter)
	{
		if (m_mapTowerData.find(iter->first) == m_mapTowerData.end())
		{
			return false;
		}
		values[iter->first] = m_mapTowerData[iter->first];
	}
	return true;
}

int CTowerTestModel::GetTowerTestField(int field)
{
	if (field == TOWER_FD_OUTERBONUSLIST)
	{
		return -1;
	}

	if (m_mapTowerData.find(field) != m_mapTowerData.end())
	{
		return m_mapTowerData[field];
	}
	return -1;
}

bool CTowerTestModel::AddOuterBonus(int outerId)
{
	m_mapOuterBonusList.push_back(outerId);

	std::string outerBonusStr = "";
	for (std::vector<int>::iterator iter = m_mapOuterBonusList.begin();
		iter != m_mapOuterBonusList.end(); ++iter)
	{
		char sub[32] = { 0 };
		snprintf(sub,sizeof(sub),"%d+", *iter);
		outerBonusStr += sub;
	}

	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		return SUCCESS == pStorer->SetHashByField(m_strTowerkey, TOWER_FD_OUTERBONUSLIST, outerBonusStr);
	}
	return false;
}

bool CTowerTestModel::AddOuterBonus(std::vector<int> &outerIds)
{
	if (outerIds.size() == 0)
	{
		//竟然没有, 不算错误
		return true;
	}

	m_mapOuterBonusList.insert(m_mapOuterBonusList.end(), outerIds.begin(), outerIds.end());

	std::string outerBonusStr = "";
	for (std::vector<int>::iterator iter = m_mapOuterBonusList.begin();
		iter != m_mapOuterBonusList.end(); ++iter)
	{
		char sub[32] = { 0 };
		snprintf(sub, sizeof(sub), "%d+", *iter);
		outerBonusStr += sub;
	}

	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		return SUCCESS == pStorer->SetHashByField(m_strTowerkey, TOWER_FD_OUTERBONUSLIST, outerBonusStr);
	}
	return false;
}

bool CTowerTestModel::ResetTowerTest()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		std::map<int, int> resetData;
		resetData[TOWER_FD_TIMES] = 1;
		resetData[TOWER_FD_FLOOR] = 1;
		resetData[TOWER_FD_STAGEID] = 0;
		resetData[TOWER_FD_FLOORSTATE] = 0;
		resetData[TOWER_FD_EVENTPARAM] = 0;
		resetData[TOWER_FD_INTEGRAL] = 0;
		resetData[TOWER_FD_STARS] = 0;

		if (SUCCESS != pStorer->SetHash(m_strTowerkey, resetData))
		{
			return false;
		}

		for (std::map<int, int>::iterator iter = resetData.begin();
			iter != resetData.end(); ++iter)
		{
			m_mapTowerData[iter->first] = iter->second;
		}

		if (SUCCESS != pStorer->SetHashByField(m_strTowerkey, TOWER_FD_OUTERBONUSLIST, ""))
		{
			return false;
		}
		return true;
	}
	return false;
}

std::vector<int> &CTowerTestModel::GetOuterBonusList()
{
	return m_mapOuterBonusList;
}
