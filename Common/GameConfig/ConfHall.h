#ifndef __CONF_HALL_H__
#define __CONF_HALL_H__

#include "ConfManager.h"
#include "ConfGameSetting.h"

/////////////////////////////////配表数据/////////////////////////////////////////
class AchieveItem
{
public:
	int				ID;					//成就ID
	int				Title;				//成就标题
	int				Desc;				//完成条件描述
	int				UnLockLv;			//解锁等级
	int				Show;				//是否显示
	int				FinishCondition;	//完成条件
	int				CompleteTimes;		//完成次数
	int				Tips;				//未完成提示
	int				AwardExp;			//奖励经验
	int				AwardCoin;			//奖励金币
	int				AwardDiamond;		//奖励钻石
	int				AwardEnergy;		//奖励体力
	int				PosType;			//位置类型
	int				AchieveStar;		//完成后星星
	int				CloseDisplay;		//关闭后是否显示
	std::string		Icon;				//成就图标
	VecInt			FinishParameters;	//条件参数1,2,3
	VecInt			EndStartID;			//结束后启动
	std::vector<ID_Num>	AwardItems;		//奖励道具
};

struct DiffcultItem
{
	int DiffID;
	int MaxLevel;
	int BasicLevel;
	int ExLevel;
	int UnlockLevel;
	VecInt Pic;
};

class ActivityInstanceItem
{
public:
	int ID;					//副本ID
	int Title;				//副本标题
	int Desc;				//副本描述
	int Place;				//排列优先级
	int Show;				//完成后入口图标
	int Type;				//副本类型
	int CompleteTimes;		//完成次数限制
	int BuyTimes;			//购买次数限制
	int RecoverType;		//恢复类型
	int RecoverParam;		//恢复参数
	VecInt PlaceTime;		//入口图标显示时间
	VecInt StartTime;		//副本开始时间
	VecInt EndTime;			//副本结束时间
	VecInt RecoverTime;		//副本次数恢复时间
	std::vector<DiffcultItem> Diffcult; //难度
	std::string Pic;		//入口图标
};

class CardGambleItem
{
public:
	int			ID;			// 卡片ID
	int			Rare;		// 稀有度1白2绿3蓝4紫5金
	int			Star;		// 星级
	int			Rate;		// 概率
	int			Ratio;		// 系数
	//Prob		Probability;	//概率和系数
};

//效果
struct Ability
{
	int						AbilityID;		//显示效果ID
	int						AbilityParam;	//显示效果参数
	int						AbilityDesc;	//显示效果描述
};

//分解材料
struct DecompositMaterial
{
	int		Decomposit;				//装备分解材料,对应的道具ID
	int		DecompositionParam;		//材料数量
};

//合成材料
struct SynthMaterial
{
	int		Synthesis;		//材料ID
	int		SynthesisParam;	//材料数量
};

struct EquipmentEffect
{
	EquipmentEffect()
	: SoliderId(0)
	, SoliderStart(0)
	{
	}

	int SoliderId;              // 此参数用于判断，如果ID相同，穿戴者才可激活天赋
	int SoliderStart;           // 额外激活的星级天赋
	std::map<int, int> Buff;    // BuffId - Buff层数
};

#define MAX_BASE_PROP	8			//基础属性填表最大选项
#define MAX_EXTRA_PROP	8			//备选属性填表最大选项

struct EffectData
{
	int nEffectID;				//效果ID
	int nMinValue;				//效果最小值
	int nMaxValue;				//效果最大值
	int nWeight;				//效果权值

	EffectData()
	{
		//memset(this, 0, sizeof(*this));
	}

};

//装备属性
class EquipmentItem
{
public:
	int								ID;				// 装备ID
	int								Suit;			// 套装ID
	int								Level;			// 装配等级
	int								Parts;			// 装配部位
	int								Gold;			// 装备分解所需金币
    int                             Rank;           // 装备排名
	EquipmentEffect                 ExtEffect;      // 特殊效果
	std::vector<DecompositMaterial>	Decomposit;		// 装备分解材料
	VecInt							Vocation;		// 装配职业
};

//装备生成属性
class EquipPropCreate
{
public:
	int							nEquipID;			//装备ID
	std::vector<EffectData>		VectBaseProp;		//装备基础属性
	std::vector<EffectData>		VectExtraProp;		//装备备选属性

	EquipPropCreate()
	{
		nEquipID = 0;
		VectBaseProp.clear();
		VectExtraProp.clear();
	}

};

