#include "BagModel.h"
#include "RedisStorer.h"
#include "Storage.h"
#include "StorageManager.h"
#include "ModelDef.h"

CBagModel::CBagModel()
{

}

CBagModel::~CBagModel()
{

}

bool CBagModel::init(int uid)
{
	m_nUid = uid;
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_USER);
	Refresh();
	return true;
}

bool CBagModel::Refresh()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
        m_Items.clear();
		std::string key = ModelKey::BagKey(m_nUid);
		if (SUCCESS != pStorer->GetHash(key, m_Items))
		{
			// 没有东西
			return false;
		}

		std::string BagInfoKey = ModelKey::BagInfoKey(m_nUid);
		pStorer->SetHashByField(BagInfoKey, PackageCureSize, m_Items.size());
		
	}

	return true;
}

std::map<int, int>& CBagModel::GetItems()
{
	return m_Items;
}

bool CBagModel::GetItem(int itemid, int &val, bool bNew)
{
	val = 0;
	std::map<int, int>::iterator iter = m_Items.find(itemid);
	if (iter != m_Items.end())
	{
		if (bNew)
		{
			if (!GetRealDataFromDB(itemid, val))
			{
				return false;
			}

			iter->second = val;
		}
		else
		{
			val = iter->second;
		}

		return true;
	}
	else
	{
		if (!GetRealDataFromDB(itemid, val))
		{
			return false;
		}

		if (val != 0)
		{
			m_Items[itemid] = val;
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool CBagModel::AddItem(int itemid, int val)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		std::string key = ModelKey::BagKey(m_nUid);
		m_Items[itemid] = m_Items[itemid] + val;
		if (SUCCESS == pStorer->SetHashByField(key, itemid, m_Items[itemid]))
		{
			return true;
		}
	}
	return false;
}

bool CBagModel::RemoveItem(int itemId)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
        std::map<int, int>::iterator iter = m_Items.find(itemId);
        if (iter != m_Items.end())
        {
            std::string key = ModelKey::BagKey(m_nUid);
            if (SUCCESS == pStorer->DelHashByField(key, itemId))
            {
                m_Items.erase(iter);
                return true;
            }
        }
	}
	return false;
}

bool CBagModel::DeleteBag()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		std::string key = ModelKey::BagKey(m_nUid);
		if (SUCCESS == pStorer->DelKey(key))
		{
			m_Items.clear();
			return true;
		}
	}
	return false;
}

bool CBagModel::ExistItem(int itemid, bool bNew)
{
	int nValue = 0;
	if (bNew)
	{
		if (!GetRealDataFromDB(itemid, nValue))
		{
			return false;
		}
		else
		{
			if (nValue != 0)
			{
				return true;
			}
		}

		return false;
	}

    std::map<int, int>::iterator iter = m_Items.find(itemid);
    if (iter != m_Items.end())
    {
        return true;
    }

    return false;
}

int CBagModel::GetCapacity()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == pStorer)
	{
		return 0;
	}

	std::string BagInfoKey = ModelKey::BagInfoKey(m_nUid);
	int nValue = 0;
	pStorer->GetHashByField(BagInfoKey, PackageCureSize, nValue);

	return nValue;
}

//增加背包容量
int CBagModel::AddCapacity(int val)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == pStorer)
	{
		return -1;
	}

	std::string BagInfoKey = ModelKey::BagInfoKey(m_nUid);

	pStorer->IncreHashByField(BagInfoKey, PackageCureSize, val);

	return val;
}

bool CBagModel::GetRealDataFromDB(int itemid, int &val)
{
	val = 0;
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == pStorer)
	{
		return false;
	}

	std::string key = ModelKey::BagKey(m_nUid);
	if (SUCCESS != pStorer->GetHashByField(key, itemid, val))
	{
		// 没有东西
		return false;
	}

	return true;
}
