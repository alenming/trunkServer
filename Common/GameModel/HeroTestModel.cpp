#include "HeroTestModel.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"

CHeroTestModel::CHeroTestModel() 
: m_nUID(0)
, m_nResetStamp(0)
, m_pStorage(NULL)
{
    m_mapTimes.clear();
}

CHeroTestModel::~CHeroTestModel()
{

}

bool CHeroTestModel::init(int uid)
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_HEROTEST);
	if (NULL == m_pStorage)
	{
		return false;
	}

    m_nUID = uid;
	m_strKey = ModelKey::HeroTestKey(uid);

	Refresh();

	return true;
}

bool CHeroTestModel::Refresh()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (pStorer == NULL)
	{
		return false;
	}

	if (SUCCESS != pStorer->ExistKey(m_strKey))
	{
        // 新用户先存储时间的FIELD
        return SUCCESS == pStorer->SetHashByField(m_strKey, HEROTEST_FD_RESETSTAMP, 0);
	}
    else
    {
        std::map<int, int> mapInfos;
        if (SUCCESS != pStorer->GetHash(m_strKey, mapInfos))
        {
            return false;
        }

        std::map<int, int>::iterator it = mapInfos.begin();
        for (; it != mapInfos.end(); ++it)
        {
            if (HEROTEST_FD_RESETSTAMP == it->first)
            {
                m_nResetStamp = it->second;
            }
            else
            {
                m_mapTimes[it->first] = it->second;
            }
        }
    }

	return true;
}

int CHeroTestModel::GetChallengeTimes(int heroTestID)
{
    std::map<int, int>::iterator it = m_mapTimes.find(heroTestID);
    if (it != m_mapTimes.end())
    {
        return it->second;
    }

    return 0;
}

bool CHeroTestModel::ResetHeroTest(int useStamp)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
    if (NULL != pStorer)
    {
        std::map<int, int> mapInfos;
        std::map<int, int>::iterator it = m_mapTimes.begin();
        for (; it != m_mapTimes.end(); ++it)
        {
            m_mapTimes[it->first] = 0;
            mapInfos[it->first] = 0;
            it->second = 0;
        }

        mapInfos[HEROTEST_FD_RESETSTAMP] = useStamp;
        if (SUCCESS == pStorer->SetHash(m_strKey, mapInfos))
        {
            m_nResetStamp = useStamp;
            return true;
        }
    }

    return false;
}

bool CHeroTestModel::AddChallengeCount(int heroTestID, int count)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
    if (NULL != pStorer)
    {
        int nCount = count;
        std::map<int, int>::iterator iter = m_mapTimes.find(heroTestID);
        if (iter != m_mapTimes.end())
        {
            nCount += iter->second;
        }

        if (SUCCESS == pStorer->SetHashByField(m_strKey, heroTestID, nCount))
        {
            m_mapTimes[heroTestID] = nCount;
            return true;
        }
    }

    return false;
}
