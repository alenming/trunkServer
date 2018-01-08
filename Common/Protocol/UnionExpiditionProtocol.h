/*
* 公会任务(个人&团队)协议
*/

#ifndef __UNION_TASK_PROTOCOL_H__
#define __UNION_TASK_PROTOCOL_H__

#include<string>

#pragma pack(1)

enum UNIONTASKPROTOCOL
{
	CMD_UNIONEXPIDITION_CSBEGIN,
	CMD_UNIONEXPIDITION_INFO_CS,			  //公会远征信息下发
    CMD_UNIONEXPIDITION_MAPSET_CS,           //公会远征设置
	CMD_UNIONEXPIDITION_STAGESTART_CS,       //公会远征关卡开始
	CMD_UNIONEXPIDITION_STAGEFINISH_CS,      //公会远征关卡结束
	CMD_UNIONEXPIDITION_REWARDGET_CS,        //公会远征奖励领取
	CMD_UNIONEXPIDITION_DAMAGERANK_CS,       //公会远征伤害排行
	CMD_UNIONEXPIDITION_STAGEINFO_CS,		 //公会远征关卡信息

	CMD_UNIONEXPIDITION_SCBEGIN = 100,
	CMD_UNIONEXPIDITION_INFO_SC,			  //公会远征信息下发
	CMD_UNIONEXPIDITION_MAPSET_SC,           //公会远征设置
	CMD_UNIONEXPIDITION_STAGESTART_SC,       //公会远征关卡开始
	CMD_UNIONEXPIDITION_STAGEFINISH_SC,      //公会远征关卡结束
	CMD_UNIONEXPIDITION_INDEX_PASS_SC,		 //关卡通关消息
	CMD_UNIONEXPIDITION_REWARDGET_SC,        //公会远征奖励领取
	CMD_UNIONEXPIDITION_DAMAGERANK_SC,       //公会远征伤害排行
	CMD_UNIONEXPIDITION_STAGEINFO_SC,		 //公会远征关卡信息
	CMD_UNIONEXPIDITION_REWARD_FLAG_SC,		 //奖励领取标记
    CMD_UNION_TASK_SCEND,
};

struct SUnionSendStageShowData
{
	int  nIndex;
	int  nHeadID;
	char szName[32];
	unsigned char cLevel;
	int nDamage;

	SUnionSendStageShowData()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 远征关卡信息
struct SExpiditionIndexInfo
{
	int nIndex;					//关卡序列
	int nBossHp;				//血量
};

//CMD_UNIONEXPIDITION_INFO_SC
struct SUnionExpiditionInfo
{
	unsigned char cExpiditonTimes;			//远征次数
	int			  nFightFinshTime;			//远征结束时间
	int			  nFightColdStartTime;		//远征休息开始时间
	int			  nRewardSendTime;			//奖励发放时间点
	int			  nWorldID;					//世界ID
	int			  nMapID;					//地图ID
	unsigned char cRewardFlag;				//奖励标识
	unsigned char cNum;						//BOSS个数
	// ... SExpiditionIndexInfo
};

//CMD_UNIONEXPIDITION_MAPSET_CS
struct UnionExpiditionMapSetCS
{
    int nWorldID;            //远征世界地图设置
};

//CMD_UNIONEXPIDITION_MAPSET_SC
struct UnionExpiditionMapSetSC
{
	int nWorldID;
	int nMapID;
	int nExpiditionFinishTime;				//远征结束时间
};

//CMD_UNIONEXPIDITION_STAGESTART_CS
struct UnionExpiditonStageStartCS
{
	int nIndex;					//挑战的关卡序列
	int summonerId;				//挑战召唤师
	int heroIds[7];				//英雄列表
	int mercenaryId;			//佣兵
};

//CMD_UNIONEXPIDITION_STAGESTART_SC
struct UnionExpiditonStageStartSC
{
	// RoomData
};

//CMD_UNIONEXPIDITION_STAGEFINISH_CS
struct UnionExpiditionStageFinishCS
{
	int nDamage;
	int nSummerID;				//召唤师ID
	int nHeroID[7];				//卡牌ID
	int nHeroStart[7];			//卡牌星级
	int mercenaryId;			//佣兵
};

//CMD_UNIONEXPIDITION_STAGEFINISH_SC
struct UnionExpiditionStageFinishSC
{
	int nIndex;
};

//CMD_UNIONEXPIDITION_REWARDGET_SC
struct UnionExpiditionRewardGetSC
{
	int nNum;
	//奖励物品ID，默认均为一个
};

struct SDamageSendRankShowData
{
	int nIndex;							//排名
	int nDamage;						//伤害值
	char szName[32];					//姓名
	int nSummerID;						//召唤师ID
	int nHeroID[7];						//卡牌ID
	int nStartID[7];					//卡牌星级
	unsigned char cBDType;				//蓝钻类型
	unsigned char cBDLev;				//蓝钻等级

	SDamageSendRankShowData()
	{
		memset(this, 0, sizeof(*this));
	}
};

//CMD_UNIONEXPIDITION_DAMAGERANK_SC
struct UnionExpiditionDamageRankSC
{
	int nMapID;								//地图ID
	int nMyIndex;
	int nNum;								//排行人数
	//SDamageSendRankShowData				//排行榜数据
};

//CMD_UNIONEXPIDITION_STAGEINFO_CS,		 //公会远征关卡信息
struct UnionExpiditionStageInfoCS
{
	int nIndex;
};

//CMD_UNIONEXPIDITION_STAGEINFO_SC,		 //公会远征关卡信息
struct UnionExpiditionStageInfoSC
{
	int nBossHp;
	int  nHeadID;
	char szName[32];
	unsigned char cLevel;
	int nDamage;
};

//CMD_UNIONEXPIDITION_REWARD_FLAG_SC
struct UnionExpditionRewardFlagSC
{
	unsigned char cFlag;
};

#pragma pack()

#endif //__UNION_TASK_PROTOCOL_H__
