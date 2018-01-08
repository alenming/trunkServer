#include "UnionMercenaryModel.h"

CUnionMercenaryModel::CUnionMercenaryModel() :m_pRedisStorer(NULL)
{
}

CUnionMercenaryModel::~CUnionMercenaryModel()
{
}

bool CUnionMercenaryModel::init(int unionID)
{
	Storage* pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);
	if (NULL == pStorage)
	{
		KXLOGDEBUG("UnionMercenaryModel GetStorage Error");
		return false;
	}

	m_pRedisStorer = dynamic_cast<CRedisStorer*>(pStorage->GetStorer(unionID));
	if (NULL == m_pRedisStorer)
	{
		KXLOGDEBUG("UnionMercenary m_pRedisStorer is null");
		return false;
	}

	m_nUnionId = unionID;
	m_strUnionMercenaryKey = ModelKey::UnionMercenaryKey(unionID);
	Refresh();

	return true;
}

bool CUnionMercenaryModel::Refresh()
{
	map<int, string> mapUnionMercenary;
	m_pRedisStorer->GetHash(m_strUnionMercenaryKey, mapUnionMercenary);
	
	//从数据库中读取公会佣兵数据，正在派遣中的和已经召回的放在不同的map，拉取数据时只下发正在派遣的
	for (map<int, string>::iterator mapit = mapUnionMercenary.begin(); mapit != mapUnionMercenary.end(); ++mapit)
	{
		UnionMercenaryInfo info;
		DBtoSt(mapit->second, info);
		
		if (info.nDispatchTime < info.nRecallTime)
		{
			m_mapUnionRecalledMercenaryInfo[mapit->first] = info;
		}
		else
		{
			m_mapUnionDispatchingMercenaryInfo[mapit->first] = info;
		}
	}

	return true;
}

//派遣佣兵，新佣兵ID为已存在的佣兵最大值+1
int CUnionMercenaryModel::DispatchMercenary(UnionMercenaryInfo& info)
{
	string attr;
	StoDB(info, attr);
	if (0 == m_mapUnionDispatchingMercenaryInfo.size() && 0 == m_mapUnionRecalledMercenaryInfo.size())
	{
		m_pRedisStorer->SetHashByField(m_strUnionMercenaryKey, 1, attr);
		m_mapUnionDispatchingMercenaryInfo[1] = info;
		return 1;
	}

	int nDispatchingMaxId = 0;
	int nRecalledMaxId = 0;
	if (m_mapUnionDispatchingMercenaryInfo.size() != 0)
	{
		nDispatchingMaxId = m_mapUnionDispatchingMercenaryInfo.rbegin()->first;
	}
	if (m_mapUnionRecalledMercenaryInfo.size() != 0)
	{
		nRecalledMaxId = m_mapUnionRecalledMercenaryInfo.rbegin()->first;
	}

	if (nDispatchingMaxId > nRecalledMaxId)
	{
		m_pRedisStorer->SetHashByField(m_strUnionMercenaryKey, nDispatchingMaxId + 1, attr);
		m_mapUnionDispatchingMercenaryInfo[nDispatchingMaxId + 1] = info;
		return nDispatchingMaxId + 1;
	}
	else
	{
		m_pRedisStorer->SetHashByField(m_strUnionMercenaryKey, nRecalledMaxId + 1, attr);
		m_mapUnionDispatchingMercenaryInfo[nRecalledMaxId + 1] = info;
		return nRecalledMaxId + 1;
	}
}

//召回佣兵
bool CUnionMercenaryModel::RecallMercenary(int nMercenaryID)
{
	map<int, UnionMercenaryInfo>::iterator mapit = m_mapUnionDispatchingMercenaryInfo.find(nMercenaryID);
	if (mapit != m_mapUnionDispatchingMercenaryInfo.end())
	{
		mapit->second.nRecallTime = (int)time(NULL);
		m_mapUnionRecalledMercenaryInfo[mapit->first] = mapit->second;
		string str;
		StoDB(mapit->second, str);
		m_pRedisStorer->SetHashByField(m_strUnionMercenaryKey, mapit->first, str);
		m_mapUnionDispatchingMercenaryInfo.erase(mapit->first);
		return true;
	}
	return false;
}

//雇佣佣兵
void CUnionMercenaryModel::EmployedMercenary(int nMercenaryID)
{
	UpdateSingleMercenary(nMercenaryID);
	map<int, UnionMercenaryInfo>::iterator mapit = m_mapUnionDispatchingMercenaryInfo.find(nMercenaryID);
	if (mapit != m_mapUnionDispatchingMercenaryInfo.end())
	{
		mapit->second.nEmployedTimes++;
		string str;
		StoDB(mapit->second, str);
		m_pRedisStorer->SetHashByField(m_strUnionMercenaryKey, mapit->first, str);
	}
}

//删除佣兵
void CUnionMercenaryModel::DelMercenary(int nMercenaryID)
{
	m_pRedisStorer->DelHashByField(m_strUnionMercenaryKey, nMercenaryID);
	m_mapUnionDispatchingMercenaryInfo.erase(nMercenaryID);
}

//清理召回超过24小时的佣兵
void CUnionMercenaryModel::ClearRecalled24HoursMercenary()
{
	for (map<int, UnionMercenaryInfo>::iterator mapit = m_mapUnionRecalledMercenaryInfo.begin(); mapit != m_mapUnionRecalledMercenaryInfo.end(); ++mapit)
	{
		if ((int)time(NULL) - mapit->second.nRecallTime > 86400)
		{
			DelMercenary(mapit->first);
		}
	}
}

