#include "StageModel.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "StorageManager.h"
#include "ConfAnalytic.h"
#include "KXServer.h"

using namespace std;

#define PR_USETIMES			"eut-"
#define PR_USESTAMP			"eus-"
#define PR_BUYTIMES			"ebt-"
#define PR_BUYSTAMP			"ebs-"
#define PR_STAGECHAPTER		"stgchp-"
#define PR_ELITECHAPTER		"elitechp-"
#define PR_CURSTAGE			"curstage"
#define PR_CURELITE			"curelite"

CStageModel::CStageModel() 
: m_nUID(0)
, m_nCurStage(0)
, m_nCurElite(0)
, m_pStorage(NULL)
{
}

CStageModel::~CStageModel()
{
}

bool CStageModel::init(int uid)
{
    m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_STAGE);
    if (NULL == m_pStorage)
    {
        return false;
    }

    m_nUID = uid;
	m_strStageStatusKey = ModelKey::StageStatusKey(uid);
	m_strEliteStatusKey = ModelKey::EliteStatusKey(uid);
	m_strEliteInfoKey = ModelKey::StageInfoKey(uid);
	m_strStageBoxStatusKey = ModelKey::StageBoxKey(uid);
	
	Refresh();
    return true;
}

bool CStageModel::Refresh()
{
	if (STORE_REDIS == m_pStorage->GetStoreType())
	{
        m_mapStageStatus.clear();
        m_mapEliteStatus.clear();
        m_mapChapterStatus.clear();
        m_mapEliteChapterStatus.clear();
        m_mapEliteStageInfo.clear();
		m_mapChanpterBoxStatus.clear();

		CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
		if (NULL != redisStorer)
		{
			// 普通关卡状态
			if (SUCCESS != redisStorer->GetHash(m_strStageStatusKey, m_mapStageStatus))
			{
				return false;
			}

			//关卡宝箱状态
			std::map<int, std::string> MapBoxStatus;
			if (SUCCESS == redisStorer->GetHash(m_strStageBoxStatusKey, MapBoxStatus))
			{
				for (std::map<int, std::string>::iterator ator = MapBoxStatus.begin(); ator != MapBoxStatus.end(); ++ator)
				{
					std::vector<int> VectValue;
					std::map<int, int> MapStatus;
					CConfAnalytic::ToJsonInt(ator->second, VectValue);
					for (size_t i = 0; i < VectValue.size()/2; i++)
					{
						MapStatus[VectValue[i * 2]] = VectValue[i * 2 + 1];
					}
					m_mapChanpterBoxStatus[ator->first] = MapStatus;
				}
			}

			// 精英关卡状态
			if (SUCCESS != redisStorer->GetHash(m_strEliteStatusKey, m_mapEliteStatus))
			{
				return false;
			}

			std::map<std::string, int> dbStageInfo;
			if (SUCCESS != redisStorer->GetHash(m_strEliteInfoKey, dbStageInfo))
			{
				return false;
			}

			for (std::map<std::string, int>::iterator iter = dbStageInfo.begin();
				iter != dbStageInfo.end(); ++iter)
			{
				std::string::size_type pos = std::string::npos;
				if (pos = iter->first.find(PR_STAGECHAPTER) != std::string::npos)
				{
					// 普通章节信息
					int stageId = atoi(iter->first.substr(strlen(PR_STAGECHAPTER)).c_str());
					m_mapChapterStatus[stageId] = iter->second;
				}
				else if (pos = iter->first.find(PR_ELITECHAPTER) != std::string::npos)
				{
					// 精英章节信息
					int stageId = atoi(iter->first.substr(strlen(PR_ELITECHAPTER)).c_str());
					m_mapEliteChapterStatus[stageId] = iter->second;
				}
				else if (pos = iter->first.find(PR_BUYTIMES) != std::string::npos)
				{
					// 精英购买次数信息
					int stageId = atoi(iter->first.substr(strlen(PR_BUYTIMES)).c_str());
					std::map<int, DBEliteStageInfo>::iterator stageIter = m_mapEliteStageInfo.find(stageId);
					if (stageIter == m_mapEliteStageInfo.end())
					{
						// 没记录先插入一个空记录
						DBEliteStageInfo info;
						memset(&info, 0, sizeof(info));
						m_mapEliteStageInfo[stageId] = info;
					}

					m_mapEliteStageInfo[stageId].buyTimes = iter->second;
				}
				else if (pos = iter->first.find(PR_BUYSTAMP) != std::string::npos)
				{
					// 精英上次购买时间戳
					int stageId = atoi(iter->first.substr(strlen(PR_BUYSTAMP)).c_str());
					std::map<int, DBEliteStageInfo>::iterator stageIter = m_mapEliteStageInfo.find(stageId);
					if (stageIter == m_mapEliteStageInfo.end())
					{
						// 没记录先插入一个空记录
						DBEliteStageInfo info;
						memset(&info, 0, sizeof(info));
						m_mapEliteStageInfo[stageId] = info;
					}

					m_mapEliteStageInfo[stageId].buyRecoverStamp = iter->second;
				}
				else if (pos = iter->first.find(PR_USETIMES) != std::string::npos)
				{
					// 精英剩余挑战次数
					int stageId = atoi(iter->first.substr(strlen(PR_USETIMES)).c_str());
					std::map<int, DBEliteStageInfo>::iterator stageIter = m_mapEliteStageInfo.find(stageId);
					if (stageIter == m_mapEliteStageInfo.end())
					{
						// 没记录先插入一个空记录
						DBEliteStageInfo info;
						memset(&info, 0, sizeof(info));
						m_mapEliteStageInfo[stageId] = info;
					}

					m_mapEliteStageInfo[stageId].challengeTimes = iter->second;
				}
				else if (pos = iter->first.find(PR_USESTAMP) != std::string::npos)
				{
					// 精英上次使用的时间戳
					int stageId = atoi(iter->first.substr(strlen(PR_USESTAMP)).c_str());
					std::map<int, DBEliteStageInfo>::iterator stageIter = m_mapEliteStageInfo.find(stageId);
					if (stageIter == m_mapEliteStageInfo.end())
					{
						// 没记录先插入一个空记录
						DBEliteStageInfo info;
						memset(&info, 0, sizeof(info));
						m_mapEliteStageInfo[stageId] = info;
					}

					m_mapEliteStageInfo[stageId].useRecoverStamp = iter->second;
				}
				else if (pos = iter->first.find(PR_CURSTAGE) != std::string::npos)
				{
					// 当前普通关卡
					m_nCurStage = iter->second;
				}
				else if (pos = iter->first.find(PR_CURELITE) != std::string::npos)
				{
					// 当前精英关卡
					m_nCurElite = iter->second;
				}
			}
			return true;
		}
	}

	return false;
}

