#include "Bullet.h"
#include "BulletComponent.h"
#include "ComponentCache.h"

#ifndef RunningInServer

#include "BulletDisplayComponent.h"
#include "ArmatureComponent.h"

#endif

#include "ConfManager.h"
#include "ConfOther.h"

#include "Count.h"
#include "Role.h"
#include "Hero.h"

CBullet::CBullet()
: m_nState(BST_NONE)
, m_FiringPoint()
, m_TargetPos()
, m_TargetPosExtend()
, m_pTarget(NULL)
, m_pOwner(NULL)
, m_pBulletConf(NULL)
{
    m_nObjectType = EObjectBullet;
}

CBullet::~CBullet()
{
}

bool CBullet::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);
	if (!CGameObject::serialize(data))
	{
		return false;
	}	

    data.writeData(m_nState);
    data.writeData(m_FiringPoint.x);
    data.writeData(m_FiringPoint.y);
    data.writeData(m_TargetPos.x);
    data.writeData(m_TargetPos.y);
    data.writeData(m_TargetPosExtend.x);
    data.writeData(m_TargetPosExtend.y);
	data.writeData(m_PrevPos.x);
    data.writeData(m_PrevPos.y);
    if (NULL == m_pOwner)
    {
        data.writeData(-1);
    }
    else
    {
        data.writeData(m_pOwner->getObjectId());
    }

	if (NULL == m_pTarget)
	{
		data.writeData(-1);
	}
	else
	{
		data.writeData(m_pTarget->getObjectId());
	}
    //子弹逻辑组件序列化
    m_pComponent->serialize(data);

	SERIALIZE_MATCH(data);

    return true;
}

bool CBullet::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);
	if (!CGameObject::unserialize(data))
	{
		return false;
	}

    data.readData(m_nState);
    float x = 0.0f;
    float y = 0.0f;
    data.readData(x);
    data.readData(y);
    m_FiringPoint = Vec2(x, y);
    data.readData(x);
    data.readData(y);
    m_TargetPos = Vec2(x, y);
    data.readData(x);
    data.readData(y);
    m_TargetPosExtend = Vec2(x, y);
    data.readData(x);
    data.readData(y);
    m_PrevPos = Vec2(x, y);

    int objid = 0;
    data.readData(objid);
    if (-1 != objid)
    {
        CRole *tmp = m_pOwner;
        m_pOwner = m_pBattle->getRoleByObjectId(objid);
        SAFE_RETAIN(m_pOwner);
        // target跟旧target是同一个对象, 先release可能会释放掉对象
        if (NULL != tmp)
        {
            SAFE_RELEASE(tmp);
        }
    }
    else
    {
        SAFE_RELEASE(m_pOwner);
        m_pOwner = NULL;
    }

    data.readData(objid);
	if (-1 != objid)
	{
		CRole *tmp = m_pTarget;
        m_pTarget = m_pBattle->getRoleByObjectId(objid);
		SAFE_RETAIN(m_pTarget);
		// target跟旧target是同一个对象, 先release可能会释放掉对象
		if (NULL != tmp)
		{
			SAFE_RELEASE(tmp);
		}
	}
    else
    {
        SAFE_RELEASE(m_pTarget);
		m_pTarget = NULL;
    }

    //子弹逻辑组件序列化
    m_pComponent->unserialize(data);

	UN_SERIALIZE_MATCH(data);

    return true;
}

