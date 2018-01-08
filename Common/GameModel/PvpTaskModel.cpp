#include "PvpTaskModel.h"
#include "ModelDef.h"
#include "Storage.h"
#include "StorageManager.h"
#include "RedisStorer.h"

CPvpTaskModel::CPvpTaskModel()
: m_nUid(0)
, m_pRedisStorer(0)
{
}

CPvpTaskModel::~CPvpTaskModel()
{
}

bool CPvpTaskModel::init(int uid)
{
	Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_PVP);
	if (NULL == pStorage)
	{
		return false;
	}

	m_pRedisStorer = dynamic_cast<CRedisStorer*>(pStorage->GetStorer(uid));
	if (NULL == m_pRedisStorer)
	{
		return false;
	}

	m_nUid = uid;
	m_strPvpTaskKey = ModelKey::PvpTaskKey(uid);
	Refresh();
	return true;
}

bool CPvpTaskModel::Refresh()
{
	if (NULL != m_pRedisStorer)
	{
		m_setPvpTasks.clear();
		return SUCCESS == m_pRedisStorer->GetSetAll(m_strPvpTaskKey, m_setPvpTasks);
	}
	return false;
}

bool CPvpTaskModel::isPvpTaskExsit(int taskId)
{
	return m_setPvpTasks.find(taskId) != m_setPvpTasks.end();
}

bool CPvpTaskModel::addPvpTask(int taskId)
{
	if (NULL == m_pRedisStorer)
	{
		return false;
	}
	if (m_setPvpTasks.find(taskId) != m_setPvpTasks.end()
		|| SUCCESS != m_pRedisStorer->SetAdd(m_strPvpTaskKey, taskId))
	{
		return false;
	}
	m_setPvpTasks.insert(taskId);
	return true;
}

bool CPvpTaskModel::removePvpTask(int taskId)
{
	if (NULL == m_pRedisStorer)
	{
		return false;
	}
	std::set<int>::iterator iter = m_setPvpTasks.find(taskId);
	if (iter != m_setPvpTasks.end())
	{
		if (SUCCESS == m_pRedisStorer->SetDel(m_strPvpTaskKey, taskId))
		{
			m_setPvpTasks.erase(iter);
			return true;
		}		
	}
	return false;
}

bool CPvpTaskModel::removeAllPvpTask()
{
	if (NULL == m_pRedisStorer)
	{
		return false;
	}
	for (std::set<int>::iterator iter = m_setPvpTasks.begin();
		iter != m_setPvpTasks.end(); ++iter)
	{
		int taskId = *iter;
		if (SUCCESS == m_pRedisStorer->SetDel(m_strPvpTaskKey, taskId))
		{
			continue;
		}
	}
	m_setPvpTasks.clear();
	return true;
}

const std::set<int>& CPvpTaskModel::getAllPvpTasks()
{
	return m_setPvpTasks;
}


CPvpChestModel::CPvpChestModel()
: m_nUid(0)
, m_pRedisStorer(NULL)
{
}

CPvpChestModel::~CPvpChestModel()
{
}

bool CPvpChestModel::init(int uid)
{
	Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_PVP);
	if (NULL == pStorage)
	{
		return false;
	}

	m_pRedisStorer = dynamic_cast<CRedisStorer*>(pStorage->GetStorer(uid));
	if (NULL == m_pRedisStorer)
	{
		return false;
	}

	m_nUid = uid;
	m_strPvpChestKey = ModelKey::PvpChestKey(uid);
	Refresh();
	return true;
}

bool CPvpChestModel::Refresh()
{
	if (NULL == m_pRedisStorer)
	{
		return false;
	}

	m_listChestIds.clear();
	if (SUCCESS != m_pRedisStorer->ListRange(m_strPvpChestKey, m_listChestIds))
	{
		return false;
	}
	return true;
}

bool CPvpChestModel::addChestId(int chestId)
{
	if (NULL == m_pRedisStorer)
	{
		return false;
	}

	if (SUCCESS != m_pRedisStorer->ListPush(m_strPvpChestKey, chestId))
	{
		return false;
	}
	m_listChestIds.push_front(chestId);
	return true;
}

int CPvpChestModel::popChestId()
{
	if (NULL == m_pRedisStorer)
	{
		return 0;
	}
	int ret = 0;
	if (SUCCESS != m_pRedisStorer->ListRPop(m_strPvpChestKey, ret))
	{
		return 0;
	}
	m_listChestIds.pop_back();
	return ret;
}

const std::list<int>& CPvpChestModel::getChestIds()
{
	return m_listChestIds;
}
