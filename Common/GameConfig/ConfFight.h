#ifndef __CONF_FIGHT_H__
#define __CONF_FIGHT_H__

#include "ConfManager.h"
#include "CommTools.h"

////////////////////////////////////配表数据//////////////////////////////////////
#ifndef RunningInServer
enum EffZOrderType
{
    EffZOrderLocal,
    EffZOrderGlobal,
    EffZOrderInherit,
};

// 特效配置表项
struct EffectConfItem
{
	int EffectId;                                   // 特效ID
    int Loop;                                       // 循环参数，0 不循环 正数为循环 -1为美术设定
    int ZOrderType;                                 // 0本地 1全局 2继承
	int ZOrder;                                     // Z轴层级默认为0
	int AudioId;                                    // 音效ID
	int ResID;										// 骨骼文件名
    int SpeedAffect;                                // 速度联动
    float LifeTime;                                 // 生命周期（0为无限时间）
	float FadeInTime;                               // 淡入时间（0不淡出）
	float FadeOutTime;                              // 淡出时间（0不淡出）
	float AnimationSpeed;                           // 动画播放速度
	Vec2 Offset;                                    // 播放位置偏移
	Vec2 Scale;                                     // 缩放值
	VecFloat AddColor;                              // 叠加色
	std::string AnimationName;                      // 骨骼动画名
	VecMusicInfos MusicInfos;                       // 特效对应的音效
};

class UIEffectConfItem
{
public:
    bool                DoodadLoop;                 // 装饰物表现是否循环
    bool                DoodadBackLoop;             // 装饰物表现回归是否循环
    bool                once;                       // 是否是一次性
    int					ID;							// ID
    int					ShakeLevel;					// 震屏强度
    int					DoodadDistance;				// 装饰物表现距离
    int                 DarkAlpha;                  // 0~100，场景变暗的透明度
    int					BlinkingResID;				// 闪屏动画资源ID
    int					BackgroundResID;			// 背景动画资源ID
    float               DarkFadeInTime;             // 场景淡入时间
    float               DarkFadeOutTime;            // 场景淡出时间
    float               DarkTime;                   // 场景变暗，单位秒，持续变暗的时间
    float               DarkDelayTime;              // 场景变暗，单位秒，开始变暗的延迟时间
    float				ShakeTime;					// 震动时间，单位秒
    float				ShakeDelayTime;				// 震动时间，单位秒，开始震动的延迟时间
    float				DoodadStartTime;			// 装饰物表现开启时间，单位秒
    float				DoodadContinuedTime;		// 装饰物持续时间，单位秒
    std::string			BlinkingCsbAniName;			// 闪屏动画，动画名
    std::string			BackgroundCsbAniName;		// 背景动画，动画名
    std::string			DoodadName;					// 装饰物表现标签
    std::string			DoodadBackName;				// 装饰物表现回归标签
};
#endif // RunningInServer

//Buff属性
class BuffConfItem
{
public:
	bool                IsEffectMove;               //特效是否跟随移动 0否1是
	int					ID;							//ID
	int                 Type;                       //1正面2负面3不可驱散的BUFF
	int                 Stack;                      //正整数1起始
	float               lifeTime;                   //回收添加新BUFF的条 负数为永久, 0为立刻回收, 正整数代表毫秒
	int                 NextBuffCondition;          //0为无条件,1为正常回收,2为强制回收
	int                 UIEffectID;                 //对应UI特效表ID
	int                 EffectWhere;                //buff特效播放点 1头2身3脚
	int                 TargetType;                 //buff目标 1buff携带者,2buff发出者,3自定义选取
	float               FirstTime;                  //首次生效时间
	int                 MaxCount;                   //生效总次数
	int                 TriggerType;                //Buff触发类型
	int                 SkillID;                    //是否加载技能
	VecInt              nextBuffID;                 //回收时候添加的Buff序列
	VecInt              AnimationID;                //对应动画表ID
    VecInt              CountID;                    //Buff生效时目标结算ID
    VecInt              SummonerCountID;            //Buff生效时召唤师和boss结算ID
    VecInt              LapseCountID;               //Buff失效时目标结算ID
    VecInt              SummonerLapseCountID;       //Buff失效时召唤师和boss目标结算ID
	VecInt              TriggerTypeParam;           //Buff触发类型参数
	std::vector<CDataFunction *> Conditions;        //生效条件, 4个
};

