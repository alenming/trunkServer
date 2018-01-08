#include "Skill.h"
#include "SkillComponent.h"
#include "Role.h"
#include "Bullet.h"
#include "CallSoldier.h"
#include "Count.h"

CSkill::CSkill()
: m_bFinish(true)
, m_bIsLock(true)
, m_bTargetBulletShoot(false)
, m_bPositionBulletShoot(false)
, m_nIndex(0)
, m_nLoopTimes(0)
, m_nTargetBulletIndex(0)
, m_nPositionBulletIndex(0)
//, m_nSkillLevel(1)
, m_fCDTime(0.0f)
, m_fLoopDelay(0.0f)
, m_fTargetTypeDelay(0.0f)
, m_fPositionTypeDelay(0.0f)
, m_fConfLoopDelay(0.0f)
, m_fConfFirstTargetTypeDelay(0.0f)
, m_fConfFirstPositionTypeDelay(0.0f)
, m_fConfTargetTypeDelay(0.0f)
, m_fConfPositionTypeDelay(0.0f)
, m_pOwner(NULL)
, m_TargetPos(SKILL_INVALID_POINT)
, m_pSkillConf(NULL)
, m_pSkillComponent(NULL)
{
}

CSkill::~CSkill()
{
    releaseAndClearVec(m_TargetList);
}

bool CSkill::serialize(CBufferData& data)
{
    // 基本属性
    data.writeData(m_bFinish);
    data.writeData(m_bIsLock);
    data.writeData(m_bTargetBulletShoot);
    data.writeData(m_bPositionBulletShoot);
    data.writeData(m_nLoopTimes);
    data.writeData(m_nTargetBulletIndex);
    data.writeData(m_nPositionBulletIndex);
    data.writeData(m_fCDTime);
    data.writeData(m_fLoopDelay);
    data.writeData(m_fTargetTypeDelay);
    data.writeData(m_fPositionTypeDelay);
    data.writeData(m_fConfLoopDelay);
    data.writeData(m_fConfFirstTargetTypeDelay);
    data.writeData(m_fConfFirstPositionTypeDelay);
    data.writeData(m_fConfTargetTypeDelay);
    data.writeData(m_fConfPositionTypeDelay);
    data.writeData(m_TargetPos.x);
    data.writeData(m_TargetPos.y);

    // 对象列表
    int targetCnt = m_TargetList.size();
    data.writeData(targetCnt);

    for (int i = 0; i < targetCnt; ++i)
    {
        data.writeData(m_TargetList[i]->getObjectId());
    }

    return true;
}

bool CSkill::unserialize(CBufferData& data)
{
    // 基本属性
    data.readData(m_bFinish);
    data.readData(m_bIsLock);
    data.readData(m_bTargetBulletShoot);
    data.readData(m_bPositionBulletShoot);
    data.readData(m_nLoopTimes);
    data.readData(m_nTargetBulletIndex);
    data.readData(m_nPositionBulletIndex);
    data.readData(m_fCDTime);
    data.readData(m_fLoopDelay);
    data.readData(m_fTargetTypeDelay);
    data.readData(m_fPositionTypeDelay);
    data.readData(m_fConfLoopDelay);
    data.readData(m_fConfFirstTargetTypeDelay);
    data.readData(m_fConfFirstPositionTypeDelay);
    data.readData(m_fConfTargetTypeDelay);
    data.readData(m_fConfPositionTypeDelay);
    float x = 0.0f;
    float y = 0.0f;
    data.readData(x);
    data.readData(y);
    m_TargetPos = Vec2(x, y);

    releaseAndClearVec(m_TargetList);
    // 对象列表
    int targetCnt = 0;
	data.readData(targetCnt);
    for (int i = 0; i < targetCnt; ++i)
    {
        int objId = 0;
        data.readData(objId);
        // 通过helper获得目标实体
        CRole *target = m_pOwner->getBattleHelper()->getRoleByObjectId(objId);
        if (target != NULL)
        {
            SAFE_RETAIN(target);
            m_TargetList.push_back(target);
        }
    }

    return true;
}