//套装属性
class SuitItem
{
public:
	int							    ID;			    // 套装ID
	int							    Name;		    // 套装名称(对应语言包ID)
	int							    Desc;		    // 套餐描述(对应语言包ID)
	std::map<int, int>			    Eq;			    // 套装所需的装备(1武器,2衣服,3裤子,4鞋子,5饰品,6饰品)
	std::map<int, Ability>		    SuitAbility;    // 套装效果, key代表几件, value代表效果参数
	std::map<int, EquipmentEffect>  SuitExtEffect;  // 套装特殊效果
};

//道具
class PropItem
{
public:
	int						ID;						// 道具ID
	int						Name;					// 道具名称 语言包ID
	int						Desc;					// 道具描述 语言包ID
	int						Quality;				// 品质
	int						Type;					// 道具类型
	int						SellPrice;				// 出售价格
	int						UseLevel;				// 最低使用等级
	int						BagLabel;				// 背包界面所属标签
	float					Ratio;					// 掉落系数
	std::string				Icon;					// 道具图标
	VecInt					TypeParam;				// 道具类型参数
	VecVecInt				QuickToStage;			// 快速前往关卡
};

//掉落单项
struct DropIdData
{
	int DropID;
	int DropRate;
	VecInt DropNum;
};

//掉落货币id
struct DropCurrencyData
{
	int CurrencyId;									//货币id
	int UpperLimit;									//货币上限
	int LowerLimit;									//货币下限
};

// 掉落规则
class DropPropItem
{
public:
	int						DropRuleID;			   //掉落规则ID
	int						IsCrit;				   //可否暴击
	int						IsRepeat;			   //可否重复
	VecInt					MeanwhileDropNum;	   //单次掉落种类数量
	VecInt					ExtraDropRuleID;	   //额外掉落
	std::vector<DropCurrencyData> DropCurrencys;   //掉落货币
	std::vector<DropIdData>	DropIDs;			   //掉落ID项

	DropPropItem& operator = (DropPropItem &Item)
	{
		DropRuleID = Item.DropRuleID;
		IsCrit = Item.IsCrit;
		IsRepeat = Item.IsRepeat;
		MeanwhileDropNum.assign(Item.MeanwhileDropNum.begin(), Item.MeanwhileDropNum.end());
		ExtraDropRuleID.assign(Item.ExtraDropRuleID.begin(), Item.ExtraDropRuleID.end());
		DropCurrencys.assign(Item.DropCurrencys.begin(), Item.DropCurrencys.end());
		DropIDs.assign(Item.DropIDs.begin(), Item.DropIDs.end());
		return (*this);
	}
};

//邮件
class MailItem
{
public:
	int	ID;			// 邮件id
	int Topic;		// 邮件主题
	int Sender;		// 发件人
	int Content;	// 邮件内容
	int	LiveTime;	// 保质期(天)
};

//英雄升星属性
class SoldierUpRateItem
{
public:
	int				SoldierID;	           // 英雄ID
	int             DefaultStar;           // 初始星级
	int				TopStar;	           // 升星上限
    int             Source;                // 获取途径
	VecVecInt       QuickToStage;          // 快速前往关卡口ID
};

//购买召唤师属性
class SaleSummonerConfItem
{
public:
	int			ID;				// 召唤师ID
	int			Type;			// 货币种类
	int			Num;			// 货币数量
	int			SummonerMusic;	// 召唤师背景音效名称ID
	int			NewLabel;		// 是否显示new标签
	std::string	Head_Name;		// 角色头像资源
	std::string	Bg_Name;		// 角色背景资源
	std::string Bg_Texture;     // 角色背景纹理
	int			HeadID;			// 头像ID
};

class TaskItem
{
public:
	int				ID;						//任务ID
	int				Title;					//任务标题
	int				Type;					//任务类型
	int				Desc;					//完成条件描述
	int				UnlockLv;				//解锁等级
	int				Show;					//是否显示
	int				FinishCondition;		//完成条件
	int				CompleteTimes;			//需完成次数
	int				Tips;					//未完成提示
	int				AwardExp;				//奖励经验
	int				AwardCoin;				//奖励金币
	int				AwardDiamond;			//奖励钻石
	int				AwardEnergy;			//奖励体力
	int				AwardFlashcard;			//抽卡券
	int				TaskReset;				//重置周期
	std::string		Icon;					//任务图标
	VecInt			QuickTo;				//立即前往
	VecInt			FinishParameters;		//条件参数
	VecInt			EndStartID;				//结束后启动
	VecInt			TaskResetParameters;    //重置周期参数
	std::vector<ID_Num>	AwardItems;			//奖励道具
};

