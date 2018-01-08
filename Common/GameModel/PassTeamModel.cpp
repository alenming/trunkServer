#include "PassTeamModel.h"
#include "KxMemPool.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"

using namespace KxServer;

CPassTeamModel::CPassTeamModel()
{
}

CPassTeamModel::~CPassTeamModel()
{
}

bool CPassTeamModel::init()
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_PASSTEAM);
	if (NULL == m_pStorage)
	{
		return false;
	}

	m_strPassTeamKey = ModelKey::PassTeamKey();
	Refresh();
	return true;
}

bool CPassTeamModel::Refresh()
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (NULL != redisStorer)
	{
		std::map<int, RedisBinaryData> teamDataMap;
		if (SUCCESS == redisStorer->GetHash(m_strPassTeamKey, teamDataMap))
		{
			for (std::map<int, RedisBinaryData>::iterator iter = teamDataMap.begin();
				iter != teamDataMap.end(); ++iter)
			{
				//将数据拷贝到本地
				int stageId = iter->first;
				int teamType = *(reinterpret_cast<int *>(iter->second.data));
				m_mapPassTeam[stageId][teamType] = PassTeamData();
				m_mapPassTeam[stageId][teamType].data = reinterpret_cast<char *>(kxMemMgrAlocate(iter->second.length));
				memset(m_mapPassTeam[stageId][teamType].data, 0, iter->second.length);
				memcpy(m_mapPassTeam[stageId][teamType].data, iter->second.data, iter->second.length);
			}
			return true;
		}
	}
	return false;
}

bool CPassTeamModel::SetPassTeam(int stageId, int teamType, void *data, int len)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (NULL != redisStorer)
	{
		std::map<int, std::map<int, PassTeamData> >::iterator iter = m_mapPassTeam.find(stageId);
		if (iter != m_mapPassTeam.end())
		{
			if (iter->second.find(teamType) != iter->second.end())
			{
				//先释放掉原来的内存
				kxMemMgrRecycle(m_mapPassTeam[stageId][teamType].data,
					m_mapPassTeam[stageId][teamType].len);
			}
		}
		//设置新数据
		PassTeamData teamData;
		len = len + sizeof(int);
		teamData.data = reinterpret_cast<char *>(kxMemMgrAlocate(len));
		teamData.len = len;
		memset(teamData.data, 0, teamData.len);
		memcpy(teamData.data, &teamType, sizeof(teamType));
		memcpy(teamData.data + sizeof(teamType), data, len);

		m_mapPassTeam[stageId][teamType] = teamData;

		return SUCCESS == redisStorer->SetHashByField(
			m_strPassTeamKey, stageId, teamData.data, teamData.len);
	}
	return false;
}

void CPassTeamModel::GetPassTeam(int stageId, int teamType, PassTeamData &teamData)
{
	std::map<int, std::map<int, PassTeamData> >::iterator iter = m_mapPassTeam.find(stageId);
	if (iter != m_mapPassTeam.end())
	{
		if (iter->second.find(teamType) != iter->second.end())
		{
			teamData.data = m_mapPassTeam[stageId][teamType].data;
			teamData.len = m_mapPassTeam[stageId][teamType].len;
		}
	}
	
	teamData.data = NULL;
	teamData.len = 0;
}

void CPassTeamModel::GetPassTeam(int stageId, std::map<int, PassTeamData>& passTeamMap)
{
	std::map<int, std::map<int, PassTeamData> >::iterator iter = m_mapPassTeam.find(stageId);
	if (iter != m_mapPassTeam.end())
	{
		passTeamMap = iter->second;
	}
}