bool CStageModel::SetStageStatus(int stageId, int status, bool remove)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (NULL != redisStorer)
	{
		if (GetStageStatus(stageId) >= STAGESTATUS_STAR3)
		{
			return true;
		}
		//如果3星, 从列表中删除
		std::map<int, int>::iterator iter = m_mapStageStatus.find(stageId);
		if (status >= STAGESTATUS_STAR3 && remove)
		{
			if (iter != m_mapStageStatus.end())
			{
				m_mapStageStatus.erase(iter);
				return SUCCESS == redisStorer->DelHashByField(m_strStageStatusKey, stageId);
			}
			return true;
		}
		// 否则状态大于已存储的状态才进行更新
		else
		{
			if (iter != m_mapStageStatus.end())
			{
				if (iter->second >= status)
				{
					return true;
				}
			}
			m_mapStageStatus[stageId] = status;
			return SUCCESS == redisStorer->SetHashByField(m_strStageStatusKey, stageId, status);
		}
	}
	return false;
}

bool CStageModel::SetCurStage(int stageId)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (NULL != redisStorer)
	{
		if (m_nCurStage >= stageId)
		{
			return false;
		}
		m_nCurStage = stageId;
		return SUCCESS == redisStorer->SetHashByField(m_strEliteInfoKey, GetStageInfoField(0, SIF_CURSTAGE), stageId) 
			&& SetStageStatus(stageId, STAGESTATUS_UNLOCK);
	}
	return false;
}

