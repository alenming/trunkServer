#ifndef __ROLE_COMM_H__
#define __ROLE_COMM_H__

//对象基本属性枚举
enum EAttributeTypes
{
    // Class - 不参与结算的属性
    EClassBegin = 0,
    EClassHPLine,                           // 角色血条长度
    EClassStatusID,                         // 角色状态表ID
    EClassAIID,                             // 角色AI表ID
    EClassHPGrowUp,                         // 角色生命成长值
    EClassPAttackGrowUp,                    // 角色物理攻击力成长值
    EClassMAttackGrowUp,                    // 角色魔法攻击力成长值
    EClassPGuardGrowUp,                     // 角色物理护甲成长
    EClassMGuardGrowUp,                     // 角色魔法护甲成长
    EClassCritGrowUp,                       // 角色暴击成长
    EClassEnd,

    // Class - int 属性
    EClassIntBegin = 100,
    EClassAnimationId,                      // 角色动画ID
    EClassHPPosX,                           // 角色血条X坐标
    EClassHPPosY,                           // 角色血条Y坐标
    EClassFireRange,                        // 角色近战攻击范围
    EClassFarFireRange,                     // 角色远程攻击范围
    EClassPAttack,                          // 角色物理攻击力
    EClassMAttack,                          // 角色魔法攻击力
    EClassPGuard,                           // 角色物理护甲
    EClassMGuard,                           // 角色魔法护甲
    EClassPPenetrate,                       // 角色物理穿透
    EClassMPenetrate,                       // 角色魔法穿透
    EClassAttackSpeed,                      // 角色攻击速度
    EClassSpeed,                            // 角色移动速度
    EClassHP,                               // 角色生命值
    EClassRage,                             // 角色怒气值
    EClassRageRecover,                      // 角色怒气回复值
    EClassMP,                               // 角色魔法值
    EClassMPRecover,                        // 角色魔法回复
    EClassStrong,                           // 角色霸体值上限
    EClassStrongRecover,                    // 角色霸体值恢复速度
    EClassHaterd,                           // 角色仇恨值
    EClassIntEnd,

    // Class - float 属性
    EClassFloatBegin = 200,
    EClassScale,                            // 角色大小缩放
    EClassEffectScale,                      // 角色特效大小缩放值
    EClassDefend,                           // 角色物理减伤
    EClassResustance,                       // 角色魔法减伤
    EClassVampire,                          // 角色吸血值
    EClassRebound,                          // 角色反弹值
    EClassMiss,                             // 角色闪避值
    EClassCrit,                             // 角色暴击值
    EClassCritDamage,                       // 角色暴击伤害
    EClassMass,                             // 角色质量
    EClassFloatEnd,

    // Attribute - 动态属性
    EAttributeBegin = 300,
	// int
    EAttributeHP,                           // 角色生命当前值
    EAttributeMP,                           // 角色魔法值当前值
    EAttributeRage,                         // 角色怒气当前值
    EAttributeStrong,                       // 角色霸体当前值
    EAttributeExtraHP,                      // 角色护盾值
    EAttributeCamp,                         // 阵营
    EAttributeBeAttackable,                 // 是否可被攻击
    EAttributeBeAoeable,                    // 是否可被AOE
    EAttributeBeBuffable,                   // 是否可被添加BUFF
	//float
    EAttributeYmin,                         // 角色个体地平值
    EAttributeX,                            // X轴坐标float
    EAttributeY,                            // Y轴坐标float
    EAttributeHitPowerX,                    // 角色个体当前受力X值
    EAttributeHitPowerY,                    // 角色个体当前受力Y值
    EAttributeEnd,

    // Attribute - int 属性
    EAttributeIntBegin = 400,
    EAttributeAnimationId,                  // 角色动画ID
    EAttributeHPPosX,                       // 角色血条当前X坐标
    EAttributeHPPosY,                       // 角色血条当前Y坐标
    EAttributeFireRange,                    // 角色近战攻击范围
    EAttributeFarFireRange,                 // 角色远程攻击范围
    EAttributePAttack,                      // 角色物理攻击力
    EAttributeMAttack,                      // 角色魔法攻击力
    EAttributePGuard,                       // 角色物理护甲
    EAttributeMGuard,                       // 角色魔法护甲
    EAttributePPenetrate,                   // 角色物理穿透
    EAttributeMPenetrate,                   // 角色魔法穿透
    EAttributeAttackSpeed,                  // 角色攻击速度
    EAttributeSpeed,                        // 角色移动速度
    EAttributeMaxHP,                        // 角色生命值
    EAttributeMaxRage,                      // 角色怒气值
    EAttributeRageRecover,                  // 角色怒气回复值
    EAttributeMaxMP,                        // 角色魔法值
    EAttributeMPRecover,                    // 角色魔法回复
    EAttributeMaxStrong,                    // 角色霸体值上限
    EAttributeStrongRecover,                // 角色霸体值恢复速度
    EAttributeHaterd,                       // 角色仇恨值
    EAttributeIntEnd,

