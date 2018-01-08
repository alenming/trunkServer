#include "GoldTestModel.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "Storage.h"
#include "StorageManager.h"

CGoldTestModel::CGoldTestModel() :m_nUid(0)
, m_pStorage(NULL)
{
    m_mapGoldTestInfo.clear();
}

CGoldTestModel::~CGoldTestModel()
{

}

bool CGoldTestModel::init(int uid)
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_GOLDTEST);
	if (NULL == m_pStorage)
	{
		return false;
	}

	m_nUid = uid;
	m_szKey = ModelKey::GoldTestKey(uid);
	Refresh();

	return true;
}

bool CGoldTestModel::Refresh()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == pStorer)
	{
		return false;
	}

    m_mapGoldTestInfo.clear();
    if (SUCCESS != pStorer->ExistKey(m_szKey))
    {
        for (int i = GT_FD_NONE + 1; i < GT_FD_END; ++i)
        {
            m_mapGoldTestInfo[i] = 0;
        }

        return SUCCESS == pStorer->SetHash(m_szKey, m_mapGoldTestInfo);
    }

    return SUCCESS == pStorer->GetHash(m_szKey, m_mapGoldTestInfo);
}

bool CGoldTestModel::GetFieldNum(int nField, int &nValue)
{
    std::map<int, int>::iterator iter = m_mapGoldTestInfo.find(nField);
	if (iter != m_mapGoldTestInfo.end())
	{
        nValue = iter->second;
        return true;
	}

    return false;
}

bool CGoldTestModel::SetFieldNum(int nField, int nValue)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
        std::map<int, int>::iterator iter = m_mapGoldTestInfo.find(nField);
        if (iter != m_mapGoldTestInfo.end())
        {
            if (SUCCESS == pStorer->SetHashByField(m_szKey, nField, nValue))
            {
                iter->second = nValue;
                return true;
            }
        }
	}

    return false;
}

bool CGoldTestModel::AddFieldNum(int nField, int nValue)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL != pStorer)
    {
        std::map<int, int>::iterator iter = m_mapGoldTestInfo.find(nField);
        if (iter != m_mapGoldTestInfo.end())
        {
            if (SUCCESS == pStorer->SetHashByField(m_szKey, nField, iter->second + nValue))
            {
                iter->second += nValue;
                return true;
            }
        }
    }

    return false;
}

bool CGoldTestModel::ResetGoldTest(int stamp)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL != pStorer)
    {
        for (int i = GT_FD_NONE + 1; i < GT_FD_END; ++i)
        {
            m_mapGoldTestInfo[i] = 0;
        }

        m_mapGoldTestInfo[GT_FD_RESETSTAMP] = stamp;
        
        return SUCCESS == pStorer->SetHash(m_szKey, m_mapGoldTestInfo);
    }

    return false;
}