int CStageModel::GetStageStatus(int stageId)
{
	std::map<int, int>::iterator iter = m_mapStageStatus.find(stageId);
	if (iter != m_mapStageStatus.end())
	{
		return iter->second;
	}
	else
	{
		if (m_nCurStage > stageId)
		{
			return STAGESTATUS_STAR3;
		}
	}
	return STAGESTATUS_LOCK;
}

bool CStageModel::SetChapterStatus(int chapterId, int status)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (NULL != redisStorer)
	{
		std::map<int, int>::iterator iter = m_mapChapterStatus.find(chapterId);
		if (iter != m_mapChapterStatus.end())
		{
			if (iter->second >= status)
			{
				return true;
			}
		}

		m_mapChapterStatus[chapterId] = status;
		return SUCCESS == redisStorer->SetHashByField(m_strEliteInfoKey,
			GetStageInfoField(chapterId, SIF_NORMALCHAPTER_STATUS), status);
	}
	return false;
}

int CStageModel::GetChapterStatus(int chapterId)
{
	std::map<int, int>::iterator iter = m_mapChapterStatus.find(chapterId);
	if (iter != m_mapChapterStatus.end())
	{
		return iter->second;
	}
	return CHAPTERSTATUS_LOCK;
}

bool CStageModel::SetEliteChapterStatus(int chapterId, int status)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (NULL != redisStorer)
	{
		std::map<int, int>::iterator iter = m_mapEliteChapterStatus.find(chapterId);
		if (iter != m_mapEliteChapterStatus.end())
		{
			if (iter->second >= status)
			{
				return true;
			}
		}

		m_mapEliteChapterStatus[chapterId] = status;
		return SUCCESS == redisStorer->SetHashByField(m_strEliteInfoKey,
			GetStageInfoField(chapterId, SIF_ELITECHAPTER_STATUS), status);
	}
	return false;
}

int CStageModel::GetEliteChapterStatus(int chapterId)
{
	std::map<int, int>::iterator iter = m_mapEliteChapterStatus.find(chapterId);
	if (iter != m_mapEliteChapterStatus.end())
	{
		return iter->second;
	}
	return CHAPTERSTATUS_LOCK;
}

bool CStageModel::SetEliteStatus(int stageId, int status, int remove)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (NULL != redisStorer)
	{
		if (GetEliteStatus(stageId) >= STAGESTATUS_STAR3)
		{
			return true;
		}
		//如果3星, 从列表中删除
		std::map<int, int>::iterator iter = m_mapEliteStatus.find(stageId);
		if (status >= STAGESTATUS_STAR3 && remove)
		{
			if (iter != m_mapEliteStatus.end())
			{
				m_mapEliteStatus.erase(iter);
				return SUCCESS == redisStorer->DelHashByField(m_strEliteStatusKey, stageId);
			}
			return true;
		}
		// 否则状态大于已存储的状态才进行更新
		else
		{
			if (iter != m_mapEliteStatus.end())
			{
				if (iter->second >= status)
				{
					return true;
				}
			}
			m_mapEliteStatus[stageId] = status;
			return SUCCESS == redisStorer->SetHashByField(m_strEliteStatusKey, stageId, status);
		}
	}
	return false;
}

bool CStageModel::SetCurElite(int stageId)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (NULL != redisStorer)
	{
		if (m_nCurElite >= stageId)
		{
			return false;
		}
		m_nCurElite = stageId;
		DBEliteStageInfo eliteInfo = { 0, 0, 0, 0 };
		if (!SetEliteInfo(stageId, eliteInfo))
		{
			return false;
		}
		if (!SetEliteStatus(stageId, STAGESTATUS_UNLOCK))
		{
			return false;
		}
		return SUCCESS == redisStorer->SetHashByField(m_strEliteInfoKey, GetStageInfoField(0, SIF_CURELITE), stageId);
	}
	return false;
}

