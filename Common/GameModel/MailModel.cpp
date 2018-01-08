#include "MailModel.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "KxMemPool.h"
#include <algorithm>
#include "ConfAnalytic.h"
#include "KXServer.h"
#include "ConfHall.h"
#include "GameDef.h"

using namespace std;
using namespace KxServer;

CMailModel::CMailModel() :m_nUID(0)
, m_pStorage(NULL)
{
}

CMailModel::~CMailModel()
{

}

bool CMailModel::init(int uid)
{
    m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_MAIL);
    if (NULL == m_pStorage)
    {
        return false;
    }
    m_nUID = uid;
	m_NormalMailKey = ModelKey::MailKey(uid);

	return Refresh();
}

bool CMailModel::Refresh()
{
    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (redisStorer == NULL)
	{
		return false;
	}
	//一开始时先清除数据
	ClearData();
	map<string, RedisBinaryData> mapRedisData;
	if (SUCCESS != redisStorer->GetHash(m_NormalMailKey, mapRedisData))
	{
		return false;
	}
	KXLOGDEBUG("get mail key %s size %d", m_NormalMailKey.c_str(), mapRedisData.size());
	DropItemInfo itemInfo;
	map<string, RedisBinaryData>::iterator iter = mapRedisData.begin();
	for (; iter != mapRedisData.end(); ++iter)
	{
		if (iter->first.length() < 2)
		{
			KXLOGERROR("mail id error %s", iter->first.c_str());
			continue;
		}

		int nMailID = atoi(iter->first.c_str() + 1);
		const char *pType = iter->first.c_str();
		char cType = (*pType);
		int nType = atoi(&cType);
		switch (nType)
		{
			case MAIL_BASE_TYPE:
			{
				if (iter->second.length > sizeof(BaseMailInfo) || iter->second.length < sizeof(int)*2)
				{
					RemoveMail(nMailID);
					KXLOGDEBUG("remove MAIL_BASE_TYPE %d", nMailID);
					continue;
				}
				memcpy(&m_MailInfoMap[nMailID], iter->second.data, sizeof(BaseMailInfo));
				//KXLOGDEBUG("add MAIL_BASE_TYPE %d", nMailID);
			}
			break;
			case MAIL_ADDGOODS_TYPE:
			{
				int num = *(reinterpret_cast<int*>(iter->second.data));
				//数据不合法
				if ((num < 0 || num >  100)
					&& iter->second.length < sizeof(int) + num * sizeof(DropItemInfo))
				{
					RemoveMail(nMailID);
					KXLOGDEBUG("add MAIL_ADDGOODS_TYPE %d remove %d", nMailID, num);
					continue;
				}
				DropItemInfo *pItem = reinterpret_cast<DropItemInfo *>(iter->second.data + sizeof(int));
				for (int k = 0; k < num; k++)
				{
					memcpy(&itemInfo, pItem, sizeof(DropItemInfo));
					m_MailItemInfoMap[nMailID].push_back(itemInfo);
					pItem++;
				}
				//KXLOGDEBUG("add MAIL_ADDGOODS_TYPE %d count %d", nMailID, num);
			}
			break;
			case MAIL_CONTEXT_TYPE:
			{
				char *pContext = reinterpret_cast<char*>(iter->second.data);
				m_MailContextMap[nMailID] = pContext;
				//KXLOGDEBUG("add MAIL_CONTEXT_TYPE %d", nMailID);
			}
			break;
			case MAIL_GMID_TYPE:
			{
				int state = *(reinterpret_cast<int*>(iter->second.data));
				m_GMMailStatMap[nMailID] = state;
				//KXLOGDEBUG("add MAIL_GMID_TYPE %d state %d", nMailID, state);
			}
			break;
			default:
				break;
		}
	}

    return true;
}

