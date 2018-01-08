#include "TaskModel.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "KxLog.h"

CTaskModel::CTaskModel() 
: m_nUid(0)
, m_pStorer(NULL)
{
}

CTaskModel::~CTaskModel()
{
}

bool CTaskModel::init(int uid)
{
	m_nUid = uid;

	m_strTaskKey = ModelKey::TaskKey(m_nUid);

	Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_TASK);
	if (NULL == pStorage)
	{
		KXLOGERROR("user %d CTaskModel get Storage error!", uid);
		return false;
	}
	m_pStorer = reinterpret_cast<CRedisStorer*>(pStorage->GetStorer(m_nUid));
	if (NULL == m_pStorer)
	{
		KXLOGERROR("user %d CTaskModel get CRedisStorer error!", uid);
		return false;
	}

    Refresh();
    return true;
}

bool CTaskModel::Refresh()
{
	if (NULL != m_pStorer)
    {
        std::map<int, RedisBinaryData> mapTask;
		if (SUCCESS != m_pStorer->GetHash(m_strTaskKey, mapTask))
        {
            return false;
        }

        TaskDBInfo taskInfo;
        std::map<int, RedisBinaryData>::iterator iter = mapTask.begin();
        for (; iter != mapTask.end(); ++iter)
        {
            memcpy(&taskInfo, iter->second.data, sizeof(TaskDBInfo));
            m_mapTasks[iter->first] = taskInfo;
        }

        return true;
    }

    return false;
}

bool CTaskModel::AddTask(int taskID, TaskDBInfo& info)
{
	if (NULL != m_pStorer)
    {
        std::map<int, TaskDBInfo>::iterator iter = m_mapTasks.find(taskID);
        if (iter == m_mapTasks.end())
        {
			if (SUCCESS == m_pStorer->SetHashByField(m_strTaskKey, taskID,
				reinterpret_cast<char*>(&info), sizeof(TaskDBInfo)))
            {
                m_mapTasks[taskID] = info;
                return true;
            }
        }
    }

    return false;
}

bool CTaskModel::SetTask(int taskID, TaskDBInfo& info)
{
	if (NULL != m_pStorer)
    {
        std::map<int, TaskDBInfo>::iterator iter = m_mapTasks.find(taskID);
        if (iter != m_mapTasks.end())
        {
			if (SUCCESS == m_pStorer->SetHashByField(m_strTaskKey, taskID,
				reinterpret_cast<char*>(&info), sizeof(TaskDBInfo)))
            {
                m_mapTasks[taskID] = info;
                return true;
            }
        }
    }

    return false;
}

bool CTaskModel::GetTask(int taskID, TaskDBInfo& info)
{
    std::map<int, TaskDBInfo>::iterator iter = m_mapTasks.find(taskID);
    if (iter != m_mapTasks.end())
    {
        memcpy(&info, &(iter->second), sizeof(TaskDBInfo));
        return true;
    }

    return false;
}

std::map<int, TaskDBInfo>& CTaskModel::GetTasks()
{
    return m_mapTasks;
}

bool CTaskModel::RemoveTask(int taskID)
{
	if (NULL != m_pStorer)
    {
		std::map<int, TaskDBInfo>::iterator iter = m_mapTasks.find(taskID);
        if (iter != m_mapTasks.end())
        {
			if (SUCCESS == m_pStorer->DelHashByField(m_strTaskKey, taskID))
            {
                m_mapTasks.erase(iter);
                return true;
            }
        }
    }

    return false;
}

bool CTaskModel::DeleteTasks()
{
	if (NULL != m_pStorer)
    {
		if (SUCCESS != m_pStorer->DelKey(m_strTaskKey))
        {
            return false;
        }

        m_mapTasks.clear();

        return true;
    }

    return false;
}

bool CTaskModel::GetTaskDataFromDB(int nTaskID)
{
	if (NULL != m_pStorer)
	{
		int len = 0;
		TaskDBInfo newTaskInfo;
		if (SUCCESS == m_pStorer->GetHashByField(m_strTaskKey, nTaskID, (char*)&newTaskInfo, len))
		{
			m_mapTasks[nTaskID] = newTaskInfo;
			return true;
		}
	}

	return false;
}
