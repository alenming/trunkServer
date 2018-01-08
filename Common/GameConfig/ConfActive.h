#ifndef _CONF_ACTIVE_H__
#define _CONF_ACTIVE_H__

#include "ConfManager.h"
#include <string>

enum
{
	NONE_ACTIVE_DROP_TYPE,
	GOODSID_ACTIVE_DROP_TYPE,			//道具掉落类型
	TYPE_ACTIVE_DROP_TYPE,				//类型掉落类型
};

enum
{
	NONE_ACTIVE_TYPE,				//活动类型
	SHOP_ACTIVE_TYPE,				//商店活动类型
	DROP_ACTIVE_TYPE,				//掉落活动类型
	TASK_ACTIVE_TYPE,				//任务活动类型
	MONTHCARD_ACTIVE_TYPE,			//月卡活动类型
	EXCHANGE_ACTIVE_TYPE,			//兑换活动类型
};

enum
{
	NONE_TIMEACTIVE_TYPE,			//永久类型
	SERVER_TIMEACTIVE_TYPE,			//服务器启动类型
	ACTOR_ACTIVETIME_TYPE,			//角色启动类型
};

struct SConfActiveTime
{
	int				nActiveType;
	int				nTitleLanguageID;
	int				LvLimit;
	int				nStartTime;
	int				nEndTime;
	std::string		szIcon;
    int             nButonLanguageID;
	int				nTimeType;
};

struct S7DayActiveData
{
	int nActiveType;
	int nActiveID;
};

struct S7DayActive
{
	int nStartDay;
	int nEndDay;
	std::vector<S7DayActiveData> VectActive;
};

//商店类型活动
struct SConfActiveShopKey
{
	int nActiveID;
	int nGiftID;

	bool operator < (const SConfActiveShopKey &Key) const
	{
		if (nActiveID < Key.nActiveID)
		{
			return true;
		}
		else if (nActiveID == Key.nActiveID)
		{
			if (nGiftID < Key.nGiftID)
			{
				return true;
			}
		}

		return false;
	}

};

struct SConfActiveShopData
{
	int nGiftID;
	int nGoodsID[4];
	int nGoodsNum[4];
	int nGoldType;					//货币类型
	int nPrice;
	int nSaleRate;
	int nMaxBuyTimes;

	SConfActiveShopData()
	{
		memset(this, 0, sizeof(*this));
	}
};

//额外加成活动
struct ExtraDropData
{
	int nGoodsID;
	int nNumMinRate;
	int nNumMaxRate;
	float fDropRateAdd;
};

struct SConfActiveExtraAdd
{
	std::string ActivePic;
	int nLanguageID;
	int nDropType;
	std::vector<int>			 VectDropID;
	std::map<int, ExtraDropData> MapDropChange;
	std::map<int, ExtraDropData> MapAddDropExtra;
	std::map<int, ExtraDropData> MapAddDropType;
	int nDropNumMin;
	int nDropNumMax;
};

struct SConfActiveTaskKey
{
	int nActiveID;
	int nTaskID;

	bool operator < (const SConfActiveTaskKey &Key) const
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

struct SConfActiveTaskData
{
	std::string ActiveIcon;
	int			nActiveLanguangeID;
	int			nFinishCondition;
	int			nConditionParam[2];
	int			nRewardDimand;
	int			nRewardGold;
	int			nRewardEnergy;
	int			nRewardGoodsID[4];
	int			nRewardGoodsNum[4];
    std::string ActivePic;
	std::string szGoTo;
	int			nActiveTaskType;

	SConfActiveTaskData()
	{
		ActiveIcon.clear();
		nActiveLanguangeID = 0;
		nFinishCondition = 0;
		memset(nConditionParam, 0, sizeof(nConditionParam));
		nRewardDimand = 0;
		nRewardGold = 0;
		nRewardEnergy = 0;
		memset(nRewardGoodsID, 0, sizeof(nRewardGoodsID));
		memset(nRewardGoodsNum, 0, sizeof(nRewardGoodsNum));
	}
};

struct SConfMonthActiveData
{
	std::string		sMonthPic;
	int				nDayLimit;			//月卡时限(天)
	int				nDiamondReward;		//钻石领取数
	int				nPid;
};

struct SConfExChangeActiveData
{
	int					nTimes;				//兑换次数
	std::vector<int>	VecGoods[2];
	std::vector<int>	VecRewardGoods[2];

	SConfExChangeActiveData()
	{
		nTimes = 0;
		for (int i = 0; i < 2; i++)
		{
			VecGoods[i].clear();
			VecRewardGoods[i].clear();
		}
	}

};

class CConfActiveTime : public CConfBase
{
public :
	CConfActiveTime();
	~CConfActiveTime();

	virtual bool LoadCSV(const std::string& str);
};


class CConfActiveShop : public CConfBase
{
public:
	CConfActiveShop();
	~CConfActiveShop();

	virtual bool LoadCSV(const std::string& str);

	SConfActiveShopData*	GetActiveShopData(int nActiveID, int nGiftID);

