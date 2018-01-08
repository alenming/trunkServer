#ifndef __HEROTESTPROTOCOL_H__
#define __HEROTESTPROTOCOL_H__
#pragma pack(1)

enum HEROTESTPROTOCOL
{
	CMD_HEROTEST_CSBEGIN,
	CMD_HEROTEST_CHALLENGE_CS,			//挑战英雄试练
	CMD_HEROTEST_FINISH_CS,				//挑战结束
	CMD_HEROTEST_CSEND,

	CMD_HEROTEST_SCBEGIN = 100,
	CMD_HEROTEST_CHALLENGE_SC,			//挑战英雄试练结果
	CMD_HEROTEST_FINISH_SC,				//挑战结束结果
	CMD_HEROTEST_SCEND,
};

//CMD_HEROTEST_CHALLENGE_CS
struct HeroTestChallengeCS
{
    int instanceId;						//英雄试练副本
	int diff;							//挑战难度
};

//CMD_HEROTEST_FINISH_CS
struct HeroTestFinishCS
{
	int instanceId;					    //挑战副本
	int diff;							//难度
	int result;							//结果						
	// 如果成功则带上ChallengeBattleInfo
};

//CMD_HEROTEST_FINISH_SC
struct HeroTestFinishSC
{
	int instanceId;					    //挑战副本
	int diff;							//难度
	int	result;						    //成功失败
};

#pragma pack()

#endif
