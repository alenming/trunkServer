#ifndef __LOGIN_PROTOCOL_H__
#define __LOGIN_PROTOCOL_H__

#pragma pack(1)

#include <map>
#include <vector>
/*
*   登录流程简介
*   目前支持3种登录校验流程：
*   1.测试登录校验，直接输入指定的UID登录，这种方式不做任何校验
*       前端发送CMD_LOGIN_CHECK_TEST_CS请求，包体携带指定的UID
*       服务器直接返回CMD_LOGIN_CHECK_SC，包体携带UID
*
*   2.游客登录校验，游客登录校验分两种情况，首次登录和非首次登录
*
*   2.1 首次游客登录校验，服务器需要为客户端生成游客的UID以及密钥【前端只允许发一次】
*       前端发送CMD_LOGIN_NEWGUEST_SC请求
*       后端生成游客的UID，使用特定前缀sum + UID进行MD5加密后作为密码
*       将guest-uid作为key，密码作为value存储到redis中
*       服务器返回CMD_LOGIN_NEWGUEST_SC给前端，包体携带UID + 密码
*
*   2.2 非首次游客登录校验，提交用户ID + 密码即可
*       前端发送CMD_LOGIN_CHECK_GUEST_CS请求，包体携带用户ID + 密码
*       服务器从redis中取出guest-uid对应的密码，进行验证
*       验证后返回CMD_LOGIN_CHECK_SC，包体携带UID，验证失败时为-1
*
*   2.3 游客绑定到平台
*       客户端先调用平台SDK进行登录，拿到token和openid
*       客户端请求CMD_LOGIN_CHECK_BINDGUEST_CS，包体携带平台类型、token、openid、游客uid、游客密码
*       服务器验证后返回CMD_LOGIN_CHECK_SC，包体携带UID，验证失败时为-1
*       【服务器验证后需要将游客的账号密码key删除】
*
*   3.平台登录校验
*       客户端先调用平台SDK进行登录，拿到token和openid
*       客户端请求CMD_LOGIN_CHECK_PF_CS，包体携带平台类型、token、openid
*       服务器验证后返回CMD_LOGIN_CHECK_SC，包体携带UID，验证失败时为-1
*
*   当校验通过之后，客户端统一发送真正的登录请求CMD_LOGIN_CS，拉取用户数据
*/

enum LOGINPROTOCOL
{
    CMD_LOGIN_CSBEGIN,
    CMD_LOGIN_CHECK_TEST_CS,        //测试登陆验证（UID登录，直接通过）
    CMD_LOGIN_CHECK_PF_CS,          //平台登录验证
    CMD_LOGIN_CHECK_GUEST_CS,       //游客登录验证
    CMD_LOGIN_CHECK_BINDGUEST_CS,   //将平台账号绑定到指定游客
	CMD_LOGIN_EXISTUSER_CS,			//使用已有账号密码登陆
    CMD_LOGIN_CS = 6,				//验证通过后请求登录
    CMD_LOGIN_UNION_CS,             //登陆帮会服
    CMD_LOGIN_RECONECT_CS,		    //登陆重连
    CMD_LOGIN_USERINFO_CS,			//提交用户信息
    CMD_LOGIN_GUIDEINFO_CS,			//提交引导信息
    CMD_LOGIN_FRESH,				//登陆数据更新
    CMD_LOGIN_CHECK_CHAT_CS,        //聊天服务器验证
    CMD_LOGIN_CSEND,

