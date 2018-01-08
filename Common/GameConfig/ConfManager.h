/*
* 配置文件管理类
* 提供所有配置文件的加载和管理
* 提供配置对象的基类
*
* 2015-1-5 By 宝爷
*/
#ifndef __CONF_MANAGER_H__
#define __CONF_MANAGER_H__

#include "KxCSComm.h"
#include "CommTools.h"
#include "CsvLoader.h"
#include "ConfAnalytic.h"

// 辅助方法
inline void fillIntVec(int times, CCsvLoader& loader, VecInt& out)
{
	int value = 0;
	for (int i = 0; i < times; ++i)
	{
		value = loader.NextInt();
		if (0 != value)
		{
			out.push_back(value);
		}
	}
}

inline void fillFloatVec(int times, CCsvLoader& loader, VecFloat& out)
{
	float value = 0.0f;
	for (int i = 0; i < times; ++i)
	{
		value = loader.NextFloat();
		if (-0.00000001f > value || 0.00000001f < value)
		{
			out.push_back(value);
		}
	}
}

// 配置文件的类型
enum ConfType
{
	CONF_BASE,                  // 配置基础偏移值，无实际意义
	CONF_AIDATA,                // AI
	CONF_BUFF,                  // Buff
	CONF_BULLET,                // 子弹
	CONF_CALL,                  // 召唤
	CONF_CAMERA,                // 相机
	CONF_AUDIO,                 // 音效配置
	CONF_CARD_COUNT,            // 卡片结算
	CONF_COUNT,                 // 结算
	CONF_CRYSTAL,               // 水晶
	CONF_HERO,                  // 英雄
	CONF_SKILL,                 // 技能
	CONF_SOLDIER,               // 战士
	CONF_BOSS,					// BOSS
	CONF_MONSTER,				// MONSTER
	CONF_STATUS,                // 状态
	CONF_UI_EFFECT,             // UI
	CONF_EFFECT,                // 特效
	CONF_RESPATH,               // 资源路径
	CONF_ROLERES,				// 英雄对应资源
	CONF_ANIMATION_PLAY_ORDER,	// 点击动画播放顺序
	CONF_STAGE,					// 关卡配置
	CONF_STAGE_SCENE,           // 关卡资源配置
	CONF_CHAPTER,				// 章节配置
	CONF_MAP,					// 地图配置
	CONF_PVPSCENE,				// pvp场景配表
	CONF_SEARCH,				// 搜索
	CONF_EQUIPMENT,				// 装备
	CONF_SUIT,					// 套装
	CONF_ITEM,					// 道具
	//CONF_DROP,					// 关卡掉落
	CONF_SALESUMMONER,			// 召唤师购买表
	CONF_SOLDIER_UP_RATE,		// 英雄升星
	CONF_INCREASE_PAY,			// 购买消耗 (购买金币或体力消耗钻石数)
	CONF_TASK,					// 任务
	CONF_ACHIEVE,				// 成就
	CONF_STORYTRIGGER,			// 剧情触发器
	CONF_STORY,					// 剧情
	CONF_MAIL,					// 邮件
	CONF_NEW_PLAYER,            // 新玩家配置信息
	CONF_ACTIVITY_INSTANCE,		// 活动副本
	CONF_BAG_SETTING,			// 背包相关
	CONF_CARD_GAMBLE,	        // 抽卡英雄表
	CONF_CARD_GAMBLE_SETTING,	// 升级卡片+抽卡概率
	CONF_ICON_SETTING,			// 头像相关
	CONF_SKILL_UP_RATE_SETTING,	// 技能升级比率
	CONF_SOLDIER_LEVEL_SETTING,	// 玩家升级相关
	CONF_SOLDIER_STAR_SETTING,	// 玩家升星相关
	CONF_SOLDIER_RARE_SETTING,	// 士兵稀有度相关
	CONF_TASK_ACHIEVE_SETTING,	// 任务,成就相关
	CONF_USER_LEVEL_SETTING,	// 玩家等级相关
	CONF_TOWER_SETTING,			// 活动副本配置
	CONF_CHAPTER_SETTING,       // 关卡最后一关配置
	CONF_ITEM_CRIT,             // 道具暴击
	CONF_SYSTEM_HEAD_ICON,      // 系统头像
	CONF_STAGE_SETTING,         // 关卡配置音乐
	CONF_ITEM_LEVEL_SETTING,    // 道具品质外框
	CONF_CHAT_SETTING,          // 聊天配置
	CONF_OUTTER_BONUS_SETTING,  // 战斗外作用属性配置
	CONF_OUTTER_BONUS,          // 战斗外加成效果
	CONF_ITEMDROP,				// 物品掉落
    CONF_UNION_LEVEL,           // 公会等级相关
    CONF_UNION,                 // 公会配置相关
    CONF_UNIONEXPIDTION,		// 公会远征
    CONF_UNIONEXPIDTION_MAP,    // 公会远征地图
    CONF_UNIONBADGE,            // 公会会徽

