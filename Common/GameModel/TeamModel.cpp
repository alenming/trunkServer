#include "TeamModel.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"

CTeamModel::CTeamModel() 
: m_nUID(0)
, m_pStorage(NULL)
{
}

CTeamModel::~CTeamModel()
{
}

bool CTeamModel::init(int uid)
{
    m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_TEAM);
    if (NULL == m_pStorage)
    {
        return false;
    }

    m_nUID = uid;
    m_strTeamKey = ModelKey::TeamKey(m_nUID);

    Refresh();
    return true;
}

bool CTeamModel::Refresh()
{
    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
    if (redisStorer)
    {
        std::map<int, RedisBinaryData> mapret;
        if (SUCCESS == redisStorer->GetHash(m_strTeamKey, mapret))
        {
            m_mapTeamInfo.clear();

            std::map<int, RedisBinaryData>::iterator iter = mapret.begin();
            for (; iter != mapret.end(); ++iter)
            {
                memcpy(&m_mapTeamInfo[iter->first], iter->second.data, sizeof(TeamInfo));
            }

            return true;
        }
    }

    return false;
}

bool CTeamModel::SetTeamInfo(int teamType, TeamInfo &teamInfo)
{
    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
    if (NULL == redisStorer)
    {
        return false;
    }

    if (SUCCESS == redisStorer->SetHashByField(m_strTeamKey, teamType, 
		reinterpret_cast<char*>(&teamInfo), sizeof(TeamInfo)))
    {
        m_mapTeamInfo[teamType] = teamInfo;
        return true;
    }
    return false;
}

bool CTeamModel::GetTeamInfo(int teamType, TeamInfo& teamInfo)
{
    std::map<int, TeamInfo>::iterator iter = m_mapTeamInfo.find(teamType);
    if (iter != m_mapTeamInfo.end())
    {
        teamInfo = iter->second;
        return true;
    }

    return false;
}

bool CTeamModel::IsExistInTeams(int heroId)
{
    std::map<int, TeamInfo>::iterator iter = m_mapTeamInfo.begin();
    for (; iter != m_mapTeamInfo.end(); ++iter)
    {
		for (size_t i = 0; i < MAX_HERO_COUNT; i++)
		{
			if (heroId == iter->second.heroId[i])
			{
				return true;
			}
		}
    }

    return false;
}

void CTeamModel::RemoveHeroFromTeams(int heroId)
{
    std::map<int, TeamInfo>::iterator iter = m_mapTeamInfo.begin();
    for (; iter != m_mapTeamInfo.end(); ++iter)
    {
        bool bNeedSave = false;
		for (int i = 0; i < MAX_HERO_COUNT; ++i)
		{
			if (iter->second.heroId[i] == heroId)
			{
				iter->second.heroId[i] = 0;
				bNeedSave = true;
			}
		}

        if (bNeedSave)
        {
            SetTeamInfo(iter->first, iter->second);
        }
    }
}