int CStageModel::GetEliteStatus(int stageId)
{
	std::map<int, int>::iterator iter = m_mapEliteStatus.find(stageId);
	if (iter != m_mapEliteStatus.end())
	{
		return iter->second;
	}
	else
	{
		if (m_nCurElite > stageId)
		{
			return STAGESTATUS_STAR3;
		}
	}
	return STAGESTATUS_LOCK;
}

bool CStageModel::SetEliteInfo(int stageId, DBEliteStageInfo &info)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (NULL != redisStorer)
	{
		m_mapEliteStageInfo[stageId] = info;
		// 调整成批量写入
		std::map<std::string, int> stageInfo;
		stageInfo[GetStageInfoField(stageId, SIF_CANUSETIMES)] = info.challengeTimes;
		stageInfo[GetStageInfoField(stageId, SIF_USESTAMP)] = info.useRecoverStamp;
		stageInfo[GetStageInfoField(stageId, SIF_BUYTIMES)] = info.buyTimes;
		stageInfo[GetStageInfoField(stageId, SIF_BUYSTAMP)] = info.buyRecoverStamp;
		return SUCCESS == redisStorer->SetHash(m_strEliteInfoKey, stageInfo);
	}
	return false;
}

bool CStageModel::SetEliteInfo(int stageId, int field, int value)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (NULL != redisStorer)
	{
		std::map<int, DBEliteStageInfo>::iterator iter = m_mapEliteStageInfo.find(stageId);
		if (iter == m_mapEliteStageInfo.end())
		{
			m_mapEliteStageInfo[stageId] = DBEliteStageInfo();
			memset(&m_mapEliteStageInfo[stageId], 0, sizeof(DBEliteStageInfo));
		}

		switch (field)
		{
		case SIF_CANUSETIMES:
			m_mapEliteStageInfo[stageId].challengeTimes = value;
			break;
		case SIF_USESTAMP:
			m_mapEliteStageInfo[stageId].useRecoverStamp = value;
			break;
		case SIF_BUYTIMES:
			m_mapEliteStageInfo[stageId].buyTimes = value;
			break;
		case SIF_BUYSTAMP:
			m_mapEliteStageInfo[stageId].buyRecoverStamp = value;
			break;
		default:
			return false;
			break;
		}

		return SUCCESS == redisStorer->SetHashByField(m_strEliteInfoKey,
			GetStageInfoField(stageId, field), value);
	}

	return false;
}

bool CStageModel::GetEliteInfo(int stageId, DBEliteStageInfo& info)
{
	std::map<int, DBEliteStageInfo>::iterator iter = m_mapEliteStageInfo.find(stageId);
	if (iter != m_mapEliteStageInfo.end())
	{
		info = iter->second;
		return true;
	}

	return false;
}

bool CStageModel::RemoveEliteInfo(int stageId)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (NULL != redisStorer)
	{
		std::map<int, DBEliteStageInfo>::iterator iter = m_mapEliteStageInfo.find(stageId);
		if (iter == m_mapEliteStageInfo.end())
		{
			return false;
		}

		if (SUCCESS != redisStorer->DelHashByField(m_strEliteInfoKey, GetStageInfoField(stageId, SIF_CANUSETIMES))
			|| SUCCESS != redisStorer->DelHashByField(m_strEliteInfoKey, GetStageInfoField(stageId, SIF_USESTAMP))
			|| SUCCESS != redisStorer->DelHashByField(m_strEliteInfoKey, GetStageInfoField(stageId, SIF_BUYTIMES))
			|| SUCCESS != redisStorer->DelHashByField(m_strEliteInfoKey, GetStageInfoField(stageId, SIF_BUYSTAMP)))
		{
			return false;
		}

		m_mapEliteStageInfo.erase(iter);
		return true;
	}

	return false;
}

