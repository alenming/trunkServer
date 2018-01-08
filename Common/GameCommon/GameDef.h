#ifndef __GAMEDEF_H__
#define __GAMEDEF_H__

//#define GAME_QQ_BLUEDIAMOND			//蓝钻版本开关

#define DEFAULT_EQUIPREF		1000000			//装备最小唯一id
#define ENERGY_RECOVER_TIME		300		        //体力恢复时间 5分钟=300秒
#define DAYSECOND				86400			//日秒数
#define WEEKSECOND				604800			//周秒数
#define ROOM_PACKAGE_LEN		4096			//定义最大包包长
#define MAX_CHEST_NUM			5				//最大pvp箱子个数

//延迟处理数据
#define CHECK_TIME_INTERVAL 300		//校验时间间隔
#define DELAY_TIME_NUM	3600
#define INFINITY_NUM   0x7FFFFFFF	//定义无限次数，实际为有符号整数最大值

#define DEFAULT_USER_ID 10000			//用户最小ID

#define MONTH_CHECK_NUM 26				//满签最高天数
#define MAX_MAIL_NUM 50					//最多邮件封数
#define FOND_PID 9						//基金购买项
#define MONTH_PID 7						//月卡PID

#define MAX_RANK_NUM	10000			//排行榜最大数
#define MAX_RANK_SHOW_NUM 50			//下发显示排行榜数据
#define MAX_TOWERRANK_INDEX 9999999		//爬塔排行榜进入时间索引递减

//货币
enum EMoneyType
{
	MONEY_NON,                            // 
	MONEY_DIAMOND,                        // 钻石
	MONEY_GOLD,                           // 金币
};

//抽卡方式
enum ELotteryType
{
	LOTTERY_1,                            // 抽一次(可能是免费)
	LOTTERY_10,                           // 抽十次
};

//道具类型, 配表相关
enum PropType
{
	PROPTYPE_NONE = 0,			//无
	PROPTYPE_EQUIP,				//装备
	PROPTYPE_EQUIPMATERIAL,		//装备材料
	PROPTYPE_HEROCARD,			//英雄卡片
	PROPTYPE_SUMMONERCARD,		//召唤师卡片
	PROPTYPE_EXPBOOK,			//经验书
	PROPTYPE_SKILLBOOK,			//技能书
	PROPTYPE_GOLDBAG,			//金币袋子
	PROPTYPE_ENERGYBAG,			//体力袋子
	PROPTYPE_EXPBAG,			//金币袋子
	PROPTYPE_DIAMONDBAG,		//钻石袋子
	PROPTYPE_TREASURE,			//宝箱
	PROPTYPE_MATERIAL,			//材料
	PROPTYPE_EQUIP_CREATE,		//装备生成类型
	PROPTYPE_RESOURCE,			//资源类型,金币/钻石等
	PROPTYPE_HEROFRAGMENT,		//英雄碎片
	PROPTYPE_HEAD,				//头像资源类型
};

//资源ID
enum ResourceID
{
    RESOURCEID_GOLD = 1,					//金币
    RESOURCEID_DIAMOND = 2,					//钻石
	RESOURCEID_PVPCOIN = 3,					//PVP币
	RESOURCEID_TOWER = 4,					//爬塔币
	RESOURCEID_ENERGY = 5,					//体力
	RESOURCEID_UNION = 6,					//公会贡献
	RESOURCEID_EXP = 7,						//经验
	RESOURCEID_xxxxx= 8,				    //(未使用)
	RESOURCEID_FLASHCARD10 = 9,				//10抽卡劵
	RESOURCEID_FLASHCARD = 10,				//抽卡劵
};

// 稀有度
enum CARDRARITY
{
	RARITY_NONE = 0,		//稀有度
	RARITY_WHITE,			//白
	RARITY_GREEN,			//绿
	RARITY_BLUE,			//蓝
	RARITY_PURPLE,			//紫
	RARITY_GOLDEN,			//金
};

//道具使用类型
enum ItemUseType
{
	USETYPE_UNUSE = 0,		//不可使用
	USETYPE_BAGUSE,			//背包使用
	USETYPE_ATONCE,			//立即使用
	USETYPE_EXPTOCARD,		//卡牌吞噬使用
	USETYPE_TOEQUIP,		//装备合成使用
};

//队伍模型
enum ETeamType
{
	ETT_PASE,				// 通关队伍
	ETT_SPORTE				// 竞技队伍
};

//关卡挑战结果
enum ChallengeResult
{
	CRESULT_CANCEL = -1,
	CRESULT_LOSE = 0,
	CRESULT_WIN = 1,
};

