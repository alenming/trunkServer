#include "UserActiveModel.h"
#include "StorageManager.h"
#include "ConfActive.h"
#include "ConfManager.h"
#include "ModelDef.h"
#include "RedisStorer.h"
#include "CommonHelper.h"
#include "UserModel.h"

using namespace std;


CUserActiveModel::CUserActiveModel()
{
	m_nCreatTime = 0;
}


CUserActiveModel::~CUserActiveModel()
{
}

bool CUserActiveModel::init(int uid)
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_USER);
	if (m_pStorage == NULL)
	{
		return false;
	}

	m_Uid = uid;
	getActiveCreatTime();

	return Refresh();
}

bool CUserActiveModel::Refresh()
{
	m_MapShopNum.clear();
	m_MapTaskNum.clear();
	m_MapTaskValue.clear();

	
	getNormalActiveData();
	get7DayActiveData();
	return true;
}

//设置商店活动数据
bool CUserActiveModel::SetShopActiveValue(int nActiveID, int nTimeInterval, int nField, int nValue)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_Uid));
	if (pStorer == NULL)
	{
		return false;
	}

	string Key = ModelKey::ActiveKey(m_Uid, nActiveID);
	map<int, std::map<int, int> >::iterator ator = m_MapShopNum.find(nActiveID);

	if (ator == m_MapShopNum.end())
	{
		if (SUCCESS != pStorer->SetHashByField(Key, nField, nValue))
		{
			return false;
		}

		if (SUCCESS != pStorer->Expire(Key, nTimeInterval))
		{
			return false;
		}

		map<int, int> MapValue;
		MapValue[nField] = nValue;
		m_MapShopNum[nActiveID] = MapValue;
	}
	else
	{
		map<int, int> &MapValue = ator->second;
		MapValue[nField] = nValue;

		if (SUCCESS != pStorer->SetHashByField(Key, nField, nValue))
		{
			return false;
		}
	}

	return true;
}

//获取商店活动数据
bool CUserActiveModel::GetShopActiveValue(int nActiveID, int nField, int &nValue,bool bNew)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_Uid));
	if (pStorer == NULL)
	{
		return false;
	}

	string Key = ModelKey::ActiveKey(m_Uid, nActiveID);
	if (bNew)
	{
		Key = ModelKey::ActiveKey(m_Uid,nActiveID);

		if (SUCCESS == pStorer->ExistKey(Key))
		{
			map<int, int> MapValue;

			if (SUCCESS == pStorer->GetHash(Key, MapValue))
			{
				m_MapShopNum.erase(nActiveID);
				m_MapShopNum[nActiveID] = MapValue;
			}
		}
	}

	map<int, std::map<int, int> >::iterator ator = m_MapShopNum.find(nActiveID);

	if (ator == m_MapShopNum.end())
	{
		nValue = 0;
	}
	else
	{
		map<int, int> &MapValue = ator->second;

		map<int, int>::iterator iter = MapValue.find(nField);

		if (iter == MapValue.end())
		{
			nValue = 0;
		}
		else
		{
			nValue = iter->second;
		}
	}

	return true;
}

//设置任务活动数据
bool CUserActiveModel::SetTaskActiveIndex(int nActiveID, int nTimeInterval, int nField, int nIndex)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_Uid));
	if (pStorer == NULL)
	{
		return false;
	}

	string Key = ModelKey::ActiveKey(m_Uid, nActiveID);
	map<int,map<int, int> >::iterator ator = m_MapTaskNum.find(nActiveID);

	if (ator == m_MapTaskNum.end())
	{
		if (SUCCESS != pStorer->SetHashByField(Key, nField,nIndex))
		{
			return false;
		}

		if (nTimeInterval != 0)
		{
			if (SUCCESS != pStorer->Expire(Key, nTimeInterval))
			{
				return false;
			}
		}

		map<int, int> MapValue;
		MapValue[nField] = nIndex;
		m_MapTaskNum[nActiveID] = MapValue;
	}
	else
	{
		map<int, int> &MapValue = ator->second;
		MapValue[nField] = nIndex;

		if (SUCCESS != pStorer->SetHashByField(Key, nField, nIndex))
		{
			return false;
		}
	}

	return true;
}

