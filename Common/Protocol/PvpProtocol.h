#ifndef __PVPPROTOCOL_H__
#define __PVPPROTOCOL_H__

#pragma pack(1)

//PVP奖励协议, 发往ServiceServer
enum PvpInfoProtocol
{
	CMD_PVPINFO_CSBEGIN,
	CMD_PVPINFO_REWARD_CS,			//奖励领取
	CMD_CHAMPIONSHIP_REWARD_CS,		//锦标赛奖励
	CMD_PVPINFO_CSEND,

	CMD_PVPINFO_SCBEGIN = 100,
	CMD_PVPINFO_REWARD_SC,			//奖励领取信息
	CMD_CHAMPIONSHIP_REWARD_SC,		//锦标赛奖励
	CMD_PVPINFO_SCEND,
};

//匹配协议, 发往MatchServer
enum MatchProtocol
{
	CMD_MATCH_CSBEGIN,
	CMD_MATCH_MATCHING_CS,			//进行匹配
	CMD_MATCH_CANCEL_CS,			//取消匹配
	CMD_MATCH_CSEND,

	CMD_MATCH_SCBEGIN = 100,
	CMD_MATCH_MATCHING_SC,			//正在匹配
	CMD_MATCH_REMATCH_SC,			//重新匹配
	CMD_MATCH_MATCHSUCCESS_SC,		//成功匹配到信息
	CMD_MATCH_CANCEL_SC,			//取消匹配
	CMD_MATCH_SCEND,
};

//PVP战斗协议, 发往BattleServer
enum PvpProtocol
{
	CMD_PVP_CSBEGIN,
	CMD_PVP_ENTERROOM_CS,			//请求进入房间
	CMD_PVP_LOADING_CS,				//报告客户端加载信息
	CMD_PVP_READY_CS,				//报告加载完毕
	CMD_PVP_RECONNECT_CS,			//重连请求
	CMD_PVP_RECONNECTROOMDATA_CS,	//请求房间数据

	CMD_PVP_ENTERROBOTROOM_CS,		//进入机器人房间
	CMD_PVP_FINISHROBOTEROOM_CS,	//结束机器人房间
	CMD_PVP_CSEND,

	CMD_PVP_SCBEGIN = 100,
	CMD_PVP_ENTERROOM_SC,			//返回进入房间的信息与状态
	CMD_PVP_PREPARE_SC,				//2人在房间时, 通知玩家可以开始loading资源, 这时有房间数据下发
	CMD_PVP_LOADING_SC,				//同步加载信息
	CMD_PVP_READY_SC,				//通知有玩家加载完成
	CMD_PVP_IDLETIME_SC,			//空闲预备时间
	CMD_PVP_START_SC,				//通知玩家开始游戏
	CMD_PVP_END_SC,					//通知玩家游戏结束
	CMD_PVP_RESULT_SC,				//发送给胜利/失败玩家
	CMD_PVP_RECONNECT_SC,			//重连消息
	CMD_PVP_RECONNECTROOMDATA_SC,	//房间数据
	CMD_PVP_OPPRECONNECT_SC,		//对手重连
	CMD_PVP_OPPDISCONNECT_SC,		//对手断线通知
	CMD_PVP_ROBOTROOMDATA_SC,		//机器人房间信息
    CMD_PVP_SCEND,

    CMD_PVP_ENTERROOM_SS = 200,     //真正的进入房间，玩家进入房间-Session-Service-Battle
    CMD_PVP_FINISH_SS,
};

//CMD_PVPINFO_REWARD_CS
struct PvpInfoRewardCS
{
	int taskId;					//任务id
};

//CMD_PVPINFO_REWARD_SC
struct PvpInfoRewardSC
{
    int taskId;					//任务id
	int count;					//奖励个数
	//DropItemInfo
};

// CMD_CHAMPIONSHIP_REWARD_CS
// CMD_CHAMPIONSHIP_REWARD_SC
struct ChampionshipRewardSC
{
	int count;
};

