/*
 * 战斗辅助类（可理解为单例对象，每场战斗会创建一个）
 * 
 * 1.负责游戏对象的创建和管理
 * 2.管理游戏中的命令队列
 * 3.管理单场游戏中所有的全局变量
 *
 * 2014-12-18 By 宝爷
 *
 */
#ifndef __BATTLE_HELPER_H__
#define __BATTLE_HELPER_H__

#include "RoleComm.h"
#include "KxCSComm.h"
#include "CommStructs.h"
#include "CommTools.h"
#include "BattleScene.h"
#include "BattleModels.h"
#include "RandomNumber.h"
#include "BufferData.h"
#include "Dispatch.h"
#include "EventManager.h"

class CBattleLayer;
class CBullet;
class CRole;
class CHero;
class CSoldier;
class CMonster;
class CBoss;
class CCallSoldier;
class CGameObject;
class CBuff;
class CBuffComponent;

class CBattleHelper : public ISerializable
{
public:
    CBattleHelper();
    virtual ~CBattleHelper();

    // 进入战斗时回调，传入主玩家ID，地图ID，双方用户模型，Director
    bool init(CRoomModel* room, CBattlePlayerModel* myModel
        , CBattlePlayerModel* enmeyModel,CEventManager<int>* eventMgr, Node* scene);
    // 战斗结束时回调
    void uninit();

    ///////////////////////////////战斗逻辑相关/////////////////////////////////////////

    // 判断当前战斗是否结束，是则自动执行onBattleOver
    bool checkBattleOver();
    // 战斗结束时回调
    void onBattleOver();
    // 驱动战斗逻辑
    void processBattle(float delta);

    ///////////////////////////////序列化和反序列化/////////////////////////////////////

    // 将游戏场景打包到data中
    bool serialize(CBufferData& data);
    // 从data中解压出游戏场景
    bool unserialize(CBufferData& data);

    // 根据ObjectId获取Role
    CRole* getRoleByObjectId(int objectId);
    // 根据ObjectId从ObjectMap中移除Role
    inline void removeObjectMapByObjectId(int objectId) { m_ObjectMap.erase(objectId); }

    /////////////////////////////////创建游戏对象/////////////////////////////////////////

    // 使用用户模型，士兵ID创建士兵
    CSoldier* createSoldier(int uid, int soldierIndex, int line = -1);
    // 创建MONSTER
    CMonster* createMonster(int monsterIndex, int line = -1);
    // 创建召唤物, ownerLv为召唤士兵的召唤者当前等级
    CCallSoldier* createCallSoldier(int uid, int callId, int ownerLv, float x, float y);
    // 使用用户模型，子弹ID，攻击者创建子弹
    CBullet* createBullet(int uid, int bulletId, CRole* attacker, CRole* target = NULL, const Vec2& pos = Vec2::ZERO);
    // 使用buff组件, 携带者, 发出者, buffid, buff层数创建buff
    CBuff* createBuff(CBuffComponent *cpnt, CRole *owner, CRole *maker, int buffid, int stack);

    /////////////////////////////////获取与操作游戏对象/////////////////////////////////////////
    
    // 获得指定阵营的英雄(包括关卡对战敌方的boss)
    CRole* getMainRole(int camp);
    // 获取对立阵营的英雄(包括关卡对战敌方的boss)
    CRole* getEnmeyMainRole(int camp);
    // 获得指定玩家的所有士兵(或者关卡对战敌方的monster)
    std::vector<CRole*>& getRoleWithCamp(int camp);
    // 根据自己的UID获取对方的士兵(包括关卡对战敌方的monster)
    std::vector<CRole*>& getEnmeyRoleWithCamp(int camp);
    // 切换阵营
    void changeCampType(CRole* role, CampType camp);
    // 获得子弹列表
    inline std::list<CBullet*>& getBullets() { return m_Bullets; }
    // 获取阵营切换列表
    inline std::map<CRole*, CampType>& getChangeCamp() { return m_ChangeCampMap; }

    ////////////////////////////////回收释放游戏对象//////////////////////////////////////////

    // 回收士兵
    void destorySoidler(CSoldier* role);
    // 回收monster
    void destoryMonster(CMonster* monster);
    // 回收子弹
    void destoryBullet(CBullet* bullet);
    // 自动判定英雄，士兵，子弹等物体
    void destroyObject(CGameObject* obj);
    // 回收buff
    void destoryBuff(CBuff* buff);
    // 添加角色到自释放列表
    void autoRleaseRole(CRole* role);
    // 自动回收已经没有其他地方引用到的角色
    void autoReleaseRoles();

    ///////////////////////////////搜索获得目标列表接口///////////////////////////////////////////

