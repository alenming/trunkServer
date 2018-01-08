/*
* 任务数据模型(普通的任务)
* 1、存储各个任务数据
* 2、添加新任务,重置任务需保存下次更新的时间戳
* 3、获取(所有/单个)任务信息
* 4、删除任务表,删除完成的任务(重置任务执行更改时间戳)
*/

#ifndef __TASK_MODEL_H__
#define __TASK_MODEL_H__

#include <map>
#include "Storage.h"
#include "IDBModel.h"

struct TaskDBInfo
{
    int taskVal;               // 数值
    int taskStatus;            // 状态
    int resetTime;             // 重置的时间戳
};

class CRedisStorer;
class CTaskModel : public IDBModel
{
public:
    CTaskModel();
    ~CTaskModel();

    bool init(int uid);

    bool Refresh();
    // 添加任务
    bool AddTask(int taskID, TaskDBInfo& info);
    // 累计或切换某个任务状态
    bool SetTask(int taskID, TaskDBInfo& info);
    // 获取某个任务
    bool GetTask(int taskID, TaskDBInfo& info);
    // 获取所有任务
    std::map<int, TaskDBInfo>& GetTasks();
    // 删除某个任务
    bool RemoveTask(int taskID);
    // 删除任务
    bool DeleteTasks();

private:

	bool GetTaskDataFromDB(int nTaskID);

private:

    int					                m_nUid;				// 玩家id
	CRedisStorer *						m_pStorer;			// 数据库操作对象
	std::string							m_strTaskKey;		// 普通任务key
    std::map<int, TaskDBInfo>           m_mapTasks;         // 普通任务
};

#endif