    CMD_LOGIN_SCBEGIN = 100,
    CMD_LOGIN_CHECK_SC,			    //验证应答
    CMD_LOGIN_NEWGUEST_SC,          //注册游客成功
    CMD_LOGIN_FINISH_SC,		    //登录完成
    CMD_LOGIN_SC,				    //玩家数据下发
    CMD_LOGIN_USERMODEL_SC,		    //玩家模型数据
    CMD_LOGIN_BAGMODEL_SC,		    //背包模型数据
    CMD_LOGIN_EQUIPMODEL_SC,	    //装备模型数据
    CMD_LOGIN_SUMMONMODEL_SC,	    //召唤师模型数据
    CMD_LOGIN_HEROMODEL_SC,		    //英雄模型数据
    CMD_LOGIN_STAGEMODEL_SC,	    //关卡模型数据
    CMD_LOGIN_TEAMMODEL_SC,         //队伍模型数据
    CMD_LOGIN_TASKMODEL_SC,         //任务模型数据
    CMD_LOGIN_ACHIEVEMODEL_SC,      //成就模型数据
    CMD_LOGIN_GUIDEMODEL_SC,	    //引导模型数据
    CMD_LOGIN_UNIONMODEL_SC,	    //公会模型数据
    CMD_LOGIN_MAILMODEL_SC, 	    //邮件模型数据
    CMD_LOGIN_INSTANCEMODEL_SC,	    //活动副本模型
    CMD_LOGIN_GOLDTESTMODEL_SC,     //金币试炼数据模型
    CMD_LOGIN_HEROTESTMODEL_SC,	    //英雄试炼数据模型
    CMD_LOGIN_TOWERTESTMODEL_SC,    //爬塔试炼数据
    CMD_LOGIN_PVPMODEL_SC,		    //pvp模型数据
    CMD_LOGIN_SHOPMODEL_SC,		    //商店模型数据
    CMD_LOGIN_ACTIVEMODEL_SC,	    //活动模型数据
    CMD_LOGIN_BAN_SC,			    //服务器封停应答
    CMD_LOGIN_RECHANGE_SC,		    //服务器角色被顶号应答
    CMD_LOGIN_TICK_SC,			    //服务器角色被T应答
	CMD_LOGIN_HEAD_SC,				//服务器头像数据应答
	CMD_LOGIN_UNIONSHOP_SC,			//服务器公会商店数据应答
	CMD_LOGIN_7DAYACTIVE_SC,		//七天活动数据应答
	CMD_LOGIN_PVPCHESTMODEL_SC,		//pvp宝箱
	CMD_LOGIN_CHECK_CHAT_SC,        //聊天服务器验证应答
	CMD_LOGIN_QQACTIVE_SC,			//QQ蓝钻活动数据下发
    CMD_LOGIN_SCEND
};


// 请求测试验证 CMD_LOGIN_CHECK_TEST_CS
struct LoginCheckTestCS
{
    int uid;
};

// 请求游客验证 CMD_LOGIN_CHECK_GUEST_CS
struct LoginCheckGuestCS
{
	char szMobile[40];				//设备号
    char password[32];
};

// 请求平台验证 CMD_LOGIN_CHECK_PF_CS
// 使用已有账号密码 CMD_LOGIN_EXISTUSER_CS
struct LoginCheckPlatformCS
{
    int nChannelID;				//渠道ID
    char openid[128];
    char openkey[256];
};

// 请求验证并绑定到游客 CMD_LOGIN_CHECK_BINDGUEST_CS
struct LoginCheckBindGuestCS
{
    int nChannelID;
    int uid;
	char szMobile[40];
    char password[32];
    char openid[128];
    char openkey[128];
};

// 验证成功返回 CMD_LOGIN_CHECK_SC
struct LoginCheckSC
{
    int uid;
	int nIsNew;				//默认0标识非新用户，1，标识新用户
	int nIsGuest;			//默认0，非游客，1-游客
};

//CMD_LOGIN_CS
struct LoginCS
{
    int userid;
};

//CMD_LOGIN_SC
struct LoginSC
{
    int serverStamp;
};

//CMD_LOGIN_USERINFO_CS,			//提交用户信息
struct LoginUserInfoCS
{
	int nIsGuest;					//是不是游客，0不是，1-是
	int nUid;
	int nIsNew;						//是不是新用户，0-不是，1-是
	int nChannelID;
	char szChannelName[128];
	char szMobile[40];
};