//设置任务活动数据
bool CUserActiveModel::SetTaskActiveValue(int nActiveID, int nTimeInterval, int nField, int nValue)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_Uid));
	if (pStorer == NULL)
	{
		return false;
	}

	string Key = ModelKey::TaskActiveValueKey(m_Uid,nActiveID);

	map<int, map<int,int> >::iterator iter = m_MapTaskValue.find(nActiveID);

	if (iter == m_MapTaskValue.end())
	{
		if (SUCCESS != pStorer->SetHashByField(Key, nField,nValue))
		{
			return false;
		}

		if (nTimeInterval != 0)
		{
			if (SUCCESS != pStorer->Expire(Key, nTimeInterval))
			{
				return false;
			}
		}

		map<int, int> MapValue;
		MapValue[nField] = nValue;
		m_MapTaskValue[nActiveID] = MapValue;
	}
	else
	{
		map<int, int> &MapValue = iter->second;
		MapValue[nField] = nValue;
		if (SUCCESS != pStorer->SetHashByField(Key, nField, nValue))
		{
			return false;
		}
	}

	return true;

}

//获得任务活动数据
bool CUserActiveModel::GetTaskActiveIndex(int nActiveID, int nField, int &nIndex, bool bNew)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_Uid));
	if (pStorer == NULL)
	{
		nIndex = 0;
		return false;
	}

	string Key = ModelKey::ActiveKey(m_Uid, nActiveID);

	if (SUCCESS != pStorer->ExistKey(Key))
	{
		nIndex = 0;
		return true;
	}

	if (bNew)
	{
		if (SUCCESS == pStorer->ExistKey(Key))
		{
			map<int, int> MapValue;

			if (SUCCESS == pStorer->GetHash(Key, MapValue))
			{
				m_MapTaskNum.erase(nActiveID);
				m_MapTaskNum[nActiveID] = MapValue;
			}
		}
	}

	map<int, std::map<int, int> >::iterator ator = m_MapTaskNum.find(nActiveID);
	map<int, int>::iterator iter;

	if (ator == m_MapTaskNum.end())
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

//获得某个活动的参数数据
bool CUserActiveModel::GetTaskActiveValue(int nActiveID, int nField, int &nValue, bool bNew)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_Uid));
	if (pStorer == NULL)
	{
		return false;
	}

	string Key = ModelKey::TaskActiveValueKey(m_Uid,nActiveID);
	map<int, map<int, int> >::iterator iter = m_MapTaskValue.find(nActiveID);

	if (bNew)
	{
		if (SUCCESS == pStorer->ExistKey(Key))
		{
			//这里有可能拿不到
			if (SUCCESS != pStorer->GetHashByField(Key,nField, nValue))
			{
				nValue = 0;
				return true;
			}

			if (iter == m_MapTaskValue.end())
			{
				map<int, int> MapValue;
				MapValue[nField] = nValue;
				m_MapTaskValue[nActiveID] = MapValue;
			}
			else
			{
				map<int, int> &MapValue = iter->second;
				MapValue[nField] = nValue;
			}
		}
	}

	if (iter == m_MapTaskValue.end())
	{
		nValue = 0;
	}
	else
	{
		map<int, int> &MapValue = iter->second;
		map<int, int>::iterator ator = MapValue.find(nField);

		if (ator == MapValue.end())
		{
			nValue = 0;
		}
		else
		{
			nValue = ator->second;
		}
	}

	return true;
}