bool CSkill::init(CSkillComponent *skillComponent, CRole *owner, int skillid, int skillindex, int skillLv)
{
    if (m_pSkillConf != NULL)
    {
        KXLOGBATTLE("CSkill init failed skillConf not null");
        return false;
    }

    m_pSkillConf = queryConfSkill(skillid);
    if (NULL == m_pSkillConf)
    {
        KXLOGBATTLE("CSkill init failed skillConf is null");
        return false;
    }
    KXLOGBATTLE("CSkill init ownerObjId %d skillId %d skillIndex %d skillLv %d",
        owner->getObjectId(), skillid, skillindex, skillLv);

    m_pSkillComponent = skillComponent;
    m_pOwner = owner;
    m_bIsLock = !(m_pSkillConf->LockType == SLT_UNLOCK);
    m_nIndex = skillindex;
    //m_nSkillLevel = skillLv;

    // 将所有属性重置为预备值
    reset();

    m_fCDTime = m_pSkillConf->CD * m_pSkillConf->CDParam;
    // 如果可以打断, 注册到技能组件上监听打断
    if (m_pSkillConf->CanBreak)
    {
        skillComponent->regSkillBreak(this);
    }

    return true;
}

void CSkill::reset()
{
    m_bFinish = true; 
    
    m_bTargetBulletShoot = false;
    m_bPositionBulletShoot = false;

    m_nLoopTimes = m_pSkillConf->MaxCast;
    m_nTargetBulletIndex = 0;
    m_nPositionBulletIndex = 0;

    m_fLoopDelay = 0.0f;
    m_fTargetTypeDelay = 0.0f;
    m_fPositionTypeDelay = 0.0f;

    m_fConfLoopDelay = m_pSkillConf->CastTime;
    m_fConfFirstTargetTypeDelay = m_pSkillConf->TargetBulletDelay;
    m_fConfFirstPositionTypeDelay = m_pSkillConf->PointBulletDelay;
    m_fConfTargetTypeDelay = m_pSkillConf->TargetBulletInterval;
    m_fConfPositionTypeDelay = m_pSkillConf->PointPointBulletInterval;

    // 初始化技能当前CD时间为，技能的CD时间 * 技能默认的CD百分比
    // CDParam可以控制一个技能默认的CD为50%或100%
    m_fCDTime = 0; //m_pSkillConf->CD * m_pSkillConf->CDParam;

    releaseAndClearVec(m_TargetList);
    m_TargetPos = SKILL_INVALID_POINT;
}

const SkillConfItem *CSkill::getSkillConf()
{
    return m_pSkillConf;
}

int CSkill::skillId()
{
    return m_pSkillConf->ID;
}

int CSkill::skillIndex()
{
    return m_nIndex;
}

float CSkill::getCDPercent()
{
    if (m_pSkillConf->CD <= 0.0000f)
    {
        return 1.0f;
    }

    float percent = m_fCDTime / m_pSkillConf->CD;
    if (percent > 1.0f)
    {
        return 1.0f;
    }

    return percent;
}

float CSkill::getExecutingPercent()
{
    // 为执行或处于无限循环中
    if (m_bFinish || m_pSkillConf->MaxCast <= 0)
    {
        return 0.0f;
    }
    else
    {
        return (m_fConfLoopDelay * (m_pSkillConf->MaxCast - m_nLoopTimes - 1)) + m_fLoopDelay /
        (m_fConfLoopDelay * m_pSkillConf->MaxCast);
    }
}

