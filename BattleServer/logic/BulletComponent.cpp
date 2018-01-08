#include "BulletComponent.h"
#include "Bullet.h"
#include "Role.h"
#include "Count.h"

#ifndef RunningInServer
#include "EffectFactory.h"
#endif

CBulletComponent::CBulletComponent()
: m_fBulletExsitTime(0.0f)
, m_pOwner(NULL)
, m_pBulletPath(NULL)
, m_pBulletHitJudge(NULL)
{
    m_eComponentType = EBulletComponent;
}

CBulletComponent::~CBulletComponent()
{
    SAFE_DELETE(m_pBulletPath);
    SAFE_DELETE(m_pBulletHitJudge);
}

bool CBulletComponent::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);
	CLogicComponent::serialize(data);

    data.writeData(m_fBulletExsitTime);
    // 目标个数   
    int nTargetCnt = m_TargetList.size();
    data.writeData(nTargetCnt);
    for (int i = 0; i < nTargetCnt; ++i)
    {
        data.writeData(m_TargetList[i]->getObjectId());
    }
    // 子弹轨迹
    m_pBulletPath->serialize(data);
    // 子弹命中判定
    m_pBulletHitJudge->serialize(data);
	SERIALIZE_MATCH(data);
    return true;
}

bool CBulletComponent::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);
	CLogicComponent::unserialize(data);

    CBattleHelper *helper = m_pOwner->getBattleHelper();
    data.readData(m_fBulletExsitTime);
    // 目标个数   
    int nTargetCnt;
    data.readData(nTargetCnt);
    m_TargetList.clear();
    for (int i = 0; i < nTargetCnt; ++i)
    {
        int objId;
        data.readData(objId);
        CRole * target = helper->getRoleByObjectId(objId);
        if (NULL == target)
        {
            // 前后端不一致
            LOG("bullet 111111111111111111111111111111 ================ error information!");
            continue;
        }
        
        m_TargetList.push_back(target);
    }
    // 子弹轨迹
    m_pBulletPath->unserialize(data);
    // 子弹命中判定
    m_pBulletHitJudge->unserialize(data);
	UN_SERIALIZE_MATCH(data);
    return true;
}

bool CBulletComponent::init(CBullet *bullet)
{
    m_pOwner = bullet;
    CHECK_RETURN(m_pOwner);
    m_pHelper = m_pOwner->getBattleHelper();
    CHECK_RETURN(m_pHelper);

    KXLOGBATTLE("bulletComponent init bulletObjId %d ownerObjId %d bulletId %d",
        bullet->getObjectId(), m_pOwner->getObjectId(), m_pOwner->getBulletConf()->ID);

    //子弹轨迹
    switch (m_pOwner->getBulletConf()->Locus)
    {
    case BPT_NONE:
        m_pBulletPath = new CBulletDuration;
        break;
    case BPT_PARABOLA:
        m_pBulletPath = new CBulletParabola;
        break;
    case BPT_TRACK:
        //m_pBulletPath = new CBulletTrack;
        break;
    case BPT_DURATION:
        m_pBulletPath = new CBulletDuration;
        break;
    case BPT_PIERCE:
        m_pBulletPath = new CBulletPierce;
        break;
    default:
        break;
    }
    if (NULL != m_pBulletPath)
    {
        if (!m_pBulletPath->init(m_pOwner))
        {
            return false;
        }
    }
    //命中判定
    m_pBulletHitJudge = new CBulletHitJudge;
    if (!m_pBulletHitJudge->init(m_pOwner, this))
    {
        return false;
    }
    return true;
}

void CBulletComponent::onEnter()
{ 
    if (NULL != m_pBulletPath)
    {
        m_pBulletPath->onEnter();
    }
}

void CBulletComponent::onExit()
{
    CLogicComponent::onExit();

    if (NULL != m_pBulletPath)
    {
        m_pBulletPath->onExit();
    }
    m_fBulletExsitTime = 0.0f;
    SAFE_DELETE(m_pBulletPath);
    SAFE_DELETE(m_pBulletHitJudge);
    m_TargetList.clear();
}