    // Attribute - float 属性
    EAttributeFloatBegin = 500,
    EAttributeScale,                        // 角色大小缩放
    EAttributeEffectScale,                  // 角色特效大小缩放值
    EAttributeDefend,                       // 角色物理减伤
    EAttributeResustance,                   // 角色魔法减伤
    EAttributeVampire,                      // 角色吸血值
    EAttributeRebound,                      // 角色反弹值
    EAttributeMiss,                         // 角色闪避值
    EAttributeCrit,                         // 角色暴击值
    EAttributeCritDamage,                   // 角色暴击伤害
    EAttributeMass,                         // 角色质量
    EAttributeFloatEnd,

    // Var 属性 - int 属性
    EAttributeIntVarBegin = 600,
    EAttributeSpeedVar,                     // 角色移动速度变化值
    EAttributePAttackVar,                   // 角色物理攻击力变化值
    EAttributeMAttackVar,                   // 角色魔法攻击力变化值
    EAttributePGuardVar,                    // 角色物理护甲变化值
    EAttributeMGuardVar,                    // 角色魔法护甲变化值
    EAttributePPenetrateVar,                // 角色物理穿透变化值
    EAttributeMPenetrateVar,                // 角色魔法穿透变化值
    EAttributeRageVar,                      // 角色怒气上限值变化值
    EAttributeRageRecoverVar,               // 角色怒气回复值变化值
    EAttributeMPVar,                        // 角色魔法值上限值变化值
    EAttributeMPRecoverVar,                 // 角色魔法回复变化值
    EAttributeStrongVar,                    // 角色霸体上限值变化值
    EAttributeStrongRecoverVar,             // 角色霸体值恢复速率
    EAttributeIntVarEnd,

    // Var 属性 - float 属性
    EAttributeFloatVarBegin = 700,
    EAttributeDefendVar,                    // 角色物理减伤变化值 
    EAttributeResustanceVar,                // 角色魔法减伤变化值 
    EAttributeVampireVar,                   // 角色吸血值变化值 
    EAttributeReboundVar,                   // 角色反弹值变化值 
    EAttributeMissVar,                      // 角色闪避值变化值 
    EAttributeCritVar,                      // 角色暴击值变化值 
    EAttributeCritDamageVar,                // 角色暴击伤害变化值 
    EAttributeAttackSpeedVar,               // 角色攻击速率变化值
    EAttributeFloatVarEnd,

    // Card 属性
    ECardBegin = 800,
    ECardRace,                              // 种族
    ECardSex,                               // 性别
    ECardAttackType,                        // 攻击方式 1近战 2远程
    ECardVocation,                          // 职业
    ECardLevel,                             // 当前等级
    ECardEnd,

    // Stat属性
    EStatBegin = 900,
	// int
    EStatCrit,                              // 暴击
    EStatBeCrited,                          // 被暴击
    EStatDodge,                             // 闪避
    EStatBeDodged,                          // 被闪避
    EStatBeTreated,                         // 治疗
    EStatBrokenShield,                      // 破盾
    EStatAddShield,                         // 加盾
    EStatBeAttacked,                        // 受击
    EStatAttack,                            // 攻击
	//float
    EStatEnd,

    // Hero 属性
    EHeroBegin = 1000,
	// int
    EHeroCrystalLevel,                      // 英雄当前水晶等级
    EHeroMaxCrystal,                        // 英雄当前最大水晶值
    // float
	EHeroCrystal,                           // 英雄当前水晶值
    EHeroCrystalSpeedParam,                 // 英雄水晶提升速率
    EHeroCrystalSpeed,                      // 英雄当前水晶提升速度
    EHeroEnd,                                 
};

