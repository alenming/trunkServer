#include "ActivityInstanceModel.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"

#define ATVT_USETIMES	   "aut-"
#define ATVT_USESTAMP	   "aus-"
#define ATVT_BUYTIMES	   "abt-"
#define ATVT_BUYSTAMP	   "abs-"
#define ATVT_EASY		   "aeasy-"
#define ATVT_NORMAL		   "anor-"
#define ATVT_DIFFICULT	   "adiff-"
#define ATVT_HELL		   "ahell-"
#define ATVT_LEGEND		   "alege-"

CInstanceModel::CInstanceModel()
: m_nUid(0)
, m_pStorage(NULL)
{
}

CInstanceModel::~CInstanceModel()
{
}

bool CInstanceModel::init(int uid)
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_ACTIVITY);
	if (NULL == m_pStorage)
	{
		return false;
	}

	m_nUid = uid;
	m_strActivityKey = ModelKey::ActivityInstanceKey(uid);

	Refresh();
	return true;
}

bool CInstanceModel::Refresh()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		std::map<std::string, int> mapInfos;
		if (SUCCESS == pStorer->GetHash(m_strActivityKey, mapInfos))
		{
			// 获得活动副本信息
			for (std::map<std::string, int>::iterator iter = mapInfos.begin();
				iter != mapInfos.end(); ++iter)
			{
				std::string::size_type pos = std::string::npos;
				//挑战次数
				if ((pos = iter->first.find(ATVT_USETIMES)) != std::string::npos)
				{
					int activityId = atoi(iter->first.substr(strlen(ATVT_USETIMES)).c_str());
					CheckActivity(activityId);
					m_mapActivityInfo[activityId].useTimes = iter->second;
				}
				//挑战时间戳
				if ((pos = iter->first.find(ATVT_USESTAMP)) != std::string::npos)
				{
					int activityId = atoi(iter->first.substr(strlen(ATVT_USESTAMP)).c_str());
					CheckActivity(activityId);
					m_mapActivityInfo[activityId].useStamp = iter->second;
				}
				//购买次数
				if ((pos = iter->first.find(ATVT_BUYTIMES)) != std::string::npos)
				{
					int activityId = atoi(iter->first.substr(strlen(ATVT_BUYTIMES)).c_str());
					CheckActivity(activityId);
					m_mapActivityInfo[activityId].buyTimes = iter->second;
				}
				//购买时间戳
				if ((pos = iter->first.find(ATVT_BUYSTAMP)) != std::string::npos)
				{
					int activityId = atoi(iter->first.substr(strlen(ATVT_BUYSTAMP)).c_str());
					CheckActivity(activityId);
					m_mapActivityInfo[activityId].buyStamp = iter->second;
				}
				//简单难度
				if ((pos = iter->first.find(ATVT_EASY)) != std::string::npos)
				{
					int activityId = atoi(iter->first.substr(strlen(ATVT_EASY)).c_str());
					CheckActivity(activityId);
					m_mapActivityInfo[activityId].easy = iter->second;
				}
				//普通难度
				if ((pos = iter->first.find(ATVT_NORMAL)) != std::string::npos)
				{
					int activityId = atoi(iter->first.substr(strlen(ATVT_NORMAL)).c_str());
					CheckActivity(activityId);
					m_mapActivityInfo[activityId].normal = iter->second;
				}
				//困难难度
				if ((pos = iter->first.find(ATVT_DIFFICULT)) != std::string::npos)
				{
					int activityId = atoi(iter->first.substr(strlen(ATVT_DIFFICULT)).c_str());
					CheckActivity(activityId);
					m_mapActivityInfo[activityId].difficult = iter->second;
				}
				//地狱难度
				if ((pos = iter->first.find(ATVT_HELL)) != std::string::npos)
				{
					int activityId = atoi(iter->first.substr(strlen(ATVT_HELL)).c_str());
					CheckActivity(activityId);
					m_mapActivityInfo[activityId].hell= iter->second;
				}
				//传说难度
				if ((pos = iter->first.find(ATVT_LEGEND)) != std::string::npos)
				{
					int activityId = atoi(iter->first.substr(strlen(ATVT_LEGEND)).c_str());
					CheckActivity(activityId);
					m_mapActivityInfo[activityId].legend = iter->second;
				}
			}
			return true;
		}
	}
	return false;
}

bool CInstanceModel::SetInstanceInfo(int activityId, int field, int value)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		std::map<int, DBActivityInstanceInfo>::iterator iter = m_mapActivityInfo.find(activityId);
		if (iter == m_mapActivityInfo.end())
		{
			m_mapActivityInfo[activityId] = DBActivityInstanceInfo();
			memset(&m_mapActivityInfo[activityId], 0, sizeof(DBActivityInstanceInfo));
		}

		switch (field)
		{
		case ACTV_FD_USETIMES:
			m_mapActivityInfo[activityId].useTimes = value;
			break;
		case ACTV_FD_USESTAMP:
			m_mapActivityInfo[activityId].useStamp = value;
			break;
		case ACTV_FD_BUYTIMES:
			m_mapActivityInfo[activityId].buyTimes = value;
			break;
		case ACTV_FD_BUYSTAMP:
			m_mapActivityInfo[activityId].buyStamp = value;
			break;
		case ACTV_FD_EASY:
			m_mapActivityInfo[activityId].easy = value;
			break;
		case ACTV_FD_NORMAL:
			m_mapActivityInfo[activityId].normal = value;
			break;
		case ACTV_FD_DIFFICULT:
			m_mapActivityInfo[activityId].difficult = value;
			break;
		case ACTV_FD_HELL:
			m_mapActivityInfo[activityId].hell = value;
			break;
		case ACTV_FD_LEGEND:
			m_mapActivityInfo[activityId].legend = value;
			break;
		default:
			return false;
			break;
		}
		return SUCCESS == pStorer->SetHashByField(m_strActivityKey,
			GetActivityField(activityId, field), value);
	}
	return true;
}