//子弹属性
class BulletConfItem
{
public:
	bool				    LockDirect;					//动画方向是否锁死
	int					    ID;							//子弹ID
	int					    Locus;						//弹道轨迹
	int					    StartType;					//子弹起始位置
	int					    EndType;					//子弹攻击位置
	int					    HitType;					//子弹命中类型
	int					    HitJudgeType;				//命中判定类型
	int				        HitJudgeParam;				//命中判定类型参数
	int                     UiEffectTime;               //UI特效调用时机
	int					    UiEffectId;					//ui特效标记ID
	int				        HitJudgeMaxTimes;			//命中判定最大次数
	int                     ZOrderType;                 //0为程序自动处理，否则-2并设置本地ZOrder
	int				        UnitMaxCountTimes;			//单位最大结算次数
	float				    FlyTime;					//子弹飞行帧数
	float				    LifeTime;					//子弹生命周期
	float				    HitJudgeFirstTime;			//首次判定时间
	float				    HitJudgePerTime;			//命中判定间隔时间
	VecFloat			    Locus_Param;				//弹道参数
    VecInt				    CountId;					//结算ID序列 
    VecInt				    SummonerCountId;			//对召唤师的结算ID序列 
	VecInt				    LinkId;						//子弹链接ID序列
	VecInt				    AnimationId;				//子弹表现动画ID
	VecInt                  EndAnimationId;             //子弹结束动画ID
	VecInt				    HitAnimationId;				//子弹命中动画表现ID
	VecInt                  HitAllAnimationId;          //子弹群体命中表现动画
	VecInt				    StartParam;					//起始位置参数
	VecInt				    EndParam;					//结束位置参数
	std::vector<ID_Num>		BuffId;                     //对应buff表ID
};

// 卡片结算属性
class CardCountConfItem
{
public:
	int				    ChangeSingo;					// 唯一性条件
	bool				CardLockChange;					// 变更卡牌锁
	int					CardCountID;					// 卡片结算ID
	int					StarCondition;					// 星级条件
	int					StarConditionPrarm;				// 星级条件参数
	int					CrystalCondition;				// 水晶条件
	int					CrystalConditionPrarm;			// 水晶条件参数
	int					CrystalChangeType;              // 变更水晶消耗
	int					CrystalChangePrarm;				// 变更水晶消耗参数
	int					CDChangeType;                   // 变更CD
	int					CDChangePrarm;					// 变更CD参数
	VecInt				RaceCondition;					// 种族条件
	VecInt				SexCondition;					// 对应性别条件
	VecInt				VocationCondition;				// 职业条件
	VecInt				AttackTypeCondition;			// 攻击方式条件
};

enum CountExpressionOperatorType
{
    OperatorUnvalid,                                    // 无效
    OperatorMulti,                                      // *
    OperatorDiv,                                        // /
    OperatorAdd,                                        // +
    OperatorMinus,                                      // -
    OperatorEqual,		                                // =
};

enum CountExpressionTargetType
{                                                       
    TargetTypeSource,                                   // 源对象
    TargetTypeTarget,                                   // 目标对象
    TargetTypeVar,                                      // Var变量
    TargetTypeValue,                                    // 固定数值
};

// 结算公式配置
class CountExpressionConfItem
{
public:
    CountExpressionConfItem();
    virtual ~CountExpressionConfItem();

    bool init(const std::string& str);

    float Value;                                        // 固定数值
    int AttributeId;                                    // 属性ID
    std::string VarName;                                // Var变量
    CountExpressionTargetType Target;                   // 运算目标
    CountExpressionOperatorType Operator;               // 运算操作符
    CountExpressionConfItem* LeftExpression;            // 左侧表达式
    CountExpressionConfItem* RightExpression;           // 右侧表达式

private:
    // 没有操作符时，解析表达式具体的值类型
    bool initValue(const std::string& str);
    // 传入字符串与操作符的位置，初始化左右两侧表达式
    bool initExpression(const std::string& str, size_t operatorPos);
    // 传入字符串、同级操作符的符号，找出优先级最低的操作符，并将其位置输出的posOut参数中
    bool checkExpression(const std::string& str, char op1, char op2,
        CountExpressionOperatorType opType1, CountExpressionOperatorType opType2, size_t& posOut);
};

