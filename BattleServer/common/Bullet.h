/*
* 子弹, 子弹轨迹, 子弹命中, 子弹链接
*
* 2014-12-23 by 宝爷
*/
#ifndef __BULLET_H__
#define __BULLET_H__

#include "GameObject.h"
#include <vector>

enum BulletPathType
{
    BPT_NONE = 0,               // 无
    BPT_PARABOLA,               // 抛物线
    BPT_TRACK,                  // 追踪
    BPT_DURATION,               // 周围持续
    BPT_PIERCE,                 // 直线穿透
};

enum BulletJudgeType
{
    BJT_NONE = 0,               // 无
    BJT_POINT,                  // 点命中判定
    BJT_RECT,                   // 矩形命中判定
};

enum BulletHitType
{
    BHT_NONE = 0,               // 无
    BHT_FLYEND,                 // 飞行结束
    BHT_FLYING,                 // 飞行中
    BHT_FLYCOLLISION,           // 飞行碰撞
};

enum BulletStateType
{
    BST_NONE,
    BST_FLYING,                 // 子弹飞行状态下
    BST_FLYEND,                 // 子弹飞行结束
};

enum BulletEndType
{
    BET_NONE,                   // 无
    BET_ATTACKERHITPOINT,       // 发出者的被击点
    BET_ATTACKERMOVEPOINT,      // 发出者的移动中心点
    BET_TARGETHITPOINT,         // 目标被击点
    BET_TARGETMOVEPOINT,        // 目标移动点
    BET_POINT,                  // 子弹攻击目标点
    BET_FRIENDHERO,             // 己方英雄
    BET_ENEMYHERO,              // 敌方英雄
    BET_ORIGIN,                 // 原点(0,0)点, 用于对屏幕内指定位置释放子弹
};

class BulletConfItem;
class CBulletComponent;
class CBullet : 
    public CGameObject
{
public:
    CBullet();
    virtual ~CBullet();

public:

    virtual bool serialize(CBufferData& data);
    virtual bool unserialize(CBufferData& data);

    virtual bool init(int bulletId, int uid, int objId, CRole *owner, CBattleHelper* battle);
    virtual void onEnter();
    virtual void onExit();

    virtual void setTarget(CRole *target);
    virtual CRole *getTarget();

    virtual CRole *getOwner();
    
	virtual void setTargetPos(const Vec2 &pos);
    virtual Vec2 getTargetPos();

    virtual const BulletConfItem *getBulletConf();
    virtual Vec2& getFiringPoint();
    
    virtual void changeState(int state);
    virtual int getState();

    inline void setPrevPosition(const Vec2& pos) { m_PrevPos = pos; }
    inline Vec2& getPrevPosition() { return m_PrevPos; }

protected:
    int                         m_nState;               //子弹状态
    Vec2                        m_FiringPoint;          //发射点
    Vec2                        m_TargetPos;            //如果不是指定目标, 需要指定攻击位置
    Vec2                        m_TargetPosExtend;      //目标点差值
    Vec2                        m_PrevPos;              //子弹上一帧的位置
    CRole*                      m_pTarget;              //目标
    CRole*                      m_pOwner;               //子弹发射者
    CBulletComponent*           m_pComponent;           //逻辑组件
    const BulletConfItem*       m_pBulletConf;          //子弹配置
    std::vector<CRole *>        m_Targets;              //目标列表, 逻辑处理完之后存在此处, 提供给表现层
};


#endif 
