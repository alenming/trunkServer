#include "UnionModel.h"
#include "StorageManager.h"
#include "ModelDef.h"
#include <algorithm>
#include "UnionComm.h"

#define CHECK_RETURN(success) if(!(success)) { return false; }

using namespace std;

#define LOG_COUNT 50

vector<int> CUnionModel::m_VectUnionID;
std::map<int, int>       CUnionModel::m_UnionInfo;

CUnionModel::CUnionModel() :m_nUnionID(0)
, m_pStorage(NULL)
{
    m_UnionName = "";
    m_UnionNotice = "";
}

CUnionModel::~CUnionModel()
{

}

bool CUnionModel::CreateUnionList(int nServerID)
{
    m_VectUnionID.clear();
    m_UnionInfo.clear();

    Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_SERVER);
    if (pStorage == NULL)
    {
        return false;
    }
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(pStorage->GetStorer(1));
    if (pStorer == NULL)
    {
        return false;
    }

    string unionkey = ModelKey::UnionIDListKey(nServerID);
    if (SUCCESS == pStorer->ExistKey(unionkey))
    {
        pStorer->ListRange(unionkey, m_VectUnionID);
    }

    for (int i = SRV_FD_NONE + 1; i < SRV_FD_END; i++)
    {
        m_UnionInfo[i] = 0;
    }

    string key = ModelKey::GlobalUnionKey(nServerID);
    if (SUCCESS == pStorer->ExistKey(key))
    {
        if (SUCCESS != pStorer->GetHash(key, m_UnionInfo))
        {
            // 获得失败，数据有问题
        }
    }
    else
    {
        if (SUCCESS != pStorer->SetHash(key, m_UnionInfo))
        {
            return false;
        }
    }

    return true;
}

//删除服务器所有公会
bool CUnionModel::CloseUnionList(int nServerID)
{
    m_VectUnionID.clear();
    m_UnionInfo.clear();
    return true;
}

bool CUnionModel::init(int unionID)
{
    m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(unionID));
    if (NULL == pStorer)
    {
        return false;
    }

    std::string unionKey = ModelKey::UnionKey(unionID);
    if (SUCCESS != pStorer->ExistKey(unionKey))
    {
        return false;
    }

    m_nUnionID = unionID;
    Refresh();

    return true;
}

bool CUnionModel::NewUnion(int unionID, int chairmanID, std::string unionName)
{
    m_nUnionID = unionID;
    m_UnionName = unionName;
    m_UnionNotice = "";
    m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    std::string unionKey = ModelKey::UnionKey(m_nUnionID);
    if (SUCCESS == pStorer->ExistKey(unionKey))
    {
        return false;
    }

    for (int i = UNION_FIELD_LV; i < UNION_FIELD_MAX; i++)
    {
        m_UnionInfoMap[i] = 0;
    }

    m_UnionInfoMap[UNION_FIELD_CHAIRMAN] = chairmanID;
    m_UnionInfoMap[UNION_FIELD_LV] = 1;
    m_UnionInfoMap[UNION_FIELD_LIMITLV] = 1;
    m_UnionInfoMap[UNION_FIELD_EMBLEM] = 1;
    m_UnionInfoMap[UNION_FIELD_AUTOAUDIT] = 1;
    m_UnionInfoMap[UNION_FIELD_ORIGINLV] = m_UnionInfoMap[UNION_FIELD_LV];

    if (SUCCESS == pStorer->SetHash(unionKey, m_UnionInfoMap)
        && SUCCESS == pStorer->SetHashByField(unionKey, UNION_FIELD_UNIONNAME, m_UnionName)
        && SUCCESS == pStorer->SetHashByField(unionKey, UNION_FIELD_NOTICE, m_UnionNotice))
    {
        return true;
    }

    return false;
}

bool CUnionModel::Refresh()
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        // 公会信息
        m_UnionInfoMap.clear();
        std::string unionKey = ModelKey::UnionKey(m_nUnionID);
        for (int i = UNION_FIELD_LV; i < UNION_FIELD_MAX; i++)
        {
            m_UnionInfoMap[i] = 0;
        }

        CHECK_RETURN(SUCCESS == pStorer->GetHash(unionKey, m_UnionInfoMap));
        CHECK_RETURN(SUCCESS == pStorer->GetHashByField(unionKey, UNION_FIELD_UNIONNAME, m_UnionName)
            && SUCCESS == pStorer->GetHashByField(unionKey, UNION_FIELD_NOTICE, m_UnionNotice));
        
        // 公会成员信息
        m_MemberMap.clear();
        CHECK_RETURN(SUCCESS == pStorer->GetHash(ModelKey::UnionMemberKey(m_nUnionID), m_MemberMap));
        // 公会审核信息
        m_AuditMap.clear();
        CHECK_RETURN(SUCCESS == pStorer->GetHash(ModelKey::UnionAuditKey(m_nUnionID), m_AuditMap));
        // 公会日志信息
        std::map<int, RedisBinaryData> mapRedisData;
        CHECK_RETURN(SUCCESS == pStorer->GetHash(ModelKey::UnionLogKey(m_nUnionID), mapRedisData));
        m_LogMap.clear();
        std::map<int, RedisBinaryData>::iterator iterLog = mapRedisData.begin();
        for (; iterLog != mapRedisData.end(); ++iterLog)
        {
            if (iterLog->second.length >= sizeof(UnionLogInfo))
            {
                memcpy(&m_LogMap[iterLog->first], iterLog->second.data, sizeof(UnionLogInfo));
            }
        }

        return true;
    }

    return false;
}