enum EAttributeType
{
    EUnknow,
    EAttributeBase,
    EAttributeInt,
    EAttributeFloat,
    EClassBase,
    EClassInt,
    EClassFloat,
};

// 辅助计算函数
inline bool isClassInt(EAttributeTypes type){ return type > EClassIntBegin && type < EClassIntEnd; }
inline bool isClassFloat(EAttributeTypes type){ return type > EClassFloatBegin && type < EClassFloatEnd; }
inline bool isClassType(EAttributeTypes type)
{
    return (type > EClassBegin && type < EClassEnd) 
        || isClassInt(type) 
        || isClassFloat(type); 
}
inline bool isAttributeInt(EAttributeTypes type)
{
    return (type > EAttributeIntBegin && type < EAttributeIntEnd) 
        || (type > EAttributeIntVarBegin && type < EAttributeIntVarEnd);
}
inline bool isAttributeFloat(EAttributeTypes type)
{
    return (type > EAttributeFloatBegin && type < EAttributeFloatEnd) 
        || (type > EAttributeFloatVarBegin && type < EAttributeFloatVarEnd);
}
inline bool isAttributeType(EAttributeTypes type)
{
    return (type > EAttributeBegin && type < EAttributeEnd) 
        || isAttributeInt(type) 
        || isAttributeFloat(type);
}
/*inline EAttributeType getAttributeType(EAttributeTypes type)
{
    if (type > EAttributeBegin && type < EAttributeEnd)
    {
        return EAttributeBase;
    }
    else if (isAttributeInt(type))
    {
        return EAttributeInt;
    }
    else
    {
        return EUnknow;
    }
}*/

// 默认的玩家ID
enum DefaultUserId
{
    EDefaultScene = -2,                     // 场景的UserID, 主要用于规避阵营问题
    EDefaultPlayer = -1,                    // 默认的玩家 UserID
    EDefaultNpc,                            // 默认的NPC UserID
};

enum ChangeCampType
{                                           
    ENoChange,                              // 阵营无切换
    EChangeToEnmey,                         // 切换到敌方
    EChangeToNetral,                        // 切换到中立
    EChangeToBlue,                          // 切换到蓝方
    EChangeToRed,                           // 切换到红方
};

// 阵营类型，对应EAttributeCamp属性
enum CampType
{
    ECamp_Neutral,                          // 中立士兵
    ECamp_Blue,                             // 蓝色方，位于左边
    ECamp_Red,                              // 红色方，位于右边
};

// 角色阵营
enum RoleCamp
{
	CAMP_NONE = -1,                         // 无
	CAMP_ALL,                               // 所有
	CAMP_ENEMY,                             // 敌人
	CAMP_FRIEND,                            // 盟友
	CAMP_NEUTRAL                            // 中立
};

//角色兵种
enum RoleType
{
	RT_NONE = -1,                           // 无
	RT_ALL,                                 // 所有
	RT_HERO,							    // 英雄
	RT_SOLDIER,                             // 士兵
	RT_MONSTER,								// 怪物
	RT_BOSS,								// boss
	RT_SUMMON,								// 召唤物
};

// 士兵种族类型
enum ESoldierType
{
    EST_RACE_BEGIN = 1,
    EST_RACE_HUMAN = EST_RACE_BEGIN,        // 种族人
	EST_RACE_BEAST,							// 种族兽
	EST_RACE_SPRITE,						// 种族精灵
	EST_RACE_GHOST,							// 种族亡灵
	EST_RACE_GIANT,							// 种族巨人
	EST_RACE_DRAGON,						// 种族龙

    EST_SEX_BEGIN = 11,
    EST_SEX_MALE = EST_SEX_BEGIN,           // 性别男
	EST_SEX_SHEMALE,						// 性别女
	EST_SEX_NEUTER,							// 性别中性

    EST_FIRE_BEGIN = 21,
    EST_FIRE_NEAR = EST_FIRE_BEGIN,         // 近战
	EST_FIRE_FAR,							// 远程

    EST_VOCATION_BEGIN = 31,
    EST_VOCATION_TANK = EST_VOCATION_BEGIN, // 职业坦克(卫士)
	EST_VOCATION_SOLDIER,					// 职业战士
	EST_VOCATION_ASSASSIN,					// 职业刺客
	EST_VOCATION_SHOOTER,					// 职业射手
	EST_VOCATION_MAGE,						// 职业魔法师
	EST_VOCATION_ASSISTANT,					// 职业辅助
};

