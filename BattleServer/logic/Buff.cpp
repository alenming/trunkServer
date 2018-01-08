#include "Buff.h"
#include "BuffComponent.h"
#include "Role.h"
#include "Count.h"

#ifndef RunningInServer
	#include "UIEffect.h"
    #include "Game.h"
#endif 

CBuff::CBuff()
: m_bFinish(false)
, m_bTriggerActive(false)
, m_bUnstoppable(false)
, m_bForce(false)
, m_nWorkTimes(0)
, m_nRecordState(0)
, m_fBuffLifeTime(0.0f)
, m_fFirstTime(0.0f)
, m_fTriggerTime(0.0f)
, m_nTrigerValue(0)
, m_pOwner(NULL)
, m_pMaker(NULL)
, m_pBuffCpnt(NULL)
, m_pBuffConf(NULL)
, m_Condition(this)
{

}

CBuff::~CBuff()
{
    // 移除播放的特效
    stopEffect();
	SAFE_RELEASE(m_pMaker);
}

bool CBuff::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);

    // 基本数据
    data.writeData(m_bFinish);
    data.writeData(m_bTriggerActive);
    data.writeData(m_bForce);
    data.writeData(m_nWorkTimes);
    data.writeData(m_nRecordState);
    data.writeData(m_fBuffLifeTime);
    data.writeData(m_fFirstTime);
    data.writeData(m_fTriggerTime);
    data.writeData(m_nTrigerValue);
    // buff条件内的目标信息
    m_Condition.serialize(data);

	SERIALIZE_MATCH(data);

    return true;
}

bool CBuff::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);

    // 基本数据
    data.readData(m_bFinish);
    data.readData(m_bTriggerActive);
    data.readData(m_bForce);
    data.readData(m_nWorkTimes);
    data.readData(m_nRecordState);
    data.readData(m_fBuffLifeTime);
    data.readData(m_fFirstTime);
    data.readData(m_fTriggerTime);
    data.readData(m_nTrigerValue);
    // buff条件内的目标信息
    m_Condition.unserialize(data);

	UN_SERIALIZE_MATCH(data);

    return true;
}

bool CBuff::init(CBuffComponent *cpnt, CRole *owner, CRole *maker, int buffid, int stack)
{
    if (owner == NULL || maker == NULL)
    {
        return false; 
    }

    m_pBuffConf = queryConfBuff(buffid, stack);
    if (m_pBuffConf == NULL)
    {
        return false;
    }

    if (m_pMaker)
    {
        //如果没有调用onExit, 再次调用了init, 会导致m_pMaker引用计数的增加
        SAFE_RELEASE(m_pMaker);
        //把数据重置
        resetData();
    }

    m_pOwner = owner;
    m_pMaker = maker;
   // Buff的发出者需要被retain
	SAFE_RETAIN(m_pMaker);
    m_pBuffCpnt = cpnt;
    m_bUnstoppable = (m_pBuffConf->Type == UNSTOPPABLE);
    // 注册触发器
    regTrigger(m_pBuffConf->TriggerType);

    return true;
}

void CBuff::onEnter()
{
    // 反序列化由于是先onEnter，再反序列化，所以这里就算获取到了错误的状态
    // buff的反序列化也会将m_nRecordState重置为正确的值
    m_nRecordState = m_pOwner->currentState();
	KXLOGBATTLE("buffid %d owner %d marker %d onEnter",
        m_pBuffConf->ID, m_pOwner->getObjectId(), m_pMaker->getObjectId());
    // 创建表现
    playEffect();
}

void CBuff::onExit(bool isFouce)
{
    KXLOGBATTLE("buffid %d owner %d marker %d onExit",
        m_pBuffConf->ID, m_pOwner->getObjectId(), m_pMaker->getObjectId());

    //停止特效
    stopEffect();
    // 移除事件监听
    unregTigger(m_pBuffConf->TriggerType);

	if (!isFouce)
	{
		// 回收技能
		recycleSkill(m_pBuffConf->SkillID);
		// 失效结算
		loseEfficacyCount();
		// 添加新buff
		int nextflag = m_pBuffConf->NextBuffCondition;
		if (nextflag == 0
			|| (nextflag == 1 && !m_bForce)
			|| (nextflag == 2 && m_bForce))
		{
			for (std::vector<int>::const_iterator iter = m_pBuffConf->nextBuffID.begin();
				iter != m_pBuffConf->nextBuffID.end(); ++iter)
			{
				m_pBuffCpnt->addBuff(m_pOwner, *iter, 1);
			}
		}
	}

    // 重置数据,可以重用该对象
    resetData();

    // 重置condition数据
    m_Condition.clearTargets();
    m_Condition.clearTargetPos();

    // 释放Buff的发出者
    SAFE_RELEASE(m_pMaker);
}

