#ifndef __ACTIVITY_PROTOCOL_H__
#define __ACTIVITY_PROTOCOL_H__

#pragma pack(1)

enum INSTANCEPROTOCOL
{
	CMD_INSTANCE_CSBEGIN,
	CMD_INSTANCE_CHALLENGE_CS,			//挑战
	CMD_INSTANCE_FINISH_CS,				//结束
	CMD_INSTANCE_BUYTIMES_CS,			//副本购买次数
	CMD_INSTANCE_CSEND,

	CMD_INSTANCE_SCBEGIN = 100,
	CMD_INSTANCE_CHALLENGE_SC,			//挑战
	CMD_INSTANCE_FINISH_SC,				//结束
	CMD_INSTANCE_BUYTIMES_SC,			//购买次数
	CMD_INSTANCE_SCEND,
};

//CMD_INSTANCE_CHALLENGE_CS
struct InstanceChallengeCS
{
	int activityId;						//挑战副本
	int difficulty;						//难度
	// heroId
	// soldiers
};

//CMD_INSTANCE_CHALLENGE_SC
struct InstanceChallengeSC
{
	int activityId;					    //挑战副本
	int difficulty;					    //难度
};

//CMD_INSTANCE_FINISH_CS
//struct StageFinishData

//CMD_INSTANCE_FINISH_SC
struct InstanceFinishSC
{
	int activityId;					    //挑战副本
	int difficulty;					    //难度
	int	wonOrFailed;					//成功失败
};

//CMD_INSTANCE_BUYTIMES_CS
struct InstanceBuyTimesCS
{
	int activityId;						//活动副本id
	int buyTimes;						//次数
};

//CMD_INSTANCE_BUYTIMES_SC
struct InstanceBuyTimesSC
{
	int activityId;						//购买的活动副本id
	int buyTimes;						//购买的次数
	int useTimes;						//现有使用次数
	int costDiamond;					//消耗的钻石
};

#pragma pack()

#endif //__ACTIVITY_PROTOCOL_H__
