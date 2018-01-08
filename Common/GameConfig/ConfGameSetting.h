#ifndef __CONF_GAME_SETTING_H__
#define __CONF_GAME_SETTING_H__

#include "ConfManager.h"
#include <list>

#define NO_KEY_DATA(_DATA_TYPE_)	\
public:								\
	inline _DATA_TYPE_& getData()	\
	{								\
	return m_Data;				\
	}								\
protected:							\
	_DATA_TYPE_ m_Data;				\


/////////////////////////////////配表数据/////////////////////////////////////////

//概率
struct Prob
{
	int		Probability;	//概率
	float	Ratio;			//系数
};

struct CardGambleSettingItem
{
	int					DiamondCardGamblePrice;		// 抽一次单价
	int					DiamondCardGamble10Price;	// 十连抽价格
	int                 exchangeRatio;              // 英雄碎片与金币兑换比
	VecInt				SoldierLvUpScuccessRate;	// 升级成功概率
	VecInt				FirstDrawCard;				// 首抽英雄
	std::map<int, Prob> RareRatios;					// 星级概率;  key: 几星, value: 概率
};

struct IconSettingItem
{
	std::vector<std::string> EqIcon;				// 装备部位图标 武器, 头, 衣服, 鞋子, 饰品, 宝器
	std::vector<std::string> RaceIcon;				// 种族图标 人, 兽, 巨人, 亡灵, 精灵, 龙
	std::vector<std::string> JobIcon;				// 职业图标 坦克, 战士, 射手, 法师, 刺客, 辅助, 卫士
};

class SkillUpRateItem
{
public:
	int					SkillLv;// 等级
	std::map<int, int>	Rate;	// 升级概率 key 几个材料  value 概率
};

//士兵升级所需属性
class SoldierLevelSettingItem
{
public:
	int					SoldierLv; // 士兵等级
	int					LvUpCost;	// 吃经验消耗金币
	int					Exp;		// 所需经验
};

//士兵星级和等级限制 属性
class SoldierStarSettingItem
{
public:
	int				SoldierStar;	// 士兵星级
	int				TopLevel;		// 等级上限
	int				UpStarLevel;	// 升星所需的等级
	int				UpStarCost;	    // 升星消耗金币
	int             TurnCardCount;  // 首次召唤英雄所需碎片数量
	int             TurnFragCount;  // 再次获得转化为碎片数量
    int             UpStarCount;    // 升至该星级所需碎片数量
};

//士兵稀有度边框信息
class SoldierRareSettingItem
{
public:
	int				Rare;			// 稀有度
	std::string		UiRes;			// 对应界面边框资源
	std::string		HeadboxRes;		// 头像界面边框资源
	std::string     UnderboxRes;    // 水晶文字底框资源
	std::string     BorderboxRes;   // 下边框U型花纹资源
	std::string     CircleboxRes;   // 唯一标识资源
	std::string		BigHeadboxRes;	// 大头像界面边框资源
	std::string		HeadboxBgRes;	// 头像背景资源
	std::vector<std::string> JobsIcon;  // 职业图标 1战士、2刺客、3射手、4法师、5辅助、6卫士
	std::string		JobBg;
};

struct TaskActiveSettingItem
{
	int			TaskFinishSound;		// 任务完成音效
	int			AchieveFinishSound;		// 成就完成音效
	std::string MainTaskIcon;			// 主线任务图标
	std::string DailyTaskIcon;			// 每日任务图标
};

//玩家升级经验和购买金币限制属性
class UserLevelSettingItem
{
public:
	int			Level;			// 玩家等级
	int			Exp;			// 升级所需经验
	int         SummonerHP;     // 召唤师血量
	VecInt		BuyCoin;		// 购买金币范围
};

// 活动副本
class TowerTestSettingItem
{
public:
	int	FirstCrystal;           // 爬塔试练初始水晶
	int nStartWeekDay;			// 爬塔周开始第几天
	int nInterval;				// 爬塔持续多长时间		//不能超过一周时间
};

// 章节配置
struct ChapterSettingItem
{
	int NormalLastChapter;  // 普通章节,最后一章ID
	int EliteLastChapter;   // 精英章节,最后一章ID
};

// 聊天配置
struct ChatSettingItem
{
	int ChatUnlockLv;       // 聊天解锁等级
	int RecoverTimes;       // 恢复次数
	int SpeedTimesLimit;    // 每日发言次数限制
	int WordNumLimit;       // 发言字数限制
	int IntervalTime;       // 发言时间间隔 单位：s
	VecInt RecoverTime;     // 发言次数日恢复时间 格式：时-分
};