// CMD_LOGIN_GUIDEINFO_CS
struct LoginGuideInfoCS
{
    int nStepId;
};

//CMD_LOGIN_USERMODEL_SC
struct LoginUserModelInfo
{
    int userId;				      // 玩家id
    int headId;				      // 玩家头像
    int	userExp;			      // 玩家经验
    int gold;				      // 玩家金币
    int diamond;			      // 玩家钻石
    int towerCoin;				  // 塔币
    int pvpCoin;				  // pvp币
    int unionContrib;			  // 公会贡献
	int flashcard10;			  // 10连抽卡券
    int flashcard;				  // 抽卡券

	int resetTimeStamp;			  // 恢复时间戳(包括金币购买次数, 体力购买次数, 体力恢复)
    unsigned short sbagCapacity;  // 现背包容量
	unsigned char  cuserLv;		  // 玩家等级
    unsigned char  cfreeHeroTimes; //免费抽卡次数
    unsigned char changeNameFree; // 0为免费,1为需要费用
	unsigned short sbuyGoldTimes; // 购买金币次数

	int payment;				  // 充值额度
    int monthCardStamp;           // 月卡过期时间戳
	int nTotalSignDay;			  //累计签到天数
	int nMonthSignDay;			  //当月累计签到天数
	int nTotalSignFlag;			  //累计签到的次数

	int nFondFlag;				  //基金标记
	unsigned char cDaySignFlag;	  //是否已经签到，0-未签到，1-已经签到
	unsigned char cFirstPayFlag;  //首冲标记
	unsigned char cBDType;		  //蓝钻类型 //蓝钻类型 1,3,5,7
	unsigned char cBDLv;		  //蓝钻等级
	int nBDTimeStamp;			  //蓝钻结束时间点
    char name[32];
};

//CMD_LOGIN_BAGMODEL_SC
struct LoginBagModelInfo
{
    unsigned short scurCapacity;
    unsigned short count;
};

struct BagItemInfo
{
    int id;
    int val;
};

// CMD_LOGIN_EQUIPMODEL_SC
struct LoginEquipModelInfo
{
    int count;
    // ... EquipItemInfo
};

struct EquipItemInfo
{
    int equipId;
    int confId;
    unsigned char cMainPropNum;		//主属性个数
    unsigned char cEffectID[8];
    unsigned short sEffectValue[8];
};

//CMD_LOGIN_SUMMONMODEL_SC
struct LoginSummonModelInfo
{
    unsigned char count;
    // summonerids;
};

//CMD_LOGIN_HEROMODEL_SC
struct LoginHeroModelInfo
{
    unsigned char count;
    //HeroInfo list
};

struct LoginHeroInfo
{
    int heroId;                   // 英雄id
	unsigned short heroFrag;	  // 英雄碎片
    unsigned char  heroLv;        // 等级
    unsigned char  heroStar;      // 星级
    int heroExp;                  // 经验
    unsigned char heroTalent[8];  // 天赋
	int equip[6];				  // 装备个数, 一定是6个, 未装备为0
};

//CMD_LOGIN_STAGEMODEL_SC
struct LoginStageModelInfo
{
    int curStage;							  // 当前普通关卡id
    int curElite;							  // 当前精英关卡id
    unsigned short chapterCount;			  // 普通章节个数
	unsigned short eliteChapterCount;		  // 精英章节个数
	unsigned short stageCount;				  // 普通关卡个数
	unsigned short eliteCount;				  // 精英关卡个数
	unsigned short eliteRecordCount;		  // 精英关卡记录个数
	
    //ChapterInfo list
    //EliteChapterInfo list
    //StageInfo list
    //EliteInfo list
    //EliteRecordInfo List
};

struct ChapterStatusInfo
{
    int chapterID;				  // 章节id
    unsigned char chapterStatus;  // 章节状态
	unsigned char chapterboxCount;// 章节宝箱数目
	//id unsigned char List;	  // 宝箱索引值
};