bool CBulletComponent::isLogicOver()
{
    return m_fBulletExsitTime > m_pOwner->getBulletConf()->LifeTime;
}

bool CBulletComponent::isFlyToScreenOut()
{
    return m_pOwner->getRealPositionX() > m_pHelper->MaxX
        || m_pOwner->getRealPositionX() < m_pHelper->MinX;
}

void CBulletComponent::logicUpdate(float delta)
{
    m_fBulletExsitTime += delta;

    if (isLogicOver()
        || isFlyToScreenOut())
    {
        KXLOGBATTLE("bullet over ownObjId %d X %f Y %f ExsitTime %f ", m_pOwner->getObjectId(), 
            m_pOwner->getRealPositionX(), m_pOwner->getRealPosition().y, m_fBulletExsitTime);

        m_pOwner->safeRemove();
        return;
    }

    // 子弹轨迹移动
    if (!m_pBulletPath->isPathOver())
    {
        m_pBulletPath->update(delta);
    }
    
    // 命中判定
    if (!m_pBulletHitJudge->isJudgeOver())
    {
        switch (m_pOwner->getBulletConf()->HitType)
        {
            //结束后判定
        case BHT_FLYEND:
            if (m_pBulletPath->isPathOver())
			{
                m_pBulletHitJudge->update(delta);
            }
            break;
            //飞行中判定
		case BHT_FLYING:
            m_pBulletHitJudge->update(delta);
            break;
            //碰撞时判定
		case BHT_FLYCOLLISION:
            m_pBulletHitJudge->update(delta);
            if (m_pBulletHitJudge->isNeedCount())
            {
                m_pBulletPath->setPathOver();
                m_pBulletHitJudge->setJudgeOver();
            }
            break;
        default:
            break;
        }

        if (m_pBulletHitJudge->isNeedCount())
		{
            KXLOGBATTLE("bulletHitJudge needCount bulletObjId %d", m_pOwner->getObjectId());
            //结算目标
            m_pBulletHitJudge->countRole();
            //增加buff
            m_pBulletHitJudge->roleAddBuff();
            if (!m_TargetList.empty())
            {
                // 播放命中动画
                playHitEffect();
                // 清空目标
                m_TargetList.clear();
            }
        }
    }
    else
    {
        // 判定结束后必然移除
        m_fBulletExsitTime = m_pOwner->getBulletConf()->LifeTime;
    }
}    

void CBulletComponent::addHitTarget(CRole* target)
{
    if (NULL != target)
    {
        m_TargetList.push_back(target);
    }
}

std::vector<CRole*>& CBulletComponent::getTargets()
{
    return m_TargetList;
}

void CBulletComponent::playHitEffect()
{
#ifndef RunningInServer
    for (auto role : m_TargetList)
    {
		CEffectFactory::createEffectsToNode(
            m_pOwner->getBulletConf()->HitAnimationId, role->getDisplayNode(),
           m_pOwner->getDirection(), 0, 0.0f, role->getBaseHitOffset());
    }

    if (!m_pOwner->getBulletConf()->HitAllAnimationId.empty())
    {
        CBattleLayer *pBattle = m_pOwner->getBattleHelper()->getBattleScene();
        CEffectFactory::createEffectsToNode(m_pOwner->getBulletConf()->HitAllAnimationId,
            pBattle, m_pOwner->getDirection(), m_pOwner->getLocalZOrder(), 0.0f, m_pOwner->getRealPosition());
    }
#endif
}

/************************************************************************
子弹轨迹
************************************************************************/
CBulletPathBase::CBulletPathBase()
: m_bIsPathOver(false)
, m_fExsitTime(0.0f)
, m_pBullet(NULL)
, m_pBulletConf(NULL)
{
}

CBulletPathBase::~CBulletPathBase()
{
}

