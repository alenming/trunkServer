#include "AIActions.h"
#include "ConfManager.h"
#include "AIComponent.h"
#include "RoleComponent.h"
#include "Role.h"
#include "Count.h"

#ifndef RunningInServer
#include "LuaSummonerBase.h"
#endif
using namespace std;

CAIActions::CAIActions()
{
    m_pAICom = NULL;
    m_pOwner = NULL;
}

CAIActions::~CAIActions()
{
    // 弱引用指针
    m_pAICom = NULL;
    m_pOwner = NULL;
}

void CAIActions::DoAIAction(CAIComponent* com, const CDataFunction* func)
{
	CHECK_RETURN_VOID(com);
	m_pAICom = com;
    m_pOwner = dynamic_cast<CRole*>(m_pAICom->getOwner());
    CHECK_RETURN_VOID(m_pOwner);

    KXLOGBATTLE("ObjId %d DoAIAction %d", m_pOwner->getObjectId(), func->NameID);
	switch (func->NameID)
	{
	case AIAction_Do_None:
		{
			AIActionDoNone();
            break;
		}
    case AIAction_Do_LockMe:
        {
            AIActionDoLockMe();
            break;
        }
	case AIAction_Do_Clear:
		{
            AIActionDoClear();
            break;
		}
	case AIAction_Do_ChangeStatus:
		{
			AIActionDoChangeStatus(func->ParamInt[0]);
            break;
		}
	case AIAction_Do_CastSkill:
		{
			AIActionDoCastSkill(func->ParamInt[0]);
            break;
		}
	case AIAction_Do_LockPoints:
		{
			AIActionDoLockPoints(func->ParamInt);
            break;
		}
    case AIAction_Do_LockPointsForMe:
        {
            AIActionDoLockPointsForMe(func->ParamInt);
            break;
        }
    case AIAction_Do_AddBuffForMe:
        {
            AIActionDoAddBuffForMe(func->ParamIntArr);
            break;
        }
	case AIAction_Do_DelBuffForMe:
		{
			AIActionDoDelBuffForMe(func->ParamIntArr[0]);
			break;
		}
    case AIAction_Do_Count:
        AIActionDoCount(func->ParamInt[0]);
        break;
#ifndef RunningInServer
    case AIAction_Do_LuaEvent:
        AIActionDoLuaEvent(func->ParamInt[0]);
        break;
#endif
	default:
		break;
	}
}

void CAIActions::AIActionDoNone()
{
	return;
}

void CAIActions::AIActionDoLockMe()
{
    // 空黑板上的目标列表，把我写进目标列表之中
    m_pOwner->getBlackBoard().clearAndAddToTargetList(m_pOwner);
}

void CAIActions::AIActionDoClear()
{
    m_pOwner->getBlackBoard().clearBlackBoard();
}

void CAIActions::AIActionDoChangeStatus(int stateid)
{
	//切换状态需要考虑优先级
    m_pOwner->changeState(stateid);
}

void CAIActions::AIActionDoCastSkill(int skillIndex)
{
    m_pOwner->executeSkill(skillIndex, 
        m_pOwner->getBlackBoard().getTargetList(), 
        m_pOwner->getBlackBoard().getTargetPoint());
}

void CAIActions::AIActionDoLockPoints(const VecInt& point)
{
    m_pOwner->getBlackBoard().setTargetPoint(
        Vec2(static_cast<float>(point[0]), static_cast<float>(point[1])));
}

void CAIActions::AIActionDoLockPointsForMe(const VecInt& point)
{
    m_pOwner->getBlackBoard().setTargetPoint(
        m_pOwner->getRealPosition() + Vec2(
        static_cast<float>(point[0] * m_pOwner->getDirection()), static_cast<float>(point[1])));
}

void CAIActions::AIActionDoAddBuffForMe(const VecVecInt& buffIds)
{
	for (unsigned int i = 0; i < buffIds.size(); ++i)
	{
		if (buffIds[i].size() >= 2)
		{
			int buffId = buffIds[i][0];
			int stack = buffIds[i][1];
			m_pOwner->addBuff(m_pOwner, buffId, stack);
		}
	}
}

void CAIActions::AIActionDoDelBuffForMe(const VecInt& buffIds)
{
	for (VecInt::const_iterator iter = buffIds.begin(); iter != buffIds.end(); ++iter)
	{
		m_pOwner->delBuff(*iter);
	}
}

void CAIActions::AIActionDoCount(int countId)
{
    if (m_pOwner->getBlackBoard().getTargetList().size() > 0)
    {
        CCount::roleExecute(countId, m_pOwner, m_pOwner->getBlackBoard().getTargetList()[0]);
    }
}
#ifndef RunningInServer
void CAIActions::AIActionDoLuaEvent(int countId)
{
    onLuaEvent(countId);
}
#endif