bool CUnionModel::GetUnionFieldVal(int field, int &value)
{
    map<int, int>::iterator iter = m_UnionInfoMap.find(field);
    if (iter != m_UnionInfoMap.end())
    {
        value = iter->second;
        return true;
    }

    return false;
}

bool CUnionModel::SetUnionFieldVal(int field, int value)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        std::string key = ModelKey::UnionKey(m_nUnionID);
        if (SUCCESS == pStorer->SetHashByField(key, field, value))
        {
            m_UnionInfoMap[field] = value;
            return true;
        }
    }
    return false;
}

bool CUnionModel::SetUnionFieldVals(std::map<int, int> info)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        std::string key = ModelKey::UnionKey(m_nUnionID);
        if (SUCCESS == pStorer->SetHash(key, info))
        {
            std::map<int, int>::iterator iter = info.begin();
            for (; iter != info.end(); ++iter)
            {
                m_UnionInfoMap[iter->first] = iter->second;
            }

            return true;
        }
    }

    return false;
}

bool CUnionModel::AddUnionFieldVal(int field, int value)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        map<int, int>::iterator iter = m_UnionInfoMap.find(field);
        if (iter != m_UnionInfoMap.end())
        {
            std::string key = ModelKey::UnionKey(m_nUnionID);
            if (SUCCESS == pStorer->SetHashByField(key, field, iter->second + value))
            {
                m_UnionInfoMap[field] += value;
                return true;
            }
        }
    }
    return false;
}

bool CUnionModel::AddMember(int userID, int pos)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        if (m_MemberMap.end() == m_MemberMap.find(userID))
        {
            std::string key = ModelKey::UnionMemberKey(m_nUnionID);
            if (SUCCESS == pStorer->SetHashByField(key, userID, pos))
            {
                m_MemberMap[userID] = pos;
                return true;
            }
        }
    }
    return false;
}

bool CUnionModel::AddAudit(int uid, int auditTime)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        std::string key = ModelKey::UnionAuditKey(m_nUnionID);
        if (SUCCESS == pStorer->SetHashByField(key, uid, auditTime))
        {
            m_AuditMap[uid] = auditTime;
            return true;
        }
    }

    return false;
}

bool CUnionModel::AddLog(UnionLogInfo& info)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        int logid = 0;
        if (SUCCESS != GetUnionFieldVal(UNION_FIELD_LOGIDREF, logid))
        {
            return false;
        }

        if (m_LogMap.size() >= LOG_COUNT)
        {
            RemoveLog(m_LogMap.begin()->first);
        }

        logid += 1;
        if (SUCCESS != SetUnionFieldVal(UNION_FIELD_LOGIDREF, logid))
        {
            return false;
        }

        if (SUCCESS == pStorer->SetHashByField(ModelKey::UnionLogKey(m_nUnionID)
            , logid, reinterpret_cast<char*>(&info), sizeof(UnionLogInfo)))
        {
            m_LogMap[logid] = info;
            return true;
        }
    }

    return false;
}

bool CUnionModel::RemoveMember(int userID)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        std::map<int, int>::iterator iter = m_MemberMap.find(userID);
        if (iter != m_MemberMap.end())
        {
            std::string key = ModelKey::UnionMemberKey(m_nUnionID);
            if (SUCCESS == pStorer->DelHashByField(key, userID))
            {
                m_MemberMap.erase(iter);
                return true;
            }
        }
    }
    return false;
}

bool CUnionModel::RemoveAudit(int uid)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        std::map<int, int>::iterator iter = m_AuditMap.find(uid);
        if (iter != m_AuditMap.end())
        {
            std::string key = ModelKey::UnionAuditKey(m_nUnionID);
            if (SUCCESS == pStorer->DelHashByField(key, uid))
            {
                m_AuditMap.erase(iter);
                return true;
            }
        }
    }
    return false;
}

bool CUnionModel::RemoveLog(int logid)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        std::map<int, UnionLogInfo>::iterator iter = m_LogMap.find(logid);
        if (iter != m_LogMap.end())
        {
            std::string key = ModelKey::UnionLogKey(m_nUnionID);
            if (SUCCESS == pStorer->DelHashByField(key, logid))
            {
                m_LogMap.erase(iter);
                return true;
            }
        }
    }
    return false;
}