void CSkill::update(float dt)
{
    if (isLock())
    {
        if (canUnlock())
        {
            unlock();
        }
        else
        {
            return;
        }
    }

    // 技能正在释放时不回复CD
    if (m_bFinish)
    {
        if (m_fCDTime < m_pSkillConf->CD)
        {
            m_fCDTime += dt;
        }
    }
    else
    {
        // 1. 当射击完毕且不限制最大循环次数或未达到最大循环次数时.
        int prevLoopTimes = m_nLoopTimes;
        m_fLoopDelay -= dt;
        if (m_fLoopDelay <= 0)
        {
            // MaxCast为0时表示无限循环
            if ((m_pSkillConf->MaxCast == 0
                || m_nLoopTimes > 0))
            {
                --m_nLoopTimes;

                // 2. 等待单次循环时间
                // 针对目标点发射目标
                if (!m_TargetPos.equals(SKILL_INVALID_POINT))
                {
                    m_nPositionBulletIndex = 0;
                    m_fPositionTypeDelay = m_fConfFirstPositionTypeDelay;
                    m_bPositionBulletShoot = true;
                }
                // 针对目标列表发射目标
                if (!m_TargetList.empty())
                {
                    m_nTargetBulletIndex = 0;
                    m_fTargetTypeDelay = m_fConfFirstTargetTypeDelay;
                    m_bTargetBulletShoot = true;
                }

                m_fLoopDelay = m_fConfLoopDelay;
            }
            else
			{
				KXLOGBATTLE("skill finish skillId %d ownerObjId %d", m_pSkillConf->ID, m_pOwner->getObjectId());
                // 重置技能属性, 技能释放结束
                m_bFinish = true;
                reset();
                return;
            }
        }

        // 3. 有目标列表且未射击完毕
        if (m_bTargetBulletShoot)
        {
            // 每一回合添加一次buff
            if (prevLoopTimes != m_nLoopTimes)
            {
                addBuff(m_TargetList);
            }

            m_fTargetTypeDelay -= dt;
            while (m_fTargetTypeDelay <= 0.0f
                && m_bTargetBulletShoot)
            {
                // 发射最后一颗子弹后, 对目标列表射击结束
                m_bTargetBulletShoot = nextTargetBullet();
                m_fTargetTypeDelay = m_fConfTargetTypeDelay;
            }
        }

        // 4. 有目标点且未射击完毕, 对目标点射击
        if (m_bPositionBulletShoot)
        {
            // 每一回合召唤一次
            if (prevLoopTimes != m_nLoopTimes)
            {
                callSoldier();
            }
            
            m_fPositionTypeDelay -= dt;
            while (m_fPositionTypeDelay <= 0.0f
                && m_bPositionBulletShoot)
            {
                // 发射最后一颗子弹后, 对目标点射击结束
                m_bPositionBulletShoot = nextPosBullet();
                m_fPositionTypeDelay = m_fConfPositionTypeDelay;
            }
        }
    }
}

bool CSkill::isFinish()
{
    return m_bFinish;
}

bool CSkill::isLock()
{
    return m_bIsLock;
}

bool CSkill::canUnlock()
{
    if (m_pSkillConf->LockType == SLT_CRYSTALLEVEL)
    {
        //水晶等级
        int nCrytalLevel = m_pOwner->getIntAttribute(EHeroCrystalLevel);
        return nCrytalLevel >= m_pSkillConf->LockTypePrarm;
    }

    // 配置其它默认开启
    return true;
}

void CSkill::unlock()
{
    m_bIsLock = false;
    // 刚解锁时计算初始cd时间
    if (m_pOwner->getBattleHelper()->getBattleType() == EBATTLE_TOWERTEST)
    {
        // 爬塔试炼需要CD
        m_fCDTime = 0.0f;
    }
    else
    {
        m_fCDTime = m_pSkillConf->CD * m_pSkillConf->CDParam;
    }
}

void CSkill::breakSkill()
{
    if (!m_bFinish)
    {
        reset();
    }
}

bool CSkill::changeState()
{
    // 如果该技能需要切状态，则必须状态切换成功才能执行技能
    int stateId = m_pSkillConf->StateID;
    if (stateId > 0 && NULL == m_pOwner->changeState(stateId))
    {
        // 如果切换状态失败，则不释放技能
        return false;
    }

    return true;
}

void CSkill::cost()
{
    switch (m_pSkillConf->CostType)
    {
    case SCT_CRYTAL:
        {
            int crytal = floatToInt(m_pOwner->getFloatAttribute(EHeroCrystal));
            m_pOwner->setAttribute(EHeroCrystal, static_cast<float>(
                crytal - m_pSkillConf->CostTypeParam));
        }
        break;

    case SCT_BLOOD:
        {
            int blood = m_pOwner->getIntAttribute(EAttributeHP);
            int maxBlood = m_pOwner->getIntAttribute(EAttributeMaxHP);
            int costBlood = floatToInt(m_pSkillConf->CostTypeParam * maxBlood * 0.01f);
			m_pOwner->setAttribute(EAttributeHP, blood - costBlood);
        }
        break;

    case SCT_RAGE:
        {
            int rage = m_pOwner->getIntAttribute(EAttributeRage);
            m_pOwner->setAttribute(EAttributeRage, rage - m_pSkillConf->CostTypeParam);
        }
        break;

    case SCT_MAGIC:
        {
            int mp = m_pOwner->getIntAttribute(EAttributeMP);
            m_pOwner->setAttribute(EAttributeMP, mp - m_pSkillConf->CostTypeParam);
        }
        break;

    default:
        break;
    }
	CCount::playCountEffect(m_pOwner, kUnKnow, 1);
}

