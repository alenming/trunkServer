#ifndef __PVPTASK_MODEL_H__
#define __PVPTASK_MODEL_H__

#include <set>
#include <list>
#include <string>
#include "IDBModel.h"

class CRedisStorer;
class CPvpTaskModel : public IDBModel
{
public:
	CPvpTaskModel();
	~CPvpTaskModel();

public:
	// 初始化
	bool init(int uid);
	// 刷新
	bool Refresh();
	// 任务是否存在
	bool isPvpTaskExsit(int taskId);
	// 添加新的pvp任务
	bool addPvpTask(int taskId);
	// 移除pvp任务
	bool removePvpTask(int taskId);
	// 移除所有pvp任务
	bool removeAllPvpTask();
	// 获得所有pvp任务
	const std::set<int>& getAllPvpTasks();

private:
	int							m_nUid;			    // uid
	CRedisStorer *				m_pRedisStorer;	    // 数据库对象
	std::string					m_strPvpTaskKey;	// pvp任务键
	std::set<int>				m_setPvpTasks;		// 所有任务
};

// 注:存储新的宝箱在头部,删除尾部
class CPvpChestModel : public IDBModel
{
public:
	CPvpChestModel();
	~CPvpChestModel();
	
public:
	//初始化
	bool init(int uid);
	//刷新
	bool Refresh();
	//增加宝箱
	bool addChestId(int chestId);
	//删除第一个宝箱, 并返回第一个宝箱id, 返回0则无宝箱
	int popChestId();
	//获得所有宝箱
	const std::list<int>& getChestIds();

private:
	int				m_nUid;
	CRedisStorer*   m_pRedisStorer;		// 数据库对象
	std::string		m_strPvpChestKey;	// pvp宝箱键
	std::list<int>  m_listChestIds;		// 宝箱id列表
};

#endif //__PVPTASK_MODEL_H__