void CUnionModel::GetMembers(std::vector<int> &members)
{
    std::map<int, int>::iterator iter = m_MemberMap.begin();
    for (; iter != m_MemberMap.end(); ++iter)
    {
        members.push_back(iter->first);
    }
}

bool CUnionModel::SetMemberPos(int userID, int pos)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        std::map<int, int>::iterator iter = m_MemberMap.find(userID);
        if (iter != m_MemberMap.end())
        {
            std::string key = ModelKey::UnionMemberKey(m_nUnionID);
            if (SUCCESS == pStorer->SetHashByField(key, userID, pos))
            {
                iter->second = pos;
                return true;
            }
        }
    }
    return false;
}

int CUnionModel::GetMemberPos(int userID)
{
    std::map<int, int>::iterator iter = m_MemberMap.find(userID);
    if (iter != m_MemberMap.end())
    {
        return iter->second;
    }

    return -1;
}

bool CUnionModel::MemberExist(int userID)
{
    return m_MemberMap.find(userID) != m_MemberMap.end();
}

bool CUnionModel::GetAuditInfo(int auditID, int &auditTime)
{
    map<int, int>::iterator iter = m_AuditMap.find(auditID);
    if (iter != m_AuditMap.end())
    {
        auditTime = iter->second;
        return true;
    }

    return false;
}

bool CUnionModel::SetUnionName(std::string unionName)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        if (SUCCESS == pStorer->SetHashByField(ModelKey::UnionKey(m_nUnionID), 
            UNION_FIELD_UNIONNAME, unionName))
        {
            m_UnionName = unionName;
            return true;
        }
    }

    return false;
}

bool CUnionModel::SetUnionNotice(std::string unionNotice)
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        if (SUCCESS == pStorer->SetHashByField(ModelKey::UnionKey(m_nUnionID),
            UNION_FIELD_NOTICE, unionNotice))
        {
            m_UnionNotice = unionNotice;
            return true;
        }
    }

    return false;
}

bool CUnionModel::DeleteUnion()
{
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUnionID));
    if (NULL != pStorer)
    {
        pStorer->DelKey(ModelKey::UnionKey(m_nUnionID));
        pStorer->DelKey(ModelKey::UnionMemberKey(m_nUnionID));
        pStorer->DelKey(ModelKey::UnionAuditKey(m_nUnionID));
        pStorer->DelKey(ModelKey::UnionLogKey(m_nUnionID));

        return true;
    }

    return false;
}

bool CUnionModel::addUnion(int nServerID,int unionID)
{
    Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_SERVER);
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(pStorage->GetStorer(1));
    if (NULL != pStorer)
    {
        string key = ModelKey::UnionIDListKey(nServerID);
        if (SUCCESS == pStorer->ListPush(key, unionID))
        {
            m_VectUnionID.push_back(unionID);
            return true;
        }
    }
    return false;
}

bool CUnionModel::deleteUnion(int nServerID, int unionID)
{
    Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_SERVER);
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(pStorage->GetStorer(1));
    if (NULL != pStorer)
    {
        vector<int>::iterator iter = find(m_VectUnionID.begin(), m_VectUnionID.end(), unionID);
        if (iter != m_VectUnionID.end())
        {
            string key = ModelKey::UnionIDListKey(nServerID);
            if (SUCCESS == pStorer->ListRemove(key, 0, unionID))
            {
                m_VectUnionID.erase(iter);
                return true;
            }
        }
    }
    return false;
}

bool CUnionModel::hasUnion(int nServerID,int unionID)
{
    //test
    Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_SERVER);
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(pStorage->GetStorer(1));
    if (NULL != pStorer)
    {
        m_VectUnionID.clear();
        pStorer->ListRange(ModelKey::UnionIDListKey(nServerID), m_VectUnionID);
    }
    return find(m_VectUnionID.begin(), m_VectUnionID.end(), unionID) != m_VectUnionID.end();
}

bool CUnionModel::getUnionField(int nServerID, int field, int &val)
{
    map<int, int>::iterator iter = m_UnionInfo.find(field);
    if (iter != m_UnionInfo.end())
    {
        val = iter->second;
        return true;
    }

    return false;
}

bool CUnionModel::setUnionField(int nServerID,int field, int val)
{
    Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_SERVER);
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(pStorage->GetStorer(1));
    if (NULL != pStorer)
    {
        string key = ModelKey::GlobalUnionKey(nServerID);
        if (SUCCESS == pStorer->SetHashByField(key, field, val))
        {
            m_UnionInfo[field] = val;
            return true;
        }
    }

    return false;
}

bool CUnionModel::getNewUnionId(int nServerID, int &unionId)
{
    Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_SERVER);
    CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(pStorage->GetStorer(1));
    if (NULL != pStorer)
    {
        unionId = 1;
        if (SUCCESS == pStorer->IncreHashByField(ModelKey::GlobalUnionKey(nServerID), SRV_FD_UNIONREF, unionId))
        {
            m_UnionInfo[SRV_FD_UNIONREF] = unionId;
            return true;
        }
    }

    return false;
}