enum EnhanceConditionTypes
{
	EnhanceConditionId = 1,                 // 配置ID
	EnhanceConditionRoleType,               // 角色类型
	EnhanceConditionStar,                   // 星级
	EnhanceConditionCrystalCost,            // 水晶消耗
	EnhanceConditionRace,                   // 种族
	EnhanceConditionSex,                    // 性别
	EnhanceConditionVocation,               // 职业
	EnhanceConditionAttackType,             // 攻击方式
};

enum EnhanceConditionJudgeTypes
{
	Greater = 1,            // 大于 >
	Less,                   // 小于 <
	Equal,                  // 等于 ==
	GreaterEqual,           // 大于等于 >=
	LessEqual,              // 小于等于 <=
};

struct EnhanceCondition
{
	int Type;               //加成作用对象条件
	VecInt Param;           //条件参数
};

struct EnhanceValue
{
	int EffectId;           // 加成属性ID
	int Param;              // 加成属性值
	int EffectLanID;        // 语言包ID
};

class OutterBonusItem
{
public:
	int ID;                                             //战斗外属性加成ID
	int Name;											//名字
	int Desc;											//描述
	std::vector<EnhanceCondition> EnhanceConditions;    //加成作用对象条件列表
	std::vector<EnhanceValue> Enhances;                 //加成效果
	std::string Pic;
	std::string PicS;
};

class GoldTestConfItem
{
public:
	int						WeekNum;				//周几
	int						Stage;					//关卡
	int						StageDesc;				//关卡介绍
	int						Frequency;				//次数
	int						StageLevel;				//等级
	VecFloat				Param;					//参数
};

class GoldTestChestConfItem
{
public:
	int						Level;					//等级
	int						Gold;					//金币
	int						Damage;					//上海
};

class HeroTestItem
{
public:
	int							ID;					//ID
	VecInt						Time;				//时间
	int							Occupation;			//职业
	int							Times;				//次数
	int							Desc;				//描述
	int							UpDesc;				//升级描述
	int							Title;				//标题
	std::vector<DiffcultItem>	Diff;				//难度
	std::string					Pic;				//图片
};

//struct TowerDiffcultInfo
//{
//	int							MaxLevel;			//等级上限
//	int							BasicLevel;			//等级基数
//	int							EXLevel;			//等级系数
//	int							Reward;				//显示积分
//	int                         ExtraStar;          //额外奖励星数
//	int							TCoin;				//塔比
//};

class TowerFloorItem
{
public:
	int								ID;					//楼层数
	int								MaxLevel;			//等级上限
	int								BasicLevel;			//等级基数
	int								EXLevel;			//等级系数
	VecInt							StageID;			//楼层备选关卡ID
	int								Place;				//战斗事件发生的位置
	int								Drop;				//掉落ID
};

struct TowerBuffInfo
{
	int							BuffID;				//战斗外属性加成BuffID
	int							Cost;				//buff消耗星星
};

class TowerBuffItem
{
public:
	int							ID;					//ID
	std::vector<TowerBuffInfo>  Buff;				//Buff
	int							Max;				//楼层上限
	int							Min;				//楼层下线						
};

struct TowerRankInfo
{
	int							ID;
	int							Num;
};

class TowerRankItem
{
public:
	int							ID;					//ID
	VecInt						Rank;				//排序
	std::vector<TowerRankInfo>	Item;				//物品
};

class ArenaRewardItem
{
public:
	int Reward_ID;
	int Reward_Type;
	int WinNum_Text;
	int Award_Coin;
	int Award_Diamond;
	int Award_PvpCoin;
	int Award_Flashcard;
	int Award_Items;
	std::string WinNum_Pic;
	VecInt Type_Parameter;
	VecInt AwardPic;
};


