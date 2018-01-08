/*
* 任务服务逻辑
* 1、完成任务,判断前端处理的状态任务是否完成并存储
* 2、领取任务,判断任务是否完成,完成开启后续任务
*   重置任务更新时间戳,非重置任务则删除
*/

#ifndef __TASK_SERVICE_H__
#define __TASK_SERVICE_H__

#include "KxCommInterfaces.h"

class CTaskService
{
public:
    static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 完成任务
    static void processFinishService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 领取奖励
    static void processAwardService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif