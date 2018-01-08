#ifndef __STAGE_PROTOCOL_H__
#define __STAGE_PROTOCOL_H__

#pragma pack(1)

enum STAGEPROTOCOL
{
	CMD_STAGE_CSBEGIN,
	CMD_STAGE_CHALLENGE_CS,			//关卡挑战
	CMD_STAGE_FINISH_CS,			//关卡结束
	CMD_STAGE_SWEEP_CS,				//扫荡
	CMD_STAGE_STRATEGY_CS,			//攻略
	CMD_STAGE_CHAPTERAWARD_CS,		//章节奖励
	CMD_STAGE_BUYTIMES_CS,			//购买挑战次数
	CMD_STAGE_BUYCHPATER_CS,		//购买章节
	CMD_STAGE_CSEND,

	CMD_STAGE_SCBEGIN = 100,
	CMD_STAGE_CHALLENGE_SC,			//关卡挑战资格验证
	CMD_STAGE_FINISH_SC,			//关卡结束战利品下发
	CMD_STAGE_SWEEP_SC,				//扫荡结果
	CMD_STAGE_STRATEGY_SC,			//攻略
	CMD_STAGE_CHAPTERAWARD_SC,		//章节奖励
	CMD_STAGE_BUYTIMES_SC,			//购买挑战次数
	CMD_STAGE_BUYCHPATER_SC,		//购买章节
	CMD_STAGE_SCEND,
};

// CMD_STAGE_CHALLENGE_CS
struct StageChallengeCS
{
	int chapterId;			//章节id
	int stageId;			//挑战关卡id
};

//CMD_STAGE_CHALLENGE_SC
struct StageChallengeSC
{
	int result;				//能否挑战
	// 如果可以则带有roomData
};

//CMD_STAGE_FINISH_CS
 struct StageFinishCS
 {
 	int chapterId;			//章节id
 	int stageId;			//关卡id
	int result;				//结果
 	//...StageFinishData
 };

struct StageFinishData
{
	int result;				//胜利或失败
	int time;				//通关时间
	int hpStatus;			//血量状况
	int costCrystal;		//花费的水晶数量
};

//CMD_STAGE_FINISH_SC
struct StageFinishSC
{
	int chapterId;			//章节id
	int stageId;			//关卡id
	int result;				//胜利1或失败0
	// result如果为1, 则会跟着stageReward的数据
};

struct StageReward
{
	int star;				//星级
	int star2Reason;		//星星1获得理由, 不获得为0
	int star3Reason;		//星星2获得理由, 不获得为0
	int rewardCount;		//奖励个数
	//...DropItemInfo
};

//CMD_STAGE_SWEEP_CS
struct StageSweepCS
{
	int chapterId;			//章节id
	int stageId;			//关卡id
	int times;				//扫荡次数
};

//CMD_STAGE_SWEEP_SC
struct StageSweepSC
{
	int chapterId;
	int stageId;
	int times;				//扫荡结果次数
	//...StageReward * count
};

//CMD_STAGE_STRATEGY_CS
struct StageStrategyCS
{
	int stageId;			//指定关卡id
};

//CMD_STAGE_STRATEGY_SC
struct StageStrategySC
{
	int stageId;
	int teamCount;			//队伍信息个数
	//... teamType(int) + PassTeam + PassTeamHero + passTeamEquip
};

//CMD_STAGE_CHAPTERAWARD_CS
struct StageChapterAwardCS
{
	int chapterId;			//领取奖励的章节
	unsigned char cIndex;	//领取的宝箱标识
};

//CMD_STAGE_CHAPTERAWARD_SC
struct StageChapterAwardSC
{
	int chapterId;			//领取奖励的章节
	int itemCount;			//物品个数
	//... DropItemInfo
};

//CMD_STAGE_BUYTIMES_CS
struct StageBuyTimesCS
{
	int chapterId;			//章节id
	int stageId;			//购买指定精英关卡的挑战次数
};

//CMD_STAGE_BUYTIMES_SC
struct StageBuyTimesSC
{
	int chapterId;			//章节id
	int stageId;			//返回精英关卡id
	int addTimes;			//返回增加的次数
};

//CMD_STAGE_BUYCHPATER_CS
struct StageBuyChapterCS
{
	int chapterId;			//购买的章节id
};

//CMD_STAGE_BUYCHPATER_SC
struct StageBuyChapterSC
{
	int chapterId;			//购买的章节id
	int diamond;			//花费钻石
};

#pragma pack()

#endif //__STAGE_PROTOCOL_H__
