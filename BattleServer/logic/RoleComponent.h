/*
* 角色组件
* 执行状态逻辑
* 实现状态切换
* 角色属性恢复流程——怒气和蓝
*
* 2014-12-24 by 宝爷
*/
#ifndef __STATE_COMPONENT_H__
#define __STATE_COMPONENT_H__

#include "LogicComponent.h"
#include "State.h"
#include "ConfManager.h"
#include "ConfRole.h"

class CState;
class CRole;
class CRoleComponent : public CLogicComponent
{
public:
    CRoleComponent();
    virtual ~CRoleComponent();

    // 传入状态配置，初始化状态组件内部
    virtual bool init(int roleId);

    virtual void onEnter();
    virtual void onExit();

	//序列化,反序列化 角色组件数据
	bool serialize(CBufferData& data);
	bool unserialize(CBufferData& data);

    // 执行状态组件的逻辑更新
    virtual void logicUpdate(float delta);

    // 进行状态切换，切换成功返回true，失败返回false
    virtual CState* changeState(int stateId);

    // 重新加载
    bool reload(int roleId);

    int getStateId();

    const StatusConfItem* getStateConf();

    inline int getStateConfId() { return m_nStateConfId; }

    // 更新血条
    void updateHpBar();

protected:
    // 切换到下一个状态
    bool nextState();

    // 传入状态ID，动态创建
    CState* getState(int stateId);


protected:
    float m_fRecoverTick;
    int m_nStateConfId;                         // 状态配置表的ID
    std::map<int, CState*> m_StateMap;          // 状态容器
    CState* m_CurState;                         // 当前状态
    CRole* m_Role;
};

#endif
