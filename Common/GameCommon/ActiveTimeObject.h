#ifndef _ACTIVE_TIME_OBJECT_H__
#define _ACTIVE_TIME_OBJECT_H__

#include "KXServer.h"

class CActiveTimeObject :public KxServer::KxTimerObject
{
public:
	CActiveTimeObject(int ActiveID);
	~CActiveTimeObject();

	// 在计划的时间到达时会被触发
	virtual void onTimer(const KxServer::kxTimeVal& now);

	//设置定时器
	virtual void setTimer(KxServer::KxTimerManager *pTimerManager, int sec, bool bUseFull);

	//设置活动是否生效
	void setUseFull(bool bUseFull) { m_bUseFull = bUseFull; }

	//该活动是否生效
	bool isUseFull() { return m_bUseFull; }


protected:
	int							m_nActiveID;					//加成活动ID
	bool						m_bUseFull;						//是否生效
	KxServer::KxTimerManager*	m_pTimeManager;					//时间管理对象
};


#endif //_ACTIVE_TIME_OBJECT_H__