	CONF_PROP_LAN,				// 道具描述文字
	CONF_STAGE_LAN,				// 关卡描述文字
	CONF_UI_LAN,				// ui描述文字
	CONF_BMC_LAN,				// BossMonsterCall 描述文字
	CONF_BMC_SKILL_LAN,		    // BossMonsterCall_Skill 描述文字
	CONF_HS_LAN,				// HeroSoldier 描述文字
	CONF_HS_SKILL_LAN,			// HeroSoldier_Skill 描述文字
	CONF_STORY_LAN,				// 剧情文字
	CONF_TASK_LAN,				// 任务文字
	CONF_ACHIEVE_LAN,			// 成就文字
	CONF_ROLE_ATTRIBUT_LAN,     // 角色属性文字
	CONF_ERROR_CODE_LAN,        // errorCode 文字
	CONF_LOADING_TIPS_LAN,       // loading提示文字

	CONF_RICH_TEXT,				// 富文本
	CONF_GOLD_TEST,				// 金币试炼
	CONF_GOLD_TEST_CHEST,		// 金币宝箱
	CONF_HERO_TEST,				// 英雄试炼
	CONF_TOWER_FLOOR,
	CONF_TOWER_BUFF,
	CONF_TOWER_RANK,
	CONF_ROLE_ZOOM,             // 角色界面缩放
	CONF_HALL_STANDING,         // 大厅界面站位
	CONF_EQUIP_QUALITY,			// 装备品质
	CONF_EQUIP_CREATE,			// 装备生成
	CONF_SHOP_GOODS,			// 商店商品表
	CONF_SHOP,					// 商店表
	CONF_FEATURE,				// 英雄特性

	CONF_GUIDE,					// 引导
	CONF_GUIDE_STEP,			// 引导步骤
	CONF_UI_NODE,				// UI节点
	CONF_UI_STATUS,				// UI状态
	CONF_GUIDE_BATTLE,          // 战斗引导

	CONF_SHOP_DIAMOND,          // 钻石商店
	CONF_TIMERECOVER,			// 体力恢复配表
	CONF_BG_MUSIC,              // 背景音乐配置
	CONF_BG_MUSIC_SETTING,      // 场景背景音乐配置
	CONF_ACTIVE_TIME,			// 活动期限
	CONF_ACTIVE_SHOP,			// 活动商店
	CONF_ACTIVE_DROP,			// 活动掉落
	CONF_ACTIVE_TASK,			// 活动任务
	CONF_7DAY_ACTIVE,			// 7天活动表
	CONF_BLUEDIAMOND_ACTIVE,	// 蓝钻活动配置
	CONF_MONTH_ACTIVE,			// 月卡类型活动
	CONF_EXCHANGE_ACTIVE,		// 兑换活动类型
	CONF_BUTTON_EFFECT,			// 按钮音效
	CONF_SOUND_EFFECT,			// UI音效路径
	CONF_SOUND_EFFECT_SETTING,  // 战斗音效焦点