//下发派遣中的佣兵时只发送客户端需要展示的数据，需要详细佣兵信息时根据佣兵ID来向后端请求
UnionMercenaryInfo* CUnionMercenaryModel::GetSingleMercenaryInfo(int nMercenaryID)
{
	map<int, UnionMercenaryInfo>::iterator mapit = m_mapUnionDispatchingMercenaryInfo.find(nMercenaryID);
	if (mapit != m_mapUnionDispatchingMercenaryInfo.end())
	{
		return &mapit->second;
	}
	else
	{
		mapit = m_mapUnionRecalledMercenaryInfo.find(nMercenaryID);
		if (mapit != m_mapUnionRecalledMercenaryInfo.end())
		{
			return &mapit->second;
		}
		else
		{
			KXLOGDEBUG("can't find UnionMercenaryInfo");
			return NULL;
		}
	}
}

bool CUnionMercenaryModel::GetSingleMercenaryInfo(int unionID, int nMercenaryID, UnionMercenaryInfo* info)
{
    Storage* pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);
    if (NULL == pStorage)
    {
        return false;
    }

    CRedisStorer *pRedisStorer = dynamic_cast<CRedisStorer*>(pStorage->GetStorer(unionID));
    if (NULL == pRedisStorer)
    {
        KXLOGDEBUG("GetSingleMercenaryInfo pRedisStorer is null!!, unionid %d", unionID);
        return false;
    }

    string strUnionMercenaryKey = ModelKey::UnionMercenaryKey(unionID);
    string strMercenaryInfo = "";
    if (SUCCESS != pRedisStorer->GetHashByField(strUnionMercenaryKey, nMercenaryID, strMercenaryInfo))
    {
        KXLOGDEBUG("GetSingleMercenaryInfo GetHashByField is fail!!, unionid %d, nMercenaryID %d", unionID, nMercenaryID);
        return false;
    }

    DBtoSt(strMercenaryInfo, *info);

    return true;
}

void CUnionMercenaryModel::UpdateSingleMercenary(int nMercenaryID)
{
	string strMercenaryInfo;
	if (FAILED != m_pRedisStorer->GetHashByField(m_strUnionMercenaryKey, nMercenaryID, strMercenaryInfo))
	{
		UnionMercenaryInfo stInfo = {};
		DBtoSt(strMercenaryInfo, stInfo);
		if (m_mapUnionDispatchingMercenaryInfo.find(nMercenaryID) != m_mapUnionDispatchingMercenaryInfo.end())
		{
			m_mapUnionDispatchingMercenaryInfo[nMercenaryID] = stInfo;
		}
	}

}


void CUnionMercenaryModel::DBtoSt(string& str, UnionMercenaryInfo& info)
{
	vector<int> vec;
	str = "[" + str +"]";
	CConfAnalytic::ToJsonInt(str, vec);

	if (vec.size() != MAX_MERCENARY_PROP_NUM)
	{
		return;
	}

	int count = 0;
	info.nUid = vec[count++];
	info.nDispatchTime = vec[count++];
	info.nRecallTime = vec[count++];
	info.nEmployedTimes = vec[count++];
	info.stSoldierCardData.SoldierId = vec[count++];
	info.stSoldierCardData.SoldierLv = vec[count++];
	info.stSoldierCardData.SoldierStar = vec[count++];
	info.stSoldierCardData.SoldierExp = vec[count++];
    for (int i = 0; i < TALENTMAX; i++)
    {
        info.stSoldierCardData.SoldierTalent[i] = vec[count++];
    }

	info.stSoldierCardData.EquipCnt = vec[count++];
	for (int i = EQUIPMENTSMIN; i < EQUIPMENTSMAX; i++)
	{
		info.stEquipments[i].confId = vec[count++];
		for (int j = EQUIPEFFETCMIN; j < EQUIPEFFECTMAX; j++)
		{
			info.stEquipments[i].cEffectID[j] = vec[count++];
			info.stEquipments[i].sEffectValue[j] = vec[count++];
		}
	}
}

void CUnionMercenaryModel::StoDB(UnionMercenaryInfo& info, string& str)
{
	str.clear();
	char temp[32];
	snprintf(temp, sizeof(temp), "%d", info.nUid);
	str += temp;
	snprintf(temp, sizeof(temp), "+%d", info.nDispatchTime);
	str += temp;
	snprintf(temp, sizeof(temp), "+%d", info.nRecallTime);
	str += temp;
	snprintf(temp, sizeof(temp), "+%d", info.nEmployedTimes);
	str += temp;
	snprintf(temp, sizeof(temp), "+%d", info.stSoldierCardData.SoldierId);
	str += temp;
	snprintf(temp, sizeof(temp), "+%d", info.stSoldierCardData.SoldierLv);
	str += temp;
	snprintf(temp, sizeof(temp), "+%d", info.stSoldierCardData.SoldierStar);
	str += temp;
	snprintf(temp, sizeof(temp), "+%d", info.stSoldierCardData.SoldierExp);
	str += temp;
    for (int i = 0; i < TALENTMAX; i++)
    {
        snprintf(temp, sizeof(temp), "+%d", info.stSoldierCardData.SoldierTalent[i]);
        str += temp;
    }
	snprintf(temp, sizeof(temp), "+%d", info.stSoldierCardData.EquipCnt);
	str += temp;
	for (int i = EQUIPMENTSMIN; i < EQUIPMENTSMAX; i++)
	{
		snprintf(temp, sizeof(temp), "+%d", info.stEquipments[i].confId);
		str += temp;
		for (int j = EQUIPEFFETCMIN; j < EQUIPEFFECTMAX; j++)
		{
			snprintf(temp, sizeof(temp), "+%u", info.stEquipments[i].cEffectID[j]);
			str += temp;
			snprintf(temp, sizeof(temp), "+%u", info.stEquipments[i].sEffectValue[j]);
			str += temp;
		}
	}
}





