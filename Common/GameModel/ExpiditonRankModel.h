#ifndef _EXPIDITION_RANK_MODEL_H__
#define _EXPIDITION_RANK_MODEL_H__

#include "IDBModel.h"
#include "StorageManager.h"
#include <set>
#include <map>
#include <string.h>

struct SExpiditionRankDBData
{
	char szName[32];
	int nSummerID;						//召唤师ID
	int nHeroID[7];						//卡牌ID
	int nStartID[7];					//卡牌星级
	unsigned char cBDType;				//蓝钻类型
	unsigned char cBDLev;				//蓝钻等级

	SExpiditionRankDBData()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SDamageRankShowData
{
	int nIndex;							//排名
	int nDamage;						//伤害值
	char szName[32];					//姓名
	int nSummerID;						//召唤师ID
	int nHeroID[7];						//卡牌ID
	int nStartID[7];					//卡牌星级
	unsigned char cBDType;				//蓝钻类型
	unsigned char cBDLev;				//蓝钻等级

	SDamageRankShowData()
	{
		memset(this, 0, sizeof(*this));
	}

};

class CExpiditonRankModel : public IDBModel
{
public:
	CExpiditonRankModel();
	~CExpiditonRankModel();

	bool init(int nUnionID);
	bool Refresh();

	//添加数据到远征伤害排行榜中
	bool addDamageData(int nUid,int &nDamage,SExpiditionRankDBData &DBData);

	//获取某个远征排行榜数据
	bool getDamageData(int nUid, SDamageRankShowData &DBData);

	//获取排名
	bool getDamageRank(int nUid, int &nRank);

	//设置地图ID
	bool setMapID(int nMapID);

	int getMapID() { return m_nMapID; }

	//清除远征伤害排行榜数据
	bool clearDamageRankData();

	//获取所有显示排行数据
	std::map<int, SDamageRankShowData>& getShowDamageRankData() { return m_MapShowRankData; }

private:
	bool buildDamageRank();


private:
	int										 m_nUnionID;
	int										 m_nMapID;
	Storage *								 m_pStorage;
	std::string								 m_strExpiditionRankkey;
	std::string								 m_strExpiditionRankDataKey;
	std::map<int, SDamageRankShowData>		 m_MapShowRankData;
};


#endif //_EXPIDITION_RANK_MODEL_H__