bool CBulletPathBase::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);

    data.writeData(m_bIsPathOver);
    data.writeData(m_fExsitTime);

	SERIALIZE_MATCH(data);

    return true;
}

bool CBulletPathBase::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);

    data.readData(m_bIsPathOver);
    data.readData(m_fExsitTime);

	UN_SERIALIZE_MATCH(data);

    return true;
}

bool CBulletPathBase::init(CBullet* bullet)
{
    if (NULL == bullet)
    {
        return false;
    }

    m_pBullet = bullet;
	m_pBullet->setAllPosition(bullet->getFiringPoint());
    m_pBullet->setPrevPosition(bullet->getFiringPoint());
    m_pBulletConf = bullet->getBulletConf();

    KXLOGBATTLE("CBulletPathBase init bulletObjId %d X %f Y %f", m_pBullet->getObjectId(),
        m_pBullet->getFiringPoint().x, m_pBullet->getFiringPoint().y);
    return true;
}

void CBulletPathBase::onEnter()
{
    KXLOGBATTLE("CBulletPathBase onEnter bulletObjId %d", m_pBullet->getObjectId());
    m_pBullet->changeState(BST_FLYING);
}

void CBulletPathBase::onExit()
{
    KXLOGBATTLE("CBulletPathBase onExit bulletObjId %d", m_pBullet->getObjectId());
}

void CBulletPathBase::update(float dt)
{
    m_fExsitTime += dt;
}

void CBulletPathBase::setPathOver(bool isOver)
{
    if (isOver)
    {
        m_pBullet->changeState(BST_FLYEND);
        //强制设置最后这一帧到了真实位置
        //m_pBullet->setPosition(m_pBullet->getRealPosition());
    }

    m_bIsPathOver = isOver;
}

bool CBulletPathBase::isPathOver()
{
    return m_bIsPathOver;
}

//抛物线轨迹
CBulletParabola::CBulletParabola()
{
}

CBulletParabola::~CBulletParabola()
{
}

bool CBulletParabola::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);
    CBulletPathBase::serialize(data);
    data.writeData(m_fFlyTime);
    data.writeData(m_fVx);
    data.writeData(m_fVy);
	SERIALIZE_MATCH(data);
	return true;
}

bool CBulletParabola::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);
    CBulletPathBase::unserialize(data);
    data.readData(m_fFlyTime);
    data.readData(m_fVx);
    data.readData(m_fVy);
	UN_SERIALIZE_MATCH(data);
    return true;
}

bool CBulletParabola::init(CBullet* bullet)
{
    if (NULL == bullet
        || !CBulletPathBase::init(bullet)
        || m_pBulletConf->Locus_Param.size() < 2)
    {
        return false;
    }
    return true;
}

void CBulletParabola::onEnter()
{
    CBulletPathBase::onEnter();
    const Vec2& start = m_pBullet->getFiringPoint();
    const Vec2& end = m_pBullet->getTargetPos();
    m_fVx = m_pBulletConf->Locus_Param[0];
    m_fG = m_pBulletConf->Locus_Param[1];
    m_fFlyTime = fabs(end.x - start.x) / m_fVx;
    m_fVy = (end.y - start.y) / m_fFlyTime - 0.5f * m_fG * m_fFlyTime;
	m_fG *= 0.5f;
    if (start.x > end.x)
    {
        m_fVx *= -1.0f;
    }
}

void CBulletParabola::update(float dt)
{
    CBulletPathBase::update(dt);
    m_pBullet->setPrevPosition(m_pBullet->getRealPosition());
    m_pBullet->addRealPositionX(m_fVx * dt);
	m_pBullet->setRealPositionY(m_pBullet->getFiringPoint().y 
		+ m_fG * m_fExsitTime * m_fExsitTime + m_fVy * m_fExsitTime);

    //KXLOGBATTLE("CBulletParabola update bulletObjId %d x %f y %f", m_pBullet->getObjectId(),
    //     m_pBullet->getRealPositionX(), m_pBullet->getRealPosition().y);

	if (m_fExsitTime >= m_fFlyTime)
    {
        setPathOver();
    }
}

