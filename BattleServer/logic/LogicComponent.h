/*
 * 逻辑组件
 * 1.控制逻辑帧的帧速
 * 2.统一逻辑回调入口
 * 3.所有逻辑组件的抽象类
 * 
 * 2014-12-19 by 宝爷
 */
#ifndef __LOGIC_COMPONENT_H__
#define __LOGIC_COMPONENT_H__

#include "KxCSComm.h"
#include "BufferData.h"

// 组件类型
enum EComponentType
{
    ELogicComponent,
    EAIComponent,
    EBuffComponent,
    EBulletComponent,
    ERoleComponent,
    ESkillComponent,
    ECallComponent,
    EHeroComponent,
};

class CGameObject;

class CLogicComponent : public Ref, public ISerializable
{
public:
    CLogicComponent();
    virtual ~CLogicComponent();

    //逻辑帧的回调
    virtual void logicUpdate(float delta);

    virtual bool init() { return true; }

    virtual void onEnter() {}

    virtual void onExit();

	virtual bool serialize(CBufferData& data);

	virtual bool unserialize(CBufferData& data);

    inline void setOwner(CGameObject* owner)
    {
        m_pOwner = owner;
    }

    inline CGameObject* getOwner()
    {
        return m_pOwner;
    }

    EComponentType getComponentType()
    { 
        return m_eComponentType; 
    }

protected:
    CGameObject* m_pOwner;
    EComponentType m_eComponentType;
};

#endif
