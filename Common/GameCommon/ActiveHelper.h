#ifndef _ACTIVE_HELPER_H__
#define _ACTIVE_HELPER_H__

#include "KXServer.h"
#include "ConfHall.h"
#include "ActiveTimeObject.h"

struct SConfActiveExtraAdd;

class CActiveHelper
{
public:
	static CActiveHelper * getInstance();
	static void destroy();

	bool init(KxServer::KxBaseServer *pServer);

	//活动加成计算
	bool activeAddAccount();

	//添加活动计时器对象
	bool addActiveTimeObject(int nActiveID, int sec, bool bUseFull = false);

	//添加永久活动对象
	bool addForeverActiveObject(int nActiveID);

	//删除活动计时器对象
	bool delActiveTimeObject(int nActiveID);

	//初始化监听活动任务事件
	bool initActiveTask(int uid);

	//初始化监听7天活动任务事件
	bool init7DayActiveTask(int uid);

	//检测启用的活动是否生效
	bool isUseFull(int uid, int nActiveID);

	//检验7天活动是否生效
	bool is7DayUseFull(int uid, int nActiveID);

	//获取活动时限
	int getActiveTimeInterval(int uid,int nActiveID);

protected:
	//计算对应相应掉落ID活动数据的影响
	bool propDropData(SConfActiveExtraAdd *pDropData);

protected:
	CActiveHelper();
	~CActiveHelper();

private:
	std::map<int, CActiveTimeObject*>	m_MapActiveTimeObject;			//即时活动
	KxServer::KxBaseServer *			m_pServer;
	static CActiveHelper*				m_pInstance;
};


#endif //_ACTIVE_HELPER_H__