struct EliteChapterStatusInfo
{
    int				chapterID;				  // 章节id
	unsigned char	chapterStatus;			  // 精英章节状态 
};

struct StageStatusInfo
{
    int			  stageId;				  // 关卡id
	unsigned char stageStatus;			  // 关卡状态
};

struct EliteStatusInfo
{
    int			  stageId;				  // 关卡id
	unsigned char stageStatus;			  // 关卡状态
};

struct EliteRecordInfo
{
    int		stageId;				  // 精英关卡id
	char	canUseTimes;			  // 可使用次数
    int		useStamp;				  // 上次使用的时间戳
	char	buyTimes;				  // 购买次数
    int		buyStamp;				  // 购买时间戳
};

//CMD_LOGIN_TEAMMODEL_SC
struct LoginTeamModelInfo
{
    int teamCount;                // 队伍数(多种类型的队伍)
    // TeamInfo list
};

struct TaskInfo
{
    int taskID;                 // 成就ID
    int taskVal;                // 数值
    int taskStatus;             // 状态ETaskStatus
    int resetTime;              // 重置的时间戳
};

//CMD_LOGIN_TASKMODEL_SC
struct LoginTaskModelInfo
{
    int taskCount;               // 任务个数
    // TaskInfo list
};

struct AchieveInfo
{
    int achieveID;               // 成就ID
    int achieveVal;              // 累计数值
    int achieveStatus;           // 成就状态EAchieveStatus
};

//CMD_LOGIN_ACHIEVEMODEL_SC
struct LoginAchieveModelInfo
{
    int achieveCount;            // 成就个数
    // AchieveInfo list
};

struct LoginGuideInfo
{
    int nNum;				//引导个数
    //引导ID
};

// CMD_LOGIN_UNIONMODEL_SC
struct LoginUnionModelInfo
{
    char hasUnion;                 // 是否拥有公会(0没有1有)
    // 0->NoUnionInfo 1->OwnUnionInfo
};

struct NoUnionInfo
{
    char applyCount;             // 申请的公会列表数据
    int todayApplyCount;         // 今日申请次数
    int applyStamp;              // 申请时间戳
    // applyCount ->ApplyInfo
};

struct ApplyInfo
{
    int applyTime;                 // 申请过期时间
    int unionID;                   // 公会ID
};

struct OwnUnionInfo
{
    int unionID;                   // 公会Id
    int todayStageLiveness;        // 今日关卡活跃度
    int todayPvpLiveness;          // 今日pvp活跃度
    int totalContribution;         // 累计贡献
    int unionLiveness;             // 公会活跃度
    int originUnionLv;             // 公会今日原始等级
    int unionLv;                   // 公会等级
    int welfareTag;                // 福利领取标识
    int emblem;                    // 会徽
    char pos;                      // 请求者的公会职位
    char hasAudit;                 // 是否有审核信息
	char hasExpiditionReward;	   // 是否有远征奖励
	unsigned int nExFinishTime;		//远征结束时间
	unsigned int nExRestFinishTime;	//休息结束时间
    char unionName[20];            // 公会名称
    char notice[128];              // 公告
};

// CMD_LOGIN_MAILMODEL_SC
struct LoginMailModelInfo
{
    char normalMailCount;        // 普通邮件
    char webMailCount;           // web邮件
    // normalMailCount ->LoginMailInfo
    // webMailCount ->LoginMailInfo
};

struct LoginMailInfo
{
	LoginMailInfo()
	{
		memset(this, 0, sizeof(*this));
	}
	int nMailID;						//邮件ID
	int mailConfID;                     // 邮件配置ID
	int sendTimeStamp;                  // 发送时间戳
	char szTitle[32];					// 邮件标题
};

//CMD_LOGIN_INSTANCEMODEL_SC
struct LoginInstanceModelInfo
{
    int instanceCount;		//活动副本个数
};

