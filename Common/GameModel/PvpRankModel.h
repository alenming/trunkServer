#ifndef __PVPRANKMODEL_H__
#define __PVPRANKMODEL_H__

#include "IDBModel.h"
#include "Storage.h"
#include "RedisStorer.h"
#include "KXServer.h"
#include <string>
#include <map>

#define MAX_FLAG_NUM 50

struct SPvpRankKey
{
	int nUid;
	int nScore;
	int nMMR;

	bool operator < (const SPvpRankKey &RankKey) const
	{
		if (nScore > RankKey.nScore)
		{
			return true;
		}
		else if (nScore == RankKey.nScore)
		{
			if (nMMR > RankKey.nMMR)
			{
				return true;
			}
			else if (nMMR == RankKey.nMMR)
			{
				if (nUid < RankKey.nUid)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		
		return false;
	}
};

struct SPvpRankData
{
	std::string UserName;
	int			nHeadID;
	std::string UnionName;
	int			nLevel;
	unsigned char cBDType;		//蓝钻类型
	unsigned char cBDLev;		//蓝钻等级	
};

class CPvpRankModel : public IDBModel
{
public:
	static CPvpRankModel* getInstance();

	static void destroy();

	bool init(KxServer::KxBaseServer *pBaseServer = NULL);

	bool Refresh();

	//添加角色到排行榜中
	bool AddPvpRank(int uid, int nScore);

	//添加竞标赛到排行榜中
	bool addChampionRank(int uid, int nScore);

	//获取角色排行
	//Day 标识获取哪天的排行，如果为0，则是现行排行
	bool GetPvpRank(int id, int &Rank,int Day = 0);

	//获取锦标赛排行
	bool getChampionRank(int id, int &Rank, int Day = 0);

	//保存PVP前一天的排行数据
	bool bakPvpData();

	//保存锦标赛周一时的排行数据
	bool bakChampionRank();

	//清空锦标赛数据
	bool clearChampionRank();

	//获取角色积分
	bool GetRankData(int id, int &Score,int Day = 0);

	//获取所有PVP排行数据
	std::map<SPvpRankKey, SPvpRankData>& GetShowPvpRankMap() { return m_ShowPvpRank; }

	//获取竞标赛排行榜数据
	std::map<SPvpRankKey, SPvpRankData>& getShowChampionMap() { return m_ShowChampionRank; }

	bool setPvpRewardFlag(int uid, int nDay);

	bool getPvpRewardFlag(int uid, int nDay);

	bool cleanPvpRewardFlag(int uid, int nStartDay);	
	
	bool BuildPvpRank();

	bool BuildChampionRank();

private:

	CPvpRankModel();
	~CPvpRankModel();

	void clearData();

private:

	std::string										m_PvpRankKey;
	std::string										m_ChampionKey;			//竞标赛Key
	Storage*										m_pStorage;				//数据库
	std::map<SPvpRankKey, SPvpRankData>				m_ShowPvpRank;			//下发PVP排行榜数据
	std::map<SPvpRankKey, SPvpRankData>				m_ShowChampionRank;		//下发竞标赛排行榜数据
	static CPvpRankModel *							m_pInstance;
	KxServer::KxBaseServer *						m_pServer;				//服务器对象指针
	int												m_CurPvpRankCount;		//当前排行榜人数
	int												m_CurPvpMinScore;		//当前排行榜最小人数
	int												m_CurChampRankCount;	//当前竞技排行榜人数
	int												m_CurChampMinScore;		//当前竞技排行榜最小人数
	int												m_CurGetIndex;			//操作次数延迟
	//已经结束的
	std::set<int>									m_ChampionIDSet;		//锦标赛列表
};

#endif //__PVPRANKMODEL_H__
