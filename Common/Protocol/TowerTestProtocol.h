#ifndef __TOWERTESTPROTOCOL_H__
#define __TOWERTESTPROTOCOL_H__

#pragma pack(1)

enum TOWERTESTPROTOCOL
{
	CMD_TOWER_CSBEGIN,
	CMD_TOWER_FIGHTING_CS,		    //迎战敌人
	CMD_TOWER_FINISH_CS,		    //战斗结束
	CMD_TOWER_CHOSEOUTERBONUS_CS,	//选择buff
	CMD_TOWER_OPENTREASURE_CS,	    //打开宝箱
	CMD_TOWER_ONEKEYFIGHTING_CS,	//一键爬塔
	CMD_TOWER_CSEND,

	CMD_TOWER_SCBEGIN = 100,
	CMD_TOWER_FIGHTING_SC,		    //迎战敌人结果
	CMD_TOWER_FINISH_SC,		    //战斗结束获得的物品
	CMD_TOWER_CHOSEOUTERBONUS_SC,   //选择buff
	CMD_TOWER_OPENTREASURE_SC,	    //打开宝箱
	CMD_TOWER_ONEKEYFIGHTING_SC,	//一键爬塔
	CMD_TOWER_SCEND,
};

//CMD_TOWER_FIGHTING_CS

//CMD_TOWER_FIGHTING_SC
struct TowerFightingSC
{
	int floor;						//楼层数
	//+ RoomData..
};

//CMD_TOWER_FINISH_CS
struct TowerFinishCS
{
	int result;						//成功1/失败0
	//struct ChallengeBattleInfo 
};

//CMD_TOWER_FINISH_SC
struct TowerFinishSC
{
	int floor;						//楼层数
	int result;						//胜利或失败
};

// 宝箱结构 DropCurrency + TowerReward + count * DropItemInfo
struct TreasureReward
{
	int count;						//宝箱物品个数
	// + count * DropItemInfo
};

//CMD_TOWER_CHOSEOUTERBONUS_CS
// 如果都为0,则代表都不选择
struct TowerChoseOuterBonusCS
{
	int outerBonus1;				//outerBonus1
	int outerBonus2;				//outerBonus2
	int outerBonus3;				//outerBonus3
};

//CMD_TOWER_CHOSEOUTERBONUS_CS
//返回选择结果并进入下个楼层
struct TowerChoseOuterBonusSC
{
	int costStars;					//消耗的星星数量
	int outerBonus1;				//outerBonus1
	int outerBonus2;				//outerBonus2
	int outerBonus3;				//outerBonus3
};

//CMD_TOWER_OPENTREASURE_CS
//如果选择不打开则进入下个楼层
struct TowerOpenTreasureCS
{
	int operate;					//操作 1打开/0不打开
};

//CMD_TOWER_OPENTREASURE_SC
struct TowerOpenTreasureSC
{
	int diamond;					//消耗的钻石数量
	int count;						//奖励物品个数
	// 宝箱结构 DropCurrency + TowerReward + count * DropItemInfo
};

#pragma pack()

#endif //__TOWERTESTPROTOCOL_H__