	CONF_DAYSIGN,				// 每日签到表
	CONF_CONDAYSIGN,			// 累计签到数据
    CONF_SYSAUTONAME,           // 随机名字库

	CONF_FIRSTPAY_SETING,		// 首冲配置
	CONF_UNION_MERCENARY,		// 公会佣兵奖励配置
	CONF_EQUIP_CAST,			// 装备打造
	CONF_UNIONSHOPDROP,			// 公会商店掉落

	CONF_TALENT,			    // 具体天赋内容
	CONF_TALENT_ARRANGE,        // 天赋页

	CONF_PVP_GRADING_CHANGE,	// 定级赛积分变化
	CONF_PVP_GRADING_INITIAL,	// 定级赛初始积分
	CONF_PVP_MATCH_TIME,		// 匹配时间
	CONF_PVP_SETTING,			// pvp参数配置
	CONF_ARENA_TASK,			// 竞技场任务
	CONF_ARENA_LEVEL,			// 竞技场解锁
	CONF_ARENA_TRAINING,		// 竞技场训练场
	CONF_ARENA_CHEST,           // pvp宝箱
	CONF_ARENA_SETTING,			// pvp基础配表 包括匹配信息
	CONF_ARENA_ROBOTNAME,		// 机器人头像表
	CONF_ARENA_ROBOT,			// 机器人配置
	CONF_ARENA_RANK,            // 竞技段位
	CONF_ARENA_SCORLLBAR,		// 竞技场匹配界面滚动图片
	CONF_ARENA_REWARD,          // 竞技奖励

    CONF_PVP_SHARE,             // pvp分享配置
    CONF_PVP_UPLOAD,            // pvp上传配置
};

// 配置文件的基类
class CConfBase
{
public:
	virtual ~CConfBase();

	virtual bool LoadCSV(const std::string& str)
	{
		return false;
	}

	inline void* getData(int dataId)
	{
		std::map<int, void*>::iterator iter = m_Datas.find(dataId);
		if (iter != m_Datas.end())
		{
			return iter->second;
		}

		return NULL;
	}

	void addData(int dataId, void* pData)
	{
		void* data = getData(dataId);
		if (!data)
		{
			m_Datas[dataId] = pData;
		}
	}

	inline std::map<int, void*>& getDatas()
	{
		return m_Datas;
	}

	// 加载csv文件,跳过几行(-1)
	bool loadCsv(CCsvLoader& csvLoader, const std::string& csvFile, int skipRow)
	{
		if (skipRow <= 0 || "" == csvFile)
		{
			return false;
		}

		if (!csvLoader.LoadCSV(csvFile.c_str()))
		{
			return false;
		}

		// 注释备注,跳过
		for (int i = 0; i < skipRow; i++)
		{
			if (!csvLoader.NextLine())
			{
				return false;
			}
		}

		return true;
	}

protected:
	std::map<int, void*> m_Datas;
};

// 配置文件管理单例
class CConfStrToID;
class CConfManager
{
private:
	CConfManager();
	virtual ~CConfManager();

public:
	static CConfManager* getInstance();

	static void destroy();

	// 批量加载所有的配置文件
	// 加载成功返回true，加载失败返回false并打印日志
	bool init();

	inline CConfBase* getConf(int confId)
	{
		std::map<int, CConfBase*>::iterator iter = m_Confs.find(confId);
		if (iter != m_Confs.end())
		{
			return iter->second;
		}

		return NULL;
	}

	bool addConf(int confId, const std::string& csvFile, CConfBase* conf);

	int getFunID(const std::string& str);
	int getStateID(const std::string& str);
	int getRoleAttributeID(const std::string& str);

    CConfStrToID* getStateNameToIDConf()
    {
        return m_pStateNameToID;
    };

private:
	static CConfManager* m_Instance;
	std::string m_szConfRoot;
	CConfStrToID* m_pFunNameToID;
	CConfStrToID* m_pStateNameToID;
	CConfStrToID* m_pRoleAttributeToID;
	std::map<int, CConfBase*> m_Confs;
};

#endif