bool CStageModel::DeleteStage()
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (NULL != redisStorer)
	{
		if (SUCCESS == redisStorer->DelKey(m_strStageStatusKey)
			&& SUCCESS == redisStorer->DelKey(m_strEliteStatusKey)
			&& SUCCESS == redisStorer->DelKey(m_strEliteInfoKey))
		{
			m_mapStageStatus.clear();
			m_mapEliteStatus.clear();
			m_mapChapterStatus.clear();
			m_mapEliteChapterStatus.clear();
			m_mapEliteStageInfo.clear();
			return true;
		}
	}
    return false;
}

//获取章节的所有宝箱信息
std::map<int, int>* CStageModel::getChapterBoxStatusMap(int chapterId)
{
	std::map<int, std::map<int, int> >::iterator ator = m_mapChanpterBoxStatus.find(chapterId);
	if (ator == m_mapChanpterBoxStatus.end())
	{
		return NULL;
	}

	return &ator->second;
}

bool CStageModel::getChapterBoxStatus(int chapterId, int nIndex, int &nStatus)
{
	std::map<int, std::map<int, int> >::iterator ator = m_mapChanpterBoxStatus.find(chapterId);
	if (ator == m_mapChanpterBoxStatus.end())
	{
		nStatus = 0;
	}
	else
	{
		std::map<int, int>& MapValue = ator->second;
		std::map<int, int>::iterator iter = MapValue.find(nIndex);

		if (iter == MapValue.end())
		{
			nStatus = 0;
		}
		else
		{
			nStatus = iter->second;
		}
	}

	return true;
}

//设置章节宝箱信息
bool CStageModel::setChapterBoxStatus(int chapterId, int nIndex, int nStatus)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (redisStorer == NULL)
	{
		return false;
	}

	std::map<int, std::map<int, int> >::iterator ator = m_mapChanpterBoxStatus.find(chapterId);
	if (ator == m_mapChanpterBoxStatus.end())
	{
		std::map<int, int> MapValue;
		MapValue[nIndex] = nStatus;
		m_mapChanpterBoxStatus[chapterId] = MapValue;
	}
	else
	{
		std::map<int, int>& MapValue = ator->second;
		MapValue[nIndex] = nStatus;
	}

	ator = m_mapChanpterBoxStatus.find(chapterId);
	std::map<int, int>& MapValue = ator->second;
	char szBuffer[256] = {};
	int nLen = 0;
	int nMaxLen = sizeof(szBuffer);
	nLen = snprintf(szBuffer, nMaxLen, "[");
	for (std::map<int, int>::iterator iter = MapValue.begin(); iter != MapValue.end();++iter)
	{
		if (iter == MapValue.begin())
		{
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "%d+%d", iter->first, iter->second);
		}
		else
		{
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%d+%d", iter->first, iter->second);
		}
		
	}
	nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "]");

	if (SUCCESS == redisStorer->SetHashByField(m_strStageBoxStatusKey,chapterId, szBuffer, nLen))
	{
		return true;
	}

	return false;
}

std::string CStageModel::GetStageInfoField(int id, int fieldType)
{
	std::string field;
	std::stringstream ss;
	ss << id;
	ss >> field;

	switch (fieldType)
	{
	case SIF_CANUSETIMES:
		return PR_USETIMES + field;
		break;
	case SIF_USESTAMP:
		return PR_USESTAMP + field;
		break;
	case SIF_BUYTIMES:
		return PR_BUYTIMES + field;
		break;
	case SIF_BUYSTAMP:
		return PR_BUYSTAMP + field;
		break; 
	case SIF_NORMALCHAPTER_STATUS:
		return PR_STAGECHAPTER + field;
		break;
	case SIF_ELITECHAPTER_STATUS:
		return PR_ELITECHAPTER + field;
		break;
	case SIF_CURSTAGE:
		return PR_CURSTAGE;
		break;
	case SIF_CURELITE:
		return PR_CURELITE;
		break;
	default:
		break;
	}
	return "";
}

