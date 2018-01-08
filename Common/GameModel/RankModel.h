#ifndef _RANK_MODEL_H__
#define _RANK_MODEL_H__

#include <string>
#include <list>
#include <map>

#include "IDBModel.h"
#include "Storage.h"
#include "RedisStorer.h"

#define TOWER_LEVEL_POSITIONNUM 100000000  //数据中最高塔层所在位数

//除了PVP排行榜以外其他排行榜
enum RankType
{
	NONE_RANK_TYPE,
	LEVEL_RANK_TYPE,				//等级排行榜
	UNION_RANK_TYPE,				//公会排行榜
	TOWER_RANK_TYPE,				//爬塔排行榜
	MAX_RANK_TYPE,
};

struct SLevelRankKey
{
	int nLevel;
	int nUid;

	bool operator < (const SLevelRankKey &RankKey) const
	{
		if (nLevel > RankKey.nLevel)
		{
			return true;
		}
		else if (nLevel == RankKey.nLevel)
		{
			if (nUid < RankKey.nUid)
			{
				return true;
			}
		}

		return false;
	}

};

struct SLevelRankData
{
	int nHeadID;
	std::string UserName;
	std::string UnionName;
	unsigned char cBDType;		//蓝钻类型
	unsigned char cBDLev;		//蓝钻等级
};

struct SUnionRankKey
{
	int nUnionID;
	int nUnionLevel;

	bool operator < (const SUnionRankKey &RankKey) const
	{
		if (nUnionLevel > RankKey.nUnionLevel)
		{
			return true;
		}
		else if (nUnionLevel == RankKey.nUnionLevel)
		{
			if (nUnionID < RankKey.nUnionID)
			{
				return true;
			}
		}

		return false;
	}
};

struct SUnionRankData
{
	std::string UnionName;
	int			nUnionNum;
	int			nEmblemID;		//公会会徽
	unsigned char cBDType;		//蓝钻类型
	unsigned char cBDLev;		//蓝钻等级
};

struct STowerRankKey
{
	int nScore;
	int nMaxTowerLevel;
	int nUid;

	bool operator < (const STowerRankKey &RankKey) const
	{
		if (nScore > RankKey.nScore)
		{
			return true;
		}
		else if (nScore == RankKey.nScore)
		{
			if (nMaxTowerLevel > RankKey.nMaxTowerLevel)
			{
				return true;
			}
			else if (nMaxTowerLevel == RankKey.nMaxTowerLevel)
			{
				if (nUid < RankKey.nUid)
				{
					return true;
				}
			}
		}

		return false;
	}
};

struct STowerRankData
{
	int nHeadID;
	std::string UserName;
	std::string UnionName;
	int nLevel;
	unsigned char cBDType;		//蓝钻类型
	unsigned char cBDLev;		//蓝钻等级
};

class CRankModel : public IDBModel
{
public:

	static CRankModel* getInstance();

	static void destroy();

	virtual bool init();				//排行榜模型是在服务器启动时即有，因而需要将其模型单列化
	virtual bool Refresh();

	//增加数据类型
	bool AddRankData(int RType, int id, int Score);

	//获取角色排行
	bool GetRankData(int RType, int id, int &Score);

	//删除排行数据
	bool DelRankData(int RType);

	bool getUserRankID(int uid, int& value);

	//获取等级排行数据
	std::map<SLevelRankKey, SLevelRankData>& GetLevelRankData() { return m_LevelShowRankMap; }

	//获取公会排行数据
	std::map<SUnionRankKey, SUnionRankData>& GetUnionRankData(){ return m_UnionShowRankMap; }

	//获取爬塔排行数据
	std::map<STowerRankKey, STowerRankData>& GetTowerRankData() { return m_TowerShowRankMap; }
	
	bool BuildLevelRank();

	bool BuildTowerRank();

	bool BuildUnionRank();

	bool setRankRewardFlag(int uid, int RType, int nFlag);

	bool getRankRewardFlag(int uid, int RType, int nFlag);

protected:
	CRankModel();
	~CRankModel();

    void clearData();

private:
	static CRankModel *							m_pInstance;
	Storage*									m_pStorage;				//数据库
	std::map<SLevelRankKey, SLevelRankData>		m_LevelShowRankMap;		//下发显示等级排行
	std::map<STowerRankKey, STowerRankData>		m_TowerShowRankMap;		//下发显示塔层排行
	std::map<SUnionRankKey, SUnionRankData>		m_UnionShowRankMap;		//下发显示
	int											m_CurRankCount[MAX_RANK_TYPE-1];			//当前排行榜人数
	int											m_CurMinScore[MAX_RANK_TYPE-1];				//当前排行榜最小分值
	int											m_CurGetIndex[MAX_RANK_TYPE-1];				//操作次数延迟
	int											m_CurTimeCount;							//进榜计数值
};


#endif //_RANK_MODEL_H__