bool CMailModel::AddNewMail(const BaseMailInfo& info, const std::vector<DropItemInfo> &items, int &nMailID)
{
    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (redisStorer == NULL)
	{
		return false;
	}

	nMailID = 1;
	redisStorer->IncreHashByField(m_NormalMailKey, 0, nMailID);
	//默认保留30天Key
	int nCurTime = time(NULL) + 30 * 24 * 3600;
	redisStorer->Expire(m_NormalMailKey, nCurTime);
	
	//删除最早的一封邮件
	if (m_MailInfoMap.size() >= MAX_MAIL_NUM)
	{
		std::map<int, BaseMailInfo>::iterator iter = m_MailInfoMap.begin();
		RemoveMail(iter->first);
	}

	map<int, BaseMailInfo>::iterator iter = m_MailInfoMap.find(nMailID);
	if (iter == m_MailInfoMap.end())
	{
		memcpy(&m_MailInfoMap[nMailID], &info, sizeof(BaseMailInfo));
		if (!items.empty())
		{
            if (items.size() > 50)
            {
                m_MailItemInfoMap[nMailID].assign(items.begin(), items.begin() + 50);
            }
            else
            {
                m_MailItemInfoMap[nMailID].assign(items.begin(), items.end());
            }
		}

		return SaveMailData(nMailID);
	}

    return true;
}

bool CMailModel::AddNewMail(const BaseMailInfo& info, int &nMailID)
{
    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
    if (redisStorer == NULL)
    {
        return false;
    }

    nMailID = 1;
    redisStorer->IncreHashByField(m_NormalMailKey, 0, nMailID);
    //默认保留30天Key
    int nCurTime = time(NULL) + 30 * 24 * 3600;
    redisStorer->Expire(m_NormalMailKey, nCurTime);

    //删除最早的一封邮件
    if (m_MailInfoMap.size() >= MAX_MAIL_NUM)
    {
        std::map<int, BaseMailInfo>::iterator iter = m_MailInfoMap.begin();
        RemoveMail(iter->first);
    }

    map<int, BaseMailInfo>::iterator iter = m_MailInfoMap.find(nMailID);
    if (iter == m_MailInfoMap.end())
    {
        memcpy(&m_MailInfoMap[nMailID], &info, sizeof(BaseMailInfo));

        return SaveMailData(nMailID);
    }

    return false;
}

bool CMailModel::GetMailInfo(int mailID, BaseMailInfo &info, std::vector<DropItemInfo> &items, bool bNew)
{
	if (bNew)
	{
		GetMailsFromDB(mailID);
	}

    map<int, BaseMailInfo>::iterator iterMail = m_MailInfoMap.find(mailID);
    if (iterMail == m_MailInfoMap.end())
    {
        return false;
    }

    memcpy(&info, &iterMail->second, sizeof(BaseMailInfo));
    map<int, std::vector<DropItemInfo> >::iterator iterItems = m_MailItemInfoMap.find(mailID);
    if (iterItems != m_MailItemInfoMap.end())
    {
        items.assign(iterItems->second.begin(), iterItems->second.end());
    }

    return true;
}

bool CMailModel::ReadMail(int mailID)
{
    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
    if (redisStorer)
    {
        map<int, BaseMailInfo>::iterator iter = m_MailInfoMap.find(mailID);
        if (iter != m_MailInfoMap.end())
        {
            return SaveMailData(mailID);
        }
    }

    return false;
}

bool CMailModel::RemoveMail(int mailID)
{
    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (redisStorer == NULL)
	{
		return false;
	}

	map<int, BaseMailInfo>::iterator iter = m_MailInfoMap.find(mailID);
	char szField[32] = { 0 };

	if (iter != m_MailInfoMap.end())
	{
		//删除基本信息
		snprintf(szField,sizeof(szField),"%d%d", MAIL_BASE_TYPE, mailID);
		if (SUCCESS != redisStorer->DelHashByField(m_NormalMailKey,(string)szField))
		{
			return false;
		}

		m_MailInfoMap.erase(iter);
		map<int, vector<DropItemInfo> >::iterator iterEquip = m_MailItemInfoMap.find(mailID);
		if (iterEquip != m_MailItemInfoMap.end())
		{
			iterEquip->second.clear();
			m_MailItemInfoMap.erase(iterEquip);
		}

		//删除附件
		snprintf(szField, sizeof(szField), "%d%d", MAIL_ADDGOODS_TYPE, mailID);
		if (SUCCESS != redisStorer->DelHashByField(m_NormalMailKey, (string)szField))
		{
			return false;
		}
	}

	map<int, string>::iterator ator = m_MailContextMap.find(mailID);
	if (ator != m_MailContextMap.end())
	{
		//删除邮件内容
		snprintf(szField, sizeof(szField), "%d%d", MAIL_CONTEXT_TYPE, mailID);
		if (SUCCESS != redisStorer->DelHashByField(m_NormalMailKey, (string)szField))
		{
			return false;
		}

		m_MailContextMap.erase(mailID);
	}

	return true;
}