int CBuff::getBuffType()
{
    return m_pBuffConf->Type;
}

bool CBuff::isFinish()
{
    return m_bFinish;
}

bool CBuff::isUnstoppable()
{
    return m_bUnstoppable;
}

void CBuff::finish(bool force)
{
    m_bForce = force;
    m_bFinish = true;
}

void CBuff::update(float dt)
{
    if (m_bFinish)
    {
        return;
    }
    // 生命周期
    m_fBuffLifeTime += dt;
    // 生效次数未达到限制
    if (m_pBuffConf->MaxCount <= 0
        || (m_pBuffConf->MaxCount > 0 && m_nWorkTimes < m_pBuffConf->MaxCount))
    {
        // 首次生效时间
        m_fFirstTime += dt;
        if (m_fFirstTime >= m_pBuffConf->FirstTime)
        {
            // 检测触发器
            checkTrigger(dt);
            // 触发成功
            if (m_bTriggerActive)
            {
                if (checkCondition())
                {
                    // 使用技能
                    executeSkill(m_pBuffConf->SkillID);
                    // 生效结算
                    takeEffectCount();
                    // 生效次数+1
                    m_nWorkTimes++;
                }

                m_bTriggerActive = false;
            }    
        }
    }
    //生命周期到, 正常结束
    if ((m_pBuffConf->lifeTime > 0 
        && m_fBuffLifeTime >= m_pBuffConf->lifeTime)
        // 携带者离开指定状态时自动回收
        || (m_pBuffConf->lifeTime == -2
        && m_nRecordState != m_pOwner->currentState()))
	{
		KXLOGBATTLE("buffid %d owner %d marker %d finish",
            m_pBuffConf->ID, m_pOwner->getObjectId(), m_pMaker->getObjectId());
		finish();
    }
}

void CBuff::reset()
{
    m_fBuffLifeTime = 0.0f;
}

int CBuff::getStack()
{
    return m_pBuffConf->Stack;
}

CRole *CBuff::getMaker()
{
    return m_pMaker;
}

const BuffConfItem *CBuff::getBuffConf()
{
    return m_pBuffConf;
}

bool CBuff::checkCondition()
{
    for (std::vector<CDataFunction *>::const_iterator iter = m_pBuffConf->Conditions.begin();
        iter != m_pBuffConf->Conditions.end(); ++iter)
    {
        CDataFunction *f = *iter;
        bool ret = false;
        switch (f->NameID)
        {
        case Func_IsHasBuffTarget:
            ret = m_Condition.isHasBuffTarget();
            break;
        case Func_IsHasSkillPoint:
            ret = m_Condition.isHasSkillPoint(f->ParamInt[0], f->ParamInt[1] * m_pOwner->getDirection(), f->ParamInt[2]);
            break;
        case Func_IsSkillReady:
            ret = m_Condition.isSkillReady(f->ParamInt[0] == 0 ? false : true, f->ParamInt[1]);
            break;
        case Func_IsRandom:
            ret = m_Condition.isRandom(f->ParamInt[0]);
            break;
        default:
            break;
        }

        if (!ret)
        {
            return false;
        }
    }

    return true;
}

void CBuff::checkTrigger(float dt)
{
    switch (m_pBuffConf->TriggerType)
    {
    case TRIGGER_TIME:
        triggerTime(dt);
        break;

    case TRIGGER_DIE:
        triggerDie();
        break;

    case TRIGGER_CRIT:
    case TRIGGER_BECRITED:
    case TRIGGER_DODGE:
    case TRIGGER_BEDODGED:
    case TRIGGER_BETREATED:
    case TRIGGER_BEATTACKED:
    case TRIGEER_ATTACK:
        triggerDValue((BuffTriggerType)m_pBuffConf->TriggerType);
        break;

    case TRIGGER_BROKENSHIELD:
    case TRIGGER_ADDSHIELD:
        triggerAccumulative((BuffTriggerType)m_pBuffConf->TriggerType);
        break;

    case TRIGGER_DISPATCHSOLIDER:
    case TRIGGER_SKILL:
        break;
    default:
        break;
    }
    
}

