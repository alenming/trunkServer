#ifndef __MATCHMANAGER_H__
#define __MATCHMANAGER_H__

#include <list>
#include <set>

#include "KxCommInterfaces.h"
#include "KxTimerManager.h"
#include "PvpModel.h"

// 注意: 客户端等同房间类型,
// 所以此类型要与房间类型对应
enum MatchType
{
	MATCH_NONE,				//无 
	MATCH_FAIRPVP,			//公平竞技
	MATCH_CPN,				//锦标赛
};

struct NewMatch
{
	int				uid;		// uid
    int             level;      // 玩家等级
	int				integral;	// 积分值
	int				nStartTime;	// 匹配开始时间点

	bool operator == (NewMatch compare)
	{
		return this->uid == compare.uid;
	}
};

class CMatchManager : public KxServer::KxTimerObject
{
private:
	CMatchManager();
	~CMatchManager();

public:

	static CMatchManager *getInstance();
	static void destroy();

public:
	//添加到匹配列表
	bool addToMatch(int matchType, int uid, KxServer::IKxComm *target);
	//取消匹配
	bool cancelMatch(int uid);
	//开始匹配
	void matchUpdate();
	//计时开始
	void onTimer(const KxServer::kxTimeVal& now);
	//清除所有玩家
	void clearMatchers();
	//退出匹配分的清理工作
	void removeFromMatchServer(int uid);

private:
	// 插入操作
	bool insert(int matchType, int uid, int integral, int level);
	// 移除操作
	bool remove(int uid);
	// 查找操作
	bool find(int uid);
	//获得battleId
	int makeBattleId();
	//匹配成功
	void matchSuccess(NewMatch &matcher1, NewMatch &matcher2);
	//匹配机器人
	void matchRobotSuccess(NewMatch &matcher);
	
private:

	static CMatchManager *				m_pInstance;		//实例
	bool								m_bFairInvert;		//公平竞技反序
	bool								m_bCPNIntert;		//锦标赛反序
	int									m_nBattleId;		//battleId
	unsigned int						m_uTotalSeconds;	//总累计时间
	unsigned int						m_uFairSeconds;		//公平竞技计时
	unsigned int						m_uCPNSeconds;		//锦标赛计时
	
	KxServer::KxTimerManager *			m_pTimerManager;	//计时器
	std::set<int>						m_setMatcherUids;	//匹配的所有玩家
	std::list<NewMatch>					m_listFairPvp;		//公平竞技匹配列表
	std::list<NewMatch>					m_listNewGuy;		//匹配次数小于5次的玩家
	std::list<NewMatch>					m_listCpn;			//锦标赛匹配列表
};

#endif //__MATCHMANAGER_H__