bool CMailModel::AddWebMail(int webMailID,int nState)
{
    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (redisStorer == NULL)
	{
		return false;
	}

	char szField[32] = { 0 };
	snprintf(szField, sizeof(szField), "%d%d", MAIL_GMID_TYPE, webMailID);
	if (SUCCESS != redisStorer->SetHashByField(m_NormalMailKey, (string)szField, (char*)&nState, sizeof(int)))
	{
		return false;
	}

	m_GMMailStatMap[webMailID] = nState;
	KXLOGDEBUG("CMailModel::AddWebMail(int webMailID,int nState),webMailID is %d,nState is %d", webMailID, nState);
	return true;
}

bool CMailModel::RemoveWebMail(int mailID)
{
    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (NULL == redisStorer)
	{
		return false;
	}

	char szField[32] = { 0 };
	snprintf(szField, sizeof(szField), "%d%d", MAIL_GMID_TYPE, mailID);
	if (SUCCESS != redisStorer->DelHashByField(m_NormalMailKey, (string)szField))
	{
		return false;
	}

	m_GMMailStatMap.erase(mailID);
	KXLOGDEBUG("CMailModel::RemoveWebMail(int webMailID,int nState),webMailID is %d", mailID);
    return true;
}

bool CMailModel::IsReadWebMail(int webMailId)
{
	std::map<int, int>::iterator ator = m_GMMailStatMap.find(webMailId);

	if (ator == m_GMMailStatMap.end())
	{
		return false;
	}
	else
	{
		int nState = ator->second;

		if (nState != MAIL_STATUS_READ)
		{
			KXLOGDEBUG("CMailModel::IsReadWebMail(int webMailId),webMailId is %d,nState is %d", webMailId,nState);
			return false;
		}
	}
	KXLOGDEBUG("CMailModel::IsReadWebMail(int webMailId),webMailId is %d", webMailId);
	return true;
}

bool CMailModel::DeleteMail()
{
    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
    if (redisStorer)
    {
        string key = ModelKey::MailKey(m_nUID);
        if (SUCCESS == redisStorer->DelKey(key))
        {
            ClearData();
            return true;
        }
    }

    return false;
}

map<int, BaseMailInfo>& CMailModel::GetMailBasicInfo()
{
    return m_MailInfoMap;
}

map<int, std::vector<DropItemInfo> >& CMailModel::GetMailItemInfo()
{
    return m_MailItemInfoMap;
}

std::map<int, int>& CMailModel::GetWebMails()
{
	return m_GMMailStatMap;
}

std::map<int, string>& CMailModel::GetMailContextInfo()
{
	return m_MailContextMap;
}

void CMailModel::ClearData()
{
    m_MailInfoMap.clear();
	m_GMMailStatMap.clear();
    m_MailItemInfoMap.clear();
	m_MailContextMap.clear();
}

bool CMailModel::SaveMailData(int mailID)
{
    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (redisStorer == NULL)
	{
		return false;
	}

	map<int, BaseMailInfo>::iterator iter = m_MailInfoMap.find(mailID);
	char szField[32] = { 0 };
	if (iter != m_MailInfoMap.end())
	{
		snprintf(szField, sizeof(szField), "%d%d", MAIL_BASE_TYPE, mailID);
		redisStorer->SetHashByField(m_NormalMailKey,(string)szField,(char*)&iter->second, sizeof(BaseMailInfo));
	}

	int nBuffSize = sizeof(int);
	map<int, vector<DropItemInfo> >::iterator iterItems = m_MailItemInfoMap.find(mailID);
	if (iterItems != m_MailItemInfoMap.end())
	{
		nBuffSize += iterItems->second.size() * sizeof(DropItemInfo);
	}

	char *buff = reinterpret_cast<char*>(kxMemMgrAlocate(nBuffSize));
	int *pMailNum = reinterpret_cast<int*>(buff);
	if (iterItems == m_MailItemInfoMap.end())
	{
		(*pMailNum) = 0;
	}
	else
	{
		(*pMailNum) = iterItems->second.size();
		DropItemInfo *pItemInfo = reinterpret_cast<DropItemInfo *>(pMailNum + 1);
		if (iterItems != m_MailItemInfoMap.end())
		{
			vector<DropItemInfo>::iterator iterItem = iterItems->second.begin();
			for (; iterItem != iterItems->second.end(); ++iterItem)
			{
				memcpy(pItemInfo, &(*iterItem), sizeof(DropItemInfo));
				pItemInfo += 1;
			}
		}
	}
	
	snprintf(szField, sizeof(szField), "%d%d", MAIL_ADDGOODS_TYPE, mailID);
	bool bRlt = (SUCCESS == redisStorer->SetHashByField(m_NormalMailKey,(string)szField, buff, nBuffSize));
	kxMemMgrRecycle(buff, nBuffSize);

	return bRlt;
}