void CBuff::takeEffectCount()
{
    KXLOGBATTLE("buff takeEffectCount buffId %d MakerObjId %d OwnerObjId %d",
        m_pBuffConf->ID, m_pMaker->getObjectId(), m_pOwner->getObjectId());

    if (m_pBuffConf->CountID.size() > 0
        || m_pBuffConf->SummonerCountID.size() > 0)
    {
        std::vector<CRole*> &targets = m_Condition.getTargets();
        for (std::vector<CRole*>::iterator ster = targets.begin();
            ster != targets.end(); ++ster)
        {
            CRole* role = *ster;
            if (role->getRoleType() == RT_BOSS || role->getRoleType() == RT_HERO)
            {
                for (std::vector<int>::const_iterator iter = m_pBuffConf->SummonerCountID.begin();
                    iter != m_pBuffConf->SummonerCountID.end(); ++iter)
                {
                    CCount::roleExecute((*iter), getMaker(), (*ster));
                }
            }
            else
            {
                for (std::vector<int>::const_iterator iter = m_pBuffConf->CountID.begin();
                    iter != m_pBuffConf->CountID.end(); ++iter)
                {
                    CCount::roleExecute((*iter), getMaker(), (*ster));
                }
            }
        }
    }
}

void CBuff::loseEfficacyCount()
{
    if (m_pBuffConf->LapseCountID.size() > 0 
        || m_pBuffConf->SummonerLapseCountID.size() > 0)
    {
        std::vector<CRole*> &targets = m_Condition.getTargets();
        for (std::vector<CRole*>::iterator ster = targets.begin();
            ster != targets.end(); ++ster)
        {
            CRole* role = *ster;
            if (role->getRoleType() == RT_BOSS || role->getRoleType() == RT_HERO)
            {
                for (std::vector<int>::const_iterator iter = m_pBuffConf->SummonerLapseCountID.begin();
                    iter != m_pBuffConf->SummonerLapseCountID.end(); ++iter)
                {
                    CCount::roleExecute((*iter), getMaker(), (*ster));
                }
            }
            else
            {
                for (std::vector<int>::const_iterator iter = m_pBuffConf->LapseCountID.begin();
                    iter != m_pBuffConf->LapseCountID.end(); ++iter)
                {
                    CCount::roleExecute((*iter), getMaker(), (*ster));
                }
            }
        }
    }
}

void CBuff::executeSkill(int skillid)
{
    KXLOGBATTLE("buff executeSkill buffId %d MakerObjId %d OwnerObjId %d skillId %d", 
        m_pBuffConf->ID, m_pMaker->getObjectId(), m_pOwner->getObjectId(), skillid);

    if (skillid > 0)
    {
        if (!m_pOwner->getSkillComponent()->hasSkillID(skillid))
        {
            m_pOwner->getSkillComponent()->addNewSkill(skillid);
        }

        if (!m_Condition.getTargets().empty())
        {
            m_pOwner->getSkillComponent()->executeSkill(skillid, m_Condition.getTargets());
        }

        if (!m_Condition.getTargetPos().equals(Vec2(0, 0)))
        {
            m_pOwner->getSkillComponent()->executeSkill(skillid, m_Condition.getTargetPos());
        }
    }
}

void CBuff::recycleSkill(int skillid)
{
	if (NULL != m_pOwner->getSkillComponent())
	{
		if (m_pOwner->getSkillComponent()->hasSkillID(skillid))
		{
			m_pOwner->getSkillComponent()->removeSkill(skillid);
		}
	}
}

