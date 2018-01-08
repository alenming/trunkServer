#include "UserModel.h"
#include "RedisStorer.h"
#include "Storage.h"
#include "StorageManager.h"
#include "ModelDef.h"
#include "ConfAnalytic.h"

CUserModel::CUserModel() 
: m_nUid(0)
, m_pStorage(NULL)
{
}

CUserModel::~CUserModel()
{
}

bool CUserModel::init(int uid)
{
	m_nUid = uid;
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_USER);
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		m_strUsrKey = ModelKey::UsrKey(m_nUid);
		if (SUCCESS != pStorer->ExistKey(m_strUsrKey))
		{
			return false;
		}
	}

	Refresh();
	return true;
}

bool CUserModel::Refresh()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
        for (int i = USR_FD_USERID; i < USR_FD_END; i++)
		{
			m_mapUserInfo[i] = 0;
		}

		if (SUCCESS != pStorer->GetHashByField(m_strUsrKey, m_mapUserInfo))
		{
			// 没有用户信息
			return false;
		}

		if (SUCCESS != pStorer->GetHashByField(m_strUsrKey, USR_FD_USERNAME, m_strUserName))
		{
			// 用户不存在
			return false;
		}

		std::string QQVipKey = ModelKey::ckExtraDataKey(m_nUid);
		std::string strQQExtraData;
		//没有蓝钻
		if (SUCCESS != pStorer->GetString(QQVipKey, strQQExtraData))
		{
			return true;
		}

		//有蓝钻，数据必须合法
		VecInt VectValue;
		strQQExtraData = "[" + strQQExtraData + "]";
		CConfAnalytic::ToJsonInt(strQQExtraData, VectValue);
		if (VectValue.size() != 3)
		{
			return false;
		}

		m_mapUserInfo[USR_FD_QQVIPTYPE] = VectValue[0];
		m_mapUserInfo[USR_FD_QQVIPLV] = VectValue[1];
		m_mapUserInfo[USR_FD_QQVIPTIMESTAMP] = VectValue[2];
		
        return true;
	}

	return false;
}

bool CUserModel::NewUser(int uid, std::string name, std::map<int, int> &info)
{
	m_nUid = uid;
	m_mapUserInfo = info;
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_USER);
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		m_strUsrKey = ModelKey::UsrKey(m_nUid);
		std::string QQVipKey = ModelKey::ckExtraDataKey(m_nUid);
		std::string strQQExtraData;
		//没有蓝钻
		if (SUCCESS == pStorer->GetString(QQVipKey, strQQExtraData))
		{
			strQQExtraData = "[" + strQQExtraData + "]";
			//有蓝钻，数据必须合法
			VecInt VectValue;
			CConfAnalytic::ToJsonInt(strQQExtraData, VectValue);
			if (VectValue.size() != 3)
			{
				return false;
			}

			m_mapUserInfo[USR_FD_QQVIPTYPE] = VectValue[0];
			m_mapUserInfo[USR_FD_QQVIPLV] = VectValue[1];
			m_mapUserInfo[USR_FD_QQVIPTIMESTAMP] = VectValue[2];
		}

		if (SUCCESS != pStorer->SetHash(m_strUsrKey, m_mapUserInfo))
		{
			return false;
		}

		m_strUserName = name;
		return SUCCESS == pStorer->SetHashByField(m_strUsrKey, USR_FD_USERNAME, name);
	}
	return false;
}

std::string& CUserModel::GetName()
{
	return m_strUserName;
}

std::map<int, int>& CUserModel::GetUserInfo()
{
	return m_mapUserInfo;
}

bool CUserModel::GetUserFieldVal(int field, int &value, bool bNew)
{
	if (bNew)
	{
		if (!GetRealDataFromDB(field, value))
		{
			return false;
		}
		m_mapUserInfo[field] = value;
	}
	else
	{
		std::map<int, int>::iterator iter = m_mapUserInfo.find(field);
		if (iter == m_mapUserInfo.end())
		{
			return false;
		}
		value = iter->second;
	}
	return true;
}

bool CUserModel::GetUserFieldVal(std::map<int, int> &kvs, bool bNew)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		if (bNew)
		{
			if (SUCCESS != pStorer->GetHash(m_strUsrKey, m_mapUserInfo))
			{
				return false;
			}
		}

		for (std::map<int, int>::iterator iter = kvs.begin();
			iter != kvs.end(); ++iter)
		{
			if (m_mapUserInfo.find(iter->first) != m_mapUserInfo.end())
			{
				kvs[iter->first] = m_mapUserInfo[iter->first];
			}
		}
		return true;	
	}
	return false;
}

bool CUserModel::SetUserFieldVal(int field, int value)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		if (SUCCESS == pStorer->SetHashByField(m_strUsrKey, field, value))
		{
			m_mapUserInfo[field] = value;
			return true;
		}
	}
	return false;
}

bool CUserModel::AddUserFieldVal(int field, int value)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		m_mapUserInfo[field] += value;
		if (SUCCESS == pStorer->SetHashByField(m_strUsrKey, field, m_mapUserInfo[field]))
		{
			return true;
		}
	}
	return false;
}

int CUserModel::IncreaseFieldVal(int field, int increase)
{
	if (field != USR_FD_HEROREF
		&& field != USR_FD_EQUIPREF
		&& field != USR_FD_MAILREF
		&& field != USR_FD_DIAMOND)
	{
		return 0;
	}

	int value = 0;
	GetUserFieldVal(field, value);
	value += increase;
	if (!SetUserFieldVal(field, value))
	{
		return 0;
	}
	return value;
}

bool CUserModel::SetUserFieldVal(std::map<int, int> &kvs)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		if (SUCCESS == pStorer->SetHash(m_strUsrKey, kvs))
		{
            std::map<int, int>::iterator iter = kvs.begin();
            for (; iter != kvs.end(); ++iter)
            {
                m_mapUserInfo[iter->first] = iter->second;
            }

			return true;
		}
	}
	return false;
}

bool CUserModel::ModUserName(std::string name)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		if (SUCCESS == pStorer->SetHashByField(m_strUsrKey, USR_FD_USERNAME, name))
		{
			m_strUserName = name;
			return true;
		}
	}
	return false;
}

bool CUserModel::AlterUserFieldVal(int field, int value)
{
    std::map<int, int>::iterator iter = m_mapUserInfo.find(field);
    if (iter != m_mapUserInfo.end())
    {
        iter->second = value;
        return true;
    }

    return false;
}

bool CUserModel::DeleteUser()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		return SUCCESS == pStorer->DelKey(m_strUsrKey);
	}
	return false;
}

bool CUserModel::SetUserInfo(std::map<int, int> &userInfo)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
    if (NULL != pStorer)
    {
		if (SUCCESS == pStorer->SetHash(m_strUsrKey, userInfo))
        {
            m_mapUserInfo = userInfo;
            return true;
        }
    }
    return false;
}

bool CUserModel::GetRealDataFromDB(int field, int &value)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == pStorer)
	{
		return false;
	}
	if (SUCCESS != pStorer->GetHashByField(m_strUsrKey, field, value))
	{
		return false;
	}

	return true;
}

int CUserModel::getIdentity()
{
    //if (time(NULL) > m_UserInfo[USR_FD_QQVIPTIMESTAMP])
    //{
    //    return 0;
    //}
    if (m_mapUserInfo[USR_FD_QQVIPTYPE] <= 0)
    {
        return 0;
    }

    return m_mapUserInfo[USR_FD_QQVIPLV] * 10 + m_mapUserInfo[USR_FD_QQVIPTYPE];
}
