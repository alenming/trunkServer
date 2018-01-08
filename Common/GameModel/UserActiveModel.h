#ifndef _USER_ACTIVE_MODEL_H__
#define _USER_ACTIVE_MODEL_H__

#include "IDBModel.h"
#include "Storage.h"
#include <map>

class CUserActiveModel : public IDBModel
{
public:
	CUserActiveModel();
	~CUserActiveModel();

	virtual bool init(int uid);

	virtual bool Refresh();

	//设置商店活动数据
	bool SetShopActiveValue(int nActiveID,int nTimeInterval,int nField, int nValue);

	//获取商店活动数据
	bool GetShopActiveValue(int nActiveID, int nField, int &nValue,bool bNew = false);

	//设置任务活动领取标识
	bool SetTaskActiveIndex(int nActiveID,int nTimeInterval, int nField, int nIndex);

	//设置任务活动数据
	bool SetTaskActiveValue(int nActiveID, int nTimeInterval, int nField, int nValue);

	//获得任务活动的所有数据
	bool GetTaskActiveIndex(int nActiveID, int nField, int &nIndex, bool bNew = false);

	//获得某个活动的参数数据
	bool GetTaskActiveValue(int nActiveID, int nField, int &nValue, bool bNew = false);

	//获取某个月卡的参数数据
	static bool getMonthCardValue(int uid,int nPid, int &nValue);

private:

	//获取普通活动数据
	bool getNormalActiveData();

	//获取7天活动数据
	bool get7DayActiveData();

	bool getActiveData(int nActiveID,int nActiveType);

	//获取角色创建时间
	bool getActiveCreatTime();


private:
	std::map<int, std::map<int,int> >		m_MapShopNum;						//活动商店对应的个人数据活动ID-礼包ID-购买次数
	std::map<int, std::map<int, int> >		m_MapTaskNum;						//活动任务对应的领取状态
	std::map<int, std::map<int, int> >		m_MapTaskValue;						//活动任务对应的值 任务条件-值
	std::map<int, int>						m_MapMonthCard;						//月卡对应Pid对应过期时间
	int										m_Uid;
	Storage*								m_pStorage;							// 数据库
	int										m_nCreatTime;						// 角色创建时间
};


#endif //_USER_ACTIVE_MODEL_H__
