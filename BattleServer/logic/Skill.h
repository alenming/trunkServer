#ifndef __SKILL_H__
#define __SKILL_H__

#include "KxCSComm.h"
#include "BufferData.h"
#include "ConfFight.h"

#define SKILL_INVALID_POINT  Vec2(-99999.0, -99999.0)

enum SkillLockType
{
    SLT_UNLOCK = 0,         //不限制
    SLT_CRYSTALLEVEL,       //水晶等级
};

enum SkillCostType
{
    SCT_NOCOST = 0,         //不消耗    
    SCT_CRYTAL,             //水晶消耗
    SCT_BLOOD,              //自身血量
    SCT_RAGE,               //怒气
    SCT_MAGIC,              //魔法值
};

class CRole;
class CSkillComponent;
class CSkill : 
    public ISerializable
{
public:
    CSkill();
    ~CSkill();

public:

    // 序列化反序列化
    virtual bool serialize(CBufferData& data);
    virtual bool unserialize(CBufferData& data);
    //召唤师（英雄表）：技能等级是玩家数据
    //英雄（士兵表）：技能等级是玩家数据
    //BOSS：技能等级是1
    //怪物：技能等级是1
    //召唤物：技能等级是1
    bool init(CSkillComponent *skillComponent, CRole *owner, int skillid, int skillindex, int skillLv = 1);

    void update(float dt);

    bool isFinish();

    const SkillConfItem * getSkillConf();

    int skillId();

    int skillIndex();

    float getCDPercent();

    float getExecutingPercent();

    bool isLock();

    bool canUnlock();

    void unlock();

    void breakSkill();

    bool changeState();

    bool canExecute();

    void execute(std::vector<CRole*> &targets, Vec2 &pos);

    void execute(std::vector<CRole*> &targets);

    void execute(Vec2 &pos);
    
    //inline int getSkillLevel() { return m_nSkillLevel; }

private:

    void reset();

    void cost();
    //计算联动时间
    void calcLinkage();
    //返回子弹是否发射完毕
    bool nextTargetBullet();
    //返回子弹是否发射完毕
    bool nextPosBullet();

    void fireBullet(std::vector<CRole*> &targets, int bulletid);

    void fireBullet(Vec2 &pos, int bulletid);

    void callSoldier();

    void addBuff(std::vector<CRole*> &targets);

private:

    bool m_bFinish;                         // 技能是否已经结束释放
    bool m_bIsLock;                         // 技能是否被锁住
    bool m_bTargetBulletShoot;              // 是否发射目标子弹
    bool m_bPositionBulletShoot;            // 是否发射位置子弹

    int m_nIndex;                           // 该技能是角色装备的第几个技能
    int m_nLoopTimes;                       // 技能循环次数（每次都会扣消耗）
    int m_nTargetBulletIndex;               // 本次循环的目标类型子弹序列下标
    int m_nPositionBulletIndex;             // 本次循环的位置类型子弹序列下标
    //int m_nSkillLevel;

    float m_fCDTime;                        // 当前CD时间
    float m_fLoopDelay;                     // 本次循环剩余时间
    float m_fTargetTypeDelay;               // 本次循环的目标类型子弹序列延迟剩余时间
    float m_fPositionTypeDelay;             // 本次循环的位置类型子弹序列延迟剩余时间
    float m_fConfLoopDelay;                 // 配置中单次循环时间
    float m_fConfFirstTargetTypeDelay;      // 配置中首次目标类型子弹序列延迟时间
    float m_fConfFirstPositionTypeDelay;    // 配置中首次位置类型子弹序列延迟时间
    float m_fConfTargetTypeDelay;           // 配置中目标类型子弹序列延迟时间
    float m_fConfPositionTypeDelay;         // 配置中位置类型子弹序列延迟时间

    CRole* m_pOwner;
    Vec2 m_TargetPos;
    std::vector<CRole*> m_TargetList;
    CSkillComponent* m_pSkillComponent;
    const SkillConfItem* m_pSkillConf;

private:
    //float m_fCurExecutingTime;                      // 当前执行时间
    //float m_fMaxExecutingTime;                      // 最大执行时间
};

#endif //__SKILL_H__