//章节类型
enum ChapterType
{
	CHAPTERTYPE_NONE,		//无
	CHAPTERTYPE_NORMAL,		//普通章节
	CHAPTERTYPE_ELITE,		//精英章节
};

//关卡类型
enum StageType
{
	STAGETYPE_REWARDONE = 0,  //常规奖励1个
	STAGETYPE_REWARDMULTI,    //常规奖励多个（包含普通、精英和副本关卡）
	STAGETYPE_TOWER,		  //爬塔试练（包含普通、精英和副本关卡）
	STAGETYPE_GOLD,			  //金币试练
	STAGETYPE_HERO,			  //英雄试练
    STAGETYPE_UNION,          //公会(任务)挑战
};

//恢复类型
enum TimesRecoverType
{
	RCVTYPE_PERSECOND = -2,	//每隔n秒恢复一次
	RCVTYPE_OPEN,			//开启时恢复
	RCVTYPE_DONTRECOVER,	//不恢复
	RCVTYPE_DAY,			//日恢复
	RCVTYPE_WEEK,			//周恢复
};

//难度类型
enum DifficultyType
{
	DIFFICULTY_EASY = 0,	//简单难度
	DIFFICULTY_NORMAL,		//普通难度
	DIFFICULTY_DIFFICULT,	//困难难度
	DIFFICULTY_HELL,		//地狱难度
	DIFFICULTY_LEGEND,		//传说难度
};

//
enum TowerDiffculty
{
	TOWERDIFFCULTY_NONE = 0, //无
	TOWERDIFFCULTY_1,		 //难度1
	TOWERDIFFCULTY_2,		 //难度2
	TOWERDIFFCULTY_3,		 //难度3
};

// 任务重置类型
enum EResetTaskType
{
	ERTT_NON,                   // 不重置
	ERTT_DAY,                   // 日重置
	ERTT_WEEK,                  // 周重置
};

// 任务状态(状态类任务,由前端计算)
enum ETaskStatus
{
	ETASK_UNACTIVE = -1,       // 未激活
	ETASK_ACTIVE,              // 激活/接受状态
	ETASK_FINISH,              // 完成(可领取)
	ETASK_GET,                 // 已经领取
};

enum EDayLiveness
{
    UNION_DAY_PVPLIVENESS,
    UNION_DAY_STAGELIVENESS,
};

// 公会转发的数据
enum EUnionForwardData
{
    UNION_FORWARD_PVPLIVENESS,         // 今日pvp活跃度
    UNION_FORWARD_STAGELIVENESS,       // 今日关卡活跃度
    UNION_FORWARD_USERLV,              // 玩家等级
    UNION_FORWARD_USERNAME,            // 玩家名字
    UNION_FORWARD_CONTRIBUTION,        // 玩家贡献
};

enum EUnionMessageType
{
    UNION_MESSAGE_LIVENESS,            // 活跃度 
    UNION_MESSAGE_AUDIT,               // 审核
	UNION_MESSAGE_MERCENARYDISPATCH,   // 公会佣兵派遣
	UNION_MESSAGE_MERCENARYRecall,	   // 公会佣兵召回
};

// 关卡任务类型
enum EStageTaskType
{
    STAGE_TASK_TYPE_NON = -3,		// 无
    STAGE_TASK_TYPE_ELITE = -2,		// 精英关卡
    STAGE_TASK_TYPE_NORMAL,			// 普通关卡
    STAGE_TASK_TYPE_ANY,			// 任意关卡
};

enum EPvpDan
{
	DAN_NEWBIE = 1,					//新手段位
	DAN_BRONZE,						//青铜段位
	DAN_SILVER,						//白银段位
	DAN_GOLD,						//黄金段位
	DAN_PLATINUM,					//白金段位
	DAN_DIAMOND,					//钻石段位
	DAN_KING,						//王者段位
};

enum EPVPRewardType
{
	PRT_DAYWINTIMES = 0,			//日累计胜场奖励 
	PRT_DAYCONTINUSWINTIMES,		//日连胜场奖励
	PRT_DAYRANK,					//日排名奖励
	PRT_NEWHIGHEST,					//历史新高奖励
	PRT_DAYBATTLETIMES,				//日战斗场次奖励
};

//商店购买货币类型
enum EBUYCOINTYPE
{
	BUY_GOLD_TYPE = 1,				//金币购买类型
	BUY_PVP_TYPE,					//竞技币
	BUY_TOWER_TYPE,					//塔币
	BUY_DIAMOND_TYPE,				//钻石币
	BUY_UNION_TYPE,					//公会币
};

// 邮件状态
enum EMailStatus
{
    MAIL_STATUS_UNREAD,				// 未读
    MAIL_STATUS_READ,				// 已读
};

