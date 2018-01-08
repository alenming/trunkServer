#ifndef __UNIONMERCENARY__MODEL_H
#define __UNIONMERCENARY__MODEL_H

#define MAX_EQUIP_EFFECT_NUM 8

#include "KxLog.h"
#include "IDBModel.h"
#include "Storage.h"
#include "RedisStorer.h"
#include "StorageManager.h"
#include "ModelDef.h"
#include <string>
#include <map>
#include <vector>
#include "ConfAnalytic.h"
#include "GameUserManager.h"
#include <stdio.h>

#define EQUIPMENTSMIN 0
#define EQUIPMENTSMAX 6
#define EQUIPEFFETCMIN 0
#define EQUIPEFFECTMAX 8
#define TALENTMAX 8
#define MAX_MERCENARY_PROP_NUM 119		//存储到数据库单个佣兵的属性条目

using namespace std;

typedef struct
{
	int nUid;					
	int nDispatchTime;
	int nRecallTime;
	int nEmployedTimes;
	SoldierCardData stSoldierCardData;
	SoldierEquip stEquipments[EQUIPMENTSMAX];
						
}UnionMercenaryInfo;

class CUnionMercenaryModel : public IDBModel
{
public:
	CUnionMercenaryModel();
	~CUnionMercenaryModel();

	virtual bool init(int unionID);

	//从数据库中读取公会佣兵信息，正在派遣的和已经召回的放在不同的map中
	virtual bool Refresh();

	//派遣佣兵
	int DispatchMercenary(UnionMercenaryInfo& info);

	//召回佣兵
	bool RecallMercenary(int nMercenaryID);

	//雇佣佣兵
	void EmployedMercenary(int nMercenaryID);

	//从本地和数据库中删除佣兵
	void DelMercenary(int nMercenaryID);

	//清理召回时间超过24小时的佣兵，每天零点定时清理
	void ClearRecalled24HoursMercenary();

	//更新单个佣兵信息
	void UpdateSingleMercenary(int nMercenaryID);

	//获得单个佣兵详细信息
	UnionMercenaryInfo* GetSingleMercenaryInfo(int nMercenaryID);

    //直接数据库中获取佣兵信息
    static bool GetSingleMercenaryInfo(int unionID, int nMercenaryID, UnionMercenaryInfo* info);

	//获得正在派遣中的佣兵列表，key为佣兵ID
	map<int, UnionMercenaryInfo>& GeUniontDispatchMercenaryInfo(){ return m_mapUnionDispatchingMercenaryInfo; }
	
private:
	//封装数据库存储格式和结构相互转化的函数
	static void DBtoSt(string& str, UnionMercenaryInfo& info);
	void StoDB(UnionMercenaryInfo& info, string& str);

	int m_nUnionId;
	string m_strUnionMercenaryKey;
	CRedisStorer* m_pRedisStorer;
	map<int, UnionMercenaryInfo> m_mapUnionDispatchingMercenaryInfo;		//正在派遣中的佣兵
	map<int, UnionMercenaryInfo> m_mapUnionRecalledMercenaryInfo;			//已经召回，但是召回时间没超过24小时的佣兵
};

#endif