#ifndef _UNION_EXPIDITION_MODEL_H__
#define _UNION_EXPIDITION_MODEL_H__

#include "IDBModel.h"
#include "StorageManager.h"
#include <set>
#include <map>
#include <string.h>

enum 
{
	UNION_EXPIDITION_STAGELIST,			//解锁关卡列表
	UNION_EXPIDITION_BOSS_HPLIST,		//BOSS血量列表
	UNION_EXPIDITION_STAGE_LIST,		//通关伤害最高显示列表
	UNION_EXPIDITION_WORLDID,			//选定的世界ID
	UNION_EXPIDITION_MAPID,				//当前选定地图ID
	UNION_EXPIDITION_FINISHTIME,		//战斗结束时间点
	UNION_EXPIDITION_COLDFINISHTIME,	//冷却结束时间点
	UNION_EXPIDITION_FINISHFLAG,		//是否通关
	UNION_EXPIDITION_REWARDSENDTIME,	//奖励发放时间点
};

struct SUnionStageShowData
{
	int  nDamage;
	int  nHeadID;
	char szName[32];
	unsigned char cLevel;

	SUnionStageShowData()
	{
		memset(this, 0, sizeof(*this));
	}
};

class CUnionExpiditionModel : public IDBModel
{
public:
	CUnionExpiditionModel();
	~CUnionExpiditionModel();

	bool init(int nUnionID);
	bool Refresh();
	//获取当前BOSS血量
	bool getCurExpiditionBossHp(int nIndex,int &BossHp);
	//设置BOSS血量
	bool setCurExpiditionBossHp(int nIndex, int BossHp);
	//清除BOSS血量数据
	bool clearCurExpiditionBossHp(int nIndex = 0);
	//添加完成关卡
	bool addExpiditionFinishIndex(int nIndex);
	//校验关卡是否为完成关卡
	bool checkExpiditionFinishIndex(int nIndex);
	//清除完成关卡
	bool clearExpiditionFinishIndex();
	//获取所有完成关卡
	std::set<int>& getAllFinishIndex() { return m_SetFinishIndex; }
	//获取远征属性
	bool getExpiditionFieldValue(int nIndex, int &nValue);
	//设置远征属性
	bool setExpiditinoFieldVale(int nIndex, int nValue);

	//设置关卡序列显示最高伤害
	bool setStageIndexShowData(int nIndex, SUnionStageShowData &Data);
	bool clearStageIndexShowData();
	bool getStageIndexShowData(int nIndex, SUnionStageShowData &Data);


private:
	//保存BOSS血量
	bool saveBossHpToDB();
	//保存完成关卡数据
	bool saveExpiditionIndexToDB();
	//保存关卡序列显示最高伤害数据
	bool saveStageIndexShowData();
	//清除关卡序列显示最高伤害数据
	bool clearStageIndexDBShowData();
private:
	int					 m_nUnionID;
	Storage *			 m_pStorage;
	std::string			 m_strUnionExpiditionkey;

	std::map<int, int>	m_MapBossHp;
	std::set<int>		m_SetFinishIndex;
	std::map<int, int>	m_MapValue;
	std::map<int, SUnionStageShowData> m_MapDamageShowData;
};


#endif //_UNION_EXPIDITION_MODEL_H__