	std::map<SConfActiveShopKey, SConfActiveShopData>& GetAllActiveShop() { return m_MapActiveShop; }

	std::vector<SConfActiveShopData>* GetActiveVector(int nActiveID);

    int getShopActiveNum() { return m_MapActiveShopList.size(); }

private:
	std::map<SConfActiveShopKey, SConfActiveShopData>		m_MapActiveShop;
	std::map<int, std::vector<SConfActiveShopData> >		m_MapActiveShopList;
};


class CConfActiveExtraAdd :public CConfBase
{
public:
	CConfActiveExtraAdd();
	~CConfActiveExtraAdd();

	virtual bool LoadCSV(const std::string& str);
};

class CConfActiveTask : public CConfBase
{
public:
	CConfActiveTask();
	~CConfActiveTask();

	virtual bool LoadCSV(const std::string& str);

	int GetTaskActiveCondition(int nActiveID);

	SConfActiveTaskData* GetTaskActiveTaskData(int nActiveID, int nActiveTaskID);

	std::map<int, SConfActiveTaskData>* GetTaskActiveTaskDataByActiveID(int nActiveID);

	std::map<SConfActiveTaskKey, SConfActiveTaskData>& GetAllTaskActiveMap() { return m_MapActiveTask; }

    int getTaskActiveNum() { return m_MapActiveDataByActiveID.size(); }

private:
	std::map<SConfActiveTaskKey, SConfActiveTaskData>	m_MapActiveTask;
	std::map<int, int>									m_MapActiveCondition;
	std::map<int, std::map<int, SConfActiveTaskData> >	m_MapActiveDataByActiveID;
};

class CConf7DayActive : public CConfBase
{
	virtual bool LoadCSV(const std::string& str);
};

class CConfMonthActive : public CConfBase
{
	virtual bool LoadCSV(const std::string& str);
	std::map<SConfActiveTaskKey, SConfMonthActiveData> m_MapActiveMonth;
	std::map<int, std::map<int, SConfMonthActiveData> >	m_MapActiveDataByActiveID;
public:
	SConfMonthActiveData * getConfMonthData(int nActiveID, int nMonthCardID);
	std::map<int, SConfMonthActiveData>* GetActiveMonthDataByActiveID(int nActiveID);
	std::map<SConfActiveTaskKey, SConfMonthActiveData>& GetAllMonthData() { return m_MapActiveMonth; }
};

class CConfExChangeActive : public CConfBase
{
	virtual bool LoadCSV(const std::string& str);

public:
	SConfExChangeActiveData * getConfExChangeData(int nActiveID, int nTaskID);
	std::map<int, SConfExChangeActiveData>* GetActiveExChangeyActiveID(int nActiveID);
	std::map<SConfActiveTaskKey, SConfExChangeActiveData>& GetAllExChangeData() { return m_MapActiveExChange; }

private:
	std::map<SConfActiveTaskKey, SConfExChangeActiveData>		m_MapActiveExChange;
	std::map<int, std::map<int, SConfExChangeActiveData> >		m_MapActiveExChangeByActiveID;
};

////////////////////////////////////////////////////////////////////////////////////////////
inline const SConfActiveTime* queryActiveTimeData(int nActiveID)
{
	//活动总表
	CConfActiveTime *conf = dynamic_cast<CConfActiveTime*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TIME));
	return static_cast<SConfActiveTime*>(conf->getData(nActiveID));
}

inline const SConfMonthActiveData* queryMonthActiveData(int nActiveID, int nMonthCardID)
{
	CConfMonthActive *conf = dynamic_cast<CConfMonthActive*>(
		CConfManager::getInstance()->getConf(CONF_MONTH_ACTIVE));
	return const_cast<const SConfMonthActiveData*> (conf->getConfMonthData(nActiveID, nMonthCardID));
}

inline std::map<int, SConfMonthActiveData>* queryMonthActiveMap(int nActiveID)
{
	CConfMonthActive *conf = dynamic_cast<CConfMonthActive*>(
		CConfManager::getInstance()->getConf(CONF_MONTH_ACTIVE));
	return conf->GetActiveMonthDataByActiveID(nActiveID);
}

inline const SConfExChangeActiveData *queryExChangeActiveData(int nActiveID, int nTaskID)
{
	CConfExChangeActive *pConf = dynamic_cast<CConfExChangeActive*>(
		CConfManager::getInstance()->getConf(CONF_EXCHANGE_ACTIVE));
	return const_cast<const SConfExChangeActiveData*> (pConf->getConfExChangeData(nActiveID, nTaskID));
}

inline std::map<int, SConfExChangeActiveData>* queryExChangeActveMap(int nActiveID)
{
	CConfExChangeActive *conf = dynamic_cast<CConfExChangeActive*>(
		CConfManager::getInstance()->getConf(CONF_EXCHANGE_ACTIVE));
	return conf->GetActiveExChangeyActiveID(nActiveID);
}

#endif //_CONF_ACTIVE_H__