struct SMercenaryPrize
{
	int TimeProfit;//每15分钟收益
	int MaxTime;//最大获得收益驻守的时间 两两配对
	int EmployedProfit;//每次被雇佣获得收益
	int MaxEmployed;//最大被雇佣获得收益的次数
};

#define MAX_EQUIP_ITEM_INDEX    5

typedef struct
{
	int nPart;
	int nLevel;
	int nVocation;
    int nSynthesisItemID[MAX_EQUIP_ITEM_INDEX];
    int nSynthesisItemCount[MAX_EQUIP_ITEM_INDEX];
	int nGoldSpend[2];
    int nEquipmentCreateID[MAX_EQUIP_ITEM_INDEX];
    int nNormalMakePercent[MAX_EQUIP_ITEM_INDEX];
    int nDelicateMakePercent[MAX_EQUIP_ITEM_INDEX];
}SEquipForCast;

//购买消耗属性
class IncreasePayItem
{
public:
	int	BuyTimes;	        // 购买次数
	int	GoldCost;	        // 购买金币消耗钻石数
	int ChallengeCost;      // 购买挑战次数消耗钻石数
	int TowerTreasureCost;	// 购买爬塔宝箱钻石
	int FreshShopCost;		// 购买商店刷新消耗钻石
    VecInt EnergyCost;      // 购买体力消耗钻石数和对应的体力
	VecInt GoldProb;        // 金币购买概率
	int BagCost;			//背包格子购买消费
};

class ItemCrit
{
public:
	int ItemType;           // 道具类型
	int	ItemQuality;        // 道具品质
	int DoubleCrit;         // 2倍暴击(万分率)
	int FourfoldCrit;       // 4倍暴击(万分率)
};

struct StageSettingItem
{
	int WinMusic;           // 胜利音乐
	int LostMusic;          // 失败音乐
};

class ItemLevelSettingItem
{
public:
	int ItemLevel;          // 品质
	std::string ItemFrame;  // 图标外框
	std::vector<int> Color;	//颜色
};

struct NewPlayerHeroInfo
{
	int heroId;
	int heroStarLv;
};

struct SoundEffectItem
{
    int MissChannel;         //左右音道消失距离
    int VolumeDecayRate;     //音量衰减系数（每多少距离音量减少1个百分点）
};

class NewPlayerItem
{
public:
	int						NewbieType;
	int						HeadId;
	int						UserLv;
	int						UserExp;
	int						Gold;
	int						Diamond;
	int						InitBagCapacity;
	int						MaxBagCapacity;
	int						MaxHeroCapacity;
	int						SummonerInGroup;
	std::vector<int>		SummonerIds;
	std::vector<NewPlayerHeroInfo> Heros;
	std::map<int, int>		Items;
	std::map<int, int>		Equips;
	std::vector<int>        Tasks;
	std::vector<int>        Achieves;
	std::vector<int>		Chapters;
	std::vector<int>		Guides;
	std::vector<int>		Heads;
    std::vector<int>		ChestIds;
    std::vector<int>		RobotIds;
	std::string				UserName;
};

class OutterBonusSetting
{
public:
	int ID;                     // 效果ID
	int AttributeID;            // 作用的属性ID（RoleAttributeToID.csv）
	int Method;                 // 作用的方法（1加减2百分比）
};

class PvpGradingChange
{
public:
	int ChangeMMRLower;			//MMR下限
	int ChangeMMRUpper;			//MMR上限
	int ChangePoint;			//定级变化积分
};

class PvpGradingInitial
{
public:
	int PlayerLevelLower;		//玩家等级下限(大于等于该值)
	int PlayerLevelUpper;		//玩家等级上限(小于该值)
	int StartPoint;				//定级初始化积分
	int StartMMR;				//定级初始化MMR
};

class PvpMatchTime
{
public:
	int MatchOnlineLower;		//匹配人数下限
	int MatchOnlineUpper;		//匹配人数上限
	int MatchTime;				//匹配时间
};

class PvpSetting
{
public:
	int GradingNum;				//定级赛场数
	int CriticalPoint;			//最低积分
	VecInt ArenaDay;			//锦标赛开赛日范围(周几到周几)
	VecInt ArenaTime;			//锦标赛开赛时间点（时+分+时+分）
};