struct ArenaTaskItem
{
	int Task_ID;				   //竞技任务ID			
	int Task_Type;				   //任务类型  0=日战斗场次   1=日累计胜场次   2=日连胜场次
	int Task_Text;				   //任务说明
	int Complete_Times;			   //完成次数
	int Award_Exp;				   //显示奖励经验 填0表示没有，不显示
	int Award_Coin;				   //显示奖励金币 填0表示没有，不显示
	int Award_Diamond;			   //显示奖励钻石 填0表示没有，不显示
	int Award_Energy;			   //显示奖励体力 填0表示没有，不显示
	int Award_PvpCoin;			   //显示奖励竞技币填0表示没有，不显示
	int Award_Flashcard;		   //显示奖励抽卡券
	int DropID;					   //掉落ID
	int IsOpen;					   //是否默认开启
	int IsReset;				   //是否重置
	VecInt Award_Items;			   //显示奖励道具 填[]表示没有，不显示 界面挤不开，最多配2个道具
	VecInt End_StartID;			   //结束后启动任务ID 填[]表示没有要启动的任务
	std::string Task_Pic;		   //任务图标
};

struct AnimationPlayOrderItem
{
	int ResID;
	std::vector<std::vector<std::string> > VecAnimations;
};

struct HallStandingItem
{
	int SpotOrder;              // 位置序号
	Vec2 Position;              // 位置坐标
	int ZOrder;                 // 层级
};

class ShopConfigData
{
public:
	ShopConfigData()
	{
		nShopName = 0;
		nLevLimit = 0;
		nTimeInterval = 0;
		VectType.clear();
		VectNum.clear();
	}

	int nShopName;				//商店名
	std::string strShopIcon;    //商店按钮图片
	int nLevLimit;				//等级限制
	int nTimeInterval;			//刷新时间间隔，分钟
	VecInt VectType;			//商店类型
	std::vector<int> VectNum;		//商店商品个数按PVP段位分
};

class ShopGoodsConfigData
{
public:

	ShopGoodsConfigData()
	{
		nShopID = 0;
		nShopGoodsID = 0;
		nGoodsID = 0;
		nGoodsNum = 0;
		nCoinType = 0;
		nCoinNum = 0;
		nWeight = 0;
		nSale = 0;
		nKind = 0;
		nFreshMinLev = 0;
		nFreshMaxLev = 0;
	}

	int		nShopID;			//所属商店ID
	int		nShopGoodsID;		//商品ID
	int		nGoodsID;			//道具ID
	int		nGoodsNum;			//道具个数
	int		nCoinType;			//货币类型
	int		nCoinNum;			//所需货币数值
	int		nWeight;			//商品权值
	int		nSale;				//商品折扣
	int		nKind;				//商品稀有度
	int		nFreshMinLev;		//刷出最小等级
	int		nFreshMaxLev;		//刷出最大等级
};

class DiamondShopConfigData
{
public:
	int nGoodsID;               // 道具ID
	std::string strPicName;     // 商品图片ID
	int nNameLanID;             // 商品名称语言包ID
	int nDescLanID;             // 商品描述语言包ID
	int nPrice;                 // 购买价格
	int nDiamond;               // 获得的钻石
};

//PVP匹配过程中机器人数据
struct SPvpRobotItem
{
	int nRobotID;				//机器人id
	int nRobotMMR;				//机器人mmr值
	int nMinMMR;				//mmr下限
	int nMaxMMR;				//mmr上限
	int nMinRobotLevel;			//机器人等级下限
	int nMaxRobotLevel;			//机器人等级上限
	VecInt stageIdVec;			//机器人列表
};


//每日签到，每日配置
struct SCheckInDayConfig
{
	int nGoodsID;
	int nGoodsNum;
	int nShowNum;						//显示数目
};

//累计签到配置
struct SConCheckInConfig
{
	int DayNeeds;						//奖励发放需求天数
	int nGoodsID[3];					//奖励道具
	int nGoodsNum[3];					//奖励道具数目
	int nShowNum[3];					//奖励道具显示数目
};

//首冲配置
struct SFirstPayData
{
	std::vector<int> vectGoodsID;
	std::vector<int> vectGoodsNum;
	int nGrowGiftPrice;
	int nGiftDiamonds;
	int nGetTimes;

	SFirstPayData()
	{
		vectGoodsID.clear();
		vectGoodsNum.clear();
		nGrowGiftPrice = 0;
		nGiftDiamonds = 0;
		nGetTimes = 0;
	}
};

//QQ蓝钻活动数据
struct SBDActiveData
{
	int nActiveType;					//活动类型
	int nUserLimit;						//领取用户限制
	int nConditionParam;				//条件参数
	std::vector<int> vectGoodsID;		//活动奖励ID
	std::vector<int> vectGoodsNum;		//活动奖励物品数量

	SBDActiveData()
	{
		nActiveType = 0;
		nUserLimit = 0;
		nConditionParam = 0;
		vectGoodsID.clear();
		vectGoodsNum.clear();
	}
};

