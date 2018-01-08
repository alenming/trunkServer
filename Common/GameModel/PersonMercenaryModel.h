#ifndef __PERSONMERCENARY__MODEL_H
#define __PERSONMERCENARY__MODEL_H

#include "IDBModel.h"
#include "Storage.h"
#include "RedisStorer.h"
#include "StorageManager.h"
#include <string>
#include "KxLog.h"
#include "ModelDef.h"
#include "TimeCalcTool.h"

#define PERSONMAXDISPATCH 100

using namespace std;

class CPersonMercenaryModel : public IDBModel
{
public:
	CPersonMercenaryModel();
	~CPersonMercenaryModel();

	virtual bool init(int uid);
	virtual bool Refresh();
	
	//派遣佣兵，分成不同的field给前端区别位置
	bool DispatchMercenary(int nMercenaryID, int field);

	//召回佣兵
	bool RecallMercenary(int nMercenaryID);

	//增加一个雇佣过的佣兵
	void AddEmployedMercenaryInfo(int nMercenaryID);

	//清空已经雇佣过的佣兵列表
	void ClearAllEmployedMercenaryInfo();

	//清空全部派遣的佣兵信息
	void ClearAllDispatchedMercenary();

	//判断佣兵是处于哪个field,1号位是全部玩家都有，2号位是VIP等级4以上才能派遣
	int GetMercenaryField(int nMercenaryID);

	//得到用户派遣的佣兵列表，key为用户的field，值为佣兵ID
	map<int, int>& GetPersonDispatchMercenaryInfo(){ return m_mapPersonDispatchMercenaryInfo; }		

	//得到用户雇佣过的佣兵列表
	set<int>& GetPersonEmployedMercenaryInfo()
	{ 
		//必须重新从数据库取，防止数据不一致
		m_pRedisStorer->GetSetAll(m_strEmployedMercenaryKey, m_setPersonEmployedMercenaryInfo); 
		return m_setPersonEmployedMercenaryInfo; 
	}

	static void SetOffRemoveMercenaryInfo(int nUid, std::map<int, int>& MemrcenaryInfo);

private:
	CRedisStorer* m_pRedisStorer;
	int m_nUid;
	string m_strDispatchingMercenaryKey;
	string m_strEmployedMercenaryKey;
	//键表示几号位，值表示佣兵ID
	map<int, int> m_mapPersonDispatchMercenaryInfo;
	set<int> m_setPersonEmployedMercenaryInfo;
};

#endif