void CSkill::calcLinkage()
{
    if (m_pSkillConf->BulletParam > 0)
    {
        // 计算联动时间
        float speedvar = 1.0f;

        switch (m_pSkillConf->BulletParam)
        {
            // 新峰说，加个2，跟回蓝速度影响
        case 1:
            speedvar = m_pOwner->getFloatAttribute(EAttributeAttackSpeedVar);
            if (speedvar <= 0.0f)
            {
                speedvar = 1.0f;
            }
            speedvar = 1.0f / speedvar;
            break;
        case 2:
            speedvar = m_pOwner->getIntAttribute(EClassMPRecover) * 1.0f / m_pOwner->getIntAttribute(EAttributeMPRecover);
            break;
        default:
            break;
        }
        m_fConfLoopDelay = m_pSkillConf->CastTime * speedvar;
        m_fConfFirstTargetTypeDelay = m_pSkillConf->TargetBulletDelay * speedvar;
        m_fConfFirstPositionTypeDelay = m_pSkillConf->PointBulletDelay * speedvar;
        m_fConfTargetTypeDelay = m_pSkillConf->TargetBulletInterval * speedvar;
        m_fConfPositionTypeDelay = m_pSkillConf->PointPointBulletInterval * speedvar;
    }
}

bool CSkill::canExecute()
{
    if (!m_bIsLock 
        && m_bFinish
        && getCDPercent() >= 1.0f)
    {
        switch (m_pSkillConf->CostType)
        {
        case SCT_NOCOST:
            return true;

        case SCT_CRYTAL:
            {
                int crytal = floatToInt(m_pOwner->getFloatAttribute(EHeroCrystal));
                return crytal >= m_pSkillConf->CostTypeParam;
            }

        case SCT_BLOOD:
            {
                float current = m_pOwner->getIntAttribute(EAttributeHP) * 1.0f / 
                    m_pOwner->getIntAttribute(EAttributeMaxHP) * 1.0f;
                float cost = m_pSkillConf->CostTypeParam / 100.0f;
                return current > cost;
            }

        case SCT_RAGE:
            {
                 int rage = m_pOwner->getIntAttribute(EAttributeRage);
                 return rage >= m_pSkillConf->CostTypeParam;
            }

        case SCT_MAGIC:
            {
                int mp = m_pOwner->getIntAttribute(EAttributeMP);
                return mp >= m_pSkillConf->CostTypeParam;
            }

        default:
            break;
        }
    }
    KXLOGBATTLE("canExecute skillId %d m_bIsLock %d m_bFinish %d Percent %f", 
        m_pSkillConf->ID, m_bIsLock, m_bFinish, getCDPercent());
    return false;
}

void CSkill::execute(std::vector<CRole*> &targets, Vec2 &pos)
{
    // 计算联动时间
    calcLinkage();
    m_bFinish = false;
    m_fCDTime = 0.0f;

    if (!targets.empty())
    {
        // 引用外部目标列表
        releaseAndClearVec(m_TargetList);
        for (std::vector<CRole*>::iterator iter = targets.begin();
            iter != targets.end(); ++iter)
        {
            CRole* role = *iter;
            SAFE_RETAIN(role);
            m_TargetList.push_back(role);
            KXLOGBATTLE("skill execute1 roleObjId skillId %d %d X %f Y %f", 
                m_pSkillConf->ID, role->getObjectId(), pos.x, pos.y);
        }
    }
    // 外部调用很多情况都是闯入默认的vec2::ZERO(主要是ai目标默认值就是zero), 所以无论如何这里都会赋值成功,
    // 因此配置的召唤物必定会被召唤出来, 配置的目标点子弹也会发射
    if (!pos.equals(SKILL_INVALID_POINT))
    {
        m_TargetPos = pos;
    }

    cost();
}

