#include "AchievementModel.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"

CAchievementModel::CAchievementModel() :m_nUid(0)
, m_pStorage(NULL)
{
    m_mapAllAchievement.clear();
}

CAchievementModel::~CAchievementModel()
{

}

bool CAchievementModel::init(int uid)
{
    m_nUid = uid;
    m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_ACHIEVEMENT);
    Refresh();

    return true;
}

bool CAchievementModel::Refresh()
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL != pStorer)
    {
        m_mapAllAchievement.clear();

        std::string key = ModelKey::AchieveKey(m_nUid);
        std::map<int, RedisBinaryData> mapRedisData;
        if (SUCCESS == pStorer->GetHash(key, mapRedisData))
        {
            std::map<int, RedisBinaryData>::iterator iter = mapRedisData.begin();
            for (; iter != mapRedisData.end(); ++iter)
            {
                memcpy(&(m_mapAllAchievement[iter->first]), iter->second.data, sizeof(AchieveDBInfo));
            }

            return true;
        }
    }

    return false;
}

bool CAchievementModel::AddAchievement(int achieveID, AchieveDBInfo &info)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL != pStorer)
    {
        std::string key = ModelKey::AchieveKey(m_nUid);

        std::map<int, AchieveDBInfo>::iterator iter = m_mapAllAchievement.find(achieveID);
        if (iter == m_mapAllAchievement.end())
        {
            if (SUCCESS == pStorer->SetHashByField(key, achieveID
                , reinterpret_cast<char*>(&info), sizeof(AchieveDBInfo)))
            {
                m_mapAllAchievement[achieveID] = info;
                return true;
            }
        }
    }

    return false;
}

bool CAchievementModel::SetAchievement(int achieveID, AchieveDBInfo &info)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL != pStorer)
    {
        std::string key = ModelKey::AchieveKey(m_nUid);

        std::map<int, AchieveDBInfo>::iterator iter = m_mapAllAchievement.find(achieveID);
        if (iter != m_mapAllAchievement.end())
        {
            if (SUCCESS == pStorer->SetHashByField(key, achieveID
                , reinterpret_cast<char*>(&info), sizeof(AchieveDBInfo)))
            {
                m_mapAllAchievement[achieveID] = info;
                return true;
            }
        }
    }

    return false;
}

bool CAchievementModel::GetAchievementByID(int achieveID, AchieveDBInfo &info, bool bNew)
{
    std::map<int, AchieveDBInfo>::iterator iter = m_mapAllAchievement.find(achieveID);
    if (iter != m_mapAllAchievement.end())
    {
		if (bNew)
		{
			if (!GetAchievementFromDB(achieveID, info))
			{
				return false;
			}

			memcpy(&iter->second, &info, sizeof(AchieveDBInfo));
		}
		else
		{
			memcpy(&info, &(iter->second), sizeof(AchieveDBInfo));
		}

        return true;
    }
	else
	{
		if (!GetAchievementFromDB(achieveID, info))
		{
			return false;
		}

		m_mapAllAchievement[achieveID] = info;
		return true;
	}

    return false;
}

std::map<int, AchieveDBInfo>& CAchievementModel::GetAllAchievement()
{
    return m_mapAllAchievement;
}

bool CAchievementModel::RemoveAchievementByID(int achieveID)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL != pStorer)
    {
        std::string key = ModelKey::AchieveKey(m_nUid);
        std::map<int, AchieveDBInfo>::iterator iter = m_mapAllAchievement.find(achieveID);
        if (iter != m_mapAllAchievement.end())
        {
            if (SUCCESS == pStorer->DelHashByField(key, achieveID))
            {
                m_mapAllAchievement.erase(iter);
                return true;
            }
        }
    }

    return false;
}

bool CAchievementModel::RemoveAchievement()
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL != pStorer)
    {
        std::string key = ModelKey::AchieveKey(m_nUid);
        if (SUCCESS == pStorer->DelKey(key))
        {
            m_mapAllAchievement.clear();
            return true;
        }
    }

    return false;
}

bool CAchievementModel::GetAchievementFromDB(int achieveID, AchieveDBInfo &info)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == pStorer)
	{
		return false;
	}

	std::string key = ModelKey::AchieveKey(m_nUid);

	int nLen = 0;
	if (SUCCESS != pStorer->GetHashByField(key, achieveID, (char*)&info,nLen))
	{
		return false;
	}

	if (nLen != sizeof(AchieveDBInfo))
	{
		return false;
	}

	return true;
}
