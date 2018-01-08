#include "HeadModel.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "ConfAnalytic.h"
#include "KXServer.h"

using namespace std;


CHeadModel::CHeadModel()
{
}


CHeadModel::~CHeadModel()
{
}


bool CHeadModel::init(int uid)
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_STORY);
	if (NULL == m_pStorage)
	{
		return false;
	}

	m_nUid = uid;
	m_strHeadKey = ModelKey::HeadKey(m_nUid);

	Refresh();
	return true;
}

bool CHeadModel::Refresh()
{
	if (STORE_REDIS != m_pStorage->GetStoreType())
	{
		return false;
	}

	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == redisStorer)
	{
		return false;
	}

	// 如果不存在引导也返回true
	if (SUCCESS != redisStorer->ExistKey(m_strHeadKey))
	{
		return true;
	}

	//引导信息
	vector<int> VectHeadID;
	if (SUCCESS != redisStorer->ListRange(m_strHeadKey,VectHeadID))
	{
		return false;
	}

	m_SetHeadID.clear();
	
	for (int i = 0; i < (int)VectHeadID.size(); i++)
	{
		m_SetHeadID.insert(VectHeadID[i]);
	}

	return true;
}

bool CHeadModel::AddHeadID(int nHeadID)
{
	std::set<int>::iterator ator = m_SetHeadID.find(nHeadID);

	if (ator != m_SetHeadID.end())
	{
		return false;
	}

	m_SetHeadID.insert(nHeadID);

	return SetHeadIDToDB(nHeadID);
}

bool CHeadModel::IsHeadIDExist(int nHeadID)
{
	std::set<int>::iterator ator = m_SetHeadID.find(nHeadID);

	if (ator != m_SetHeadID.end())
	{
		return true;
	}

	return false;
}


bool CHeadModel::SetHeadIDToDB(int nHeadID)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == pStorer)
	{
		return false;
	}

	if (SUCCESS == pStorer->ListPush(m_strHeadKey,nHeadID))
	{
		return true;
	}

	return false;
}
