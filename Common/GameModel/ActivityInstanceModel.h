#ifndef __ACTIVITY_INSTANCE_MODEL_H__
#define __ACTIVITY_INSTANCE_MODEL_H__

#include "IDBModel.h"
#include <map>

#if KX_TARGET_PLATFORM == KX_PLATFORM_LINUX
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string>
#endif

enum AcitivityInstanceField
{
	ACTV_FD_NONE = 0,	
	ACTV_FD_USETIMES,
	ACTV_FD_USESTAMP,
	ACTV_FD_BUYTIMES,
	ACTV_FD_BUYSTAMP,
	ACTV_FD_EASY,
	ACTV_FD_NORMAL,
	ACTV_FD_DIFFICULT,
	ACTV_FD_HELL,
	ACTV_FD_LEGEND,
};

struct DBActivityInstanceInfo
{
	int		useTimes;
	int		useStamp;
	int		buyTimes;
	int		buyStamp;
	int		easy;
	int		normal;
	int		difficult;
	int		hell;
	int		legend;
};

class Storage;
class CInstanceModel : public IDBModel
{
public:
	CInstanceModel();
	~CInstanceModel();

public:

	bool init(int uid);

	bool Refresh();

	bool SetInstanceInfo(int activityId, int field, int value);

	bool SetInstanceInfo(int activityId, DBActivityInstanceInfo &infos);

	bool GetInstanceInfo(int activityId, int field, int &value);

	bool GetInstanceInfo(int activityId, DBActivityInstanceInfo &infos);

	bool DeleteInstanceKey();

	std::map<int, DBActivityInstanceInfo>& GetAllInstanceInfo() { return m_mapActivityInfo; }

private:

	bool CheckActivity(int activityId);

	std::string GetActivityField(int activityId, int field);

private:

	int										m_nUid;					//玩家id
	Storage *								m_pStorage;				//storage
	std::string								m_strActivityKey;		//活动副本key
	std::map<int, DBActivityInstanceInfo>	m_mapActivityInfo;		//<difficulty, star>
};

#endif //__ACTIVITY_INSTANCE_MODEL_H__
