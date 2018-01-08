#ifndef _ACTIVE_TASK_H__
#define _ACTIVE_TASK_H__

#include "UserActionListener.h"

enum PayType
{
	emCum_PayType,			//总累计充值
	emActive_PayType,		//活动时间内累计充值
	emPoint_PayType,		//指定额度充值
};

class CActiveTask : public IUserActionListener
{
public:
	CActiveTask();
	~CActiveTask();

	bool init(int uid, int nActiveID,int nTaskID);

	virtual bool onAction(int actID, void *data, int len);

	//检测是否当前活动的所有任务都完成
	bool checkAllTaskIsFinish();

protected:
	bool processActiveTaskData(int nActionID,void *data, int len);

	//处理次数事件处理
	bool processTimesEventFinish(int actID,int Param1, int Param2);

	//处理数值增加事件Param1 条件，Param2 增加的数值
	bool processNumEventFinish(int actID,int Param1, int Param2);

	//处理升星等级条件
	bool processLvUpEventFinish(int actID,int Param1);

	//处理玩家属性事件
	bool processParamEvent(int actID);

	//处理玩家充值
	bool processUserPay(int actID,int Param1);

	//处理玩家英雄星级数据
	bool processHeroStarEvent(int actID);

	//处理玩家英雄颜色数据
	bool processHeroColorEvent(int actID);

	//处理玩家英雄等级数据
	bool processHeroLevEvent(int actID);

	//处理玩家在线时间累计
	bool processOLTimeEvent(int actID);

	//处理玩家基金数据
	bool processFund(int actID);

private:
	int						m_uid;
	int						m_ActiveID;
	int						m_TaskID;				//任务ID
};

#endif //_ACTIVE_TASK_H__

