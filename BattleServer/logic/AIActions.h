#ifndef __AI_ACTIONS_H__
#define __AI_ACTIONS_H__

#include "KxCSComm.h"
#include "ConfAnalytic.h"

//AI处理类型
enum AIActionType
{
    AIAction_Do_None = 1,				    // 空动作
    AIAction_Do_LockMe,						// 锁定自己
    AIAction_Do_Clear,						// 清空所有目标动作
    AIAction_Do_ChangeStatus,				// 改变状态动作
    AIAction_Do_CastSkill,				    // 释放技能动作
    AIAction_Do_LockPoints,					// 锁定目标点动作
    AIAction_Do_LockPointsForMe,            // 以我为中心点锁定目标点
    AIAction_Do_AddBuffForMe,				// 给自己添加Buff
	AIAction_Do_DelBuffForMe,				// 给自己删除Buff
    AIAction_Do_Count,                      // 执行一个结算
    AIAction_Do_LuaEvent,                   // 触发一个Lua事件，传入事件ID
};

class CRole;
class CAIComponent;
class CAIActions
{
public:
	CAIActions();
	~CAIActions();
	void DoAIAction(CAIComponent* com, const CDataFunction* func);

protected:
	void AIActionDoNone();
    // 锁定自己为目标
    void AIActionDoLockMe();
    // 清空黑板
    void AIActionDoClear();
    // 切换自身状态
	void AIActionDoChangeStatus(int stateid);
    // 释放第几个技能
	void AIActionDoCastSkill(int skillIndex);
    // 锁定一系列目标点到黑板中
    void AIActionDoLockPoints(const VecInt& points);
    // 以我为中心点锁定目标点
    void AIActionDoLockPointsForMe(const VecInt& points);
    // 为自己添加BUFF
    void AIActionDoAddBuffForMe(const VecVecInt& buffIds);
	// 给自己删除指定Buff序列
	void AIActionDoDelBuffForMe(const VecInt& buffIds);
    // 执行一个结算
    void AIActionDoCount(int countId);
#ifndef RunningInServer
    // 触发一个Lua事件，传入事件ID
    void AIActionDoLuaEvent(int eventId);
#endif

private:
	CAIComponent* m_pAICom;
    CRole* m_pOwner;
};

#endif