//持续轨迹
CBulletDuration::CBulletDuration()
{
}

CBulletDuration::~CBulletDuration()
{
}

bool CBulletDuration::init(CBullet* bullet)
{
    return CBulletPathBase::init(bullet);
}

void CBulletDuration::onEnter()
{
    CBulletPathBase::onEnter();
    m_pBullet->setAllPosition(m_pBullet->getTargetPos());
    m_pBullet->setPrevPosition(m_pBullet->getTargetPos());
}

void CBulletDuration::update(float dt)
{
    CBulletPathBase::update(dt);
    //子弹存在时间到, 轨迹结束
    if (m_fExsitTime >= m_pBullet->getBulletConf()->FlyTime)
    {
        setPathOver();
    }
}

//直线穿透
CBulletPierce::CBulletPierce()
: m_fSpeed(0.0f)
, m_fPullBackTime(0.0f)
{
}

CBulletPierce::~CBulletPierce()
{
}

bool CBulletPierce::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);
    CBulletPathBase::serialize(data);
    // 其他属性在初始化时自动计算
    data.writeData(m_fSpeed);
    data.writeData(m_fPullBackTime);
	SERIALIZE_MATCH(data);
    return true;
}

bool CBulletPierce::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);
    CBulletPathBase::unserialize(data);
    data.readData(m_fSpeed);
    data.readData(m_fPullBackTime);
	UN_SERIALIZE_MATCH(data);
	return true;
}

bool CBulletPierce::init(CBullet* bullet)
{
    if (CBulletPathBase::init(bullet)
        && m_pBulletConf->Locus_Param.size() == 2)
    {
        return true;
    }
    return false;
}

void CBulletPierce::onEnter()
{
    CBulletPathBase::onEnter();
    m_fSpeed = m_pBulletConf->Locus_Param[0];
    m_fPullBackTime = m_pBulletConf->Locus_Param[1] / 1000.0f;
    if (m_pBullet->getFiringPoint().x > m_pBullet->getTargetPos().x)
    {
        m_fSpeed *= -1.0f;
    }
}

void CBulletPierce::update(float dt)
{
    CBulletPathBase::update(dt);
    if (m_fPullBackTime > 0.0f && m_fExsitTime >= m_fPullBackTime)
    {
        // 折返速度相反
        m_fSpeed *= -1.0f;
        // 只折返一次，所以置为0的关闭折返
        m_fPullBackTime = 0.0f;
    }
    m_pBullet->setPrevPosition(m_pBullet->getRealPosition());
	m_pBullet->addRealPositionX(m_fSpeed * dt);
    //KXLOGBATTLE("CBulletPierce update bulletObjId %d X %f Y %f", m_pBullet->getObjectId(), 
    //    m_pBullet->getRealPositionX(), m_pBullet->getRealPosition().y);

    // 子弹飞行帧数到期, 轨迹结束
    if (m_fExsitTime >= m_pBullet->getBulletConf()->FlyTime)
    {
        setPathOver();
    }
}

/************************************************************************
命中判定
************************************************************************/
CBulletHitJudge::CBulletHitJudge()
: m_nHitType(0)
, m_nJudgeTimes(0)
, m_fFirstJudgeTime(0.0f)
, m_fInterval(0.0f)
, m_fParam(0.0f)
, m_bNeedCount(false)
, m_bJudgeOver(false)
, m_pBullet(NULL)
, m_pBulletComponent(NULL)
{
}

CBulletHitJudge::~CBulletHitJudge()
{
}

bool CBulletHitJudge::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);

    data.writeData(m_nJudgeTimes);
    data.writeData(m_fFirstJudgeTime);
    data.writeData(m_fInterval);
    data.writeData(m_fParam);
    data.writeData(m_bNeedCount);
    data.writeData(m_bJudgeOver);
    //最大结算次数
    int cnt = m_TargetCountTimes.size();
    data.writeData(cnt);
    for (std::map<int, int>::iterator iter = m_TargetCountTimes.begin();
        iter != m_TargetCountTimes.end(); ++iter)
    {
        data.writeData(iter->first);
        data.writeData(iter->second);
    }

	SERIALIZE_MATCH(data);

    return true;
}