bool CInstanceModel::SetInstanceInfo(int activityId, DBActivityInstanceInfo &infos)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		std::map<int, DBActivityInstanceInfo>::iterator iter = m_mapActivityInfo.find(activityId);
		if (iter == m_mapActivityInfo.end())
		{
			m_mapActivityInfo[activityId] = infos;
		}

		std::map<std::string, int> mapInfos;
		mapInfos[GetActivityField(activityId, ACTV_FD_USETIMES)] = infos.useTimes;
		mapInfos[GetActivityField(activityId, ACTV_FD_USESTAMP)] = infos.useStamp;
		mapInfos[GetActivityField(activityId, ACTV_FD_BUYTIMES)] = infos.buyTimes;
		mapInfos[GetActivityField(activityId, ACTV_FD_BUYSTAMP)] = infos.buyStamp;
		mapInfos[GetActivityField(activityId, ACTV_FD_EASY)] = infos.easy;
		mapInfos[GetActivityField(activityId, ACTV_FD_NORMAL)] = infos.normal;
		mapInfos[GetActivityField(activityId, ACTV_FD_DIFFICULT)] = infos.difficult;
		mapInfos[GetActivityField(activityId, ACTV_FD_HELL)] = infos.hell;
		mapInfos[GetActivityField(activityId, ACTV_FD_LEGEND)] = infos.legend;

		return SUCCESS == pStorer->SetHash(m_strActivityKey, mapInfos);
	}

	return false;
}

bool CInstanceModel::GetInstanceInfo(int activityId, int field, int &value)
{
	std::map<int, DBActivityInstanceInfo>::iterator iter = m_mapActivityInfo.find(activityId);
	if (iter != m_mapActivityInfo.end())
	{
		switch (field)
		{
		case ACTV_FD_USETIMES:
			value = iter->second.useTimes;
			break;
		case ACTV_FD_USESTAMP:
			value = iter->second.useStamp;
			break;
		case ACTV_FD_BUYTIMES:
			value = iter->second.buyTimes;
			break;
		case ACTV_FD_BUYSTAMP:
			value = iter->second.buyStamp;
			break;
		case ACTV_FD_EASY:
			value = iter->second.easy;
			break;
		case ACTV_FD_NORMAL:
			value = iter->second.normal;
			break;
		case ACTV_FD_DIFFICULT:
			value = iter->second.difficult;
			break;
		case ACTV_FD_HELL:
			value = iter->second.hell;
			break;
		case ACTV_FD_LEGEND:
			value = iter->second.legend;
			break;
		default:
			return false;
			break;
		}
		return true;
	}
	return false;
}

bool CInstanceModel::GetInstanceInfo(int activityId, DBActivityInstanceInfo &infos)
{
	std::map<int, DBActivityInstanceInfo>::iterator iter = m_mapActivityInfo.find(activityId);
	if (iter != m_mapActivityInfo.end())
	{
		infos = iter->second;
		return true;
	}
	return false;
}

bool CInstanceModel::DeleteInstanceKey()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		return SUCCESS == pStorer->DelKey(m_strActivityKey);
	}
	return true;
}

bool CInstanceModel::CheckActivity(int activityId)
{
	std::map<int, DBActivityInstanceInfo>::iterator iter = m_mapActivityInfo.find(activityId);
	if (iter == m_mapActivityInfo.end())
	{
		m_mapActivityInfo[activityId] = DBActivityInstanceInfo();
		memset(&m_mapActivityInfo[activityId], 0, sizeof(DBActivityInstanceInfo));
	}
	return true;
}

std::string CInstanceModel::GetActivityField(int activityId, int field)
{
	std::string activityfield;
	std::stringstream ss;
	ss << activityId;
	ss >> activityfield;

	switch (field)
	{
	case ACTV_FD_USETIMES:
		return ATVT_USETIMES + activityfield;
		break;
	case ACTV_FD_USESTAMP:
		return ATVT_USESTAMP + activityfield;
		break;
	case ACTV_FD_BUYTIMES:
		return ATVT_BUYTIMES + activityfield;
		break;
	case ACTV_FD_BUYSTAMP:
		return ATVT_BUYSTAMP + activityfield;
		break;
	case ACTV_FD_EASY:
		return ATVT_EASY + activityfield;
		break;
	case ACTV_FD_NORMAL:
		return ATVT_NORMAL + activityfield;
		break;
	case ACTV_FD_DIFFICULT:
		return ATVT_DIFFICULT + activityfield;
		break;
	case ACTV_FD_HELL:
		return ATVT_HELL + activityfield;
		break;
	case ACTV_FD_LEGEND:
		return ATVT_LEGEND + activityfield;
		break;
	default:
		break;
	}
	return "";
}
