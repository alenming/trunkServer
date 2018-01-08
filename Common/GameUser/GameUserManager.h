#ifndef __GAMEUSER_MANAGER_H__
#define __GAMEUSER_MANAGER_H__

#include <map>
#include "GameUser.h"
#include "KXServer.h"
#include "GameDef.h"
#include "UnionModel.h"
#include "MailModel.h"
#include "PassTeamModel.h"

//延迟处理结构
struct SDelayDelData
{
	int nDelayTime;
	int nUid;
};

class CServerModel;
class CPvpRankModel;
class CPassTeamModel;
class CGameUserManager : public KxServer::KxTimerObject
{
private:
	CGameUserManager();
	~CGameUserManager();

public:

	static CGameUserManager *getInstance();
	static void destroy();

public:

	bool init(KxServer::KxTimerManager *pTimerManager);

	void addModelType(ModelType Type);

	void onTimer(const KxServer::kxTimeVal& now);
	// 获得已有用户，当createWhenNull为true时，获取不到会自动从数据库中初始化它
	CGameUser* getGameUser(int uid, bool createWhenNull = true);
	// 初始化用户
	CGameUser* initGameUser(int uid);
	CGameUser *newGameUser(int uid);
	//更新用户需要重置的数据
	void reSetGameUserData(int uid, bool bLogin = false);
    // 刷新用户数据
    void updateGameUserData(CGameUser* gameUsr, bool bLogin = false);
	// 添加用户, 注册的时候添加
	void addGameUser(int uid, CGameUser* gameUsr);
	//检测用户在该服务器是否存在
	bool checkUserIsExist(int uid);
	// 移除用户
	void removeGameUser(int uid);
	// 删除移除用户数据
	void donotDeleteUser(int uid);

	// 获得通关队伍模型
	CPassTeamModel *getPassTeamModel() { return m_pPassTeamModel; }
	// 获得所有模型类型
	std::vector<ModelType>& getModelType() { return m_VectServerModel; }

	bool initUserData(CGameUser* gameUsr);

    const std::map<int, CGameUser *>& getGameUsers() { return m_GameUsers; }

private:

	//真正删除用户
	void RealremoveGameUser(int uid);

private:

	static CGameUserManager *							m_pInstance;
	CPassTeamModel *									m_pPassTeamModel;
	std::map<int, CGameUser *>							m_GameUsers;
	std::vector<ModelType>								m_VectServerModel;					//服务器所需求服务器模型
	std::map<int, std::list<SDelayDelData>::iterator>	m_MapDelUserList;					//玩家对应的向量迭代器
	std::list<SDelayDelData>							m_DelUserList;						//延迟玩家下线列表
	KxServer::KxTimerManager *							m_TimeManager;
};

#endif //__GAMEUSER_MANAGER_H__
