#include "State.h"
#include "Role.h"
#include "KxCSComm.h"
#include "KxLog.h"
#include "Count.h"

CState::CState()
:m_bIsDone(true),
m_fDuration(0.0f),
m_nNextStateId(0),
m_nStateId(0),
m_pComponent(NULL),
m_pOwner(NULL),
m_pStateConf(NULL)
{

}

CState::~CState()
{

}

bool CState::init(int stateId, const StatusConfItem* confItem, CRoleComponent* component)
{
    CHECK_RETURN(confItem);
    // 加载状态配置
    m_nNextStateId = confItem->NextStateId;
    m_pStateConf = confItem;

    // 初始化自身状态
    m_bIsDone = true;
    m_nStateId = stateId;
    m_fDuration = 0.0f;
    m_pComponent = component;

    return true;
}

void CState::update(float dt)
{
    // 通用状态逻辑，冰冻，死亡等等
    m_fDuration -= dt;
    if (m_fDuration <= 0.0f && m_pStateConf->LifeTime != 0)
    {
        m_bIsDone = true;
    }
}

void CState::onEnter(bool isFouce)
{
    // 初始化自身状态
    m_pOwner = dynamic_cast<CRole*>(m_pComponent->getOwner());
    m_fDuration = m_pStateConf->LifeTime;
    m_nNextStateId = m_pStateConf->NextStateId;
    m_bIsDone = false;

    if (!isFouce && m_pStateConf->Action != NULL)
    {
        // 进入状态时的额外动作
        switch (m_pStateConf->Action->NameID)
        {
        case StateAction_AddBuffForMeOnce:
            {
				VecVecInt& buffIds = m_pStateConf->Action->ParamIntArr;
                for (unsigned int i = 0; i < buffIds.size(); ++i)
                {
                    if (buffIds[i].size() >= 2)
                    {
                        int buffId = buffIds[i][0];
                        int stack = buffIds[i][1];
                        // 如果没有该buffId，
                        if (!m_pOwner->haveBuff(buffId))
                        {
                            m_pOwner->addBuff(m_pOwner, buffId, stack);
                        }
                    }
                }
            }
            break;
        case StateAction_DoCount:
            if (m_pStateConf->Action->ParamInt.size() > 0)
            {
                CCount::roleExecute(m_pStateConf->Action->ParamInt[0], m_pOwner, m_pOwner);
            }
            break;
        default:
            break;
        }
    }
    
    // 攻击速率影响
    switch (m_pStateConf->SpeedAffect)
    {
    case StatusSpeed_Attack:
        m_fDuration /= m_pOwner->getFloatAttribute(EAttributeAttackSpeedVar);
        break;
    case StatusSpeed_MPRecover:
        m_fDuration = m_fDuration * m_pOwner->getIntAttribute(EClassMPRecover) / m_pOwner->getIntAttribute(EAttributeMPRecover);
        break;
    default:
        break;
    }

    KXLOGBATTLE("State onEnter ownerObjId %d stateId %d Duration %f x %f y %f", m_pOwner->getObjectId(),
        m_pStateConf->StateId, m_fDuration, m_pOwner->getRealPositionX(), m_pOwner->getRealPosition().y);
}

void CState::onExit(bool isFouce)
{
    if (isNegativeState(m_nStateId) 
        && !isNegativeState(m_nNextStateId)
        && NULL != m_pOwner
        && !isFouce)
    {
        m_pOwner->setAttribute(EAttributeStrong, m_pOwner->getIntAttribute(EAttributeMaxStrong));
	}

    if (State_Charging == m_nStateId && !isFouce)
    {
        m_pOwner->setAttribute(EAttributeMP, 0);
        m_pComponent->updateHpBar();
    }
	
    KXLOGBATTLE("State onExit ownerObjId %d stateId %d x %f y %f", m_pOwner->getObjectId(),
		m_pStateConf->StateId, m_pOwner->getRealPositionX(), m_pOwner->getRealPosition().y);
}

bool CState::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);
	data.writeData(m_bIsDone);
	data.writeData(m_fDuration);
	data.writeData(m_nNextStateId);
	SERIALIZE_MATCH(data);
	return true;
}

