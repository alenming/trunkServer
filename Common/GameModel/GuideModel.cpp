#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "ConfAnalytic.h"
#include "GuideModel.h"
#include "KXServer.h"

using namespace std;

CGuideModel::CGuideModel()
: m_nUid(0)
, m_pStorage(NULL)
{
}

CGuideModel::~CGuideModel()
{
}

bool CGuideModel::init(int uid)
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_STORY);
	if (NULL == m_pStorage)
	{
		return false;
	}

	m_nUid = uid;
	m_strGuideKey = ModelKey::GuideKey(m_nUid);
	
	Refresh();
	return true;
}

bool CGuideModel::Refresh()
{
	CRedisStorer *redisStorer = dynamic_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == redisStorer)
	{
		return false;
	}
	// 如果不存在引导也返回true
	if (SUCCESS != redisStorer->ExistKey(m_strGuideKey))
	{
		return true;
	}
	//引导信息
	string guideInfo;
	if (SUCCESS != redisStorer->GetHashByField(m_strGuideKey, NewPlayerGuideType, guideInfo))
	{
		return false;
	}
	guideInfo = "[" + guideInfo + "]";
	m_SetGuide.clear();
	VecInt vec;
	CConfAnalytic::ToJsonInt(guideInfo, vec);
	for (size_t i = 0; i < vec.size(); i++)
	{
		m_SetGuide.insert(vec[i]);
	}
	return true;
}

bool CGuideModel::AddGuideID(int nValue)
{
	std::set<int>::iterator ator = m_SetGuide.find(nValue);
	if (ator != m_SetGuide.end())
	{
		return false;
	}
	m_SetGuide.insert(nValue);
	return SaveGuideToDB();
}

bool CGuideModel::DelGuideID(int nValue)
{
	std::set<int>::iterator ator = m_SetGuide.find(nValue);
	if (ator == m_SetGuide.end())
	{
		return false;
	}
	m_SetGuide.erase(nValue);
	return SaveGuideToDB();
}

bool CGuideModel::SaveGuideToDB()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == pStorer)
	{
		return false;
	}

	std::string saveStr = "";
	for (std::set<int>::iterator iter = m_SetGuide.begin(); 
		iter != m_SetGuide.end(); ++iter)
	{
		char temp[32] = {};
		if (iter == m_SetGuide.begin())
		{
			sprintf(temp, "%d", *iter);
		}
		else
		{
			sprintf(temp, "+%d", *iter);
		}
		saveStr += temp;
	}

	return SUCCESS == pStorer->SetHashByField(m_strGuideKey, NewPlayerGuideType, saveStr);
}