bool CBullet::init(int bulletId, int uid, int objId, CRole *owner, CBattleHelper* battle)
{
    if (CGameObject::init(bulletId, uid, objId, battle))
	{		
        // 子弹释放者
        m_pOwner = owner;
        // 如果子弹是持续, 死亡的对象还有数据供子弹获取
        SAFE_RETAIN(m_pOwner);
        // 子弹配置 
        m_pBulletConf = queryConfBullet(bulletId);
        if (NULL == m_pBulletConf || NULL == m_pOwner)
        {
            // 没这子弹
            LOG("no this bullet %d", bulletId);
            return false;
        }

        m_nDirection = m_pOwner->getDirection();
        //重置属性
        m_nState = BST_NONE;
        // 子弹发射点
        m_FiringPoint = dynamic_cast<CRole*>(m_pOwner)->getFirePoint();
        
        int param1 = m_pBulletConf->StartParam[0];
        int param2 = m_pBulletConf->StartParam[1];
        int param3 = m_pBulletConf->StartParam[2];

        if (param3 <= 0)
        {
            m_FiringPoint += Vec2(
                static_cast<float>(m_pOwner->getDirection() * param1),
                static_cast<float>(param2));
        }
        else
        {
			int x = battle->RandNum.random(-1 * param3, param3);
			int y = battle->RandNum.random(-1 * param3, param3);
            m_FiringPoint += Vec2(
                static_cast<float>(m_pOwner->getDirection() * (param1 + x)), 
                static_cast<float>(param2 + y));
		}

		KXLOGBATTLE("CBullet init ObjId %d Firing x %f y %f Role x %f y %f", getObjectId(),
			m_FiringPoint.x, m_FiringPoint.y, m_pOwner->getRealPositionX(), m_pOwner->getRealPosition().y);
        // 目标点差值
        param1 = m_pBulletConf->EndParam[0];
        param2 = m_pBulletConf->EndParam[1];
        param3 = m_pBulletConf->EndParam[2];

        if (param3 <= 0)
        {
            m_TargetPosExtend = Vec2(
                static_cast<float>(m_pOwner->getDirection() * param1),
                static_cast<float>(param2));
        }
        else
        {
			int x = battle->RandNum.random(-1 * param3, param3);
			int y = battle->RandNum.random(-1 * param3, param3);
            m_TargetPosExtend = Vec2(
                static_cast<float>(m_pOwner->getDirection() * (param1 + x)),
                static_cast<float>(param2 + y));
        }

        // 子弹逻辑组件
        m_pComponent = createComponent<CBulletComponent>(EBulletComponent);
        CHECK_RETURN(m_pComponent);
        if (!m_pComponent->init(this))
        {
            CComponentCache::getInstance()->freeComponent(m_pComponent);
            KXLOGDEBUG("CBullet bulletComponent init fail bulletId %d", bulletId);
            return false;
        }

#ifndef RunningInServer
        if (m_pBulletConf->ZOrderType == 0)
        {
            setLocalZOrder(m_pOwner->getLocalZOrder());
        }
        else
        {
            setLocalZOrder(m_pBulletConf->ZOrderType - 2);
        }
        setName("Bullet");
        // 子弹显示组件
        CBulletDisplayComponent* m_pDisplayComponent = new CBulletDisplayComponent;
        m_pDisplayComponent->init(this, m_pComponent);
        addComponent(m_pDisplayComponent);
        m_pDisplayComponent->release();
#endif
        return true;
    }
    return false;
}

// 这个函数应该合并到init
void CBullet::onEnter()
{
    KXLOGBATTLE("bullet onEnter ObjId %d", getObjectId());
    CGameObject::onEnter();
    // 延迟添加子弹组件，以便于外部获取子弹设置相关属性
    addLogicComponent(m_pComponent);
    // update为了驱动显示组件
#ifndef RunningInServer
    scheduleUpdate();
#endif // !RunningInServer
}

void CBullet::onExit()
{
    KXLOGBATTLE("bullet onExit ObjId %d", getObjectId());
    CGameObject::onExit();
    SAFE_RELEASE(m_pOwner);
    SAFE_RELEASE(m_pTarget);
}

void CBullet::setTarget(CRole *target)
{
    SAFE_RELEASE(m_pTarget);
    m_pTarget = target;
    SAFE_RETAIN(m_pTarget);
}

CRole* CBullet::getTarget()
{
    return m_pTarget;
}

CRole* CBullet::getOwner()
{
    return m_pOwner;
}

void CBullet::setTargetPos(const Vec2 &pos)
{
    m_TargetPos = pos;
}

Vec2 CBullet::getTargetPos()
{
    switch (m_pBulletConf->EndType)
    {
    case BET_ATTACKERHITPOINT:
		// 攻击者的受击点
        m_TargetPos = dynamic_cast<CRole*>(m_pOwner)->getHitPoint();
        break;

    case BET_ATTACKERMOVEPOINT:
		// 攻击者的自身移动点
        m_TargetPos = m_pOwner->getRealPosition();
        break;

    case BET_TARGETHITPOINT:
        //获得目标
        if (NULL != m_pTarget)
        {
            m_TargetPos = dynamic_cast<CRole*>(m_pTarget)->getHitPoint();
        }
        break;

    case BET_TARGETMOVEPOINT:
        //获得目标
        if (NULL != m_pTarget)
        {
            m_TargetPos = m_pTarget->getRealPosition();
        }
        break;

    case BET_POINT:
        //攻击点使用技能时已赋值
        //m_TargetPos = blackBoard.getTargetPoint();
        break;

    case BET_FRIENDHERO:
        {
			//获得友方英雄
			int camp = m_pOwner->getIntAttribute(EAttributeCamp);
			CRole *pHero = m_pBattle->getMainRole(camp);
            if (NULL != pHero)
            {
                m_TargetPos = pHero->getHitPoint();
            }
        }
        break;

    case BET_ENEMYHERO:
        {
            //获得敌方英雄
			int camp = m_pOwner->getIntAttribute(EAttributeCamp);
			CRole *pHero = m_pBattle->getEnmeyMainRole(camp);
            if (NULL != pHero)
            {
                m_TargetPos = pHero->getHitPoint();
            }
        }
        break;

    case BET_ORIGIN:
        //获得原点坐标
        m_TargetPos = Vec2(0, 0);
        break;
    }

    return m_TargetPos + m_TargetPosExtend;
}

const BulletConfItem* CBullet::getBulletConf()
{
    return m_pBulletConf;
}

Vec2& CBullet::getFiringPoint()
{
    return m_FiringPoint;
}

void CBullet::changeState(int state)
{
    if (m_nState != state)
    {
        m_nState = state;
    }
}

int CBullet::getState()
{
    return m_nState;
}