bool CBulletHitJudge::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);

    data.readData(m_nJudgeTimes);
    data.readData(m_fFirstJudgeTime);
    data.readData(m_fInterval);
    data.readData(m_fParam);
    data.readData(m_bNeedCount);
    data.readData(m_bJudgeOver);

    //最大结算次数
    int cnt = 0;
    data.readData(cnt);
    m_TargetCountTimes.clear();
    for (int i = 0; i < cnt; ++i)
    {
        int first = 0;
        int second = 0;
        data.readData(first);
        data.readData(second);
        m_TargetCountTimes[first] = second;
    }

	UN_SERIALIZE_MATCH(data);

    return true;
}

bool CBulletHitJudge::init(CBullet *bullet, CBulletComponent *btComponent)
{
    m_pBullet = bullet;
    m_pBulletComponent = btComponent;

    m_fParam = static_cast<float>(m_pBullet->getBulletConf()->HitJudgeParam);
    m_nHitType = m_pBullet->getBulletConf()->HitJudgeType;
    return true;
}

void CBulletHitJudge::update(float dt)
{
    if (m_bJudgeOver)
    {
        return;
    }

    //命中开始前计时
    m_fFirstJudgeTime += dt;
    if (m_fFirstJudgeTime >= m_pBullet->getBulletConf()->HitJudgeFirstTime)
    {
        //命中判断间隔
        m_fInterval += dt;
        if (m_fInterval >= m_pBullet->getBulletConf()->HitJudgePerTime)
        {
            // 未达到最大命中次数
            if (m_pBullet->getBulletConf()->HitJudgeMaxTimes < 0
                || m_nJudgeTimes < m_pBullet->getBulletConf()->HitJudgeMaxTimes)
            {
                KXLOGBATTLE("CBulletHitJudge update bulletObjId %d", m_pBullet->getObjectId());
                //检测命中
                judge();
                // 如果命中次数大于最大命中次数, 命中判定结束
                if (++m_nJudgeTimes >= m_pBullet->getBulletConf()->HitJudgeMaxTimes)
                {
                    m_bJudgeOver = true;
                }
            }

            m_fInterval = 0.0f;
        }
    }
}

void CBulletHitJudge::setJudgeOver()
{
    m_bJudgeOver = true;
}

bool CBulletHitJudge::isJudgeOver()
{
    return m_bJudgeOver;
}

bool CBulletHitJudge::isNeedCount()
{
    return m_bNeedCount;
}

bool CBulletHitJudge::judge()
{
    bool ret = false;
    switch (m_nHitType)
    {
    case BJT_POINT:
    ret = judgePointHit();
    break;
    case BJT_RECT:
    ret = judgeRectHit();
    break;
    default:
    break;
    }

    m_bNeedCount = ret;
    return ret;
}

bool CBulletHitJudge::judgePointHit()
{
    // 点命中需要有目标的横纵坐标
    CRole *pTarget = dynamic_cast<CRole*>(m_pBullet->getTarget());
    if (NULL != pTarget)
    {
        if (isTargetCollide(pTarget) && !isMaxCount(pTarget))
        {
            //KXLOGBATTLE("CBulletHitJudge judgePointHit bulletObjId %d", m_pBullet->getObjectId());
            m_pBulletComponent->addHitTarget(m_pBullet->getTarget());
            return true;
        }
    }

    return false;
}