struct InstanceInfo
{
    int		activityId;		 //活动副本id
    int		useTimes;		 //挑战次数
    int		useStamp;		 //挑战时间戳
    int		buyTimes;		 //购买次数
    int		buyStamp;		 //购买时间戳
    int		easy;			 //简单获得星级
    int		normal;			 //普通获得星级
    int		difficult;		 //困难获得星级
    int		hell;			 //地狱获得星级
    int		legend;			 //传说获得星级
};

//CMD_LOGIN_GOLDTESTMODEL_SC
struct LoginGoldTestModelInfo
{
    int nUseTimes;		//挑战次数
    int nUseStamp;		//时间戳
    int nDamage;		//总伤害
    int nChestData;		//宝箱状态
};

//CMD_LOGIN_HEROMODEL_SC
struct HeroTestTimesInfo
{
    int nInstanceId;
    int nTimes;
};
struct LoginHeroTestModelInfo
{
    int nUseStamp;
    int nTimesNum;
    /*nTimesNum * HeroTestTimesInfo*/
};

//CMD_LOGIN_TOWERTESTMODEL_SC
struct LoginTowerTestModelInfo
{
    int floor;			   //当前楼层数
};

struct TowerOuterBonusInfo
{
    int id;
};

//CMD_LOGIN_PVPMODEL_SC
struct LoginPvpModelInfo
{
    int battleId;					//战斗id, 如果战斗id不为0, 请求断线重连
    int rank;						//当前排名
    int integral;					//竞技积分
    int continusWinTimes;			//连续胜场
	int historyHighestRank;			//历史最高排名
	int historyHighestIntegral;		//历史最高积分
    int historyContinusWinTimes;    //历史最高连胜

	//int cpnRank;					//锦标赛排名
	//int cpnWeekResetStamp;			//锦标赛周重置时间
	//int cpnGradingNum;				//锦标赛定级赛场数
	//int cpnGradingDval;				//锦标赛定级赛分差
	//int cpnIntegral;				//锦标赛竞技积分
	//int cpnContinusWinTimes;		//锦标赛段位连续胜场
	//int cpnHistoryHigestRank;		//锦标赛历史最高排名
	//int cpnHistoryHigestIntegral;	//锦标赛历史最高积分
	//int cpnHistoryContinusWinTimes;	//锦标赛历史最高连胜场数

	int dayResetStamp;				//日重置时间
	int dayWinTimes;				//日胜场
	int dayContinusWinTimes;		//日连续胜场数
	int dayMaxContinusWinTimes;		//日最高连胜场数
	int dayBattleTimes;				//日战斗场数
	int dayBuyChestTimes;			//日宝箱购买次数
	int lastChestGenStamp;			//最后宝箱时间
	int chestStatus;				//宝箱状态
	int chestInsurance;				//宝箱保险次数
	int rewardFlag;					//日奖励领取标示符
};

// CMD_LOGIN_PVPTASKMODEL_SC
struct LoginPvpTaskModelInfo
{
	int taskCount;
};

// CMD_LOGIN_PVPCHESTMODEL_SC
struct LoginPvpChestModelInfo
{
    char chestCount;
};

//CMD_LOGIN_SHOPMODEL_SC
struct LoginShopModelInfo
{
    unsigned char cCount;					//商店个数
    //商店数据
};

//商店结构
struct SLoginShopData
{
	unsigned int    nTimes;					//已经刷新的次数
	int				nNextFreshTime;			//下次刷新时间点
    unsigned char	cShopID;				//商店ID
    unsigned char   cCurCount;				//当前物品数
    //商品结构列表
};

//商品结构
struct SLoginShopGoodsData
{
    int				nGoodsID;				//道具ID
    int				nGoodsNum;				//道具个数
	int				nCoinNum;				//价格
	unsigned short	sGoodsShopID;			//商品ID
    unsigned char	cCoinType;				//货币类型
	unsigned char	cIndex;					//商品索引
    unsigned char	cSale;					//折扣值
};

struct SLoginDiamondData
{
	int				nTimeStamp;
	unsigned char	cPid;
};

