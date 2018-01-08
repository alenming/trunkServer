#ifndef __STATE_H__
#define __STATE_H__

#include "RoleComponent.h"
#include "BufferData.h"
#include "ConfRole.h"

enum StateActionType
{
    StateAction_None,
    StateAction_Move,
    StateAction_HitPower,
    StateAction_Dead,
    StateAction_AddBuffForMeOnce,
    StateAction_DoCount,
};

//状态类型ID
enum StateTypeID
{
    State_None, 		        // 无效
    State_Death,		        // 死亡
    State_Born,			        // 出生
    State_Stand,		        // 待机
    State_Move,			        // 移动
    State_Fight,		        // 战斗
    State_FightStand,	        // 战斗待机
    State_Charging,             // 吟唱
    State_Win,                  // 胜利
    State_Lose,                 // 失败

    State_Hit = 10,		        // 被击
    State_HitVerigo,	        // 击晕
    State_HitBack,		        // 击退
    State_HitFly,		        // 击飞
    State_HitDown,		        // 倒地
    State_GetUp,		        // 起身
    State_Frozen,		        // 冰冻
    State_Petrified,	        // 石化
    State_Negative_Begin = State_Hit,             // 第一个负面状态
    State_Negative_End   = State_Petrified,       // 最后一个负面状态

    State_SkillBase = 30,       // 技能释放状态（+n表示第n技能，n从1开始）
};

inline bool isNegativeState(int state) { return (state >= State_Negative_Begin) && (state <= State_Negative_End); }

class CRoleComponent;
class CRole;

// 一个具体的状态
class CState : public ISerializable
{
public:
	CState();
	virtual ~CState();

    // 只初始化一次，进行配置加载等
    virtual bool init(int stateId, const StatusConfItem* confItem, CRoleComponent* component);

    // 执行状态更新
    virtual void update(float dt);

    // 进入该状态时回调，用于初始化状态
    virtual void onEnter(bool isFouce = false);

    // 离开该状态时回调，用于重置状态
    virtual void onExit(bool isFouce = false);

	//序列化,反序列化 状态
	bool serialize(CBufferData& data);
	bool unserialize(CBufferData& data);

    // 查询状态是否完成
    inline bool isDone()
    {
        return m_bIsDone;
    }

    inline bool isLock()
    {
        return m_pStateConf->Lock;
    }

    // 状态优先级
    inline int getPriority()
    {
        return m_pStateConf->CanBreakParam;
    }

    // 状态ID
    inline int getStateId()
    {
        return m_nStateId;
    }

    // 获取下一个状态
    inline int getNextStateId()
    {
        return m_nNextStateId;
    }

    // 改变下一个状态
    inline void setNextStateId(int stateId)
    {
        m_nNextStateId = stateId;
    }

    inline const StatusConfItem* getStateConf()
    {
        return m_pStateConf;
    }

    inline float getDuration()
    {
        return m_fDuration;
    }

    inline void setDuration(float duration)
    {
        m_fDuration = duration;
    }

protected:
    bool m_bIsDone;                         // 状态是否结束了
    float m_fDuration;                      // 剩余时间
    int m_nNextStateId;                     // 自动切换状态
    int m_nStateId;                         // 状态Id
    CRoleComponent* m_pComponent;           // 状态组件
    CRole* m_pOwner;                        // 拥有者
    const StatusConfItem* m_pStateConf;     // 状态配置
};

// 移动状态
class CStateMove : public CState
{
    // 执行状态更新
    virtual void update(float dt);

	//序列化,反序列化 状态
	bool serialize(CBufferData& data);
	bool unserialize(CBufferData& data);

private:
    float m_fSpeed;                         // 当前移动速度
    float m_fAcceleration;                  // 当前移动加速度
};

// 被击飞状态
class CStateHitPower : public CState        
{
    // 执行状态更新
    virtual void update(float dt);

    // 进入该状态时回调，用于初始化状态 
    virtual void onEnter(bool isFouce = false);

	//序列化,反序列化 状态
	bool serialize(CBufferData& data);
	bool unserialize(CBufferData& data);

private:
    float m_fPowerY;
    float m_fBaseY;                         // 地平Y值
    float m_fA;                             // 摩擦系数
    float m_fG;                             // 重力加速度
    float m_fVx;                            // X方向速度
    float m_fVy;                            // Y方向速度
};

class CStateDeath : public CState
{
    virtual void onEnter(bool isFouce = false);
	// 执行状态更新
	virtual void update(float dt);
};

#endif 
