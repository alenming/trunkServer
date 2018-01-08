#include "BlueDiamondModel.h"
#include "StorageManager.h"
#include "ModelDef.h"
#include "RedisStorer.h"
#include "ConfManager.h"
#include "ConfHall.h"

using namespace std;

CBlueDiamondModel::CBlueDiamondModel()
{
	m_MapQQIndexNum.clear();
}


CBlueDiamondModel::~CBlueDiamondModel()
{
}

bool CBlueDiamondModel::init(int uid)
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_USER);
	if (m_pStorage == NULL)
	{
		return false;
	}

	m_Uid = uid;
	return Refresh();
}

bool CBlueDiamondModel::Refresh()
{
	//活动对应时限表
	CConfBDActive *pBDActive = dynamic_cast<CConfBDActive*>(
		CConfManager::getInstance()->getConf(CONF_BLUEDIAMOND_ACTIVE));
	if (pBDActive == NULL)
	{
		return false;
	}

	std::map<SBDActiveKey, SBDActiveData> MapActiveData = pBDActive->getBDAllData();
	for (std::map<SBDActiveKey, SBDActiveData>::iterator ator = MapActiveData.begin(); ator != MapActiveData.end(); ++ator)
	{
		getActiveData(ator->first.nActiveID, ator->first.nTaskID);
	}

	return true;
}

bool CBlueDiamondModel::getActiveData(int nActiveID, int nTaskID)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_Uid));
	if (pStorer == NULL)
	{
		return false;
	}

	string Key;
	Key = ModelKey::QQActiveKey(m_Uid, nActiveID);

	if (SUCCESS == pStorer->ExistKey(Key))
	{
		map<int, int> MapValue;

		if (SUCCESS == pStorer->GetHash(Key, MapValue))
		{
			m_MapQQIndexNum[nActiveID] = MapValue;
		}
	}

	return true;
}

//获取QQ蓝钻活动数据
bool CBlueDiamondModel::getQQActiveValue(int nActiveID, int nField, int &nIndex, bool bNew)
{
	if (bNew)
	{
		CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_Uid));
		if (pStorer == NULL)
		{
			nIndex = 0;
			return false;
		}

		string Key = ModelKey::QQActiveKey(m_Uid, nActiveID);
		if (SUCCESS == pStorer->ExistKey(Key))
		{
			map<int, int> MapValue;
			if (SUCCESS == pStorer->GetHash(Key, MapValue))
			{
				m_MapQQIndexNum.erase(nActiveID);
				m_MapQQIndexNum[nActiveID] = MapValue;
			}
		}
	}

	map<int, std::map<int, int> >::iterator ator = m_MapQQIndexNum.find(nActiveID);
	map<int, int>::iterator iter;
	if (ator == m_MapQQIndexNum.end())
	{
		nIndex = 0;
	}
	else
	{
		map<int, int> &MapValue = ator->second;
		iter = MapValue.find(nField);
		if (iter == MapValue.end())
		{
			nIndex = 0;
			return true;
		}

		nIndex = iter->second;
	}

	return true;
}

//设置QQ蓝钻活动数据
bool CBlueDiamondModel::setQQActiveValue(int nActiveID, int nField, int nIndex)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_Uid));
	if (pStorer == NULL)
	{
		return false;
	}

	string Key = ModelKey::QQActiveKey(m_Uid, nActiveID);
	map<int, map<int, int> >::iterator iter = m_MapQQIndexNum.find(nActiveID);

	if (iter == m_MapQQIndexNum.end())
	{
		if (SUCCESS != pStorer->SetHashByField(Key, nField, nIndex))
		{
			return false;
		}

		map<int, int> MapValue;
		MapValue[nField] = nIndex;
		m_MapQQIndexNum[nActiveID] = MapValue;
	}
	else
	{
		map<int, int> &MapValue = iter->second;
		MapValue[nField] = nIndex;
		if (SUCCESS != pStorer->SetHashByField(Key, nField, nIndex))
		{
			return false;
		}
	}

	return true;
}

//重置QQ蓝钻活动数据
bool CBlueDiamondModel::reSetQQActiveValue()
{
	//活动对应时限表
	CConfBDActive *pBDActive = dynamic_cast<CConfBDActive*>(
		CConfManager::getInstance()->getConf(CONF_BLUEDIAMOND_ACTIVE));
	if (pBDActive == NULL)
	{
		return false;
	}

	std::map<SBDActiveKey, SBDActiveData> MapActiveData = pBDActive->getBDAllData();
	for (std::map<SBDActiveKey, SBDActiveData>::iterator ator = MapActiveData.begin(); ator != MapActiveData.end(); ++ator)
	{
		if (ator->second.nActiveType == eBDActive_DailyType)
		{
			setQQActiveValue(ator->first.nActiveID, ator->first.nTaskID, 0);
		}
	}

	return true;
}

