#ifndef __TASK_PROTOCOL_H__
#define __TASK_PROTOCOL_H__

#pragma pack(1)

enum TASKPROTOCOL
{
    CMD_TASK_CSBEGIN,
    CMD_TASK_FINISH_CS,             // 前端发送完成任务
    CMD_TASK_AWARD_CS,              // 发送领取任务奖励
    CMD_TASK_CSEND,

    CMD_TASK_SCBEGIN = 100,
    CMD_TASK_FINISH_SC,             // 前端发送完成任务回发
    CMD_TASK_AWARD_SC,              // 发送领取奖励回发
    CMD_TASK_SCEND,
};

// CMD_TASK_FINISH_CS
// CMD_TASK_FINISH_SC
struct TaskFinishCSC
{
    int taskID;             // 任务ID
    int extend;             // 扩展
};

// CMD_TASK_AWARD_CS
struct TaskAwardCS
{
    int taskID;             // 任务ID
};

// CMD_TASK_AWARD_SC
struct TaskAwardSC
{
    int taskID;             // 任务ID
    int awardCount;         // 奖励物品数量
    // awardCount ->DropItemInfo
};

#pragma pack()

#endif