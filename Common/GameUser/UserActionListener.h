/*
* 监听动作接口
*   目前由任务/成就类继承
*/

#ifndef __USER_ACTION_LISTENER_H__
#define __USER_ACTION_LISTENER_H__

// 监听动作
enum EListenerAction
{
    ELA_BEGIN,
    ELA_PASS_STAGE,                   // 通关指定/任意关卡
    ELA_HERO_EQUIP,                   // 为任意英雄穿戴装备
    ELA_HERO_LEVEL_UP,                // 为任意英雄升级
    ELA_HERO_UPGRADE_STAR,            // 为任意英雄升星
    ELA_HERO_UPGRADE_SKILL,           // 为任意英雄技能升级
    ELA_DRAW_CARD,                    // 抽卡N次
    ELA_USE_EXP_BOOK,                 // 使用任意经验卡
    ELA_BUY_GOLD_TIMES,               // 点金（钻石购买金币）X次
    ELA_DAILY_ENERGY,                 // 领取每日体力
    
    ELA_USER_LEVEL_UP,                // 玩家等级达到X级(包括特定任务和非激活任务)
    ELA_OWN_HERO,                     // 拥有N个X级英雄
    ELA_HERO_UPGRADE_STAR_TO_X,       // 升星任意英雄到X星
    ELA_HERO_UPGRADE_SKILL_TO_X,      // 升级任意英雄技能到X星
    ELA_OWN_EQUIP,                    // 拥有X件Ｘ色XX装备
    ELA_OWN_DIFFEREN_HERO,            // 拥有X个不同的英雄
    ELA_OWN_SUMMONER_COUNT,           // 拥有X个召唤师
    ELA_OWN_ASSIGN_SUMMONER,          // 获得XXX召唤师
    ELA_STAGE_STAR,                   // 玩家关卡获得XXX个星星
    ELA_ACCUMULATE_GOLD,              // 累计获得XXXXX金币

    ELA_HERO_TEST,                    // 完成N次英雄试炼
    ELA_GOLD_TEST,                    // 完成N次金币试炼
    ELA_TOWER_TEST_FLOOR,             // 完成N层爬塔试炼
    ELA_PVP,                          // 完成N次竞技
    ELA_UNION_TASK,                   // 完成N次公会任务
    ELA_FB_STAGE,                     // 完成N次副本关卡
    ELA_MONTH_CARD,                   // 月卡领取
    ELA_PASSED_STAGE,                 // 通过指定关卡

	ELA_USER_FIRSTLOGIN,			  // 用户首次登陆 

    ELA_HAVE_UNION = 30,              // 创建/加入公会
    ELA_BUY_ITEM_COUNT,               // 商店累计购买N件商品
    ELA_HAVE_HERO_X_STAR,             // 拥有N个X星级的英雄
    ELA_USE_SAME_RACE,                // 累计只使用X种族英雄取得N场胜利
    ELA_PVP_KEEP_WIN,                 // 竞技场取得N连胜
    ELA_PVP_SCORE,                    // 竞技场积分达到N分
    ELA_PVP_RANK,                     // 竞技场历史排名达到N名以内
	ELA_CONTINUELOGIN = 40,			  // 连续登陆天数
	ELA_TOTALLOGIN = 41,			  // 累计登陆天数
    ELA_UNION_LIVENESS,               // 公会活跃
    ELA_UNION_DISPATCH_MERCENARY,     // 公会派遣佣兵
    ELA_UNION_USE_MERCENARY,          // 公会使用佣兵

	ELA_FUND = 46,					  // 基金触发事件
	ELA_DIAMONDCOSTNUM = 47,		  // 钻石消耗次数事件
	ELA_GOLDCOSTNUM,				  // 金币消耗累计数
	ELA_PVPCOINCOSTNUM,				  // PVP币消耗累计数
	ELA_TOWERCOINCOSTNUM,			  // 塔币消耗累计数
	ELA_ENERGY_BUYTIMES,			  // 体力购买次数

	ELA_RMVBCOST = 100,				 //人民币充值
	ELA_CHAMPION = 101,				 //竞标赛
	ELA_PAYDIAMOND = 102,			// 累计充值钻石数

	ELA_OLTIME	= 201,				//累计在线时长
	ELA_ACTIVELIFTCOST,				//体力消耗
	ELA_SHOPFRESHTIMES,				//商店刷新次数
	ELA_EQUIPBUILD,					//装备打造
	ELA_OWNCOLORHERO,				//拥有X色的英雄
	ELA_FINISHACTIVETASK = 207,		//完成成长任务数目
	ELA_OWNLEVELHERO,				//拥有指定等级的英雄数目
    ELA_END,
};


//param1 PVP事件中1是胜利，2是失败
struct ActionCommonInfo
{
    int param1;
    int param2;
};

class IUserActionListener
{
public:
    virtual bool onAction(int actID, void *data, int len) = 0;
};

#endif