#ifndef _ERROR_CODE_PROTOCOL_H__
#define _ERROR_CODE_PROTOCOL_H__

#pragma pack(1)

enum ERRORCODEPROTOCOL
{
	ERRORCODE_PROTOCOL = 1,					//错误码子消息
};

enum ErrorCode
{
///////////////////////100战斗////////////////////////////////////////
	//ERROR_BATTLE_
	ERROR_BATTLE_EXIST = 100,				//角色存在其他战斗
	ERROR_BATTLE_TIMESNOTENOUGH = 101,		//战斗次数不足
	ERROR_BATTLE_STATEERROR = 102,			//战斗状态异常
	ERROR_BATTLE_MAXTOWER= 103,				//爬塔楼层最高
	ERROR_BATTLE_STARTERROR = 104,			//消耗星星不足
	ERROR_BATTLE_ENERGYERROR = 105,			//体力不足
	// pvp
	ERROR_BATTLE_PVPHAVEROOM = 150,			//pvp房间已有
	ERROR_BATTLE_LOGIC = 199,

///////////////////////200背包///////////////////////////////////////
	//背包不足
	ERROR_BAG_ISNOTENOUGH = 200,			//背包不足
	ERROR_BAG_ITEMNOTEXIST = 201,			//道具不存在
	ERROR_BAG_EQUIPNOTEXIST = 202,			//装备不存在
	ERROR_BAG_GOODSNOTENOUGH = 203,			//道具不足
	ERROR_BAG_MAX_BAGNUM	 = 204,			//最大格子数
/////////////////////////300英雄/////////////////////////////////////////////
	ERROR_HERO_BAGNOTENOUGH		= 300,			//卡包不足
	ERROR_HERO_NOTEXIST			= 301,			//英雄不存在
	ERROR_HERO_EQUIPNOTEXIST	= 302,			//英雄没有穿装备
	ERROR_HERO_MAXSTART			= 303,			//英雄满星
	ERROR_HERO_LEVELNOTENOUGH   = 304,			//英雄等级不足
	ERROR_HERO_FRAGNOTENOUGH    = 305,			//英雄碎片不足
	ERROR_HERO_COMPLETE			= 306,			//英雄为整卡
	ERROR_HERO_SKILLBOOK		= 307,			//技能书不对应
	ERROR_HERO_LEVELLIMIT       = 308,			//英雄等级限制
//////////////////////////350天赋///////////////////////////////////////////////////
	ERROR_SKILL_STARTFAIL		= 350,			//天赋开启失败
	ERROR_SKILL_ISEXIST			= 351,			//天赋已经开启过
////////////////////////资源400////////////////////////////////////////
	ERROR_GOLD_NOTENOUGH		= 400,			//金钱不足
	ERROR_DIAMOND_NOTENOUGH		= 402,			//钻石不足
	ERROR_ENERGY_NOTENOUTH		= 403,			//体力不足
	ERROR_TOWERCOIN_NOTENOUGH	= 404,			//爬塔币不足
	ERROR_PVPCOIN_NOTENOUGH		= 405,			//竞技币不足
	ERROR_VIP_NOTENOUGH			= 406,			//VIP不足
	ERROR_LEVEL_NOTENOUGH		= 407,			//等级不足
	ERROR_SHILIANTIMES_NOTENOUGH = 408,			//试炼次数不足
	ERROR_GOLDBUYTIMES_NOTENOUGH = 409,			//金币购买次数不足
	ERROR_ENERGYBUYTIMES_NOTENOUGH = 410,		//体力购买次数不足
	ERROR_EITLETIMES_NOTENOUGH = 411,			//精英关卡挑战次数不足
	ERROR_UNIONCONTRIB_NOTENOUGH = 412,			//公会贡献不足
	ERROR_ELTLEBUYTIMES_NONENOUGH = 413,		//精英关卡购买次数不足
	ERROR_SHOPFRESHTIMES_NOTENOUGH = 414,		//商店刷新次数不足
///////////////////////////邮件450//////////////////////////////////
	ERROR_EMAIL_HAVEGET		= 450,				//邮件已领取
////////////////////////////任务500//////////////////////////////////////
	ERROR_TASK_HAVEGET		= 500,				//任务已领取
	ERROR_TASK_NOTFINISH	= 502,				//任务未完成
	ERROR_TASK_NOTEXIST		= 503,				//任务不存在
//////////////////////////////成就550/////////////////////////////////////////
	ERROR_ACHIEVE_HAVEGET	= 550,				//成就已领取
	ERROR_ACHIEVE_NOTFINISH	= 551,				//成就未完成
	ERROR_ACHIVEV_NOTEXIST	= 552,				//成就不存在
///////////////////////////////召唤师600/////////////////////////////////////////////
	ERROR_SUMMEONER_NOTEXIST = 600,				//召唤师不存在
///////////////////////////////试炼650/////////////////////////////////////////////
	ERROR_SHILIAN_TIMESNOTENOUGH = 650,			//试炼次数不足
	ERROR_SHILIAN_NOTSTART	= 651,				//试炼未开启
////////////////////////////////关卡700///////////////////////////////////////////////////
	ERROR_STAGE_NOTSTART = 700,					//关卡未开启
	ERROR_STAGE_NOTUNLOCK = 701,				//关卡未解锁
	ERROR_STAGE_FINISH = 702,					//关卡已完成, 不能再次挑战
/////////////////////////////////公会////////////////////////////////////////////////////////
	ERROR_UNION_EXPIDITION_OVER = 750,			//远征结束
	ERROR_UNION_EXPIDITION_STAGEPASS,			//关卡已经通关
	ERROR_UNION_EXPIDITION_REARDERROR,			//奖励未发放，不能设置
////////////////////////////////礼包900//////////////////////////////////////////////
	ERROR_GIFTCODE_INVALID = 900,				//礼包码非法
	ERROR_GIFTPLATFORM_INVALID = 901,			//平台非法
	ERROR_GIFTCHANNELID_INVALID = 902,			//渠道非法
	ERROR_GIFTSERVERID_INVALID = 903,			//服务器非法
/////////////////////////////////活动/////////////////////////////////////
	ERROR_MONTHCAR_OUTDATE = 950,				// 月卡过期
	ERROR_MONTHCARD_NOTEXIST,					//没有购买月卡
	ERROR_CURDAY_CANTGET,						//月卡当日不能领取
};

struct ErrorCodeData
{
	int nCode;
};


#pragma pack()

#endif //_ERROR_CODE_PROTOCOL_H__