// 普通结算属性
class CountConfItem
{
public:
	~CountConfItem()
	{
        deleteAndClearVec<CountExpressionConfItem*>(TargetProperty);
	}

	bool						Miss;					// 闪避开关
	bool						Crit;					// 暴击开关
	bool						Damage;					// 伤害流程开关
	bool						DamageReturn;			// 反弹开关
	int							ID;						// 结算ID
	int							Strong;					// 攻击强度
	int							Status;					// 负面状态id
	int							DamageType;				// 伤害类型
	int							BuffListDel;			// 是否进行驱散
	int							DeBuffListDel;			// 是否进行净化
	int							BuffDel;				// 指定Buff删除
	int							AIIDChange;				// 是否进行更换AI
	int							StatusIDChange;			// 是否进行更换状态
	int                         CampChange;             // 是否进行阵营切换
	float						StatusTime;				// 负面状态持续时间
	float                       PowerPercent;           // 攻击力继承百分比
	float                       PowerExt;               // 额外的攻击力
	VecInt						CardCountID;			// 对应的卡片结算ID
    std::vector<CountExpressionConfItem*> TargetProperty;   // 变化属性
};

//水晶属性
class CrystalConfItem
{
public:
	int					Level;						//水晶等级
	float				Speed;						//水晶提升速度
	int					Max;						//水晶最大值
	int                 Price;                      //升级所需消耗
};

enum SearchCondition
{
    ConditionIdentity,
    ConditionHP,
    ConditionCareer,
    ConditionSex,
    ConditionAttackType,
    ConditionRace,
    ConditionStar,
    ConditionBuff,
    ConditionType,
    ConditionState,
    ConditionRoleId,
};

// 搜索
class SearchConfItem
{
public:
	int					ID;							 //搜索ID
	int					ListType;					 //列表选取(0全部1敌人2盟友)
	int					RangeType;					 //距离类型(1单向选取2双向选取)
	int					RangeParam;					 //距离参数(0全场-1近范-2远范,正整数代表固定数值)
	int					Type;						 //搜索类型(1 A类,2 B类,3 C类)
	int					Reorder;					 //指定条件排序(1血量, 2 X轴排序)
	int					Num;						 //搜索的角色个数
	int				    AttackType;					 //角色是否是近战(0跳过1近战2远程)
	bool				Self;						 //是否去掉自己
	bool                Death;                       //是否搜索死亡
    VecInt              RoleID;                      //角色Id是否匹配
    VecInt              Conditions;                  //需要判断的条件
	VecInt				Identity;					 //身份条件(是否取反+身份参数)	
	VecInt				Career;						 //角色职业(是否取反+职业)
	VecInt				Sex;						 //角色性别(是否取反+性别)
	VecInt				Race;						 //角色种族(是否取反+种族)
	VecInt              Buff;                        //角色Buff(是否取反+Buff序列)
	VecInt				Star;						 //角色性别(是否取反+性别)
	VecInt              State;                       //在某个状态下是否取反+状态ID序列
	VecFloat			HP;							 //角色血量(是否取反+血量浮点数)
};

 enum SkillCastType
 {
     SkillCastNone = 0,                              //无效
     SkillCastAtOnce,                                //直接释放
     SkillCastAtPoint,                               //点释放
 };

//技能属性
class SkillConfItem
{
public:
	bool                    CanBreak;                   // 能否打断
	int					    ID;							// 技能ID
	int					    CastType;					// 技能释放类型
    int                     CastRange;                  // 技能释放范围
	int					    LockType;					// 技能解锁类型
	int					    LockTypePrarm;				// 技能解锁参数
	float                   CD;                         // 技能cd
	int                     CostType;                   // 技能消耗类型
	int                     CostTypeParam;              // 技能消耗参数
	int                     MaxCast;                    // 技能循环次数
	float                   TargetBulletDelay;          // 目标子弹发射时间
	float                   TargetBulletInterval;       // 目标子弹序列间隔时间
	float                   PointBulletDelay;           // 目标点子弹发射时间
	float                   PointPointBulletInterval;   // 目标点子弹序列间隔时间
	int                     BulletParam;                // 子弹发射速率（1为攻速联动）
	int                     Name;                       // 技能名字语言包
	int						CostDesc1;					// 技能消耗描述1
	int						CostDesc2;					// 技能消耗描述2
	int                     Desc;                       // 技能描述语言包
	int                     StateID;                    // 技能跳转状态
	float                   CDParam;                    // 默认cd百分比
	float                   CastTime;                   // 单次循环时间
	VecInt                  TargetBullet;               // 对技能目标发射子弹序列
	VecInt                  PointBullet;                // 对技能目标点发射子弹序列
	VecInt                  Call;                       // 在技能目标点召唤序列
	std::vector<ID_Num>     Buff;                       // 对技能目标添加Buff序列
	std::string			    IconName;					// 技能图标
    std::string             BattleSkillIcon;             // 战斗技能图标
};