bool CState::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);
	data.readData(m_bIsDone);
	data.readData(m_fDuration);
	data.readData(m_nNextStateId);
	UN_SERIALIZE_MATCH(data);
	return true;
}

void CStateMove::update(float dt)
{
    CState::update(dt);

    // 一直向前移动，直到其他状态来切换
    float speed = m_pOwner->getIntAttribute(EAttributeSpeed) * 1.0f;
	m_pOwner->addRealPositionX(dt * speed * m_pOwner->getDirection());
	
    //KXLOGBATTLE("CStateMove update ownerObjId %d x %f y %f", m_pOwner->getObjectId(),
    //    m_pOwner->getRealPositionX(), m_pOwner->getRealPosition().y);
}

bool CStateMove::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);

	//序列化基类数据
	CState::serialize(data);

	//序列化状态数据
	data.writeData(m_fSpeed);
	data.writeData(m_fAcceleration);

	SERIALIZE_MATCH(data);

	return true;
}

bool CStateMove::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);

	//反序列化基类数据
	CState::unserialize(data);

	//反序列化状态数据
	data.readData(m_fSpeed);
	data.readData(m_fAcceleration);

	UN_SERIALIZE_MATCH(data);

	return true;
}

void CStateHitPower::update(float dt)
{
    // 如果是击飞
    if (m_fPowerY > 0.0f)
    {
        // 当Y小于等于0时结束
        m_fVy += m_fG;
        m_pOwner->addRealPositionX(dt * m_fVx * m_pOwner->getDirection() * -1);
        if (m_pOwner->addRealPositionY(dt * m_fVy) <= m_fBaseY)
        {
            m_pOwner->setRealPositionY(m_fBaseY);

            // 结束状态
            m_bIsDone = true;
        }
    }
    // 如果是击退
    else
    {
        m_fVx += m_fA * m_fG;
        if (m_fVx > 0.0f)
        {
            m_pOwner->addRealPositionX(dt * m_fVx * m_pOwner->getDirection() * -1);
        }
        else
        {
            // 结束状态
            m_bIsDone = true;
        }
    }

    //KXLOGBATTLE("CStateHitPower update ownerObjId %d x %f y %f", m_pOwner->getObjectId(),
    //    m_pOwner->getRealPositionX(), m_pOwner->getRealPosition().y);
}

void CStateHitPower::onEnter(bool isFouce)
{
    CState::onEnter(isFouce);
	float mass = m_pOwner->getFloatAttribute(EAttributeMass);
	float powerX = m_pOwner->getFloatAttribute(EAttributeHitPowerX);
	m_fPowerY = m_pOwner->getFloatAttribute(EAttributeHitPowerY);
	m_fBaseY = m_pOwner->getFloatAttribute(EAttributeYmin);

    //m_fA = cocos2d::UserDefault::getInstance()->getFloatForKey("mocali", 0.8f);
    //m_fG = cocos2d::UserDefault::getInstance()->getFloatForKey("zhongli", -49.0f);
    m_fA = 2.5f;
    m_fG = -250;

    m_fVx = (powerX / mass);
    m_fVy = (m_fPowerY / mass); //(m_fPowerY / mass);
}

bool CStateHitPower::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);

	//序列化基类数据
	CState::serialize(data);

	//序列化状态数据
	data.writeData(m_fVx);
	data.writeData(m_fVy);

	SERIALIZE_MATCH(data);

	return true;
}

bool CStateHitPower::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);

	//反序列化基类数据
	CState::unserialize(data);

	//反序列化状态数据
	data.readData(m_fVx);
	data.readData(m_fVy);

	UN_SERIALIZE_MATCH(data);

	return true;
}

void CStateDeath::onEnter(bool isFouce)
{
    CState::onEnter(isFouce);
    if (m_pOwner && !isFouce)
    {
        m_pOwner->realDead();
        CBuffComponent* buffCom = m_pOwner->getBuffComponent();
        if (buffCom)
        {
            buffCom->clear();
        }
    }
}

void CStateDeath::update(float dt)
{
	CState::update(dt);
	if (m_bIsDone)
	{
		m_pOwner->safeRemove();
	}
}
