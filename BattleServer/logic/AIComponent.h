#ifndef __AI_COMPONENT_H__
#define __AI_COMPONENT_H__

#include "KxCSComm.h"
#include "LogicComponent.h"
#include "ConfManager.h"
#include "ConfRole.h"

//AI组件
class CRole;
class CRoleComponent;

class CAIComponent : public CLogicComponent
{
public:
	CAIComponent();
	virtual ~CAIComponent();

	// 初始化一些东西
	virtual bool init(int AIId, CRole* pOwner);
	virtual void logicUpdate(float delta);

    virtual void onExit();

    // 重新加载
    bool reload(int AIId);

    inline CRole* getRole()
    {
        return m_pOwner;
    }

    inline CRoleComponent* getRoleCom()
    {
        return m_pRoleCom;
    }

    inline const AIConfMap* getAIConfMap()
    {
        return m_pAIConf;
    }

	virtual bool serialize(CBufferData& data);

	virtual bool unserialize(CBufferData& data);

protected:
	// 执行条件组
	bool DoAIConditions(const AIConfItem* item);
	// 执行动作组
	void DoAIActions(const AIConfItem* item);

private:
	int						m_CurAIID;				// 当前AI ID
    CRole*					m_pOwner;				// 自身角色
    AIConfMap*				m_pAIConf;				// AI配置
    CRoleComponent*			m_pRoleCom;				// 角色组件
};

#endif