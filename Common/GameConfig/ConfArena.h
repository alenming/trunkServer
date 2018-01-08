#ifndef __CONF_ARENA_H__
#define __CONF_ARENA_H__

#include "ConfManager.h"

// pvp段位信息
struct ArenaRankItem
{
	int ArenaLevel;				//阶段等级
	float MMR_K;				//MMR_K值
	float MMR_kx;				//MMR_kx值
	float Arena_K;				//Arena_K值
	std::string GNPic;			//段位图片素材
	VecInt GNRank;				//积分上下限
	VecInt MMR_Range;			//MMR上下限
};

class CConfArenaRank : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

// pvp训练师配置
class CConfArenaTraining : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);

	std::map<int, int>& getArenaTrainings()
	{
		return m_mapArenaTraining;
	}

private:
	std::map<int, int> m_mapArenaTraining;
};

// 匹配机器人信息配置
struct SArenaRobotItem
{
	int index;						  // id
	int robotType;					  // 机器人类型, 1简单, 2普通, 3高级
	VecInt robotAP;					  // 机器人积分差值
	VecInt robotLevel;				  // 机器人等级差值
	VecInt stageID;					  // 实际关卡
	VecInt robotTime;				  // 虚拟匹配秒数
};

class CConfArenaRobot : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);

	std::vector<SArenaRobotItem*> m_RobotEasy;
	std::vector<SArenaRobotItem*> m_RobotNormal;
	std::vector<SArenaRobotItem*> m_RobotAdvance;
};

// 机器人名字与头像配置
struct ArenaRobotNameItem
{
	int IndexId;
	std::string robotName;
};

class CConfArenaRobotName : public CConfBase
{
public:
	virtual ~CConfArenaRobotName();
	virtual bool LoadCSV(const std::string& str);

	std::vector<ArenaRobotNameItem *>& getRobotItems()
	{
		return m_vecPvpRobotName;
	}

private:
	std::vector<ArenaRobotNameItem *> m_vecPvpRobotName;
};

// 机器人滚动名字
class ArenaScollNameItem
{
public:
	int ComputerID;                // 电脑ID
	int ComputerName;              // 电脑名(语言包)
	std::string ComputerPic;       // 电脑召唤师头像
};

class CConfArenaScrollBar : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

// 胜利掉落
struct ArenaWinDrop
{
	VecInt WinLevelLimit;
	VecInt DropId;
};

// pvp设置配表
struct ArenaSettingItem
{
	int ChestPrice; 
	int ChestHalfPriceNum;
	int ActivityChestRate;
	int ChestRefreshTime;
	int LimitWaitingTime;
	int CriticalPoint;
	VecInt Low;
	VecInt Mid;
	VecInt High;
	VecInt LowTime;
	VecInt MidTime;
	VecInt HighTime;
	VecInt LowWaitingTime;
	VecInt MidWaitingTime;
	VecInt HighWaitinghTime;
	VecInt WinTimesLimit;
	VecInt ChestOrders;
	std::vector<ArenaWinDrop> WinDrops;
};

class CConfArenaSetting : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

// 宝箱配置
struct ArenaChestItem
{
	int ChestId;				//宝箱id
	int ChestType;				//宝箱类型 1普通 2活动
	int ChestLevel;				//宝箱T级 7个等级
	int ChestQuality;			//品质
	int ChestWeight;			//权重
	int ChestName;				//宝箱名称
	int ChestDesc;				//宝箱描述
	int ChestDropId;			//宝箱掉落
	std::string ChestIcon1;		//宝箱关闭图标
	std::string ChestIcon2;		//宝箱开启图标
	VecInt ChestAppearLevel;	//出现等级上下限
};

class CConfArenaChest: public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);

	const std::vector<ArenaChestItem*>& getAreanChest(int type, int dan);

private:

	std::map<int, std::map<int, std::vector<ArenaChestItem*> > > m_Chests;
};

// 获取竞技场模式解锁等级
class CConfArenaLevel : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	
	int getUnlockLevel(int mode);

	std::map<int, int>& getArenaLevels() 
	{ 
		return m_mapArenaLevel; 
	}

private:
	std::map<int, int> m_mapArenaLevel;
};

inline const ArenaRobotNameItem *randConfRobotName()
{
	CConfArenaRobotName *pConf = dynamic_cast<CConfArenaRobotName*>(
		CConfManager::getInstance()->getConf(CONF_ARENA_ROBOTNAME));
	std::vector<ArenaRobotNameItem *> &datas = pConf->getRobotItems();
	if (datas.empty())
	{
		return NULL;
	}
	int index = rand() % datas.size();
	return reinterpret_cast<ArenaRobotNameItem *>(datas[index]);
}

inline const SArenaRobotItem *queryConfArenaRobotItem(int robotId)
{
	CConfArenaRobot *pConf = dynamic_cast<CConfArenaRobot*>(
		CConfManager::getInstance()->getConf(CONF_ARENA_ROBOT));

	return reinterpret_cast<SArenaRobotItem*>(pConf->getData(robotId));
}

inline const SArenaRobotItem *randRobotWithLevel(int level)
{
	CConfArenaRobot *pConf = dynamic_cast<CConfArenaRobot*>(
		CConfManager::getInstance()->getConf(CONF_ARENA_ROBOT));
	int index = 0;
	if (level == 0)
	{
		if (pConf->m_RobotEasy.empty())
		{
			return NULL;
		}
		index = rand() % pConf->m_RobotEasy.size();
		return pConf->m_RobotEasy[index];
	}
	else if (level == 1)
	{
		if (pConf->m_RobotNormal.empty())
		{
			return NULL;
		}
		index = rand() % pConf->m_RobotNormal.size();
		return pConf->m_RobotNormal[index];
	}
	else
	{
		if (pConf->m_RobotAdvance.empty())
		{
			return NULL;
		}
		index = rand() % pConf->m_RobotAdvance.size();
		return pConf->m_RobotAdvance[index];
	}
}

inline const ArenaSettingItem *queryArenaSetting()
{
	CConfArenaSetting *pSettingConf = dynamic_cast<CConfArenaSetting*>(
		CConfManager::getInstance()->getConf(CONF_ARENA_SETTING));
	return reinterpret_cast<ArenaSettingItem *>(pSettingConf->getData(0));
}

inline const ArenaChestItem* queryConfArenaChestItem(int cheseId)
{
	CConfArenaChest *pConf = dynamic_cast<CConfArenaChest*>(
		CConfManager::getInstance()->getConf(CONF_ARENA_CHEST));
	return static_cast<ArenaChestItem*>(pConf->getData(cheseId));
}

inline int queryPvpChestPrice(int count)
{
	CConfArenaSetting *pConf = dynamic_cast<CConfArenaSetting*>(
		CConfManager::getInstance()->getConf(CONF_ARENA_SETTING));
	ArenaSettingItem *pSettingItem = reinterpret_cast<ArenaSettingItem *>(pConf->getData(0));
	if (count < pSettingItem->ChestHalfPriceNum) 
	{
		return pSettingItem->ChestPrice / 2;
	}
	return pSettingItem->ChestPrice;
}

inline const ArenaRankItem* queryConfArenaRankItem(int level)
{
	CConfArenaRank *conf = dynamic_cast<CConfArenaRank*>(
		CConfManager::getInstance()->getConf(CONF_ARENA_RANK));
	return static_cast<ArenaRankItem*>(conf->getData(level));
}


#endif //__CONF_ARENA_H__
