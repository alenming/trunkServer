#include "CallComponent.h"
#include "Role.h"

CCallComponent::CCallComponent()
: m_fCallTime(0.0f)
{
    m_eComponentType = ECallComponent;
}

CCallComponent::~CCallComponent()
{
}

bool CCallComponent::init(int roleid, const CallConfItem* callConf)
{
    if (!CRoleComponent::init(roleid))
    {
        return false;
    }

    m_pCallConf = callConf;
    return true;
}

void CCallComponent::onExit()
{
    CRoleComponent::onExit();

    m_fCallTime = 0.0f;
}

void CCallComponent::logicUpdate(float delta)
{
    m_fCallTime += delta;
    if (NULL != m_CurState)
    {
        //等于0表示永久
        if (m_pCallConf->RoleLifeTime > 0 
            && m_fCallTime >= m_pCallConf->RoleLifeTime 
            && m_CurState->getStateId() != State_Death)
        {
            m_fCallTime = 0.0f;
            changeState(State_Death);
        }

        if (m_CurState->isDone())
        {
            // 结束之后自切换状态
            nextState();
        }
        else
        {
            m_CurState->update(delta);
        }
    }

    if (NULL != m_Role && State_Charging == m_Role->currentState())
    {
        m_Role->updateAttribute(EAttributeMP, floatToInt(m_Role->getIntAttribute(EAttributeMPRecover) * delta));
        updateHpBar();
    }
}

bool CCallComponent::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);

	if (!CRoleComponent::serialize(data))
	{
		return false;
	}

    data.writeData(m_fCallTime);
	
	SERIALIZE_MATCH(data);

	return true;
}

bool CCallComponent::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);

	if (!CRoleComponent::unserialize(data))
	{
		return false;
	}

    data.readData(m_fCallTime);

	UN_SERIALIZE_MATCH(data);

	return true;
}
