#include "SummonModel.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"

using namespace std;

CSummonModel::CSummonModel() 
{
	m_pStorage = NULL;
	m_nUID = 0;
	m_VectAllSummon.clear();
}

CSummonModel::~CSummonModel()
{
}

bool CSummonModel::init(int uid)
{
    m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_SUMMONER);
    if (NULL == m_pStorage)
    {
        return false;
    }

    m_nUID = uid;
    m_strSummonKey = ModelKey::SummonerKey(m_nUID);

	Refresh();
    return true;
}

bool CSummonModel::Refresh()
{
	m_VectAllSummon.clear();
	if (STORE_REDIS == m_pStorage->GetStoreType())
	{
		CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
		if (NULL != redisStorer)
		{
			std::vector<int> summonerTemp;
			redisStorer->ListRange(m_strSummonKey, summonerTemp);

			for (std::vector<int>::iterator iter = summonerTemp.begin();
				iter != summonerTemp.end(); ++iter)
			{
				m_VectAllSummon.push_back(*iter);
			}
		}
	}

	return true;
}


bool CSummonModel::HaveSummoner(int cardId, bool bNew)
{
	if (bNew)
	{
		Refresh();
	}

	std::vector<int>::iterator ator = find(m_VectAllSummon.begin(), m_VectAllSummon.end(), cardId);

	if (ator == m_VectAllSummon.end())
	{
		return false;
	}

	return true;
}

bool CSummonModel::AddSummon(const int& cardId)
{
     if (STORE_REDIS == m_pStorage->GetStoreType())
     {
         CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
         if (NULL == redisStorer)
         {
             return false;
         }
 
         // Ìí¼Ó¿¨Æ¬
         if(SUCCESS == redisStorer->ListPush(m_strSummonKey, cardId))
         {
			 m_VectAllSummon.push_back(cardId);
             return true;
         }
     }

     return false;
}

std::vector<int>& CSummonModel::GetAllSummon()
{
	return m_VectAllSummon;
}

bool CSummonModel::DeleteSummon()
{
    if (STORE_REDIS == m_pStorage->GetStoreType())
    {
        CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
        if (NULL != redisStorer)
        {
            if (SUCCESS == redisStorer->DelKey(m_strSummonKey))
            {
				m_VectAllSummon.clear();
                return true;
            }
        }
    }

    return false;
}