//QQ蓝钻活动Key
struct SBDActiveKey
{
	int nActiveID;
	int nTaskID;

	bool operator < (const SBDActiveKey &Key) const
	{
		if (nActiveID < Key.nActiveID)
		{
			return true;
		}
		else if (nActiveID == Key.nActiveID)
		{
			if (nTaskID < Key.nTaskID)
			{
				return true;
			}
		}

		return false;
	}
};

struct STalentData
{
    int TalentID;                       // 天赋id
    int TalentName;                     // 天赋名称
    int TalentDes;                      // 天赋描述
    std::string TalentPic;                      // 天赋资源
    std::map<int, int> OutterBonus;     // 属性加成
    std::vector<ID_Num> BuffId;         // buff加成
};

struct STalentArrangeData
{
    int ArrangeID;                      // 天赋页id(职业天赋、种族天赋固定)
    VecVecInt FloorTalent;              // 天赋层上的天赋 
};

struct SPVPShareData
{
    int ReplayShowCount;                // 回放频道最大显示回放数
    int ReplayShareCD;                  // 回放频道分享CD（秒）
    int ReplayShareCount;               // 回放频道每日分享次数限制
    int BattleShareCount;               // 每日战斗分享次数限制
    int RefreshCD;                      // 观看量刷新间隔（秒）
    int ReplayShowRank;                 // 回放频道低于X名不显示名次
    int ShareDescLength;                // 分享描述字数限制
    int Desc;                           // 默认描述
};

struct SPVPUploadData
{
    int AutoUploadHP;                   // 自动上传的剩余血量标准
    int ApplyRank;                      // 申请上榜最低排名
    int ApplyCount;                     // 当日申请次数限制
    int AutoUploadRank;                 // 自动上传的名次范围
};

////////////////////////////////解析配表//////////////////////////////////////////
class CConfGoldTest : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfGoldTestChest : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfHeroTest : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfTowerFloor : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	int getMaxFloor() { return m_nMaxFloor; }
private:
	int m_nMaxFloor;
};

class CConfTowerBuff : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
#ifdef RunningInServer
	std::vector<int> &getFloorsBuff(int id);
private:
	std::map<int, std::vector<int> > m_mapFloorsBuffId;
#endif 
};

class CConfTowerRank : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfAchieve : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	int getPreAchieveID(int achieveID);

private:
	std::map<int, int> m_mapPreAchieve;  // <当前成就ID, 前置成就ID>
};

class CConfActivityInstance : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

typedef std::map<int, std::vector<CardGambleItem*> > MAP_CARDGAMEBLEITEM;
class CConfCardGamble : public CConfBase
{
public:
    virtual ~CConfCardGamble();
	virtual bool LoadCSV(const std::string& str);

	bool getData(int star, std::vector<CardGambleItem*>& vec)
	{
		MAP_CARDGAMEBLEITEM::iterator iter = m_GardGambleData.find(star);
		if (iter != m_GardGambleData.end())
		{
			vec = iter->second;
			return true;
		}

		return false;
	}

private:
	MAP_CARDGAMEBLEITEM m_GardGambleData;
};

class CConfEquipment : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfEqupmentCreate :public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfProp : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfDropProp : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);

	virtual std::map<int, DropPropItem>& GetShowDropPropItem() { return m_ShowDropPropItem; }

	virtual DropPropItem* GetRealDropPropItem(int nDropID);

	virtual bool ReSetShowDropPropItem();

	virtual bool SetShowDropPropItem(int nDropID, DropPropItem &Item);

protected:
	std::map<int, DropPropItem>  m_ShowDropPropItem;				//可做修改的掉落数据
};

class CConfMail : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfSoldierUpRate : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfSuit : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfSaleSummoner : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfTask : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfOutterBonus : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfArenaReward : public CConfBase
{
public:
	CConfArenaReward();
	~CConfArenaReward();
	virtual bool LoadCSV(const std::string& str);

public:

	ArenaRewardItem *		m_pDayWinItem;				//日累计胜场奖励
	ArenaRewardItem *		m_pDayContinusWinItem;		//日连胜场奖励   
	ArenaRewardItem *		m_pDayBattleItem;			//日战斗场次奖励
	std::vector<ArenaRewardItem *> m_RankRewards;		//公平竞技排行奖励
	std::vector<ArenaRewardItem *> m_CampionRankRewards;		//锦标赛排行奖励
};

class CConfArenaTask : public CConfBase
{
public:
	bool LoadCSV(const std::string& str);
};

