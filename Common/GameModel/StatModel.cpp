#include "StatModel.h"
#include "ModelDef.h"
#include "RedisStorer.h"
#include "StorageManager.h"

CStatModel*		CStatModel::m_pInstance = NULL;

CStatModel *CStatModel::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CStatModel();
	}

	return m_pInstance;
}
void	CStatModel::destroy()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CStatModel::init()
{
	return true;
}

bool CStatModel::MessageStart(int nUid, std::string szType)
{
	m_MessageData.clear();
	m_MessageData = "{";
	
	Storage * pStorage = StorageManager::getInstance()->GetStorage(STORAGE_STAT);
	if (NULL == pStorage)
	{
		return false;
	}

	m_ModNum = pStorage->GetModeSize();

	m_StatUidKey = ModelKey::StatUidKey(nUid%m_ModNum);

	m_pRedisStorer = static_cast<CRedisStorer*>(pStorage->GetStorer(nUid%m_ModNum));

	if (m_pRedisStorer == NULL)
	{
		return false;
	}

	return MessageTypeAdd(szType);
}

bool CStatModel::MessageTypeAdd(std::string szType)
{
	m_MessageData = m_MessageData + "\"msgType\":" + "\"" + szType + "\",";
	m_MessageData = m_MessageData + "\"data\":{";
	return true;
}

bool CStatModel::MessageDataAdd(std::string szDataKey, std::string szDataValue,int Type)
{
	if (!Type)
	{
		m_MessageData = m_MessageData + "\"" + szDataKey + "\":" +"\"" +szDataValue + "\",";
	}
	else
	{
		m_MessageData = m_MessageData + "\"" + szDataKey + "\":" + "\"" + szDataValue + "\"";
	}
	
	return true;
}

bool CStatModel::MessageEnd(std::string szTime)
{
	m_MessageData = m_MessageData + "},";
	m_MessageData = m_MessageData + "\"timeStamp\":" + "\"" + szTime + "\"}";


	if (SUCCESS != m_pRedisStorer->ListPush(m_StatUidKey, (char*)m_MessageData.c_str(), m_MessageData.size()))
	{
		return false;
	}

	return true;
}

CStatModel::CStatModel()
{
}


CStatModel::~CStatModel()
{
}
