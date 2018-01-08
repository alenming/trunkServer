#ifndef _SUM_COUNT_
#define _SUM_COUNT_

#include "KxCSComm.h"

//伤害种类
enum eHurtType
{
    kUnCrit = 1,        // 普通攻击
    kCrit,              // 暴击
    kAddBlood,          // 治疗
    kMiss,              // 闪避
    kUnKnow,
};

//结算伤害类型
enum CountDamageType
{
	CDT_UNVALID,	//无效的
	CDT_PHYSICAL,   //物理伤害
	CDT_MAGIC,		//魔法伤害
	CDT_REAL,		//真实伤害
	CDT_TREAT,		//治疗伤害
    CDT_PERCENT,    //百分比伤害
};

//结算判断类型
enum CountJudgeType
{
	CJT_UNVALID,	//无效的
	CJT_BIGGER,		//大于
	CJT_EQUAL,		//等于
	CJT_SMALLER,	//小于
};

//结算更改类型
enum CountChangeType
{
	CCT_UNVALID,	// 无效的
    CCT_RESET,      // 重置还原
    CCT_PERCENT,    // 百分比
	CCT_NUMBER,		// 数值
	CCT_EQUAL,		// 赋值
};

class CRole;
class CountConfItem;
class CCount
{
public:
	// 普通结算
	static bool roleExecute(int countID, CRole* src, CRole* aim);
	// 卡片结算
	static bool cardExecute(int countID, CRole* aim);
    // 播放结算效果
    static void playCountEffect(CRole* role, eHurtType hurtType, int hurtValue);
protected:
	// V4.0 ex
	static void extra(const CountConfItem* item, CRole* aim);
	// 伤害流程
	static void damage(const CountConfItem* item, CRole* src, CRole* aim);
	// 比较判断
	static bool judge(int cardAttr, int countCondition, int countParam);
	// 查找判断
	static bool judge(int cardAttr, const VecInt& countCondition);
	// 状态判断
	static bool judge(int cardAttr, int countCondition);
	// 状态更改
	static void change(bool& cardAttr, bool countValue);

private:
};

#endif