void CBuff::regTrigger(int trigtype)
{
    CEventManager<int>* eventMgr = m_pOwner->getBattleHelper()->pEventManager;

    switch (trigtype)
    {
    case TRIGGER_TIME:
    case TRIGGER_DIE:
        break;
    case TRIGGER_CRIT:
    case TRIGGER_BECRITED:
    case TRIGGER_DODGE:
    case TRIGGER_BEDODGED:
    case TRIGGER_BROKENSHIELD:
    case TRIGGER_ADDSHIELD:
    case TRIGGER_BETREATED:
    case TRIGGER_BEATTACKED:
    case TRIGEER_ATTACK:
        // 这些直接检测自身属性
        m_nTrigerValue = getAttributeValue((BuffTriggerType)trigtype);
        break;

    case TRIGGER_SKILL:
        eventMgr->addEventHandle(0xfff2, this, CALLBACK_FUNCV(CBuff::triggerSkill));
        break;
    case TRIGGER_DISPATCHSOLIDER:
        eventMgr->addEventHandle(0xfffa, this, CALLBACK_FUNCV(CBuff::triggerDispatchSoldier));
        break;
    default:
        break;
    }
}

void CBuff::unregTigger(int trigtype)
{
    switch (trigtype)
    {
    case TRIGGER_SKILL:
    case TRIGGER_DISPATCHSOLIDER:
        m_pOwner->getBattleHelper()->pEventManager->removeEventHandle(this);
        break;
    default:
        break;
    }
}

void CBuff::triggerTime(float dt)
{
    m_fTriggerTime -= dt;
    if (m_pBuffConf->TriggerTypeParam.size() > 0 
        && m_fTriggerTime <= 0.0f)
    {
        m_bTriggerActive = true;
        m_fTriggerTime = (m_pBuffConf->TriggerTypeParam[0] / 1000.0f);
        KXLOGBATTLE("TriggerActive1 true owner %d marker %d", m_pOwner->getObjectId(), m_pMaker->getObjectId());
    }
}

void CBuff::triggerDie()
{
    // Buff携带者死亡的时候触发
    if (m_pOwner->currentState() == State_Death)
    {
        m_bTriggerActive = true;
        KXLOGBATTLE("TriggerActive2 true owner %d marker %d", m_pOwner->getObjectId(), m_pMaker->getObjectId());
    }
}

void CBuff::triggerSkill(void *data)
{
    // 第一个正整数代表：0有人释放技能，1己方释放，2敌方释放
    // 第二个正整数代表：计数器，释放几次后触发
    int executeUid = *static_cast<int*>(data);
    if (m_pBuffConf->TriggerTypeParam[0] == 0
        || (m_pBuffConf->TriggerTypeParam[0] == 1 && m_pOwner->getOwnerId() == executeUid)
        || (m_pBuffConf->TriggerTypeParam[0] == 2 && m_pOwner->getOwnerId() != executeUid))
    {
        if (++m_nTrigerValue >= m_pBuffConf->TriggerTypeParam[1])
        {
            m_bTriggerActive = true;
            m_nTrigerValue = 0;
            KXLOGBATTLE("TriggerActive3 true owner %d marker %d", m_pOwner->getObjectId(), m_pMaker->getObjectId());
        }
    }
}

void CBuff::triggerDispatchSoldier(void *data)
{
    //召唤师召唤士兵时触发
    //1我方，2敌方，3全体
    int dispatchUid = *static_cast<int*>(data);
    if (m_pBuffConf->TriggerTypeParam[0] == 3 
        || (dispatchUid == m_pOwner->getOwnerId() && m_pBuffConf->TriggerTypeParam[0] == 1)
        || (dispatchUid != m_pOwner->getOwnerId() && m_pBuffConf->TriggerTypeParam[0] == 2))
    {
        m_bTriggerActive = true;
    }
}

