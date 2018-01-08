/*
* 任务类
*   监听处理各种任务并存储数据
*/

#ifndef __TASK_H__
#define __TASK_H__

#include "UserActionListener.h"
#include "ConfHall.h"
#include "TaskModel.h"

class CTask : public IUserActionListener
{
public:
    CTask();
    ~CTask();

    // 初始化任务
    bool init(int userID, int taskID, const TaskDBInfo& info);
    
    // 动作操作
    virtual bool onAction(int actID, void *data, int len);

private:
    //////////////////////各类任务处理方法////////////////////////
    bool onUserLevelUpAction(int actID, void *data, int len);
    bool onPassStageAction(int actID, void *data, int len);
    bool onHeroEquipAction(int actID, void *data, int len);
    bool onHeroLevelUpAction(int actID, void *data, int len);
    bool onHeroStarAction(int actID, void *data, int len);
    bool onHeroSkillAction(int actID, void *data, int len);
    bool onDrawCardAction(int actID, void *data, int len);
    bool onUseExpBookAction(int actID, void *data, int len);
    bool onBuyGoldAction(int actID, void *data, int len);

    bool onHeroTestAction(int actID, void *data, int len);
    bool onGoldTestAction(int actID, void *data, int len);
    bool onTowerFloorAction(int actID, void *data, int len);
    bool onPvpAction(int actID, void *data, int len);
    bool onEquipAction(int actID, void *data, int len);

private:
    bool canFinish(int val = 1);

private:
    int             m_nUserID;        // 用户ID
    int             m_nTaskID;        // 任务ID 
    TaskDBInfo      m_TaskInfo;       // 任务信息
    CTaskModel     *m_pTaskModel;     // 任务数据模型
    const TaskItem* m_pTaskItemConf;  // 配置表
};

#endif