class TimeRecoverItem
{
public:
	int AllTimeReset;
};

struct SSystemHeadIconItem
{
    std::string     IconName;
    int             IconTips;
};

/////////////////////////////////解析配表/////////////////////////////////////////

class CConfCardGambleSetting : public CConfBase
{
public:
	
	virtual bool LoadCSV(const std::string& str);

	NO_KEY_DATA(CardGambleSettingItem);
};
class CConfTowerTestSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfChapterSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	NO_KEY_DATA(ChapterSettingItem);
};

class CConfIconSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	NO_KEY_DATA(IconSettingItem);
};

class CConfSkillUpRateSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfSoldierLevelSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfSoldierStarSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfSoldierRareSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfTaskAchieveSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	NO_KEY_DATA(TaskActiveSettingItem);
};

class CConfUserLevelSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	int GetUserMaxLv(){ return m_nUserMaxLv; }

private:
	int m_nUserMaxLv;
};

class CConfNewPlayerSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfChatSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	NO_KEY_DATA(ChatSettingItem);
};

class CConfIncreasePaymentPrice : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

// 物品暴击
class CConfItemCrit : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfSystemHeadIcon : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	bool hasHeadIcon(int headID);
    const std::map<int, SSystemHeadIconItem>& getAllHeadIcon() { return m_mapHeadIcom; }

private:
    std::map<int, SSystemHeadIconItem> m_mapHeadIcom;   // <headID, {png, tip}>
};

class CConfStageSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	NO_KEY_DATA(StageSettingItem);
};

class CConfItemLevelSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfOutterBonusSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CEquipSet : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);

	int GetPropNum(int nQuality);		//根据品质获取装备属性数目

private:
	std::map<int, int>		m_MapQuality;
};

class CConfPvpGradingChange : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);

	std::vector<PvpGradingChange> &GetGradingChange();

private:
	std::vector<PvpGradingChange> m_vecGradingChange;
};

class CConfPvpGradingInitial : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);

	std::vector<PvpGradingInitial> &GetGradingInitial();

private:
	std::vector<PvpGradingInitial> m_vecGradingInitial;
};

class CConfPvpMatchTime : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	std::vector<PvpMatchTime> &GetMatchTime();

private:
	std::vector<PvpMatchTime> m_vecMatchTime;
};

class CConfPvpSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
public:
	PvpSetting m_PvpSetting;
};

class CConfTimeRecover : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfSoundEffectSetting : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
    NO_KEY_DATA(SoundEffectItem);
};

class CConfSysAutoName : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);

    std::map<int, std::vector<std::string> > getAutoNames(){ return m_mapAutoName; }
private:
    std::map<int, std::vector<std::string> > m_mapAutoName;
};

class CConfUnionMercenaryPrize : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
private:
};

class CConfEquipmentForCast : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
private:
};

/////////////////////////////////查询配表/////////////////////////////////////////

// 查询物品暴击率
inline const ItemCrit* queryConfItemCrit(int type, int quality)
{
	int key = type * 100 + quality;
	CConfItemCrit *conf = dynamic_cast<CConfItemCrit*>(
		CConfManager::getInstance()->getConf(CONF_ITEM_CRIT));
	return static_cast<ItemCrit*>(conf->getData(key));
}

// 查询购买消耗表
inline const IncreasePayItem* queryConfIncreasePay(int buyTimes)
{
	CConfIncreasePaymentPrice *conf = dynamic_cast<CConfIncreasePaymentPrice*>(
		CConfManager::getInstance()->getConf(CONF_INCREASE_PAY));
	return static_cast<IncreasePayItem*>(conf->getData(buyTimes));
}

// 查询玩家升级、金币购买表
inline const UserLevelSettingItem* queryConfUserLevel(int userLv)
{
	CConfUserLevelSetting *conf = dynamic_cast<CConfUserLevelSetting*>(
		CConfManager::getInstance()->getConf(CONF_USER_LEVEL_SETTING));
	return static_cast<UserLevelSettingItem*>(conf->getData(userLv));
}

inline const CardGambleSettingItem& queryConfCardGambleSetting()
{
	CConfCardGambleSetting* conf = dynamic_cast<CConfCardGambleSetting*>(
		CConfManager::getInstance()->getConf(CONF_CARD_GAMBLE_SETTING));
	return conf->getData();
}

