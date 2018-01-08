#ifndef __BUFF_H__
#define __BUFF_H__

#include <vector>
#include "KxCSComm.h"
#include "BufferData.h"

#ifndef RunningInServer
#include "EffectFactory.h"
#endif

enum BuffConditionID
{
    Func_IsHasBuffTarget = 1,
    Func_IsHasSkillPoint,
    Func_IsSkillReady,
    Func_IsRandom,
};

enum BuffType
{
    BUFF = 1,
    DEBUFF,
    UNSTOPPABLE,
};

enum BuffTriggerType
{
    TRIGGER_TIME = 1,           // 时间
    TRIGGER_DIE,                // Buff携带者死亡的时候触发
    TRIGGER_SKILL,              // 释放技能
    TRIGGER_CRIT,               // 暴击
    TRIGGER_BECRITED,           // 被暴击
    TRIGGER_DODGE,              // 闪避
    TRIGGER_BEDODGED,           // 被闪避
    TRIGGER_BETREATED,          // 治疗
    TRIGGER_BROKENSHIELD,       // 破盾
    TRIGGER_ADDSHIELD,          // 加盾
    TRIGGER_DISPATCHSOLIDER,    // 派兵
    TRIGGER_BEATTACKED,         // 受击
    TRIGEER_ATTACK,             // 攻击
};

enum BuffTargetType
{
    BUFFTARGET_MAKER = -2,      // 发出者
    BUFFTARGET_OWNER = -1,      // 携带者
    BUFFTARGET_NONE,            // 无
    BUFFTARGET_CUSTOM,          // 自定义选取(大于0的搜索id)
};

class CRole;
class BuffConfItem;
class CBuffComponent;
class CBuff;

class CBuffCondition : public ISerializable
{
public:
    CBuffCondition(CBuff *parent);
    ~CBuffCondition();

public:

    bool serialize(CBufferData& data);

    bool unserialize(CBufferData& data);

    bool isHasBuffTarget();

    bool isHasSkillPoint(int flag, int x, int y);

    bool isSkillReady(bool ng, int skillid);

    bool isRandom(int rand);

    std::vector<CRole *> &getTargets();

    void clearTargets();

    Vec2 &getTargetPos();

    void clearTargetPos();

    void getCustomTargets(int searchid, std::vector<CRole*> &targets);

private:

    CBuff*                 m_pParent;
    Vec2                   m_TargetPos;
    std::vector<CRole *>   m_Targets;
};

class CBuff 
    : public Ref
    , public ISerializable
{
public:
    CBuff();
    ~CBuff();

public:

    // 序列化反序列化
    virtual bool serialize(CBufferData& data);
    virtual bool unserialize(CBufferData& data);

    bool init(CBuffComponent *cpnt, CRole *owner, CRole *maker, int buffid, int stack);

    void onEnter();

    void onExit(bool isFouce = false);

    int getBuffType();

    bool isFinish();

    bool isUnstoppable();

    void finish(bool force = false);

    void update(float dt);

	void reset();

    int getStack();

    CRole *getMaker();

    const BuffConfItem *getBuffConf();

private:
    // 检查是否生效
    bool checkCondition();
    // 检测触发器
    void checkTrigger(float dt);
    // 生效结算
    void takeEffectCount();
    // 失效结算
    void loseEfficacyCount();
    // 释放/回收技能
    void executeSkill(int skillid);
    void recycleSkill(int skillid);

    // 注册触发器
    void regTrigger(int trigtype);
    // 取消注册
    void unregTigger(int trigtype);
    // 时间
    void triggerTime(float dt);
    // 死亡
    void triggerDie();

    // 事件监听回调
    // 使用技能
    void triggerSkill(void *data);
    // 派兵
    void triggerDispatchSoldier(void *data);

    // 特效表现
    void playEffect();
    // 停止特效表现
    void stopEffect();

    // 根据触发类型获取相应的属性值
    int getAttributeValue(BuffTriggerType triggerType);
    // 触发比较差值条件
    void triggerDValue(BuffTriggerType triggerType);
    // 触发比较累计数值条件
    void triggerAccumulative(BuffTriggerType triggerType);
    // 重置所有数据
    void resetData();

private:
    bool     m_bFinish;                             // 是否结束
    bool     m_bTriggerActive;                      // 是否触发buff
    bool     m_bUnstoppable;                        // 是否不可驱散
    bool     m_bForce;                              // 是否为强制回收
    int      m_nWorkTimes;                          // 工作的次数
    int      m_nRecordState;                        // 添加该BUFF时角色的状态
    float    m_fBuffLifeTime;                       // buff生命周期计时
    float    m_fFirstTime;                          // 首次生效时间计时

    float    m_fTriggerTime;                        // 触发器计时
    int      m_nTrigerValue;                        // 触发值 - 不同触发类型有不同的意义
    CRole *                m_pOwner;                // buff携带者
    CRole *                m_pMaker;                // buff发出者
    CBuffComponent *       m_pBuffCpnt;             // buff所属组件
    const BuffConfItem *   m_pBuffConf;             // buff配表
    CBuffCondition         m_Condition;             // 生效条件
    // 让buff条件方便获得buff的信息
    friend class CBuffCondition;

#ifndef RunningInServer
    std::vector<cocos2d::Node *> m_EffectNodes;
#endif 
};

#endif //__BUFF_H__
