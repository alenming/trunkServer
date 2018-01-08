/*
 * GameObject游戏对象，对存在于游戏场景中的所有对象的定义
 *
 * 1.封装EventManager，BattleHelper等对象指针（前后端差异封装）
 * 2.封装公共属性map，提供修改和获取方法
 *
 * 2014-12-19 by 宝爷
 */
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "KxCSComm.h"
#include "BattleHelper.h"
#include "LogicComponent.h"
#include "BufferData.h"
#include "EventManager.h"

class CBattleHelper;

enum EObjectType
{
    EObjectHero,        // 英雄
    EObjectSoldier,     // 士兵
    EObjectCall,        // 召唤物
	EObjectBoss,		//BOSS
	EObjectMonster,		//怪物
    EObjectBullet,      // 子弹
};

class CGameObject : public Node, public ISerializable
{
public:
    CGameObject();
    virtual ~CGameObject();

    // 传入对象类型ID，所有者ID，对象唯一ID，战斗单例
    virtual bool init(int typeId, int ownerId, int objId, CBattleHelper* battle);

    virtual void onExit();

    // 清空数据
    virtual void reset();

    ///////////////////////////////序列化和反序列化/////////////////////////////////////

    // 将游戏场景打包到data中
    virtual bool serialize(CBufferData& data);
    // 从data中解压出游戏场景
    virtual bool unserialize(CBufferData& data);

    //////////////////////////////////////////////////////////////////////////

    void addLogicComponent(CLogicComponent* com);

    void clearLogicComponents();

    void updateLogic(float dt);

    void remove();

    //////////////////////////////////////////////////////////////////////////

    // 设置逻辑位置与显示位置
    inline void setAllPosition(const Vec2& pos)
    {
        m_Position = pos;
		setPosition(pos);
    }

    inline const Vec2& getRealPosition()
    {
        return m_Position;
    }

    inline void setRealPosition(const Vec2& pos)
    {
        m_Position = pos;
    }

    inline float getRealPositionX() const
    {
        return m_Position.x;
    }

    inline float getRealPositionY() const
    {
        return m_Position.y;
    }

    inline void setRealPositionX(float x)
    {
        m_Position.x = x;
    }

    inline void setRealPositionY(float y)
    {
        m_Position.y = y;
    }

    inline void addRealPosition(const Vec2& v)
    {
        m_Position += v;
    }

    inline float addRealPositionX(float x)
    {
        return m_Position.x += x;
    }

    inline float addRealPositionY(float y)
    {
        return m_Position.y += y;
    }

    // 获取EventManager
    CEventManager<int>* getEventManager() { return m_pEventManager; }
    // 获取BattleHelper
    CBattleHelper* getBattleHelper() { return m_pBattle; }
    // 获取对象的类型ID，子弹，英雄，士兵，召唤物，场景召唤物
    inline EObjectType getObjectType() { return m_nObjectType; }
    // 获取对象的唯一ID
    inline int getObjectId() { return m_nGameObjectId; }
    // 获取类型ID，如果是士兵和Monster，这里将返回Index，而不是类型ID
    inline int getTypeId() { return m_nTypeId; }
    // 获取所有者的ID
    inline int getOwnerId() { return m_nOwnerId; }
    // 获取方向 1为向右，-1为向左
    inline int getDirection() { return m_nDirection; }
    // 是否可被删除
    inline bool canRemove() { return m_bSafeRemove; }
    // 删除对象
    inline void safeRemove() { m_bSafeRemove = true; }

    // 真正死亡
    inline void realDead(){ m_bRealDead = true; }
    // 是否真正死亡
    inline bool isRealDead(){ return m_bRealDead; }

protected:
    bool m_bRealDead;                               // 是否已经真的死亡了
    bool m_bSafeRemove;                             // 是否可被安全地移除
    int m_nDirection;                               // 方向    
    int m_nGameObjectId;                            // 对象唯一ID
    int m_nOwnerId;                                 // 主人的UserID
    int m_nTypeId;                                  // 类型ID（英雄ID，士兵ID，子弹ID）
    EObjectType m_nObjectType;                      // 对象类型（子弹，英雄，士兵，召唤物，场景召唤物）
    Vec2 m_Position;                                // 逻辑位置

    std::list<CLogicComponent*> m_LogicComponents;  // 逻辑组件列表
    std::map<int, int> m_IntAttributes;             // Int属性
    std::map<int, float> m_FloatAttributes;         // Float属性
    CEventManager<int>* m_pEventManager;            // 事件对象
    CBattleHelper* m_pBattle;                       // 战斗辅助类
};

#endif