inline const SoldierLevelSettingItem* queryConfSoldierLevelSetting(int lv)
{
	CConfSoldierLevelSetting *conf = dynamic_cast<CConfSoldierLevelSetting*>(
		CConfManager::getInstance()->getConf(CONF_SOLDIER_LEVEL_SETTING));
	return static_cast<SoldierLevelSettingItem*>(conf->getData(lv));
}

inline const NewPlayerItem* queryConfNewPlayerItem(int type)
{
	CConfNewPlayerSetting *conf = dynamic_cast<CConfNewPlayerSetting*>(
		CConfManager::getInstance()->getConf(CONF_NEW_PLAYER));
	return static_cast<NewPlayerItem*>(conf->getData(type));
}

inline const SoldierStarSettingItem* queryConfSoldierStarSetting(int star)
{
	CConfSoldierStarSetting *conf = dynamic_cast<CConfSoldierStarSetting*>(
		CConfManager::getInstance()->getConf(CONF_SOLDIER_STAR_SETTING));
	return static_cast<SoldierStarSettingItem*>(conf->getData(star));
}

inline const SoldierRareSettingItem* queryConfSoldierRareSetting(int rare)
{
	CConfSoldierRareSetting *conf = dynamic_cast<CConfSoldierRareSetting*>(
		CConfManager::getInstance()->getConf(CONF_SOLDIER_RARE_SETTING));
	return static_cast<SoldierRareSettingItem*>(conf->getData(rare));
}

// 查询技能升级概率表
inline const SkillUpRateItem* queryConfSkillUpRate(int skillLv)
{
	CConfSkillUpRateSetting *conf = dynamic_cast<CConfSkillUpRateSetting*>(
		CConfManager::getInstance()->getConf(CONF_SKILL_UP_RATE_SETTING));
	return static_cast<SkillUpRateItem*>(conf->getData(skillLv));
}

// 查询战斗外作用属性表
inline const OutterBonusSetting* queryConfOutterBonusSetting(int ID)
{
	CConfOutterBonusSetting *conf = dynamic_cast<CConfOutterBonusSetting*>(
		CConfManager::getInstance()->getConf(CONF_OUTTER_BONUS_SETTING));
	return static_cast<OutterBonusSetting*>(conf->getData(ID));
}

// 查询战斗外作用属性表
inline const TowerTestSettingItem* queryConfTowerSetting()
{
	CConfTowerTestSetting *conf = dynamic_cast<CConfTowerTestSetting*>(
		CConfManager::getInstance()->getConf(CONF_TOWER_SETTING));
	return static_cast<TowerTestSettingItem*>(conf->getData(1));
}

// 查询所有系统头像
inline const std::map<int, SSystemHeadIconItem>& queryConfHeadIcon()
{
	CConfSystemHeadIcon *conf = dynamic_cast<CConfSystemHeadIcon*>(
		CConfManager::getInstance()->getConf(CONF_SYSTEM_HEAD_ICON));
	return conf->getAllHeadIcon();
}

// 查询是否有配置某个头像
inline bool queryConfHasHeadIcon(int headIconID)
{
	CConfSystemHeadIcon *conf = dynamic_cast<CConfSystemHeadIcon*>(
		CConfManager::getInstance()->getConf(CONF_SYSTEM_HEAD_ICON));
	return conf->hasHeadIcon(headIconID);
}

//查询某个品质装备对应属性条数
inline const int queryConfQualityProp(int nQuality)
{
	CEquipSet *conf = dynamic_cast<CEquipSet*>(
		CConfManager::getInstance()->getConf(CONF_EQUIP_QUALITY));
	return conf->GetPropNum(nQuality);
}

inline const TimeRecoverItem* queryConfTimeRecoverSetting()
{
	CConfTimeRecover *conf = dynamic_cast<CConfTimeRecover*>(
		CConfManager::getInstance()->getConf(CONF_TIMERECOVER));
	return static_cast<TimeRecoverItem*>(conf->getData(1));
}

inline const SoundEffectItem& queryConfEffectSetting()
{
    CConfSoundEffectSetting *conf = dynamic_cast<CConfSoundEffectSetting*>(
        CConfManager::getInstance()->getConf(CONF_SOUND_EFFECT_SETTING));
    return conf->getData();
}

inline const IconSettingItem& queryConfIconSetting()
{
    CConfIconSetting *pConf = dynamic_cast<CConfIconSetting*>(
        CConfManager::getInstance()->getConf(CONF_ICON_SETTING));
    return pConf->getData();
}

#endif
