#include "AIComponent.h"
#include "RoleComponent.h"
#include "Role.h"
#include "AIConditions.h"
#include "AIActions.h"

using namespace std;

CAIComponent::CAIComponent()
: m_CurAIID(0)
{
    m_eComponentType = EAIComponent;
	m_pOwner = NULL;
	m_pRoleCom = NULL;
	m_pAIConf = NULL;
}

CAIComponent::~CAIComponent()
{
	m_pOwner = NULL;
	m_pRoleCom = NULL;
	m_pAIConf = NULL;
}

bool CAIComponent::init(int AIId, CRole* pOwner)
{
	//AI配置
	m_pAIConf = const_cast<AIConfMap*>(queryConfAIMap(AIId));
	CHECK_RETURN(m_pAIConf);

	m_CurAIID = AIId;

	//自身角色
	CRole* pRole = dynamic_cast<CRole*>(pOwner);
	CHECK_RETURN(pRole);
	m_pOwner = pRole;
	
	//自身角色组件
	CRoleComponent* comRole = m_pOwner->getRoleComponent();
	CHECK_RETURN(comRole);
    m_pRoleCom = comRole;	

	return true;
}

void CAIComponent::logicUpdate(float delta)
{
	// 如果当前状态没有匹配的ID，则直接返回
	int stateID = m_pRoleCom->getStateId();
	const std::vector<AIConfItem*>* items = m_pAIConf->getAIItems(stateID);
	CHECK_RETURN_VOID(items);

	// 遍历AI规则组
	for(std::vector<AIConfItem*>::const_iterator iter = items->begin();
		iter != items->end(); ++iter)
	{
		// 遍历当前AI规则组的所有条件
		if(DoAIConditions(*iter))
		{
			// 条件成立则执行匹配的动作组，并结束AI
			DoAIActions(*iter);
			break;
		}
	}
}

void CAIComponent::onExit()
{
    CLogicComponent::onExit();
}

bool CAIComponent::reload(int AIId)
{
	if (m_CurAIID == AIId)
	{
		return false;
	}

    return init(AIId, m_pOwner);
}

bool CAIComponent::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);

	if (!CLogicComponent::serialize(data))
	{
		return false;
	}

	data.writeData(m_CurAIID);

	SERIALIZE_MATCH(data);

	return true;
}

bool CAIComponent::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);

	if (!CLogicComponent::unserialize(data))
	{
		return false;
	}

	int nAIID = 0;
	data.readData(nAIID);

	// 状态更换
	reload(nAIID);

	UN_SERIALIZE_MATCH(data);

	return true;
}

bool CAIComponent::DoAIConditions(const AIConfItem* item)
{
	CAIConditions condition;
	for(std::vector<CDataFunction*>::const_iterator iter = item->Condition.begin();
		iter != item->Condition.end(); ++iter)
	{
		//如果空白，直接跳过0
        if (!condition.DoAICondition(this, *iter))
        {
            return false;
        }
	}
	return true;
}

void CAIComponent::DoAIActions(const AIConfItem* item)
{
	CAIActions action;
	for(std::vector<CDataFunction*>::const_iterator iter = item->Action.begin();
        iter != item->Action.end(); ++iter)
	{
		action.DoAIAction(this, *iter);
	}
}