class CConfAnimationPlayOrder : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfHallStanding : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CShopGoodsData : public CConfBase
{
	virtual bool LoadCSV(const std::string& str);

public:
	std::list<ShopGoodsConfigData*>* GetShopList(int nShopID);

private:

	std::map<int, std::list<ShopGoodsConfigData*> > m_MapShopID;
};

class CShopData : public CConfBase
{
	virtual bool LoadCSV(const std::string& str);
};

class CConfDiamondShop : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

//每日签到
class CConfDaySign : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);

	SCheckInDayConfig * GetMonthSignDay(int nMonth, int nDay);

private:
	std::map<int, std::map<int, SCheckInDayConfig> > m_MapYearSign;			//一年签到的所有信息
};

//累计签到
class CConfConDaySign : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfFirstPay : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	SFirstPayData * GetFirstPayData() { return &m_FirstPayData; }
protected:
	SFirstPayData	m_FirstPayData;
};

class CConfBDActive : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	std::map<SBDActiveKey, SBDActiveData> & getBDAllData() { return m_MapBDActive; }
	SBDActiveData*		getDBActiveData(int nActiveID, int nTaskID);

protected:
	std::map<SBDActiveKey, SBDActiveData>	m_MapBDActive;
};

class CConfTalent : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
};

class CConfTalentArrange : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
};

class CConfPVPShare : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
    NO_KEY_DATA(SPVPShareData);
};

class CConfPVPUpload : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
    NO_KEY_DATA(SPVPUploadData);
};

////////////////////////////////查询配表//////////////////////////////////////////
// 查询活动副本配表
inline const ActivityInstanceItem* queryConfActivityInstance(int instanceId)
{
	CConfActivityInstance *conf = dynamic_cast<CConfActivityInstance*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVITY_INSTANCE));
	return static_cast<ActivityInstanceItem*>(conf->getData(instanceId));
}

// 查询装备表
inline const EquipmentItem* queryConfEquipment(int equipID)
{
	CConfEquipment *conf = dynamic_cast<CConfEquipment*>(
		CConfManager::getInstance()->getConf(CONF_EQUIPMENT));
	return static_cast<EquipmentItem*>(conf->getData(equipID));
}

//查询装备生成表
inline const EquipPropCreate *queryConfEquipCreat(int equipCreatID)
{
	CConfEqupmentCreate *conf = dynamic_cast<CConfEqupmentCreate*>(
		CConfManager::getInstance()->getConf(CONF_EQUIP_CREATE));
	return static_cast<EquipPropCreate*>(conf->getData(equipCreatID));
}

// 查询套装表
inline const SuitItem* queryConfSuit(int suitId)
{
	CConfSuit* conf = dynamic_cast<CConfSuit*>(
		CConfManager::getInstance()->getConf(CONF_SUIT));
	return static_cast<SuitItem*>(conf->getData(suitId));
}

// 查询道具表
inline const PropItem* queryConfProp(int itemID)
{
	CConfProp *conf = dynamic_cast<CConfProp*>(
		CConfManager::getInstance()->getConf(CONF_ITEM));
	return static_cast<PropItem*>(conf->getData(itemID));
}

// 查询任务表
inline const TaskItem* queryConfTask(int taskID)
{
	CConfTask *conf = dynamic_cast<CConfTask*>(
		CConfManager::getInstance()->getConf(CONF_TASK));
	return static_cast<TaskItem*>(conf->getData(taskID));
}

// 查询成就
inline const AchieveItem* queryConfAchieve(int achieveId)
{
	CConfAchieve* conf = dynamic_cast<CConfAchieve*>(
		CConfManager::getInstance()->getConf(CONF_ACHIEVE));
	return static_cast<AchieveItem*>(conf->getData(achieveId));
}

// 查询购买召唤师数据
inline const SaleSummonerConfItem* queryConfSaleSummoner(int summonerId)
{
	CConfSaleSummoner* conf = dynamic_cast<CConfSaleSummoner*>(
		CConfManager::getInstance()->getConf(CONF_SALESUMMONER));
	return static_cast<SaleSummonerConfItem*>(conf->getData(summonerId));
}

// 查询抽奖概率表
inline const bool queryConfCardGamble(int rare, std::vector<CardGambleItem*> &vec)
{
	CConfCardGamble *conf = dynamic_cast<CConfCardGamble*>(
		CConfManager::getInstance()->getConf(CONF_CARD_GAMBLE));
	return conf->getData(rare, vec);
}

