#ifndef __NEW_MATCH_MANAGER_H__
#define __NEW_MATCH_MANAGER_H__

#include <map>
#include <vector>

#include "KxCommInterfaces.h"
#include "KxTimerManager.h"

// 负载状态
enum MatchStatus
{
	MATCHSTATUS_LOW = 0,
	MATCHSTATUS_MID,
	MATCHSTATUS_HIGH,
};

// 匹配玩家信息
struct NewMatcher
{
	int uid;			//玩家id
	int startStamp;		//开始的匹配时间
	int curLevelStamp;	//入队列匹配时间
	int userLv;			//玩家等级
	int integral;		//积分
	int matchLevel;		//当前匹配的T级
    int totalWin;       //总胜场
	NewMatcher *prev;
	NewMatcher *next;
};

// 等级从高到低排序, 等级相同积分从高到低排序
struct NewMatcherList
{
	bool reverse;		//是否反向
	int prevMatchStamp;	//上次匹配时间
	NewMatcher *head;
	NewMatcher *tail;
};

class CNewMatchManager : public KxServer::KxTimerObject
{
private:
	CNewMatchManager();
	~CNewMatchManager();

public:
	static CNewMatchManager *getInstance(); 
	static void destroy();

public:
	// 初始化
	bool init();
	// 匹配机器人
	bool insertRobot(NewMatcher *matcher);
	// 插入到对应等级
	bool insert(int level, NewMatcher *matcher);
	// 玩家是否存在
	bool isExsit(int uid);
	// 当前计时
	int getCurrentStamp();
	// 移除玩家, del为true会释放NewMatcher对象(匹配成功释放, 移入下个T级不释放)
	NewMatcher* removeFromMatchList(int uid, bool del = true);
	// 从机器人列表中移除
	bool removeFromRobotList(int uid);

	//计时开始
	void onTimer(const KxServer::kxTimeVal& now);
	// 进行匹配
	void matchUpdate();
	// 获得一个NewMatcher对象
	NewMatcher *newMatcher();
	// 释放一个NewMatcher对象
	void deleteMatcher(NewMatcher* matcher);
	// 清理所有匹配人员
	void clearMatchers();

private:

	//T级为1的都匹配机器人
	void matchLevel1List(NewMatcherList &list);
	// 对对应T级列表进行匹配
	void matchUpdateList(int level, NewMatcherList &list);
	// 玩家与玩家匹配成功
	void matchSuccess(NewMatcher *matcher1, NewMatcher *matcher2);
	// 玩家匹配机器人成功, robotLevel匹配的到的机器人等级
    void matchRobotSuccess(NewMatcher *matcher, int robotLevel);
    // 玩家匹配新手引导机器人成功,
    void matchGuideRobotSuccess(NewMatcher *matcher, int robotId);

	// 根据当前人数获得负载状态
	int getMatcherStatus();
	// 获得匹配时间
	int getMatchTime(int matchLevel);
	// 获得匹配等待时间
	int getWaitingTime(int matchLevel);
	// 获得battleId
	int getBattleId();

private:

	static CNewMatchManager*		m_pInstance;			//实例
	int								m_nBattleId;			//战斗id
	int								m_nMatcherCount;		//匹配总人数
	int								m_nTotalStamp;			//总时间

	KxServer::KxTimerManager*		m_pTimerManager;		//计时器
	std::map<int, NewMatcherList>	m_mapMatcherList;		//拼配列表
	std::map<int, NewMatcher*>		m_mapMatchers;			//记录玩家, 用来移除玩家
	std::map<int, NewMatcher*>		m_mapRobotMatchers;		//匹配机器人玩家
};

#endif //__NEW_MATCH_MANAGER_H__