bool CMailModel::GetMailsFromDB()
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (redisStorer == NULL)
	{
		return false;
	}
	//一开始时先清除数据
	ClearData();
	map<string, RedisBinaryData> mapRedisData;
	if (SUCCESS != redisStorer->GetHash(m_NormalMailKey, mapRedisData))
	{
		return false;
	}
	DropItemInfo itemInfo;
	map<string, RedisBinaryData>::iterator iter = mapRedisData.begin();
	for (; iter != mapRedisData.end(); ++iter)
	{
		const char *pType = iter->first.c_str();
		char cType = (*pType);
		int nType = atoi(&cType);
		int nMailID = 0;
		switch (nType)
		{
			case MAIL_BASE_TYPE:
			{
				pType++;
				nMailID = atoi(pType);
				memcpy(&m_MailInfoMap[nMailID], iter->second.data, sizeof(BaseMailInfo));
			}
				break;
			case MAIL_ADDGOODS_TYPE:
			{
				pType++;
				nMailID = atoi(pType);
				int *pNum = reinterpret_cast<int*>(iter->second.data);
				DropItemInfo *pItem = reinterpret_cast<DropItemInfo *>(iter->second.data + sizeof(int));
				for (int k = 0; k < (*pNum); k++)
				{
					memcpy(&itemInfo, pItem, sizeof(DropItemInfo));
					m_MailItemInfoMap[nMailID].push_back(itemInfo);
					pItem++;
				}
			}
			break;
			case MAIL_CONTEXT_TYPE:
			{
				pType++;
				nMailID = atoi(pType);
				char *pContext = reinterpret_cast<char*>(iter->second.data);
				m_MailContextMap[nMailID] = pContext;
			}
			break;
			case MAIL_GMID_TYPE:
			{
				pType++;
				nMailID = atoi(pType);
				int *pState = reinterpret_cast<int*>(iter->second.data);
				m_GMMailStatMap[nMailID] = *pState;
			}
			break;
			default:
				break;
		}
	}

	return true;
}