// 查询士兵升星表
inline const SoldierUpRateItem* queryConfSoldierUpRateItem(int soldierID)
{
	CConfSoldierUpRate *conf = dynamic_cast<CConfSoldierUpRate*>(
		CConfManager::getInstance()->getConf(CONF_SOLDIER_UP_RATE));
	return static_cast<SoldierUpRateItem*>(conf->getData(soldierID));
}

// 查询邮件表
inline const MailItem* queryConfMailItem(int mailID)
{
	CConfMail *conf = dynamic_cast<CConfMail*>(
		CConfManager::getInstance()->getConf(CONF_MAIL));
	return static_cast<MailItem*>(conf->getData(mailID));
}

// 查询战斗外属性效果表
inline const OutterBonusItem* queryConfOutterBonusItem(int ID)
{
	CConfOutterBonus *conf = dynamic_cast<CConfOutterBonus*>(
		CConfManager::getInstance()->getConf(CONF_OUTTER_BONUS));
	return static_cast<OutterBonusItem*>(conf->getData(ID));
}

// 查询掉落规则配表
inline const DropPropItem* queryConfDropProp(int dropId)
{
	CConfDropProp *conf = dynamic_cast<CConfDropProp*>(
		CConfManager::getInstance()->getConf(CONF_ITEMDROP));
	return static_cast<DropPropItem*>(conf->GetRealDropPropItem(dropId));
}

//金币试练信息
inline const GoldTestConfItem * queryConfGoldTest(int nWeekDay)
{
	CConfGoldTest * conf = dynamic_cast<CConfGoldTest *>(
		CConfManager::getInstance()->getConf(CONF_GOLD_TEST));
	return static_cast<GoldTestConfItem *>(conf->getData(nWeekDay));
}

// 查询活动副本配表
inline const HeroTestItem* queryConfHeroTest(int instanceId)
{
	CConfHeroTest *conf = dynamic_cast<CConfHeroTest*>(
		CConfManager::getInstance()->getConf(CONF_HERO_TEST));
	return static_cast<HeroTestItem*>(conf->getData(instanceId));
}

//获得爬塔配置
inline const TowerFloorItem *queryConfTowerFloor(int floor)
{
	CConfTowerFloor *conf = dynamic_cast<CConfTowerFloor*>(
		CConfManager::getInstance()->getConf(CONF_TOWER_FLOOR));
	return static_cast<TowerFloorItem*>(conf->getData(floor));
}

// 获得最高楼层数
inline int queryMaxTowerFloor()
{
	CConfTowerFloor *conf = dynamic_cast<CConfTowerFloor*>(
		CConfManager::getInstance()->getConf(CONF_TOWER_FLOOR));
	return conf->getMaxFloor();
}

//获得爬塔buff
inline const TowerBuffItem *queryConfTowerBuff(int id)
{
	CConfTowerBuff *conf = dynamic_cast<CConfTowerBuff*>(
		CConfManager::getInstance()->getConf(CONF_TOWER_BUFF));
	return static_cast<TowerBuffItem*>(conf->getData(id));
}

//获取爬塔一周奖励
inline const TowerRankItem* queryConfTowerRankReward(int nIndex)
{
	CConfTowerRank *conf = dynamic_cast<CConfTowerRank*>(
		CConfManager::getInstance()->getConf(CONF_TOWER_RANK));
	std::map<int, void*> MapData = conf->getDatas();

	for (std::map<int,void*>::reverse_iterator rator = MapData.rbegin(); rator != MapData.rend(); ++rator)
	{
		TowerRankItem * pRewardItem = static_cast<TowerRankItem*>(rator->second);

		if (pRewardItem == NULL || pRewardItem->Rank.size() != 2)
		{
			continue;
		}

		if (nIndex <= pRewardItem->Rank[1] && nIndex >= pRewardItem->Rank[0])
		{
			return pRewardItem;
		}
	}

	return NULL;
}

// 获取金币试炼宝箱配置
inline const GoldTestChestConfItem *queryConfGoldTestChest(int lv)
{
	CConfGoldTestChest *conf = dynamic_cast<CConfGoldTestChest*>(
		CConfManager::getInstance()->getConf(CONF_GOLD_TEST_CHEST));
	return static_cast<GoldTestChestConfItem*>(conf->getData(lv));
}

