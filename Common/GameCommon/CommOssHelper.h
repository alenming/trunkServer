#ifndef _COMMOSS_HELPER_H__
#define _COMMOSS_HELPER_H__

#include <string>
#include "TeamModel.h"

//钻石消费类型
enum DiamondPayType
{
	NONE_PAY_TYPE,						//0 其他
	SINGLE_FLOOP_CARD_TYPE,				//1.单次抽卡
	TEN_FLOOP_CARD_TYPE,				//2.十连抽
	DIAMONDBUY_SUMMER_TYPE,				//3.购买召唤师
	DIAMONDBUY_HERO_TYPE,				//4.购买英雄
	DIAMONDBUY_ACTIVE_TYPE,				//5.购买体力
	DIAMONDBUY_GOLD_TYPE,				//6.购买金币
	DIAMONDBUY_GOODS_TYPE,				//7.购买道具
	DIAMONDBUY_EQUIP_TYPE,				//8.购买装备
	DIAMONDRESET_SHOP_TYPE,				//9.重置商店
	DIAMONDRESET_ELITE_TYPE,			//10.充值精英关卡
	DIAMONDRRENAME_TYPE,				//11 改名消耗钻石
	DIAMONDBUYPVPCHEST_TYPE,			//12 购买PVP宝箱
};

//钻石获得原因
enum DiamnodGetReason
{
	MAIL_DIAMOND_GET_REASON,			//邮件获得
	PAY_DIAMOND_GET_REASON,				//充值获得钻石
	MONTH_DIAMOND_GET_REASON,			//月卡所得
	TASK_DIAMOND_GET_REASON,			//任务所得
	ACTIVE_DIAMOND_GET_REASON,			//活动所得
	PVP_DIAMOND_GET_REASON,				//竞技场所得
	ITEMUSE_DIAMOND_GET_REASON,			//物品使用获得
	SIGN_DIAMOND_GET_REASON,			//签到获得钻石
	OTHER_DIAMOND_GET_REASON,			//其他钻石获得
};

//资源类型
enum ResOurceType
{
	NONE_RESOURCE_TYPE,
	GOLD_RESOURCE_TYPE,					//金币
	TOWER_RESOURCE_TYPE,				//塔币
	PVP_RESOURCE_TYPE,					//竞技币
	UNION_RESOURCE_TYPE,				//公会贡献
};

//非钻石支付类型
enum ShopPayForType
{
	NONE_SHOP_PAYFOR_TYPE,				
	SUMMER_SHOP_PAYFOR_TYPE,
	HERO_SHOP_PAYFOR_TYPE,
	GOODS_SHOP_PAYFOR_TYPE,
	EQUIP_SHOP_PAYFOR_TYPE,
	TASK_PAYLER_TYPE,				//任务/成就
};

//试炼挑战类型
enum ChallengeType
{
	NONE_CHALLENGE_TYPE,
	GOLD_CHALLENGLE_TYPE,
	HERO_CHALLENGLE_TYPE,
	TOWER_CHALLENGLE_TYPE,
	ACTIVE_CHALLENGLE_TYPE,
	UNIONEXPIDITION_TYPE,			//公会远征
};

//道具获得枚举
enum GAINGOODSREASON
{
	NONE_GAIN_REASON,
	WORDSTAGE_GAIN_REASON,		//世界副本（关卡）
	WORDSTAGEBOX_GAIN_REASON,	//世界副本章节宝箱
	SIGN_GAIN_REASON,			//签到
	ACTIVE_GAIN_REASON,			//活动
	FIRSTPAY_GAIN_REASON,		//首充
	SHOP_GAIN_REASON,			//商店
	MAIL_GAIN_REASON,			//邮件
	MAKE_GAIN_REASON,			//打造
	TASK_GAIN_REASON,			//任务
	ACHIEVE_GAIN_REASON,		//成就
	FLOPCARD_GAIN_REASON,		//抽卡
	SUMMONER_GAION_REASON,		//召唤师系统
	GOODSUSE_GAION_REASON,		//可使用道具
	GOLDTEST_GAION_REASON,		//金币试炼
	UNIONSTAGEBOX_GAION_REASON,	//工会远征宝箱
	UNIONACTIVEBOX_GAION_REASON,//工会活跃征宝箱
	PVPBOX_GAIN_REASON,			//竞技场宝箱
	CDKEY_GAIN_REASON,			//兑换码
	PVPDROP_GAIN_REASON,		//PVP掉落
};

enum PvpMatchType
{
	enNormalMathType = 1,
	enChamopinMatchType,
};

//角色PVE日志数据
struct SOssPveLogData
{
	int  nSummonerID;
	int  nHeroID[MAX_HERO_COUNT];
	int  nStageID;
	bool bWin;
	int  UnLockStageID;
	int  nCryStalLevel;
	int  nStartNum;
	int  nTime;
};

//角色PVP日志数据
struct SOssPvpLogData
{
	bool bWin;
	int  nPvpNum;
	int  nSummonerID;
	int  nOpponentSummer;
	int  nPvpType;
	int  nHeroID[MAX_HERO_COUNT];
	int  nCryStalLevel;
	int  nTime;
};

class CCommOssHelper
{
public:
	CCommOssHelper();
	~CCommOssHelper();

	//注册日志
	static void registerStatOss(int uid, std::string Mobile, int nChannelType, std::string ChannelName);

	//用户渠道绑定信息
	static void userChannelOss(int uid, int nChannelType, std::string ChannelName);

	//创建角色信息日志
	static void crateActorStatOss(int uid);

	//角色登陆信息日志
	static void userLoginOss(int uid, int LoginOutTime);

	//角色登出日志
	static void userLoginOutOss(int uid, int LoginTime);

	//用户新手引导日志
	static void newGuidOss(int uid, int nStep);

	//用户升级信息
	static void userLevelUpOss(int uid);

	//用户充值信息
	static void userCashOss(int uid, int nPayValue);

	//用户钻石消费信息
	static void userDiamondPayOss(int uid, int Diamond, int nPayType, int nNum, int Info = 0);

	//用户钻石购买信息
	static void userDiamondGetOss(int uid, int Diamond,int nReason);

	//用户非钻石购买信息
	static void userTokenShopOss(int uid, int TokenType, int Cost, int PayforType, int Num, int Info);

	//用户VIP升级信息
	static void UserVipUpOss(int uid);

	//PVE日志统计
	static void StageLogicOss(int uid, SOssPveLogData &LogData);

	//PVP日志统计
	static void PvpLogicOss(int uid, SOssPvpLogData &LogData);

	//其他副本战斗信息
	static void UserSpStageOss(int uid, int StageID, int StageType, bool bWin);

	//关卡扫荡统计
	static void StageSweepOss(int uid, int nStageID, int nTimes);

	//卡牌升星信息
	static void UserCardUpStartOss(int uid, int heroId);

	//卡牌升级信息
	static void UserCardLvUpOss(int uid, int ndynCardID);

	//当前在线用户信息
	static void ServerStatusOss();

	//用户礼包领取消息
	static void UserGiftGetOss(int uid, int nChannelID, int nGiftID, std::string CDKey);

	//用户匹配消息
	static void UserMatchOss(int uid, int PvpType, int nMathTime, int nMatchFlag);

	//活动数据消息
	static void UserActiveOss(int uid, int nActiveID, int nParamID);

	//用户道具信息
	static void userGoodsGainOss(int uid, int nActionID, int nGoodsID, int nGoodsNum);
};


#endif