//商店刷新类型
enum EFRESHTYPE
{
	EAUTOFRESH_TYPE,				//自动刷新
	EMANNALFRESH_TYPE,				//手动刷新
};

// 邮件类型
enum EMailType
{
    MAIL_TYPE_NORMAL,				// 普通邮件(活动、背包不足)
    MAIL_TYPE_WEB,					// web邮件(手动填写标题、内容等)
};

// 邮件配置ID
enum EMailConfID
{
    MAIL_CONFID_NON,                // 
    MAIL_CONFID_FULLBAG,            // 背包已满
    MAIL_CONFID_RESETTASK,          // 任务重置
	MAIL_CONFID_PVPRANK,			// 竞技场排名
    MAIL_CONFID_UNIONKICK,          // 公会踢人
    MAIL_CONFID_UNIONDISMISS,       // 公会解散
	MAIL_CONFID_TOWERRANK,			// 爬塔排行
};

//PVP胜利失败标识
enum EPvpBattleFlag
{
	EPVP_NONE_BATTLE_FLAG,
	EPVP_WIN_BATTLE_FLAG,			//胜利标识
	EPVP_FAILED_BATTLE_FLAG,		//失败标识
	EPVP_DRAW_BATTLE_FLAG,			//平局标识
};

enum EClientLoginType
{
	NORMAL_LOGIN_TYPE,				//默认手机登陆
	WEIXIN_LOGIN_TYPE,				//微信登陆
};

enum MailFiledType
{
	MAIL_NONE_TYPE,		//无
	MAIL_BASE_TYPE,		//基础类型
	MAIL_ADDGOODS_TYPE,	//附加类型
	MAIL_CONTEXT_TYPE,  //邮件内容
	MAIL_GMID_TYPE,		//GM邮件状态
};

enum EActiveTaskType
{
	ACTIVE_TASK_NORMAL_TYPE,			//普通任务类型
	ACTIVE_TASK_GROWUP_TYPE,			//成长任务类型
};

// 道具品质      
enum EItemQuality
{
    EITEM_QUALITY_WHITE = 1,         // 白
    EITEM_QUALITY_GREEN,             // 绿
    EITEM_QUALITY_BLUE,              // 蓝
    EITEM_QUALITY_PURPLE,            // 紫
    EITEM_QUALITY_GOLD,              // 金
    EITEM_QUALITY_ORANGE,            // 橙
    EITEM_QUALITY_PLATINA,           // 白金
};

enum ENoticeID
{
    // 系统类
    NOTICE_SYSTEM,
    NOTICE_SYSTEM_CHAMPIONSHIP_START,         // 锦标赛开始
    NOTICE_SYSTEM_END,
    // 公会类
    NOTICE_UNION,
    NOTICE_UNION_NEWMEMBER = NOTICE_SYSTEM_END, // 新成员加入
    NOTICE_UNION_QUIT,                        // 成员退出
    NOTICE_UNION_APPOINT,                     // 任命
    NOTICE_UNION_TRASFER,                     // 转职
    NOTICE_UNION_NOTICEUPDATE,                // 公会更新
    NOTICE_UNION_EXPIDITION_SET,              // 远征设置
    NOTICE_UNION_EXPIDITION_START,            // 远征开始
    NOTICE_UNION_EXPIDITION_SUCCESS,          // 远征胜利
    NOTICE_UNION_EXPIDITION_FAIL,             // 远征失败
    NOTICE_UNION_EXPIDITION_AWARD,            // 远征奖励
    NOTICE_UNION_END,
    // 玩家类
    NOTICE_USER,
    NOTICE_USER_DRAWCARD_PURPLE = NOTICE_UNION_END, // 抽卡获得紫卡
    NOTICE_USER_DRAWCARD_GOLD,                // 抽卡获得金卡
    NOTICE_USER_FORGE_PURPLE,                 // 打造获得紫装
    NOTICE_USER_FORGE_GOLD,                   // 打造获得金装
};

enum ETalentID
{
    // 种族天赋
    TALENT_HUMAN = 1,                      // 人族
    TALENT_NATURE,                         // 自然
    TALENT_GHOST,                          // 亡灵
    
    // 职业天赋
    TALENT_SOLIDER = 101,                  // 战士
    TALENT_ASSASSIN,                       // 刺客
    TALENT_SHOOT,                          // 射手
    TALENT_MAGIC,                          // 法师
    TALENT_ASSIST,                         // 辅助
    TALENT_BODYGUARD,                      // 卫士
};

//充值
enum PAYBACK_TYPE
{
	PAYBACK_INVALID_TYPE = -1,	//充值异常
	PAYBACK_FAILE_TYPE,		//充值失败
	PAYBACK_SUCESS_TYPE,	//充值成功
};

#endif  //__GAMEDEF_H__
