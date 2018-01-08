/*
* 服务层辅助类
* 1、提供发送数据接口
* 2、提供发送任务/成就事件接口
* 3、提供创建新英雄模型接口(包括默认解锁技能/天赋)
* 4、提供创建新任务模型接口(包括重置任务时间戳计算)
* 5、提供玩家获取经验计算等级接口(包括满级判断)
*/

#ifndef __TASK_ACHIEVE_HELPER_H__
#define __TASK_ACHIEVE_HELPER_H__

#include "TaskModel.h"

class CTaskAchieveHelper
{
public:
    // 初始化任务
    static bool initTask(int uid);
    // 初始化成就
    static bool initAchieve(int uid);

    // 添加新任务
    static bool addTask(int uid, int taskID);
    // 获取重置任务的重置时间戳
    static int getTaskResetTimeStamp(int taskID);
    // 检测任务是否需要重置
    static void checkTask(int uid, int taskID, TaskDBInfo &info);
    // 实例化任务
    static bool instanceTask(int uid, int taskID);
    // 判断特殊任务是否完成
    static bool isFinishSpecialTask(int uid, int taskID, const TaskDBInfo& taskInfo);
    // 检测状态任务是否完成
    static bool canFinishTask(int uid, int taskID);

    // 添加新成就
    static bool addAchieve(int uid, int achieveID, bool instance = false);
    // 实例化成就
    static bool instanceAchieve(int uid, int achieveID);
    // 检测成就是否完成(隐藏的完成成就会自动开启新成就)
    static bool canFinishAchieve(int uid, int achieveID, int times = 0);
    // 判断英雄相关的成就是否完成
    static bool canFinishHeroAchieve(int uid, int actID, int compelteTimes);

private:
    static bool canFinishCommon(int uid, int type, int compelteTimes, int finishParam);
    static bool needListenTask(int actID);
    static bool needListenAchieve(int actID);
};

#endif 