//CMD_MATCH
//CMD_MATCH_MATCHING_CS
struct MatchMatchingCS
{
	int matchType;				//匹配类型, 比如匹配机器人
};

//CMD_MATCH_MATCHING_SC

//CMD_MATCH_MATCHPLAYER_SC
struct MatchMatchSuccessSC
{
	int battleId;				//battleId
	int robotId;
};

//CMD_MATCH_CANCEL_CS

//CMD_MATCH_CANCEL_SC

//CMD_PVP
//CMD_PVP_ENTERROOM_CS
struct PvpEnterRoomCS
{
	int battleId;				//battleid
	int roomType;				//房间类型
	int summonerId;				//召唤师id
	int heroCount;				//英雄个数
};

//CMD_PVP_ENTERROOM_SC
struct PvpEnterRoomSC
{
	int result;					//加入房间结果, 为了告诉玩家服务器成功添加到房间
	int camp;					//自己阵容

	int uid;				    //对手玩家id
	int usrLv;				    //对手玩家等级
	int summonerId;			    //对手玩家召唤师id
	int integral;				//对手玩家积分
	int progress;				//加载进度, 重连时对手的可能不为0
    int extend;				    //扩展字段(蓝钻等级*10+类型)
	char usrName[32];		    //对手玩家名称
};

//CMD_PVP_PREPARE_SC
//RoomData*
//通知前端开始加载

//CMD_PVP_LOADING_CS
struct PvpLoadingCS
{
	int progress;               //加载进度
};

//CMD_PVP_LOADING_SC
struct PvpLoadingSC
{
    int uid;                    //谁的进度
    int progress;               //进度数据
};

//CMD_PVP_READY_CS
//玩家已加载完成

//CMD_PVP_READY_SC
struct PvpReadySC
{
	int uid;					//房间内通知谁准备完成
};

//CMD_PVP_RECONNECT_CS
struct PvpReconnectCS
{
	int battleId;				//重连战斗id
};

//CMD_PVP_RECONNECT_SC
struct PvpReconnectSC
{
	int roomState;				//此时房间状态
	int roomType;				//房间类型
};

//CMD_PVP_RECONNECTROOMDATA_CS
struct PvpReconnectRoomDataCS
{
	int battleId;				//匹配id
};

//CMD_PVP_ENTERROBOTROOM_CS
struct PvpEnterRobotRoomCS
{
	int summonerId;				//召唤师id
	int heroCount;				//英雄个数
	// heroId ...
};

//CMD_PVP_FINISHROBOTEROOM_CS
struct PvpFinishRobotRoomCS
{
	int wonOrFailed;			//胜利失败
};

//CMD_PVP_START_SC
struct PvpStartSC
{
	int startStamp;			    //开始时间戳
};

//CMD_PVP_END_SC

//CMD_PVP_RESULT_SC
struct PvpResultSC
{
	int roomType;				//房间类型
	int result;					//战斗结果
	int integral;				//现有积分
	int rankNow;				//现排名 (0为10000名以外)
	int itemCount;				//奖励个数
};

//CMD_PVP_ENTERROOM_SS
struct PvpEnterRoomSS
{
    int battleId;               //战斗id
    int roomType;               //房间类型
    int mmr;                    //玩家的MMR
    int integral;               //玩家的竞技积分
    int continueWin;            //玩家的连胜场数
    int dayWinTimes;            //玩家的日胜场数
    // PlayerData*
};

// CMD_PVP_FINISH_SS
struct PvpFinishSS
{
    int roomType;               // 房间类型
    int result;                 // 结果
    int newintegral;            // 计算后的积分
    int integral;               // 改变的积分
    int mmr;                    // 改变的MMR
    int crylv;                  // 水晶等级
    int dt;                     // 耗时（秒）
	int summonerid;				// 召唤师ID
	int heroid[7];				// 英雄ID
	int enemysummonerid;		// 敌方召唤师
    int itemCount;              // 物品数量
    // DropItemInfo*
};

#pragma pack()

#endif 