bool CMailModel::GetMailsFromDB(int nMailID)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUID));
	if (redisStorer == NULL)
	{
		return false;
	}

	char szField[32] = { 0 };
	RedisBinaryData RedisData;
	int nLen = 0;
	snprintf(szField, sizeof(szField), "%d%d", MAIL_BASE_TYPE, nMailID);
	if (SUCCESS != redisStorer->GetHashByField(m_NormalMailKey,(string)szField, (char*)&RedisData, nLen))
	{
		return false;
	}

	map<int, BaseMailInfo>::iterator iter = m_MailInfoMap.find(nMailID);
	std::map<int, std::vector<DropItemInfo> >::iterator ator = m_MailItemInfoMap.find(nMailID);
	if (iter != m_MailInfoMap.end())
	{
		memcpy(&iter->second, RedisData.data,sizeof(BaseMailInfo));
		snprintf(szField, sizeof(szField), "%d%d", MAIL_ADDGOODS_TYPE, nMailID);
		if (SUCCESS != redisStorer->GetHashByField(m_NormalMailKey, (string)szField, (char*)&RedisData, nLen))
		{
			m_MailItemInfoMap.erase(nMailID);
		}
		else
		{
			m_MailItemInfoMap[nMailID].clear();
			int *pNum = reinterpret_cast<int *>(RedisData.data);
			DropItemInfo itemInfo;
			DropItemInfo *pItemInfo = reinterpret_cast<DropItemInfo *>(RedisData.data + sizeof(int));
			for (int k = 0; k < (*pNum); k++)
			{
				memcpy(&itemInfo,pItemInfo, sizeof(DropItemInfo));
				m_MailItemInfoMap[nMailID].push_back(itemInfo);
				pItemInfo++;
			}
		}
		snprintf(szField, sizeof(szField), "%d%d", MAIL_CONTEXT_TYPE, nMailID);
		if (SUCCESS != redisStorer->GetHashByField(m_NormalMailKey, (string)szField, (char*)&RedisData, nLen))
		{
			m_MailContextMap.erase(nMailID);
		}
		else
		{
			char *pContext = reinterpret_cast<char*>(RedisData.data);
			m_MailContextMap[nMailID] = pContext;
		}
	}
	else
	{
		BaseMailInfo MailInfo;
		memcpy(&MailInfo, RedisData.data, sizeof(BaseMailInfo));
		m_MailInfoMap[nMailID] = MailInfo;

		snprintf(szField, sizeof(szField), "%d%d", MAIL_ADDGOODS_TYPE, nMailID);
		if (SUCCESS == redisStorer->GetHashByField(m_NormalMailKey, (string)szField, (char*)&RedisData, nLen))
		{
			int *pNum = reinterpret_cast<int *>(RedisData.data);
			DropItemInfo itemInfo;
			DropItemInfo *pItemInfo = reinterpret_cast<DropItemInfo *>(RedisData.data + sizeof(int));
			vector<DropItemInfo> VectItem;
			for (int k = 0; k < (*pNum); k++)
			{
				memcpy(&itemInfo, pItemInfo, sizeof(DropItemInfo));
				VectItem.push_back(itemInfo);
				pItemInfo++;
			}

			m_MailItemInfoMap[nMailID] = VectItem;
		}

		snprintf(szField, sizeof(szField), "%d%d", MAIL_CONTEXT_TYPE, nMailID);
		if (SUCCESS == redisStorer->GetHashByField(m_NormalMailKey, (string)szField, (char*)&RedisData, nLen))
		{
			char *pContext = reinterpret_cast<char*>(RedisData.data);
			m_MailContextMap[nMailID] = pContext;
		}
	}

	return true;
}

bool CMailModel::AddOfflineNewMail(int uid, const BaseMailInfo& info, const std::vector<DropItemInfo> &items)
{
    Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_MAIL);
    if (NULL == pStorage)
    {
        return false;
    }

    CRedisStorer *redisStorer = static_cast<CRedisStorer*>(pStorage->GetStorer(uid));
    if (redisStorer == NULL)
    {
        return false;
    }

    std::string strNormalMailKey = ModelKey::MailKey(uid);
    if (SUCCESS != redisStorer->ExistKey(strNormalMailKey))
    {
        return false;
    }

    int nMailID = 1;
    redisStorer->IncreHashByField(strNormalMailKey, 0, nMailID);
    //默认保留30天Key
    int nCurTime = time(NULL) + 30 * 24 * 3600;
    redisStorer->Expire(strNormalMailKey, nCurTime);

    char szField[32] = { 0 };
    snprintf(szField, sizeof(szField), "%d%d", MAIL_BASE_TYPE, nMailID);
    redisStorer->SetHashByField(strNormalMailKey, (string)szField, (char*)&info, sizeof(BaseMailInfo));

    int nBuffSize = sizeof(int)+items.size() * sizeof(DropItemInfo);
    char *buff = reinterpret_cast<char*>(kxMemMgrAlocate(nBuffSize));
    int *pMailNum = reinterpret_cast<int*>(buff);
    (*pMailNum) = items.size();

    DropItemInfo *pItemInfo = reinterpret_cast<DropItemInfo *>(pMailNum + 1);
    for (unsigned int i = 0; i < items.size(); ++i)
    {
        memcpy(pItemInfo, &(items[i]), sizeof(DropItemInfo));
        pItemInfo += 1;
    }

    snprintf(szField, sizeof(szField), "%d%d", MAIL_ADDGOODS_TYPE, nMailID);
    bool bRlt = (SUCCESS == redisStorer->SetHashByField(strNormalMailKey, (string)szField, buff, nBuffSize));
    kxMemMgrRecycle(buff, nBuffSize);

    return bRlt;
}
