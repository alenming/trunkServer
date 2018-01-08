#ifndef _BLUDE_DIAMOND_MODEL_H__
#define _BLUDE_DIAMOND_MODEL_H__

#include "IDBModel.h"
#include "Storage.h"
#include <map>

//蓝钻活动类型
enum eBDActiveType
{
	eBDActive_NoneType,					//无
	eBDActive_DailyType,				//每日礼包
	eBDActive_NewPType,					//新手礼包
	eBDActive_IncrLvType,				//升级礼包
	eBDActive_CPayType,					//续费礼包
};

//蓝钻类型
enum eBDType
{
	eBDNormalType = 0x1,			//蓝钻类型
	eDBYearType = 0x2,				//年费类型
	eDBVipType = 0x4,				//豪华类型
};

enum eBDUserType
{
	eBDNoneUserType,				//普通用户
	eBDNormalUserType = 1,			//普通蓝钻用户
	eBDNYearUserType = 2,			//蓝钻年费用户
	eBDLuxuryType = 3,				//豪华蓝钻用户
	eBDLuxuryYType = 4,				//豪华年费蓝钻用户
};

class CBlueDiamondModel : public IDBModel
{
public:
	CBlueDiamondModel();
	~CBlueDiamondModel();

	virtual bool init(int uid);

	virtual bool Refresh();

	//获取QQ蓝钻活动数据
	bool getQQActiveValue(int nActiveID, int nField, int &nIndex, bool bNew = false);

	//设置QQ蓝钻活动数据
	bool setQQActiveValue(int nActiveID, int nField, int nIndex);

	//重置QQ蓝钻活动数据
	bool reSetQQActiveValue();

private:

	bool getActiveData(int nActiveID, int nTaskID);

	std::map<int, std::map<int, int> >		m_MapQQIndexNum;					//QQ活动任务对应的领取状态
	int										m_Uid;
	Storage*								m_pStorage;							// 数据库
};

#endif 