    // 通过搜索配表id
    bool getTargetsWithId(int searchid, CGameObject* searcher, std::vector<CRole *> *targets);
    // 英雄是否在范围内
    bool isHeroInRange(int uid, int camp, Vec2 &curPos, int range);
    // 排序敌方和友方列表
    void sortSoldiers();

    ////////////////////////////////操作游戏命令//////////////////////////////////////////

    // 插入一条命令，根据命令的时间进行插入排序
    // 命令已过期则插入失败
    bool insertBattleCommand(BattleCommandInfo cmd);
    // 查询队列头部的命令
    BattleCommandInfo& topBattleCommand();
    void popBattleCommand();
    const std::vector<BattleCommandInfo>& getBattleCommandQueue(){ return m_CommandQueue; }
	void resetBattleCommandQueue(std::vector<BattleCommandInfo>& newCmdQueue);
    void clearBattleCommandQueue();
    void processCommand();
    inline int getCurExecutCommandCount() { return m_nCurExecutCommandCount; }
    inline int commandCount() { return m_CommandQueue.size() - m_nCurExecutCommandCount; }

    ////////////////////////////////getter and setter////////////////////////////////////

    // 获取玩家所在的阵营，玩家ID不存在，则返回中立
    int getCampWithUid(int uid);
    // 获取玩家所在的敌对阵营，玩家ID不存在，则返回中立
    int getEnmeyCampWithUid(int uid);
    // 获取阵营的敌对阵营，中立阵营没有敌对
    int getEnmeyCamp(int camp);

    inline int getBattleType() { return m_nBattleType; }
    inline CRoomModel* getRoom() { return m_pRoom; }
    inline int getMasterId() { return m_nMasterId; }
    inline int getStageId() { return m_nStageId; }
    inline CBattleLayer* getBattleScene() { return m_pBattleScene; }
    inline int getUserId() { return m_nUserId; }
    inline int getEnmeyUserId() { return m_nEnmeyUserId; }
    inline CComputerModel* getComputerModel() { return dynamic_cast<CComputerModel*>(pEnemyUserModel); }
    inline CSettleAccountModel* getSettleAccountModel()
    {
        return NULL != m_pRoom ? m_pRoom->getSettleAccountModel() : NULL;
    }
    inline int getEnmeyUserId(int uid)
    {
        return uid == m_nUserId ? m_nEnmeyUserId : m_nUserId; 
    }
    inline CBattlePlayerModel* getUserModel(int uid)
    {
        return uid == m_nUserId ? pMyUserModel : pEnemyUserModel;
    }
    // 获得英雄站立点
    const Vec2& getOrigin(int camp);
    // 传入阵营获取站立点，line默认为-1表示循环切换位置
    Vec2 getSoliderOrigin(int camp, int line = -1);
	// 获得0, 1, 2线路的y值
	float getSoldierLineY(int line);

    // 获取已经阵亡的士兵数量
    bool isEnoughDeadSoldierCount(int uid, int compareNum);
    // 获取派发的数量
    bool isEnoughDispatchCount(int compareNum);
    // 获取指定阵营
    int getTreatCount(int camp);

private:
    /////////////////////////////////// 初始化其他 ///////////////////////////////////////
    bool initStage();
    bool initModel();

    //////////////////////////////// 序列化与反序列化 ////////////////////////////////////
    // 序列化角色目录
    void serializeRoleTable(CBufferData& data, std::vector<CRole*>& vec);
    // 序列化角色列表
    void serializeRoles(CBufferData& data, std::vector<CRole*>& vec);
    // 序列化子弹目录
    void serializBulletTable(CBufferData& data, std::list<CBullet*>& l);
    // 序列化子弹列表
    void serializBullets(CBufferData& data, std::list<CBullet*>& l);
    // 反序列化时创建角色
    bool createRole(int ownerId, int objId, int objType, int typeId, int lv,
		CBufferData& data, std::vector<CRole*>& vec);
    // 反序列化角色目录
    void unserializeRoleTable(CBufferData& data, std::vector<CRole*>& vec, std::vector<CRole*>& vecRemove, std::vector<int>& idVec);
    // 反序列化角色列表
    void unserializeRoles(CBufferData& data, std::vector<int>& vec);
    // 反序列化时清除角色
    void removeRoles(std::vector<CRole*>& roles);
    // 反序列化子弹目录
    void unserializeBulletTable(CBufferData& data, std::list<CBullet*>& l, std::vector<CBullet*>& vecRemove, std::vector<int>& idVec);
    // 反序列化子弹列表
    void unserializeBullets(CBufferData& data, std::vector<int>& vec);
    // 反序列化时清除子弹
    void removeBullets(std::vector<CBullet*>& bullets);

