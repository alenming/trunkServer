#ifndef __CHALLENGE_ROOM_MANAGER_H__
#define __CHALLENGE_ROOM_MANAGER_H__

#include <map>
#include <vector>
#include "ChallengeRoom.h"
#include "GameDef.h"
#include "KXServer.h"
/*
 挑战房间管理器, 每个玩家进行挑战时需要在这地方登记
*/

struct SDelayDelData;
class CChallengeRoomManager : public KxServer::KxTimerObject
{
private:
	CChallengeRoomManager();
	~CChallengeRoomManager();

public:

	static CChallengeRoomManager *getInstance();
	static void destroy();

public:

	bool init(KxServer::KxTimerManager *pTimerManager);

	virtual void onTimer(const KxServer::kxTimeVal& now);

	// 玩家是否可以挑战指定关卡
	bool isStageEnough(int uid, int chapterId, int stageId, int times = 1);
	// 玩家是否可以挑战指定副本
	bool isInstanceEnough(int uid, int activityId, int times = 1);
	// 玩家是否可以挑战金币试练
	bool isGoldTestEnough(int uid, int wday);
	// 玩家是否可以挑战英雄副本
	bool isHeroTestEnough(int uid, int nInstanceId, int nDiff);
	// 玩家是否可以爬塔
	bool isTowerTestEnough(int uid);
	// 获得玩家挑战房间
	CChallengeRoom *getRoom(int uid);
	// 创建挑战房间
	CChallengeRoom *createRoom(int uid, int battleType);
	// 是否有房间
	bool haveRoom(int uid);
	// 真正移除房间
	void removeRoom(int uid);
	//延迟删除房间
	void PushDelRoom(int uid);
	//去除延迟删除数据
	void ReSetDelRoom(int uid);
private:

	static CChallengeRoomManager *	m_pInstance;
	std::map<int, CChallengeRoom *>	m_mapRooms;
	std::map<int, std::list<SDelayDelData>::iterator>	m_MapDelUserList;					//玩家对应的向量迭代器
	std::list<SDelayDelData>							m_DelUserList;						//延迟玩家下线列表
	KxServer::KxTimerManager *		m_TimeManager;
};

#endif //__CHALLENGE_ROOM_MANAGER_H__
