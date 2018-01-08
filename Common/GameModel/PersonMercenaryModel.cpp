#include "PersonMercenaryModel.h"

CPersonMercenaryModel::CPersonMercenaryModel() :m_pRedisStorer(NULL)
{
}

CPersonMercenaryModel::~CPersonMercenaryModel()
{
}

bool CPersonMercenaryModel::init(int uid)
{
	Storage* pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);//得到数据库对象
	if (NULL == pStorage)
	{
		KXLOGDEBUG("PersonMercenaryModel GetStorage Error");
		return false;
	}

	m_pRedisStorer = dynamic_cast<CRedisStorer*>(pStorage->GetStorer(uid));
	if (NULL == m_pRedisStorer)
	{
		KXLOGDEBUG("PersonMercenaryModel GetCRedisStorer Error");
		return false;
	}

	m_nUid = uid;
	m_strDispatchingMercenaryKey = ModelKey::PersonDispatchMercenaryKey(uid);
	m_strEmployedMercenaryKey = ModelKey::PersonEmployedMercenaryKey(uid);
	Refresh();

	return true;
}

bool CPersonMercenaryModel::Refresh()
{
	m_pRedisStorer->GetHash(m_strDispatchingMercenaryKey, m_mapPersonDispatchMercenaryInfo);
	m_pRedisStorer->GetSetAllString(m_strEmployedMercenaryKey, m_setPersonEmployedMercenaryInfo);

	return true;
}

//派遣佣兵到指定field，记录了佣兵ID，根据佣兵ID可以去公会索引佣兵详细信息
bool CPersonMercenaryModel::DispatchMercenary(int nMercenaryID, int field)
{
	if (m_mapPersonDispatchMercenaryInfo.size() != PERSONMAXDISPATCH)
	{
		m_mapPersonDispatchMercenaryInfo[field] = nMercenaryID;
		m_pRedisStorer->SetHashByField(m_strDispatchingMercenaryKey, field, nMercenaryID);

		return true;
	}
	return false;
}

//召回佣兵
bool CPersonMercenaryModel::RecallMercenary(int nMercenaryID)
{
	for (map<int, int>::iterator mapit = m_mapPersonDispatchMercenaryInfo.begin(); mapit != m_mapPersonDispatchMercenaryInfo.end(); ++mapit)
	{
		if (mapit->second == nMercenaryID)
		{
			m_pRedisStorer->DelHashByField(m_strDispatchingMercenaryKey, mapit->first);
			m_mapPersonDispatchMercenaryInfo.erase(mapit->first);
			return true;
		}
	}

	return false;
}

//记录雇佣过佣兵ID
void CPersonMercenaryModel::AddEmployedMercenaryInfo(int nMercenaryID)
{
	m_setPersonEmployedMercenaryInfo.insert(nMercenaryID);
	m_pRedisStorer->SetAdd(m_strEmployedMercenaryKey, nMercenaryID);

	//登陆时设置雇佣过的佣兵列表过期时间，防止服务器关闭而没有清除当天雇佣过的佣兵数据
	int nNowTime = (int)time(NULL);
	m_pRedisStorer->Expire(m_strEmployedMercenaryKey, CTimeCalcTool::nextTimeStampToZero(nNowTime, 0) - nNowTime);
}

//每天零点定时清除雇佣过的佣兵列表
void CPersonMercenaryModel::ClearAllEmployedMercenaryInfo()
{
	m_setPersonEmployedMercenaryInfo.clear();
	m_pRedisStorer->DelKey(m_strEmployedMercenaryKey);
}

//清除所有派遣的佣兵
void CPersonMercenaryModel::ClearAllDispatchedMercenary()
{
	m_mapPersonDispatchMercenaryInfo.clear();
	m_pRedisStorer->DelKey(m_strDispatchingMercenaryKey);
}

//根据佣兵ID返回是几号位
int CPersonMercenaryModel::GetMercenaryField(int nMercenaryID)
{
	for (map<int, int>::iterator mapit = m_mapPersonDispatchMercenaryInfo.begin(); mapit != m_mapPersonDispatchMercenaryInfo.end(); ++mapit)
	{
		if (mapit->second == nMercenaryID)
		{
			return mapit->first;
		}
	}
	return 0;
}

void CPersonMercenaryModel::SetOffRemoveMercenaryInfo(int nUid, std::map<int, int>& MemrcenaryInfo)
{
	Storage* pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);//得到数据库对象
	if (NULL == pStorage)
	{
		KXLOGDEBUG("PersonMercenaryModel GetStorage Error");
		return;
	}

	CRedisStorer* pRedisStorer = dynamic_cast<CRedisStorer*>(pStorage->GetStorer(nUid));
	if (NULL == pRedisStorer)
	{
		KXLOGDEBUG("PersonMercenaryModel GetCRedisStorer Error");
		return;
	}

	string strDispatchingMercenaryKey = ModelKey::PersonDispatchMercenaryKey(nUid);
	string strEmployedMercenaryKey = ModelKey::PersonEmployedMercenaryKey(nUid);

	pRedisStorer->GetHash(strDispatchingMercenaryKey,MemrcenaryInfo);
	pRedisStorer->DelKey(strDispatchingMercenaryKey);
	pRedisStorer->DelKey(strEmployedMercenaryKey);
}


