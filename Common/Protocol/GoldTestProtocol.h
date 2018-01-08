#ifndef __GOLDTRIALPROTOCOL_H__
#define __GOLDTRIALPROTOCOL_H__

#pragma pack(1)


enum GOLDTESTPROTOCOL
{
	CMD_GOLDTEST_CSBEGIN,
	CMD_GOLDTEST_CHALLENGE_CS,   // 金币试炼挑战
	CMD_GOLDTEST_FINISH_CS,      // 完成挑战
	CMD_GOLDTEST_REWARDCHEST_CS, // 领取宝箱

	CMD_GOLDTEST_CSEND,


	CMD_GOLDTEST_SCBEGIN = 100,
    CMD_GOLDTEST_CHALLENGE_SC,    // 返回挑战信息
    CMD_GOLDTEST_FINISH_SC,       // 返回完成挑战
    CMD_GOLDTEST_REWARDCHEST_SC,  // 领取结果

	CMD_GOLDTEST_SCEND,

};

//CMD_GOLDTEST_CHALLENGE_CS
//CMD_GOLDTEST_CHALLENGE_SC

//CMD_GOLDTEST_FINISH_CS
struct GoldTestFinishCS
{
	int damage;                   // 基础伤害值
};

//CMD_GOLDTEST_FINISH_SC
struct GoldTestFinishSC
{
	int damage;                   // 基础伤害值
	int damageReward;             // 伤害奖励(金币)
	int levelReward;              // 等级加成(金币)
};

//CMD_GOLDTEST_REWARDCHEST_CS	  
struct GoldTestRewardChestCS
{
	int chestFlag;                // 0为一键领取,>0为具体单领
};

//CMD_GOLDTEST_REWARDCHEST_SC
struct GoldTestReardChestSC
{
	int goldReward;              // 奖励金币数量
	int chestFlag;               // 当前所有宝箱的状态
};

#pragma pack()


#endif