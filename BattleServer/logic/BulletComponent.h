/*
*  BulletComponent负责子弹的移动以及子弹的状态切换
*
*/

#ifndef __BULLET_COMPONENT_H__
#define __BULLET_COMPONENT_H__

#include "LogicComponent.h"
#include "BufferData.h"

class CRole;
class CBullet;
class CBulletPathBase;
class CBulletHitJudge; 
class BulletConfItem;
class CBattleHelper;
class CBulletComponent : public CLogicComponent
{
public:
    
    CBulletComponent();
    ~CBulletComponent();

public:
    virtual bool serialize(CBufferData& data);
    virtual bool unserialize(CBufferData& data);

    virtual bool init(CBullet *bullet);
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool isLogicOver();
    virtual bool isFlyToScreenOut();
    virtual void logicUpdate(float delta);
    virtual void addHitTarget(CRole* target);
    virtual std::vector<CRole*>& getTargets();

private:
    // 播放命中特效 —— for client
    inline void playHitEffect();

private:
    
    float                     m_fBulletExsitTime;   //存在时间
    CBattleHelper*            m_pHelper;
    CBullet*                  m_pOwner;             //所属子弹对象
    CBulletPathBase*          m_pBulletPath;        //路径
    CBulletHitJudge*          m_pBulletHitJudge;    //命中判定
    std::vector<CRole*>       m_TargetList;         //目标列表
};


/************************************************************************
子弹轨迹
************************************************************************/
class CBulletPathBase : public ISerializable
{
public:
    CBulletPathBase();
    virtual ~CBulletPathBase();

public:

    virtual bool serialize(CBufferData& data);
    virtual bool unserialize(CBufferData& data);

    virtual bool init(CBullet* bullet);
    virtual void onEnter();
    virtual void onExit();
    virtual void update(float dt);
    //轨迹是否结束
    virtual void setPathOver(bool isOver = true);
    virtual bool isPathOver();

protected:

    bool             m_bIsPathOver;         //结束
    float            m_fExsitTime;          //已经存在时间
    CBullet*         m_pBullet;             //子弹
    const BulletConfItem*  m_pBulletConf;   //子弹配置
};

//抛物线轨迹
class CBulletParabola : public CBulletPathBase
{
public:
    CBulletParabola();
    ~CBulletParabola();

public:
    virtual bool serialize(CBufferData& data);
    virtual bool unserialize(CBufferData& data);
    bool init(CBullet* bullet);
    void onEnter();
    void update(float dt);

private:
    float m_fVx;                            // X轴的速度
    float m_fVy;                            // Y轴的速度
    float m_fG;                             // 重力加速度G
    float m_fFlyTime;                       // 总的飞行所需时间
};

//持续轨迹
class CBulletDuration : public CBulletPathBase
{
public:
    CBulletDuration();
    ~CBulletDuration();

public:
	bool init(CBullet* bullet);
    void onEnter();
    void update(float dt);
};

//直线穿透
class CBulletPierce : public CBulletPathBase
{
public:
    CBulletPierce();
    ~CBulletPierce();

public:
    virtual bool serialize(CBufferData& data);
    virtual bool unserialize(CBufferData& data);
    bool init(CBullet* bullet);
    void onEnter();
    void update(float dt);

private:
    float   m_fSpeed;                       // 子弹的速度
    float   m_fPullBackTime;                // 折返时间
};

/************************************************************************/
/* 命中判定                                                               */
/************************************************************************/
class CBulletHitJudge : public ISerializable
{
public:
    CBulletHitJudge();
    virtual ~CBulletHitJudge();

public:

    virtual bool serialize(CBufferData& data);
    virtual bool unserialize(CBufferData& data);

    virtual bool init(CBullet *bullet, CBulletComponent *btComponent);
    virtual void update(float dt);
    virtual void setJudgeOver();
    virtual bool isJudgeOver();
    virtual bool isNeedCount();
    virtual bool judge();
    //结算角色, 对未达到最大结算次数的目标进行结算 
    void countRole();
    //给所有命中目标增加buff
    void roleAddBuff();

private:
    //返回是否命中
    bool judgePointHit();
    //返回是否命中
    bool judgeRectHit();
    //是否跟子弹目标碰撞, 判定是否命中,
    bool isTargetCollide(CRole *target);
    //是否达到最大结算次数
    bool isMaxCount(CRole *target);

protected:

    int   m_nHitType;                       //碰撞类型
    int   m_nJudgeTimes;                    //已判断次数
    float m_fFirstJudgeTime;                //首次判定时间
    float m_fInterval;                      //经过间隔

    float m_fParam;                         //命中判定参数
    bool  m_bNeedCount;                     //是否需要结算
    bool  m_bJudgeOver;                     //是否判定结束

    CBullet *m_pBullet;                     //子弹
    CBulletComponent* m_pBulletComponent;   //子弹逻辑组件
    std::map<int, int> m_TargetCountTimes;  //单位结算次数<objid, times>
};

#endif 