// 种族类型
enum ERaceType
{
    RACE_TYPE_NON,
    RACE_TYPE_HUMAN,             // 人族
    RACE_TYPE_GHOST,             // 亡灵
    RACE_TYPE_NATURE,            // 自然
    RACE_TYPE_OTHER,             // 其他
};

// 性别类型
enum ESexType
{
    SEX_TYPE_NON,
    SEX_TYPE_MALE,               // 性别男
    SEX_TYPE_SHEMALE,            // 性别女
    SEX_TYPE_NEUTER,             // 性别中性
};

// 攻击方式
enum EFireType
{
    FIRE_TYPE_NON,
    FIRE_TYPE_NEAR = 1,           // 近战
    FIRE_TYPE_FAR,                // 远程
};

// 职业
enum EVocationType
{
    VOCATION_TYPE_NON,
    VOCATION_TYPE_SOLDIER,        // 战士
    VOCATION_TYPE_ASSASSIN,       // 刺客
    VOCATION_TYPE_SHOOTER,        // 射手
    VOCATION_TYPE_MAGE,           // 魔法师
    VOCATION_TYPE_ASSISTANT,      // 辅助
    VOCATION_TYPE_BODYGUARD,      // 卫士
};

enum EEquipAddType
{
    EEquipNon,
    EEquipAdd,
    EEquipPercent,
    EEquipAddPercent,
};

enum EEquipAbility
{
    EA_NON,                     // 空
    EA_FireRange,               // 增加或减少百分之多少
    EA_FarFireRange,            // 增加或减少百分之多少
    EA_Speed,                   // 增加或减少百分之多少
    EA_HP,                      // 增加或减少多少
    EA_HPPercent,               // 增加或减少百分之多少
    EA_PAttack,                 // 增加或减少多少
    EA_PAttackPercent,          // 增加或减少百分之多少
    EA_Mattack,                 // 增加或减少多少
    EA_MattackPercent,          // 增加或减少百分之多少
    EA_DefendPercent,           // 增加或减少百分之多少，这个值本身就是个百分比值，最后是2个百分比值的加减法
    EA_PGuard,                  // 增加或减少多少
    EA_PGuardPercent,           // 增加或减少百分之多少
    EA_Resustance,              // 增加或减少百分之多少，这个值本身就是个百分比值，最后是2个百分比值的加减法
    EA_MGuard,                  // 增加或减少多少
    EA_MGuardPercent,           // 增加或减少百分之多少
    EA_PPenetrate,              // 增加或减少多少
    EA_PPenetratePercent,       // 增加或减少百分之多少
    EA_Mpenetrate,              // 增加或减少多少
    EA_MpenetratePercent,       // 增加或减少百分之多少
    EA_AttackSpeed,             // 增加或减少百分之多少
    EA_Vampire,                 // 增加或减少百分之多少，这个值本身就是个百分比值，最后是2个百分比值的加减法
    EA_Rebound,                 // 增加或减少百分之多少，这个值本身就是个百分比值，最后是2个百分比值的加减法
    EA_Miss,                    // 增加或减少百分之多少，这个值本身就是个百分比值，最后是2个百分比值的加减法
    EA_Crit,                    // 增加或减少百分之多少，这个值本身就是个百分比值，最后是2个百分比值的加减法
    EA_CritDamage,              // 增加或减少百分之多少，这个值本身就是个百分比值，最后是2个百分比值的加减法
    EA_Rage,                    // 增加或减少多少
    EA_RagePercent,             // 增加或减少百分之多少
    EA_RageRecover,             // 增加或减少百分之多少
    EA_MP,                      // 增加或减少多少
    EA_MPPercent,               // 增加或减少百分之多少
    EA_MPRecover,               // 增加或减少百分之多少
    EA_Mass,                    // 增加或减少多少
    EA_Endure,                  // 增加或减少多少
    EA_EndurePercent,           // 增加或减少百分之多少
    EA_EndureRecover,           // 增加或减少百分之多少
    EA_Haterd,                  // 增加或减少多少
    EA_BuffID = 999,            // 表示为角色添加一个指定ID的BUFF，添加层数为1层
    EA_SoldierID = 10000,       // 5位数（这个数代表士兵的ID）	激活哪个士兵的天赋	整数，范围2-7	激活第几个天赋
};

#endif