void CBuff::playEffect()
{
#ifndef RunningInServer

    Vec2 pos = Vec2::ZERO;
    switch (m_pBuffConf->EffectWhere)
    {
    case 1:
        pos = m_pOwner->getBaseHeadOffset();
        break;

    case 2:
        pos = m_pOwner->getBaseHitOffset();
        break;

    default:
        break;
    }

    for (std::vector<int>::const_iterator iter = m_pBuffConf->AnimationID.begin();
        iter != m_pBuffConf->AnimationID.end(); ++iter)
    {
        Node *eff = CEffectFactory::create(*iter, m_pOwner->getDirection());
        if (NULL != eff)
        {
            // 根据是否跟随来决定特效挂载的对象
            if (m_pBuffConf->IsEffectMove)
            {
                eff->retain();
                float effScaleX = m_pOwner->getFloatAttribute(EAttributeTypes::EAttributeEffectScale) * eff->getScaleX();
                float effScaleY = m_pOwner->getFloatAttribute(EAttributeTypes::EAttributeEffectScale) * eff->getScaleY();
                eff->setScaleX(effScaleX);
                eff->setScaleY(effScaleY);
                eff->setPosition(pos);
				m_pOwner->getDisplayNode()->addChild(eff);
				m_EffectNodes.push_back(eff);
            }
            else
            {
                pos += m_pOwner->getPosition();
                eff->setPosition(pos);
                m_pOwner->getBattleHelper()->getBattleScene()->addChild(eff);
            }
        }
        else
        {
#if (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)
            if (CGame::getInstance()->isDebug())
            {
                LOG("te xiao id is %d", *iter);
                cocos2d::MessageBox(gb23122utf8(cocos2d::String::createWithFormat(
                    "Fuck大策划, BUFF给角色添加特效 %d 失败了", *iter)->getCString()).c_str(), "error");
            }
#endif
        }
    }

	//添加UI特效
	CUIEffectManager::getInstance()->execute(m_pBuffConf->UIEffectID, m_pOwner->getPosition());

#endif 
}

void CBuff::stopEffect()
{
#ifndef RunningInServer
    
	for (std::vector<Node *>::iterator iter = m_EffectNodes.begin();
        iter != m_EffectNodes.end(); ++iter)
    {
        Node* node = *iter;
        if (node->isRunning())
        {
            node->removeFromParentAndCleanup(true);
        }
        node->release();
    }
    m_EffectNodes.clear();

#endif 
}

int CBuff::getAttributeValue(BuffTriggerType triggerType)
{
    int val = 0;
    switch (triggerType)
    {
    case TRIGGER_CRIT:
        val = m_pOwner->getIntAttribute(EStatCrit);
        break;
    case TRIGGER_BECRITED:
        val = m_pOwner->getIntAttribute(EStatBeCrited);
        break;
    case TRIGGER_DODGE:
        val = m_pOwner->getIntAttribute(EStatDodge);
        break;
    case TRIGGER_BEDODGED:
        val = m_pOwner->getIntAttribute(EStatBeDodged);
        break;
    case TRIGGER_BROKENSHIELD:
        val = m_pOwner->getIntAttribute(EStatBrokenShield);
        break;
    case TRIGGER_ADDSHIELD:
        val = m_pOwner->getIntAttribute(EAttributeExtraHP);
        break;
    case TRIGGER_BETREATED:
        val = m_pOwner->getIntAttribute(EStatBeTreated);
        break;
    case TRIGGER_BEATTACKED:
        val = m_pOwner->getIntAttribute(EStatBeAttacked);
        break;
    case TRIGEER_ATTACK:
        val = m_pOwner->getIntAttribute(EStatAttack);
        break;

    default:
        KXLOGDEBUG("Invalid BuffTriggerType!!");
        KXLOGBATTLE("Invalid BuffTriggerType owner %d marker %d", m_pOwner->getObjectId(), m_pMaker->getObjectId());
        break;
    }

    return val;
}

void CBuff::triggerDValue(BuffTriggerType triggerType)
{
    int val = getAttributeValue(triggerType);
    if ((val - m_nTrigerValue) >= m_pBuffConf->TriggerTypeParam[0])
    {
        m_bTriggerActive = true;
        m_nTrigerValue = val;
        KXLOGBATTLE("TriggerActive4 true owner %d marker %d", m_pOwner->getObjectId(), m_pMaker->getObjectId());
    }
}

void CBuff::triggerAccumulative(BuffTriggerType triggerType)
{
    int val = getAttributeValue(triggerType);
    if (val > m_nTrigerValue)
    {
        m_bTriggerActive = true;
        m_nTrigerValue = val;
        KXLOGBATTLE("TriggerActive5 true owner %d marker %d", m_pOwner->getObjectId(), m_pMaker->getObjectId());
    }
}

void CBuff::resetData()
{
    m_bFinish = false;
    m_bForce = false;
    m_fBuffLifeTime = 0;
    m_fFirstTime = 0;
    m_nWorkTimes = 0;
    m_nRecordState = 0;
    m_bTriggerActive = false;
    m_nTrigerValue = 0;

    m_fTriggerTime = 0;
    m_pBuffConf = NULL;
}