//获取PVP每日奖励(type 0为公平竞技,1为锦标赛)
inline ArenaRewardItem* queryConfPvpRankReward(int nIndex, int type = 0)
{
	CConfArenaReward *conf = dynamic_cast<CConfArenaReward*>(
		CConfManager::getInstance()->getConf(CONF_ARENA_REWARD));

	std::vector<ArenaRewardItem *> VectPvpRankReward;
	if (0 == type)
	{
		VectPvpRankReward = conf->m_RankRewards;
	}
	else
	{
		VectPvpRankReward = conf->m_CampionRankRewards;
	}

	for (std::vector<ArenaRewardItem *>::reverse_iterator rator = VectPvpRankReward.rbegin(); rator != VectPvpRankReward.rend(); ++rator)
	{
		ArenaRewardItem * pRewardItem = *rator;

		if (pRewardItem == NULL || pRewardItem->Type_Parameter.size() != 2)
		{
			continue;
		}

		if (nIndex <= pRewardItem->Type_Parameter[1] && nIndex >= pRewardItem->Type_Parameter[0])
		{
			return pRewardItem;
		}
	}

	return NULL;
}

inline const ArenaTaskItem * queryArenaTaskItem(int taskId)
{
	CConfArenaTask *pConf = dynamic_cast<CConfArenaTask*>(
		CConfManager::getInstance()->getConf(CONF_ARENA_TASK));
	return static_cast<ArenaTaskItem*>(pConf->getData(taskId));
}

inline const AnimationPlayOrderItem* queryConfAnimationPlayOrder(const int& resID)
{
	CConfAnimationPlayOrder *confPlayOrder = dynamic_cast<CConfAnimationPlayOrder*>(
		CConfManager::getInstance()->getConf(CONF_ANIMATION_PLAY_ORDER));
	return static_cast<AnimationPlayOrderItem*>(confPlayOrder->getData(resID));
}

inline const HallStandingItem* queryConfHallStanding(const int& order)
{
	CConfHallStanding *conf = dynamic_cast<CConfHallStanding*>(
		CConfManager::getInstance()->getConf(CONF_HALL_STANDING));
	return static_cast<HallStandingItem*>(conf->getData(order));
}

//获取某个商店对应的数据列表
inline const std::list<ShopGoodsConfigData*>* queryConfShopList(int nShopID)
{
	CShopGoodsData *pConf = dynamic_cast<CShopGoodsData*>(
		CConfManager::getInstance()->getConf(CONF_SHOP_GOODS));

	return pConf->GetShopList(nShopID);
}

inline const ShopGoodsConfigData * queryConfShopData(int nShopGoodsID)
{
	CShopGoodsData *pConf = dynamic_cast<CShopGoodsData*>(
		CConfManager::getInstance()->getConf(CONF_SHOP_GOODS));

	return static_cast<ShopGoodsConfigData *>(pConf->getData(nShopGoodsID));
}

//签到
inline const SCheckInDayConfig *queryCheckInDaySign(int nMonth, int Days)
{
	CConfDaySign *pConf = dynamic_cast<CConfDaySign*>(
		CConfManager::getInstance()->getConf(CONF_DAYSIGN));
	return reinterpret_cast<SCheckInDayConfig *>(pConf->GetMonthSignDay(nMonth, Days));
}

//累计签到数据
inline const SConCheckInConfig * queryConCheckInSign(int nTimes)
{
	CConfConDaySign *pConf = dynamic_cast<CConfConDaySign*>(
		CConfManager::getInstance()->getConf(CONF_CONDAYSIGN));
	return reinterpret_cast<SConCheckInConfig *>(pConf->getData(nTimes));
}

//首冲
inline const SFirstPayData *queryFirstPayData()
{
	CConfFirstPay * pConf = dynamic_cast<CConfFirstPay*>(
		CConfManager::getInstance()->getConf(CONF_FIRSTPAY_SETING));
	return pConf->GetFirstPayData();
}

//获取天赋信息
inline const STalentData *queryTalentData(int talentId)
{
    CConfTalent * pConf = dynamic_cast<CConfTalent*>(
        CConfManager::getInstance()->getConf(CONF_TALENT));
    return static_cast<STalentData *>(pConf->getData(talentId));
}

//获取天赋页信息
inline const STalentArrangeData *queryTalentArrangeData(int arrangeId)
{
    CConfTalentArrange * pConf = dynamic_cast<CConfTalentArrange*>(
        CConfManager::getInstance()->getConf(CONF_TALENT_ARRANGE));
    return static_cast<STalentArrangeData *>(pConf->getData(arrangeId));
}

#endif