void CSkill::execute(std::vector<CRole*> &targets)
{   
    if (!targets.empty())
    {
        // 计算联动时间
        calcLinkage();
        m_bFinish = false;
        m_fCDTime = 0.0f;

        // 引用外部目标列表
        releaseAndClearVec(m_TargetList);
        for (std::vector<CRole*>::iterator iter = targets.begin();
            iter != targets.end(); ++iter)
        {
            CRole* role = *iter;
            SAFE_RETAIN(role);
            m_TargetList.push_back(role);
            KXLOGBATTLE("skill execute2 skillId %d roleObjId %d",
                m_pSkillConf->ID, role->getObjectId());
        }

        cost();
    }
}

void CSkill::execute(Vec2 &pos)
{
    if (!pos.equals(SKILL_INVALID_POINT))
    {
        // 计算联动时间
        calcLinkage();
        m_bFinish = false;
        m_fCDTime = 0.0f;
        m_TargetPos = pos;
        cost();
        KXLOGBATTLE("skill execute3 skillId %d, X %f Y %f",
            m_pSkillConf->ID, m_TargetPos.x, m_TargetPos.y);
    }
}

bool CSkill::nextTargetBullet()
{
    if (m_nTargetBulletIndex < (int)m_pSkillConf->TargetBullet.size())
    {
        fireBullet(m_TargetList, m_pSkillConf->TargetBullet[m_nTargetBulletIndex++]);
    }

    return m_nTargetBulletIndex < (int)m_pSkillConf->TargetBullet.size();
}

bool CSkill::nextPosBullet()
{
    if (m_nPositionBulletIndex < (int)m_pSkillConf->PointBullet.size())
    {
        fireBullet(m_TargetPos, m_pSkillConf->PointBullet[m_nPositionBulletIndex++]);
    }

    return m_nPositionBulletIndex < (int)m_pSkillConf->PointBullet.size();
}

void CSkill::fireBullet(std::vector<CRole*> &targets, int bulletid)
{
    if (!targets.empty())
    {
        CBattleHelper *pHelper = m_pOwner->getBattleHelper();
		CHECK_RETURN_VOID(pHelper);
        for (std::vector<CRole*>::iterator iter = targets.begin();
            iter != targets.end(); iter++)
        {
			CBullet* pBullet = pHelper->createBullet(m_pOwner->getOwnerId(), bulletid, m_pOwner, *iter);
			CHECK_RETURN_VOID(pBullet);
        }
    }
}

void CSkill::fireBullet(Vec2 &pos, int bulletid)
{
    if (!pos.equals(SKILL_INVALID_POINT))
    {
        CBattleHelper *pHelper = m_pOwner->getBattleHelper();
		CHECK_RETURN_VOID(pHelper);
		CBullet* pBullet = pHelper->createBullet(m_pOwner->getOwnerId(), bulletid, m_pOwner, NULL, pos);
		CHECK_RETURN_VOID(pBullet);
    }
}

void CSkill::callSoldier()
{
    CBattleHelper *pHelper = m_pOwner->getBattleHelper();
    for (std::vector<int>::const_iterator iter = m_pSkillConf->Call.begin();
        iter != m_pSkillConf->Call.end(); ++iter)
    {
		BattleCommandInfo info;
		info.Tick		= pHelper->GameTick;
		info.CommandId	= CommandCallSolider;
		info.ExecuterId = m_pOwner->getOwnerId();
		info.Ext1		= *iter;
		info.Ext2		= m_pOwner->getIntAttribute(ECardLevel);
        info.Ext3       = static_cast<int>(m_TargetPos.x);
        info.Ext4       = static_cast<int>(m_TargetPos.y);
		pHelper->insertBattleCommand(info);
    }
}

void CSkill::addBuff(std::vector<CRole*> &targets)
{
    for (std::vector<ID_Num>::const_iterator iter = m_pSkillConf->Buff.begin();
        iter != m_pSkillConf->Buff.end(); ++iter)
    {
        for (std::vector<CRole*>::iterator siter = targets.begin(); 
            siter != targets.end(); ++siter)
        {
            CRole *pTarget = *siter;
            if (!pTarget->canRemove())
            {
                pTarget->getBuffComponent()->addBuff(m_pOwner, iter->ID, iter->num);
            }
        }
    }
}