//获取某个PID的参数数据
bool CUserActiveModel::getMonthCardValue(int uid,int nPid, int &nValue)
{
	Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_USER);
	if (pStorage == NULL)
	{
		return false;
	}

	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(pStorage->GetStorer(uid));
	if (pStorer == NULL)
	{
		return false;
	}

	string Key = ModelKey::pidKey(uid);
	if (SUCCESS == pStorer->ExistKey(Key))
	{
		//这里有可能拿不到
		if (SUCCESS != pStorer->GetHashByField(Key, nPid, nValue))
		{
			nValue = 0;
			return false;
		}
	}
	else
	{
		nValue = 0;
		return false;
	}

	return true;
}

//获取普通活动数据
bool CUserActiveModel::getNormalActiveData()
{
	int CurTime = time(NULL);
	//活动对应时限表
	CConfActiveTime *ActiveTime = dynamic_cast<CConfActiveTime*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TIME));
	if (ActiveTime == NULL)
	{
		return false;
	}

	map<int, void*> MapActiveData = ActiveTime->getDatas();
	for (map<int, void*>::iterator ator = MapActiveData.begin(); ator != MapActiveData.end(); ++ator)
	{
		SConfActiveTime *pActiveTime = static_cast<SConfActiveTime *>(ator->second);
		if (pActiveTime == NULL)
		{
			continue;
		}

		getActiveData(ator->first, pActiveTime->nActiveType);
	}

    return true;
}

//获取7天活动数据
bool CUserActiveModel::get7DayActiveData()
{
	int nCreateDay = CCommonHelper::getTotalDay(m_nCreatTime);
	int nCurDay = CCommonHelper::getTotalDay();
	int nDay = nCurDay - nCreateDay;

	//初始化7天活动
	CConf7DayActive *pConf7DayActive = dynamic_cast<CConf7DayActive*>(
		CConfManager::getInstance()->getConf(CONF_7DAY_ACTIVE));
	CHECK_RETURN(pConf7DayActive != NULL);

	S7DayActive *pDayActive = static_cast<S7DayActive *>(pConf7DayActive->getData(nDay + 1));
	CHECK_RETURN(pDayActive != NULL);

	for (int i = 0; i < pDayActive->VectActive.size(); i++)
	{
		getActiveData(pDayActive->VectActive[i].nActiveID, pDayActive->VectActive[i].nActiveType);
	}

	return true;
}

bool CUserActiveModel::getActiveData(int nActiveID, int nActiveType)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_Uid));
	if (pStorer == NULL)
	{
		return false;
	}

	string Key;
	Key = ModelKey::ActiveKey(m_Uid, nActiveID);
	switch (nActiveType)
	{
		case SHOP_ACTIVE_TYPE:
		{
			if (SUCCESS == pStorer->ExistKey(Key))
			{
				map<int, int> MapValue;

				if (SUCCESS == pStorer->GetHash(Key, MapValue))
				{
					m_MapShopNum[nActiveID] = MapValue;
				}
			}
		}
		break;
		case TASK_ACTIVE_TYPE:
		case MONTHCARD_ACTIVE_TYPE:
		case EXCHANGE_ACTIVE_TYPE:
		{
			if (SUCCESS == pStorer->ExistKey(Key))
			{
				map<int, int> MapValue;

				if (SUCCESS == pStorer->GetHash(Key, MapValue))
				{
					m_MapTaskNum[nActiveID] = MapValue;
				}
			}

			Key = ModelKey::TaskActiveValueKey(m_Uid,nActiveID);
			if (SUCCESS == pStorer->ExistKey(Key))
			{
				map<int, int> MapValue;

				if (SUCCESS == pStorer->GetHash(Key, MapValue))
				{
					m_MapTaskValue[nActiveID] = MapValue;
				}
			}
		}
		break;
		default:
		return false;
		break;
	}

	return true;
}

//获取角色创建时间
bool CUserActiveModel::getActiveCreatTime()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_Uid));
	if (NULL == pStorer)
	{
		return false;
	}

	std::string key = ModelKey::UsrKey(m_Uid);

	if (SUCCESS != pStorer->GetHashByField(key, USR_FD_CREATETIME, m_nCreatTime))
	{
		// 用户不存在
		return false;
	}

	return true;
}