//////////////////////////////////解析配表////////////////////////////////////////
#ifndef RunningInServer
class CConfEffect : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfUIEffect : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

// 查询特效配置表的辅助方法
inline const EffectConfItem* queryConfEffect(int effId)
{
	return reinterpret_cast<EffectConfItem*>(
		CConfManager::getInstance()->getConf(CONF_EFFECT)->getData(effId));
}
#endif // RunningInServer

class CConfBuff : public CConfBase
{
public:
    virtual ~CConfBuff();
	virtual bool LoadCSV(const std::string& str);
	BuffConfItem * getData(int buffid, int stack);
	int getMaxStack(int buffid);

	std::map<int, std::map<int, BuffConfItem*> >& getBuffData()
	{
		return m_BuffData;
	}

private:
	std::map<int, std::map<int, BuffConfItem*> > m_BuffData;
};

class CConfBullet : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfCardCount : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfCount : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfCrystal : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfSearch : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfSkill : public CConfBase
{
public:
    virtual ~CConfSkill();
	virtual bool LoadCSV(const std::string& str);
};

/////////////////////////////////查询配表/////////////////////////////////////////
// 查询buff配表
inline const BuffConfItem* queryConfBuff(int buffId, int stack)
{
	CConfBuff *conf = dynamic_cast<CConfBuff*>(
		CConfManager::getInstance()->getConf(CONF_BUFF));
	return static_cast<BuffConfItem*>(conf->getData(buffId, stack));
}

// 查询最大buff层数
inline const int queryMaxBuffStack(int buffid)
{
	CConfBuff *conf = dynamic_cast<CConfBuff*>(
		CConfManager::getInstance()->getConf(CONF_BUFF));
	return conf->getMaxStack(buffid);
}

// 查询子弹配表
inline const BulletConfItem* queryConfBullet(int bulletId)
{
	CConfBullet *conf = dynamic_cast<CConfBullet*>(
		CConfManager::getInstance()->getConf(CONF_BULLET));
	return static_cast<BulletConfItem*>(conf->getData(bulletId));
}

inline const CardCountConfItem* queryConfCardCount(int countId)
{
	return  reinterpret_cast<CardCountConfItem*>(
		CConfManager::getInstance()->getConf(CONF_CARD_COUNT)->getData(countId));
}

// 查询水晶配置表的辅助方法
inline const CrystalConfItem* queryConfCrystal(int crystalLevel)
{
	CConfCrystal* conf = dynamic_cast<CConfCrystal*>(
		CConfManager::getInstance()->getConf(CONF_CRYSTAL));
	return static_cast<CrystalConfItem*>(conf->getData(crystalLevel));
}

inline const CountConfItem* queryConfCount(int countId)
{
	return  reinterpret_cast<CountConfItem*>(
		CConfManager::getInstance()->getConf(CONF_COUNT)->getData(countId));
}

// 查询Search配表
inline const SearchConfItem* queryConfSearch(int searchId)
{
	CConfSearch *conf = dynamic_cast<CConfSearch*>(
		CConfManager::getInstance()->getConf(CONF_SEARCH));
	return static_cast<SearchConfItem*>(conf->getData(searchId));
}

// 查询skill配表
inline const SkillConfItem* queryConfSkill(int skillId)
{
	CConfSkill *conf = dynamic_cast<CConfSkill*>(
		CConfManager::getInstance()->getConf(CONF_SKILL));
	return static_cast<SkillConfItem*>(conf->getData(skillId));
}


#endif