CBuffCondition::CBuffCondition(CBuff *parent) 
: m_pParent(parent)
, m_TargetPos()
{

}

CBuffCondition::~CBuffCondition()
{

}

bool CBuffCondition::serialize(CBufferData& data)
{
    data.writeData(m_TargetPos.x);
    data.writeData(m_TargetPos.y);
    
    int targetCnt = m_Targets.size();
    data.writeData(targetCnt);
    for (std::vector<CRole *>::iterator iter = m_Targets.begin(); 
        iter != m_Targets.end(); ++iter)
    {
        data.writeData((*iter)->getObjectId());
    }

    return true;
}

bool CBuffCondition::unserialize(CBufferData& data)
{
    float x = 0.0f, y = 0.0f;
    data.readData(x);
    data.readData(y);
    m_TargetPos = Vec2(x, y);
    releaseAndClearVec(m_Targets);
    int targetCnt;
    data.readData(targetCnt);
    for (int i = 0; i < targetCnt; ++i)
    {
        int objId;
        data.readData(objId);
        CRole *target = m_pParent->m_pOwner->getBattleHelper()->getRoleByObjectId(objId);
        if (NULL != target)
        {
            SAFE_RETAIN(target);
            m_Targets.push_back(target); 
        }
    }

    return true;
}

bool CBuffCondition::isHasBuffTarget()
{
    releaseAndClearVec(m_Targets);
    switch (m_pParent->m_pBuffConf->TargetType)
    {
    case BUFFTARGET_OWNER:
        m_Targets.push_back(m_pParent->m_pOwner);
        break;

    case BUFFTARGET_MAKER:
        m_Targets.push_back(m_pParent->m_pMaker);
        break;
        
    default:
        // 正整数代表搜索Id
        getCustomTargets(m_pParent->m_pBuffConf->TargetType, m_Targets);
        break;
    }

    for (std::vector<CRole *>::iterator iter = m_Targets.begin();
        iter != m_Targets.end(); ++iter)
    {
        (*iter)->retain();
    }

    KXLOGBATTLE("isHasBuffTarget type %d targets %d buffId %d", m_pParent->m_pBuffConf->TargetType, 
        m_Targets.size(), m_pParent->getBuffConf()->ID);

    return !m_Targets.empty();
}

bool CBuffCondition::isHasSkillPoint(int flag, int x, int y)
{
    KXLOGBATTLE("isHasSkillPoint flag %d owner %d X %f Y %f rX %f rY %f", flag, x, y, m_pParent->m_pOwner->getObjectId(),
        m_pParent->m_pOwner->getRealPositionX(), m_pParent->m_pOwner->getRealPosition().y);
    switch (flag)
    {
    case 0:
        m_TargetPos.x = static_cast<float>(x);
        m_TargetPos.y = static_cast<float>(y);
        break;

    case 1:
        m_TargetPos = m_pParent->m_pOwner->getRealPosition() 
            + Vec2(static_cast<float>(x), static_cast<float>(y));
        break;

    default:
        break;
    }

    return true;
}

bool CBuffCondition::isSkillReady(bool ng, int skillid)
{
    bool ret = m_pParent->m_pOwner->getSkillComponent()->canExecuteSkill(skillid);
    return ng ? !ret : ret;
}

bool CBuffCondition::isRandom(int rand)
{
    int curRand = m_pParent->m_pOwner->getBattleHelper()->RandNum.random(0, 100);
    return curRand < rand;
}

std::vector<CRole *> &CBuffCondition::getTargets()
{
    return m_Targets;
}

void CBuffCondition::clearTargets()
{
    releaseAndClearVec(m_Targets);
}

Vec2 &CBuffCondition::getTargetPos()
{
    return m_TargetPos;
}

void CBuffCondition::clearTargetPos()
{
    m_TargetPos = Vec2(0, 0);
}

void CBuffCondition::getCustomTargets(int searchid, std::vector<CRole*> &targets)
{
    if (searchid > 0)
    {
        m_pParent->m_pOwner->getBattleHelper()->getTargetsWithId(searchid, m_pParent->m_pOwner, &targets);
    }
}