    //////////////////////////////// 游戏对象相关私有方法 ////////////////////////////////////
    // 使用用户模型创建英雄
    CHero* createHero(CPlayerModel* player);
    // 创建BOSS
    CBoss* createBoss(int bossID);
    // 从池中获取一个士兵
    CSoldier* newSoldier();
    // 回收一个士兵到池中
    void freeSoldier(CSoldier* soldier);
	//从池中获取一个怪物
	CMonster* newMonster();
	//回收一个怪物到池中
	void freeMonster(CMonster* monster);
    // 从池中获取一个子弹
    CBullet* newBullet();
    // 回收一个子弹到池中
    void freeBullet(CBullet* bullet);
    // 从池子中获取一个Buff
    CBuff* newBuff();
    // 回收一个buff到池中
    void freeBuff(CBuff* buff);

private:
    // 玩家胜利
    void setUserWinRolesState();
    // 玩家失败
    void setUserLoseRolesState();

public:
    int CurTick;                                    // 当前时间进行到第几帧，恒速不变
    int GameTick;                                   // 当前游戏逻辑进行到第几帧，根据时间补偿加速等变化
    int MaxTick;                                    // 这一局游戏会有多少逻辑帧（最大限制，根据配表的秒数换算）
    int TickPerSecond;                              // 游戏每一秒有多少逻辑帧
    float MinX;                                     // 场景最左边坐标
    float MaxX;                                     // 场景最右边坐标
    CDispatch* Dispatch;                            // 发兵对象（创建Monster）
    CEventManager<int>* pEventManager;              // 事件管理器
    CBattlePlayerModel* pMyUserModel;               // 我的用户模型（服务端为房主）
    CBattlePlayerModel* pEnemyUserModel;            // 敌人的用户模型
    bool IsBattleOver;                              // 战斗是否已结束
    CRandomNumber RandNum;							// 随机数表

private:
    int m_nUserId;                                  // 我的用户ID（服务端为房主ID）
    int m_nMasterId;                                // 主ID，左边用户的ID
    int m_nEnmeyUserId;                             // 敌人ID
    int m_nStageId;                                 // 地图ID
    int m_nStageLevel;                              // 关卡等级
    int m_nBattleType;                              // 战斗类型
    int m_nObjectId;                                // 累加的对象唯一ID
    int m_nCurLine;                                 // 当前是第几条线

	int m_nCurExecutCommandCount;					// 当前执行完成的命令数

    int m_nUserDeadSoldierCount;					// 己方已经阵亡的士兵数量(发兵使用,会重置)
    int m_nEnemyDeadSoldierCount;					// 敌方已经阵亡的士兵数量(发兵使用,会重置)
    int m_nCurDispatchCount;						// 己方当前派发的士兵数量(发兵使用,会重置)

    Vec2 m_BlueHeroOrigin;                          // 蓝方英雄点
    Vec2 m_RedHeroOrigin;                           // 红方英雄点
    CBattleLayer* m_pBattleScene;                   // 战斗场景
    CRoomModel* m_pRoom;                            // 房间模型
    CRole* m_BlueRole;                              // 左边的英雄
    CRole* m_RedRole;                               // 右边的英雄(可能是BOSS)
    CRole* m_MyRole;                                // 我的英雄（或房主的英雄）
    CRole* m_EnmeyRole;                             // 敌方的英雄(或房主的英雄，可能是BOSS)
    const StageConfItem* m_pStageConf;              // 战斗配置

    std::map<CRole*, CampType> m_ChangeCampMap;     // 切换阵营的对象列表，无需序列化和反序列化
    std::map<int, CGameObject*> m_ObjectMap;        // 当前游戏中有引用到的所有对象（包括子弹和角色，及已经从场景中移除的对象）
    std::vector<CRole*> m_BlueRoles;                // 左边的士兵列表
    std::vector<CRole*> m_RedRoles;                 // 右边的士兵列表(关卡会是monster)
    std::vector<CRole*> m_NeutralRoles;             // 中立的士兵列表
    std::vector<CRole*> m_AutoReleaseRoles;         // 已经从场景中移除
    std::list<CRole*> m_RoleCache;                  // 士兵缓存池
	std::list<CRole*> m_MonsterCache;				// 怪物缓存池
    std::list<CBullet*> m_Bullets;                  // 子弹列表
    std::list<CBullet*> m_BulletCache;              // 子弹缓存池
    std::list<CBuff*> m_BuffCache;                  // buff缓存池
    std::vector<BattleCommandInfo> m_CommandQueue;  // 命令队列
    std::vector<float> m_BlueLineX;
    std::vector<float> m_RedLineX;
    std::vector<float> m_LineY;
    std::map<int, int> m_TreatCount;                // 每个阵营对应需要治疗的计数器，用于快速判断是否需要进行治疗搜索
};

#endif
