#ifndef __DISPATCH_H__
#define __DISPATCH_H__

#include <list>
#include "ConfStage.h"

enum EDispatchCondition
{
	DC_Non,								  // 无条件
	DC_PCHeroDistance,					  // 电脑英雄某个范围内（参数决定）是否有玩家的士兵
	DC_PCHeroHP,						  // 电脑英雄的血量小于等于多少百分比
	DC_CrystalLv,						  // 玩家的水晶到达几级
	DC_SoldierCount,					  // 玩家方在场上还存活的士兵的数量到达几个
	DC_SoldierMaxStar,					  // 玩家在场上的还存活的士兵中最高星级到达几级
	DC_DispatchSoldierCount,			  // 玩家已经派发的士兵数量达到一定数量
	DC_DeadSoldierCount,				  // 玩家派发的士兵死亡的数量达到一定数量
	DC_PCDeadSoldierCount,				  // 电脑派发的士兵死亡的数量达到一定数量
	DC_DispatchSoldierType,				  // 玩家在场的士兵是否有指定类型的兵种，指定类型为int参数，对应士兵的卡片属性参数值
	DC_DispatchSoldierID				  // 玩家在场的士兵是否有指定ID的兵种，指定ID为int参数，对应士兵ID
};

class CBattleHelper;
class CRole;
class CDispatch
{
public:
	CDispatch();
	~CDispatch();

	// 初始化(关卡ID,玩家ID)
	bool init(int stageID, int playerID, int enemyID, CBattleHelper *battleHelper);

	void logicUpdate(float dt);
	// 条件判断
	bool checkCondition(const int& conditionType, const int& conditionVal);
	// 发兵
	void execute(const SDispatchInfo &info);

private:
	// 电脑英雄某个范围内（参数决定）是否有玩家的士兵
	bool checkPCHeroDistance(const int& conditionVal);
	// 电脑英雄的血量小于等于多少百分比
	bool checkPCHeroHP(const int& conditionVal);
	// 玩家的水晶到达几级
	bool checkCrystalLv(const int& conditionVal);
	// 玩家方在场上还存活的士兵的数量到达几个
	bool checkSoldierCount(const int& conditionVal);
	// 玩家在场上的还存活的士兵中最高星级到达几级
	bool checkSoldierMaxStar(const int& conditionVal);
	// 玩家已经派发的士兵数量达到一定数量(注:会重置)
	bool checkDispatchSoldierCount(const int& conditionVal);
	// 玩家派发的士兵死亡的数量达到一定数量
	bool checkDeadSoldierCount(const int& conditionVal);
	// 电脑派发的士兵死亡的数量达到一定数量
	bool checkPCDeadSoldierCount(const int& conditionVal);
	// 玩家在场的士兵是否有指定类型的兵种，指定类型为int参数，对应士兵的卡片属性参数值
	bool checkDispatchSoldierType(const int& conditionVal);
	// 玩家在场的士兵是否有指定ID的兵种，指定ID为int参数，对应士兵ID
	bool checkDispatchSoldierID(const int& conditionVal);

	// 处理关闭列表
	void processCloseList(const int &conditionID);
	// 处理开启列表
	void processOpenList(const int &conditionID);
	// 处理发兵信息列表
	void processDispatchList(const int &conditionID);

    // 获取某个士兵的信息
    bool getSoldierInfo(CRole *role, int &confID, int &star);
    // 判断士兵类型
    bool isSoldierType(int confid, int star, int type);

private:
    int							m_nPlayerCamp;			// 玩家阵营
    int							m_nEnemyCamp;			// 敌人阵营

    std::list<CDispatchItem*>	m_listDispatchOpen;		// 加载时开启的列表
    std::list<CDispatchItem*>	m_listDispatchClose;	// 加载时关闭的列表
    std::list<CDispatchItem*>	m_listDispatchCache;	// 发兵缓存列表
    std::list<SDispatchInfo>	m_listDispatchInfo;		// 发兵信息列表

    const StageConfItem			*m_ConfStageItem;		// 关卡配置
    CBattleHelper				*m_pBattleHelper;		// 战斗辅助
};

#endif