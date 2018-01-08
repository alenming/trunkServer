#include "GlobalMailModel.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "GameDef.h"
#include "ItemDrop.h"
#include "ConfAnalytic.h"
#include "KXServer.h"

using namespace std;

CGlobalMailModel::CGlobalMailModel()
:m_pRedisStorer(NULL)
{

}

CGlobalMailModel::~CGlobalMailModel()
{

}

CGlobalMailModel *CGlobalMailModel::m_pInstance = NULL;
CGlobalMailModel *CGlobalMailModel::getInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CGlobalMailModel;
	}
	return m_pInstance;
}

void CGlobalMailModel::destroy()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CGlobalMailModel::init()
{
	Storage* pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_GLOBALMAIL);
	if (NULL == pStorage)
	{
		return false;
	}

	m_pRedisStorer = dynamic_cast<CRedisStorer*>(pStorage->GetStorer(0));
	if (m_pRedisStorer == NULL)
	{
		return false;
	}

	m_szGlobalMailKey = ModelKey::GlobalMailKey();
	Refresh();
	return true;
}

bool CGlobalMailModel::Refresh()
{
	m_IsLoadIng = true;
	if (m_pRedisStorer == NULL)
	{
		return false;
	}
	//全局邮件基本信息
	std::map<std::string, RedisBinaryData> globalMails;
	if (SUCCESS != m_pRedisStorer->GetHash(m_szGlobalMailKey, globalMails))
	{
		return false;
	}

	//KXLOGDEBUG("CGlobalMailModel::Refresh() size %d", globalMails.size());
	m_GlobalMailBase.clear();
	m_GlobalMailContexts.clear();
	m_GlobalMailItems.clear();
	DropItemInfo itemInfo;
	for (std::map<std::string, RedisBinaryData>::iterator iter = globalMails.begin();
		iter != globalMails.end(); ++iter)
	{
		int nMailID = atoi(iter->first.c_str() + 1);
		const char *pType = iter->first.c_str();
		char cType = (*pType);
		int nType = atoi(&cType);
		switch (nType)
		{
			case MAIL_BASE_TYPE:
			{
				if (iter->second.length > sizeof(BaseMailInfo))
				{
					KXLOGDEBUG("CGlobalMailModel::Refresh() continue,webMailID is %d", nMailID);
					continue;
				}

				memcpy(&m_GlobalMailBase[nMailID], iter->second.data, sizeof(BaseMailInfo));
			}
			break;
			case MAIL_ADDGOODS_TYPE:
			{
				int *pNum = reinterpret_cast<int*>(iter->second.data);
				DropItemInfo *pItem = reinterpret_cast<DropItemInfo *>(iter->second.data + sizeof(int));
				for (int k = 0; k < (*pNum); k++)
				{
					memcpy(&itemInfo, pItem, sizeof(DropItemInfo));
					m_GlobalMailItems[nMailID].push_back(itemInfo);
					pItem++;
				}
			}
			break;
			case MAIL_CONTEXT_TYPE:
			{
				char *pContext = reinterpret_cast<char*>(iter->second.data);
				m_GlobalMailContexts[nMailID] = pContext;
			}
			break;
			default:
				break;
			}
	}

	m_IsLoadIng = false;
	return true;
}

bool CGlobalMailModel::RemoveGlobalMail(int globalMailId)
{
	if (m_pRedisStorer == NULL)
	{
		return false;
	}
	char szField[32] = { 0 };
	std::map<int, BaseMailInfo>::iterator baseIter = m_GlobalMailBase.find(globalMailId);
	if (baseIter != m_GlobalMailBase.end())
	{
		snprintf(szField,sizeof(szField),"%d%d", MAIL_BASE_TYPE, globalMailId);
		if (SUCCESS != m_pRedisStorer->DelHashByField(m_szGlobalMailKey,(string)szField))
		{
			return false;
		}
		m_GlobalMailBase.erase(baseIter);
	}

	std::map<int, std::string>::iterator ctxIter = m_GlobalMailContexts.find(globalMailId);
	if (ctxIter != m_GlobalMailContexts.end())
	{
		snprintf(szField, sizeof(szField), "%d%d", MAIL_CONTEXT_TYPE, globalMailId);
		if (SUCCESS != m_pRedisStorer->DelHashByField(m_szGlobalMailKey, (string)szField))
		{
			return false;
		}
		m_GlobalMailContexts.erase(ctxIter);
	}
	
	std::map<int, std::vector<DropItemInfo> >::iterator itemIter = m_GlobalMailItems.find(globalMailId);
	if (itemIter != m_GlobalMailItems.end())
	{
		snprintf(szField, sizeof(szField), "%d%d", MAIL_ADDGOODS_TYPE, globalMailId);
		if (SUCCESS != m_pRedisStorer->DelHashByField(m_szGlobalMailKey, (string)szField))
		{
			return false;
		}
		m_GlobalMailItems.erase(itemIter);
	}

	return true;
}

bool CGlobalMailModel::GetGlobalMailBase(int globalMailId, BaseMailInfo& mailBase)
{
	if (m_GlobalMailBase.find(globalMailId) != m_GlobalMailBase.end())
	{
		mailBase = m_GlobalMailBase[globalMailId];
		return true;
	}
	return false;
}

bool CGlobalMailModel::GetGlobalMailContext(int globalMailId, string& mailCtx)
{
	if (m_GlobalMailContexts.find(globalMailId) != m_GlobalMailContexts.end())
	{
		mailCtx = m_GlobalMailContexts[globalMailId];
		return true;
	}
	return false;
}

bool CGlobalMailModel::GetGlobalMailItems(int globalMailId, std::vector<DropItemInfo> &items)
{
	if (m_GlobalMailItems.find(globalMailId) != m_GlobalMailItems.end())
	{
		items = m_GlobalMailItems[globalMailId];
		return true;
	}
	return false;
}

