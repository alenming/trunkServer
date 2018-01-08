/*
* 公共结构头文件
* 前后端共用的数据结构
* 可直接用作协议结构体
*
* 2015-2-5 By 宝爷
*/
#ifndef __COMM_STRUCTS_H__
#define __COMM_STRUCTS_H__

#if KX_TARGET_PLATFORM == KX_PLATFORM_LINUX
#include<string.h>
#endif

enum ERoomState
{
	ROOMSTATE_NONE,					//无
	ROOMSTATE_NOROOM,				//房间不存在
	ROOMSTATE_PREPARELOAD,			//等待玩家进入房间
	ROOMSTATE_LOADING,				//加载中
	ROOMSTATE_IDLETIME,				//战斗预备(动画播放时间段)
	ROOMSTATE_FIGHTING,				//战斗中
	ROOMSTATE_GAMEOVER,				//游戏结束
};
// 注意: 客户端等同匹配类型,
// 所以此类型要与匹配类型对应
enum EPvpRoomType
{
	PVPROOMTYPE_NONE,
	PVPROOMTYPE_PVP,				//pvp房间		
	PVPROOMTYPE_CHAMPIONSHIP,		//锦标赛房间
	PVPROOMTYPE_ROBOT,				//机器人房间
};

//PVP挑战结果
enum ChallengeResultType
{
	CHALLENGE_CANCEL = -1,  //挑战取消
	CHALLENGE_FAILED,		//挑战失败
	CHALLENGE_WON,			//挑战成功
	CHALLENGE_DRAW,			//平局
};

#pragma pack(1)

// 对战类型
enum EBattleType
{
	EBATTLE_NONE,			   // 无
	EBATTLE_CHAPTER,		   // 章节关卡
	EBATTLE_PVP,			   // 匹配对战
	EBATTLE_INSTANCE,		   // 副本关卡
	EBATTLE_GOLDTEST,		   // 金币试练
	EBATTLE_HEROTEST,		   // 英雄试练
	EBATTLE_TOWERTEST,		   // 爬塔试练
	EBATTLE_UNIONEXPEDITION,		   // 公会远征挑战
	EBATTLE_GUIDE,             // 引导关卡
	EBATTLE_PVPROBOT,          // pvp机器人
};

// 战斗指令信息
// CommandId 1 召唤士兵 Ext1 士兵ID Ext2 士兵线路
// CommandId 2 释放技能 Ext1 技能ID Ext2 释放点X位置
// CommandId 3 升级水晶
// CommandId 4 召唤召唤物 Ext1 召唤ID Ext2 召唤者等级 Ext3 X坐标 Ext4 Y坐标
// CommandId 5 说话 Ext1 第几句话
enum BattleCommandType
{
	CommandSummoner = 1,
	CommandSkill,
	CommandCrystal,
	CommandCallSolider,
	CommandTalk,
};

struct BattleCommandInfo
{
	int Tick;
	int CommandId;
	int ExecuterId;
	int Ext1;
	int Ext2;
	int Ext3;
	int Ext4;
};

struct SoldierItem
{
	int SoldierID;             //士兵ID
};

struct SoldierEquip
{
	int confId;				   //配置id
	unsigned char  cEffectID[8];
	unsigned short sEffectValue[8];

	SoldierEquip()
	{
		memset(this, 0, sizeof(SoldierEquip));
	}
};

// 协议——士兵卡片信息
struct SoldierCardData
{
	int SoldierId;			   // 士兵id
	int SoldierLv;			   // 士兵等级
	int SoldierStar;		   // 士兵星级
	int SoldierExp;			   // 士兵经验
	unsigned char SoldierTalent[8];     // 天赋
	int EquipCnt;			   // 装备个数
};

// 协议——英雄卡片信息
struct HeroCardData
{
	int HeroId;				   // 英雄id
};

// 战斗buff
struct BuffData
{
	int Target;
	int BuffId;
	int BuffLv;
};

// 协议——用户信息
struct PlayerData
{
	int UserId;				    // 玩家id
	int UserLv;				    // 玩家等级
	int Camp;				    // 玩家阵营
	int OuterBonusCount;        // OuterBonus数量
	int SoldierCount;		    // 玩家士兵个数
	int MecenaryCount;			// 佣兵士兵个数
    int Identity;			    // 玩家身份(蓝钻之类)
	char UserName[32];		    // 玩家名字
	// int*OuterBonusCount
	// HeroCardData*
	// SoldierCardData*
};

// 协议——房间信息
struct RoomData
{
	int StageId;			   // 关卡ID
	int StageLevel;			   // 关卡等级
	int BattleType;			   // 房间对战类型
	int Ext1;				   // 扩展字段1
	int Ext2;				   // 扩展字段2
	int BuffCount;			   // Buff数量
	int PlayerCount;		   // 房间内的玩家数量
	// BuffData *
	// PlayerData*
};

//邮件基本类容结构
struct BaseMailInfo
{
	int mailConfID;            // 邮件配置ID
	int sendTimeStamp;         // 发送时间戳
	char szTitle[32];		   // 邮件标题

	BaseMailInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 战斗开始数据
struct ChallengeTeamInfo
{
	int summonerId;				//挑战召唤师
	int heroIds[7];				//英雄列表
	int mercenaryId;			//佣兵
};

// 战斗结束数据
struct ChallengeBattleInfo
{
	int summonerId;				//召唤师
	int heroIds[7];				//英雄列表
	int mercenaryId;			//佣兵
	int timeCost;				//时间消耗
	int crystalCost;			//水晶消耗
	int hpPercent;				//召唤师血量百分比
	int nCrystalLv;				//水晶等级
};

#pragma pack()

#endif