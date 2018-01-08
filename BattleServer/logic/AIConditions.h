#ifndef __AI_CONDITIONS_H__
#define __AI_CONDITIONS_H__

#include "KxCSComm.h"
#include "ConfAnalytic.h"

#define itob(a)				(0 != a ? true : false)
#define RETURN_REV(rev,b)	return rev ? !(b) : (b)

enum AIConditionType
{
	AICondition_Is_None = 1,   						// 无条件
	AICondition_Is_FindTarget,					    // 是否找到目标(搜索结果写进写进黑板)
	AICondition_Is_HasTarget,						// 范围内是否有符合需求目标(搜索结果不写进黑板)
	AICondition_Is_MyHpLessThan,                    // 自己血量是否满足条件
	AICondition_Is_SkillReady,						// 技能是否就绪
	AICondition_Is_HasTargetPoint,	                // 自身的目标点是否为空
	AICondition_Is_MyBuff,						    // 自己是否有指定Buff
	AICondition_Is_RedOrBlue,						// 我的势力阵营判断
	AICondition_Is_Random,							// 概率是否满足
    AICondition_Is_MyStarForAI,                     // 星级是否满足条件
    AICondition_Is_MyAttribute,                     // 属性是否满足条件
    AICondition_Is_EnemySoldierNumber,              // 场上敌方士兵数量是否满足条件(搜索结果不写进黑板)
    AICondition_Is_MySoldierNumber,                 // 场上己方士兵数量是否满足条件(搜索结果不写进黑板)
};

enum AICompareType
{
    AICompareLarge,
    AICompareEqual,
    AICompareLess,
};

//AI攻击范围
enum AIAttackRange
{
	AIAR_ANYWHERE,									// 全图
	AIAR_NEAR,										// 近战范围
	AIAR_FAR,										// 远战范围
};

class CRole;
class CAIComponent;
class CAIConditions
{
public:
	CAIConditions();
	~CAIConditions();
	bool DoAICondition(CAIComponent* com, const CDataFunction* func);

protected:
    // 无条件
	bool AIConditionIsNone(bool rev);
    // 是否找到目标(搜索结果写进写进黑板)
    bool AIConditionIsFindTarget(bool rev, int searchId);
    // 范围内是否有符合需求目标(搜索结果不写进黑板)
    bool AIConditionIsHasTarget(bool rev, int searchId);
    // 自己血量是否满足条件
	bool AIConditionIsMyHpLessThan(bool rev, float hp);
    // 技能是否就绪，（传入技能ID，非技能下标！！！）
    bool AIConditionIsSkillReady(bool rev, int skillId);
	// 自身的目标点是否为空
    bool AIConditionIsHasTargetPoint(bool rev);
    // 自己是否有指定Buff
    bool AIConditionIsMyBuff(bool rev, int buffid);
    // 我的势力阵营判断
    // 游戏中默认左边队伍为红方，右边队伍为蓝方（1代表红方2代表蓝方）
    bool AIConditionIsRedOrBlue(bool rev, int camp);
	// 概率是否满足
    bool AIConditionIsRandom(bool rev, int ProbabilityValue);
    // 星级是否满足
    bool AIConditionIsMyStarForAI(bool rev, int star);
    // 判断当前属性是否满足条件
    bool AIConditionIsMyAttribute(bool rev, int attId, int eqType, float var);
    // 场上敌方士兵数量是否满足条件(搜索结果不写进黑板)
    bool AIConditionIsEnemySoldierNumber(bool rev, int num);
    // 场上己方士兵数量是否满足条件(搜索结果不写进黑板)
    bool AIConditionIsMySoldierNumber(bool rev, int num);

private:
	CAIComponent* m_pAICom;
    CRole* m_pOwner;
    int m_pOwnerId;
};

#endif