bool CBulletHitJudge::judgeRectHit()
{
    // 获得所有指定阵容的角色,命中加入命中列表
    CBattleHelper *pBattleHelper = m_pBullet->getBattleHelper();
    bool ret = false;
    std::vector<CRole *> targets;
    // 获得所有目标, 并添加到受击列表
	if (pBattleHelper->getTargetsWithId(m_pBullet->getBulletConf()->HitJudgeParam, m_pBullet, &targets))
	{
		//KXLOGBATTLE("CBulletHitJudge judgeRectHit bulletObjId %d Targets %d", m_pBullet->getObjectId(), targets.size());

		for (std::vector<CRole *>::iterator iter = targets.begin();
			iter != targets.end(); ++iter)
		{
			if (!isMaxCount(*iter))
			{
				m_pBulletComponent->addHitTarget(*iter);
				ret = true;
			}
		}

		return ret;
	}

    return ret;
}

void CBulletHitJudge::countRole()
{        
    //结算目标
    if (m_pBullet->getBulletConf()->CountId.size() > 0
        || m_pBullet->getBulletConf()->SummonerCountId.size() > 0)
    {
        std::vector<CRole*> &targets = m_pBulletComponent->getTargets();
        for (std::vector<CRole*>::iterator it = targets.begin();
            it != targets.end(); ++it)
        {
            CRole *pTarget = dynamic_cast<CRole*>(*it);
            // 对召唤师、Boss和其他普通的士兵使用不同的结算
            if (pTarget->getRoleType() == RT_HERO || pTarget->getRoleType() == RT_BOSS)
            {
                for (std::vector<int>::const_iterator iter = m_pBullet->getBulletConf()->SummonerCountId.begin();
                    iter != m_pBullet->getBulletConf()->SummonerCountId.end(); ++iter)
                {
                    CCount::roleExecute((*iter), dynamic_cast<CRole*>(m_pBullet->getOwner()), pTarget);
                    m_TargetCountTimes[pTarget->getObjectId()] += 1;
                }
            }
            else
            {
                for (std::vector<int>::const_iterator iter = m_pBullet->getBulletConf()->CountId.begin();
                    iter != m_pBullet->getBulletConf()->CountId.end(); ++iter)
                {
                    CCount::roleExecute((*iter), dynamic_cast<CRole*>(m_pBullet->getOwner()), pTarget);
                    m_TargetCountTimes[pTarget->getObjectId()] += 1;
                }
            }
        }
    }
    
	
	// 结算的时候没有countId的时候, 并且有buffId, 也需要计算次数进行累加
	if (m_pBullet->getBulletConf()->CountId.size() == 0 && m_pBullet->getBulletConf()->BuffId.size() != 0)
	{
		std::vector<CRole*> &targets = m_pBulletComponent->getTargets();
		for (std::vector<CRole*>::iterator it = targets.begin();
			it != targets.end(); ++it)
		{
			CRole *pTarget = dynamic_cast<CRole*>(*it);
			m_TargetCountTimes[pTarget->getObjectId()] += 1;
		}
	}

    m_bNeedCount = false;
}

void CBulletHitJudge::roleAddBuff()
{
    for (std::vector<ID_Num>::const_iterator iter = m_pBullet->getBulletConf()->BuffId.begin();
        iter != m_pBullet->getBulletConf()->BuffId.end(); ++iter)
    {
        //给所有目标附加buff
        std::vector<CRole*> &targets = m_pBulletComponent->getTargets();
        for (std::vector<CRole*>::iterator it = targets.begin();
            it != targets.end(); ++it)
        {
            CRole *pTarget = dynamic_cast<CRole*>(*it);
            pTarget->addBuff(m_pBullet->getOwner(), iter->ID, iter->num);
        }
    }
}

bool CBulletHitJudge::isTargetCollide(CRole *target)
{
    // 死亡的角色不判定
    if (State_Death == target->currentState()
        || State_None == target->currentState())
    {
        return false;
    }

    float fDistanceX = fabs(m_pBullet->getRealPositionX() - target->getHitPoint().x);
    return fDistanceX <= m_fParam;
}

bool CBulletHitJudge::isMaxCount(CRole *target)
{
    return m_TargetCountTimes[target->getObjectId()] >= m_pBullet->getBulletConf()->UnitMaxCountTimes;
}