//钻石商店数据
struct SLoginDiamondShopData
{
	unsigned char cNum;					//已经充值的个数
	//PID+时间戳
};

//CMD_LOGIN_ACTIVEMODEL_SC
struct SLoginActiveSC
{
    unsigned short sCount;				//活动个数
};

struct SLoginActiveData
{
	int			   nStartTime;		//开始时间
	int			   nEndTime;		//结束时间
	unsigned short sActiveID;
    unsigned char  cActiveType;
	unsigned char  cTimeType;		//活动时间类型
    unsigned char  cLevLimit;		//活动等级限制
    //具体数据 SLoginActiveShopData/SLoginActiveTaskData
};

struct SLoginActiveShop
{
    unsigned short sShopNum;				//礼包个数
    //礼包具体数据 SLoginActiveShopData
};

struct SLoginActiveShopData
{
    int nGoodsID[4];
    int nGoodsNum[4];
    int nPrice;
	unsigned char cGiftID;
	unsigned char cGoldType;					//货币类型
    unsigned char cSaleRate;
    unsigned char cMaxBuyTimes;
    unsigned char cBuyTimes;					//已经购买次数
};

struct SLoginActiveTask
{
    unsigned short sActiveTaskNum;				//活动任务数
};

struct SLoginActiveTaskData
{
    int						nConditionParam[2];
    int						nRewardGold;
    int						nRewardGoodsID[4];
    int						nRewardGoodsNum[4];
    int						nValue;					//任务完成进度
	unsigned short			sFinishCondition;
	unsigned short			sRewardDimand;
	unsigned char			cTaskID;
    unsigned char			cFinishFlag;			//是否领取，0-未领取，1-领取
};

struct SLoginActiveMonthCard
{
	unsigned char cNum;					//月卡活动个数
};

struct SLoginActiveMonthCardData
{
	int nMonthCardID;				//月卡ID
	int nRewardTime;				//月卡领取时间
	int nFinishTimestamp;			//月卡充值时间
	unsigned char cType;			//月卡类型(0-永久，1-时限)
};

struct SLoginExChangeActive
{
	unsigned char cNum;
};

struct SLoginAcitveExChangeData
{
	unsigned short sActiveID;
	unsigned char  cTaskID;
	unsigned char cCurTimes;
};

//CMD_LOGIN_BAN_SC
//四字节整数

struct LoginHeadInfo
{
	int nNum;				//头像个数
	//头像ID
};

//CMD_LOGIN_UNIONSHOP_SC
///公会商店结构
struct SLoginUnionShopGoodsData
{
	int				nGoodsID;			    //道具ID
	int			   nCoinNum;				//价格
	unsigned short sGoodsShopID;			//商品ID
	unsigned char cGoodsNum;				//道具个数
	unsigned char cCoinType;				//货币类型
	unsigned char cSale;					//折扣值
};

//公会商店数据
struct SLoginUnionShop
{
	unsigned short sNum;
	unsigned char cFlag;		//0-普通状态，1-正在上架状态，2-首次
	//SLoginUnionShopGoodsData
};

//CMD_LOGIN_7DAYACTIVE_SC
struct SLogin7DayActiveSC
{
	unsigned int Day;			//活动天数
	unsigned short sNum;
	//活动个数
};

struct SLogin7DayActiveData
{
	unsigned short sActiveID;
	unsigned char  cActiveType;
	//具体数据 SLoginActiveShopData/SLoginActiveTaskData
};

//CMD_LOGIN_QQACTIVE_SC
struct SLoginBDActiveSC
{
	int nNum;
	//蓝钻活动数据
};

struct SLoginBDActiveData
{
	int nActiveID;				//活动ID
	int nTaskID;				//任务标记
	int nFlag;					//活动标记
};

// 请求测试验证 CMD_LOGIN_CHECK_CHAT_CS
struct LoginCheckChatCS
{
    int uid;
};

#pragma pack()

#endif //__LOGIN_PROTOCOL_H__
