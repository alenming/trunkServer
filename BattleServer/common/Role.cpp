#include "Role.h"
#include "RoleComponent.h"
#include "ComponentCache.h"

#ifndef RunningInServer

#include "ArmatureComponent.h"
#include "RoleDisplayComponent.h"
#include "SpineComponent.h"
#include "ConfOther.h"
#include "HPBar.h"
#include "BlinkAction.h"
#include "Game.h"

USING_NS_CC;

#endif

using namespace std;

void dumpIntMap(std::map<int, int>& m)
{
    for (map<int, int>::iterator iter = m.begin();
        iter != m.end(); ++iter)
    {
        KXLOGBATTLE("int key %d value %d", iter->first, iter->second);
    }
}

void dumpFloatMap(std::map<int, float>& m)
{
    for (map<int, float>::iterator iter = m.begin();
        iter != m.end(); ++iter)
    {
        KXLOGBATTLE("float key %d value %f", iter->first, iter->second);
    }
}

CBlackBoard::CBlackBoard()
	:m_BattleHelper(NULL)
{
}

CBlackBoard::~CBlackBoard()
{
    clearBlackBoard();
}

void CBlackBoard::clearTargetList()
{
    releaseAndClearVec<CRole*>(m_TargetList);
}

void CBlackBoard::clearBlackBoard()
{
    releaseAndClearVec<CRole*>(m_TargetList);
    m_TargetPoint = Vec2::ZERO;
}

void CBlackBoard::addToTargetList(CRole* role)
{
    if (NULL != role)
    {
        SAFE_RETAIN(role);
        m_TargetList.push_back(role);
    }
}

void CBlackBoard::clearAndAddToTargetList(CRole* role)
{
    releaseAndClearVec<CRole*>(m_TargetList);
    if (NULL != role)
    {
		SAFE_RETAIN(role);
        m_TargetList.push_back(role);
    }
}

void CBlackBoard::removeFormTargetList(CRole* role)
{
    if (NULL != role)
    {
        for (vector<CRole*>::iterator iter = m_TargetList.begin();
            iter != m_TargetList.end(); ++iter)
        {
            if ((*iter)->getObjectId() == role->getObjectId())
            {
                SAFE_RELEASE((*iter));
                m_TargetList.erase(iter);
                break;
            }
        }
    }
}

bool CBlackBoard::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);

	data.writeData(m_TargetPoint.x);
    data.writeData(m_TargetPoint.y);

	int length = m_TargetList.size();
	data.writeData(length);	
	std::vector<CRole*>::iterator iter = m_TargetList.begin();
	for (; iter != m_TargetList.end(); ++iter)
	{
		data.writeData((*iter)->getObjectId());
	}

	SERIALIZE_MATCH(data);

	return true;
}

bool CBlackBoard::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);

	if (NULL == m_BattleHelper)
	{
		return false;
	}
    
    float x = 0.0f, y = 0.0f;
    data.readData(x);
    data.readData(y);
    m_TargetPoint = Vec2(x, y);
	
	int nTargetCount = 0;
	data.readData(nTargetCount);
    
	// 清理目标列表
	releaseAndClearVec(m_TargetList);

	for (int i = 0; i < nTargetCount; i++)
	{
		int nObjectID = 0;
		data.readData(nObjectID);
		// 根据objectID获取CROLE
		CRole *pRole = m_BattleHelper->getRoleByObjectId(nObjectID);
		if (NULL == pRole)
		{
			continue;
			LOG("error about role NULL!");
		}
		else
		{
			SAFE_RETAIN(pRole);
		}

		m_TargetList.push_back(pRole);
	}

	UN_SERIALIZE_MATCH(data);

	return true;
}

CRole::CRole()
: m_AIComponent(NULL),
m_BuffComponent(NULL),
m_SkillComponent(NULL),
m_RoleComponent(NULL),
m_DisplayNode(NULL)
{
}


CRole::~CRole()
{
}

bool CRole::init(int typeId, int ownerId, int objId, CBattleHelper* battle)
{
#ifndef RunningInServer
	setOpacity(255);
	setCascadeOpacityEnabled(true);
#endif 

    for (int i = EAttributeIntVarBegin + 1; i < EAttributeIntVarEnd; ++i)
    {
        m_IntAttributes[i] = 0;
    }
    for (int i = EAttributeFloatVarBegin + 1; i < EAttributeFloatVarEnd; ++i)
    {
        m_FloatAttributes[i] = 0.0f;
    }
	for (int i = EStatBegin + 1; i < EStatEnd; i++)
	{
		m_IntAttributes[i] = 0;
	}
    // 初始化固定的动态属性
    initAttribute(EAttributeExtraHP, 0);
    initAttribute(EAttributeBeAoeable, 1);
    initAttribute(EAttributeBeAttackable, 1);
    initAttribute(EAttributeBeBuffable, 1);
    initAttribute(EAttributeHitPowerX, 0.0f);
    initAttribute(EAttributeHitPowerY, 0.0f);
    initAttribute(EAttributeX, 0.0f);
    initAttribute(EAttributeY, 0.0f);
    // EAttributeYmin 由创建对象者设置
    // Camp由具体子类设置
    m_FloatAttributes[EAttributeAttackSpeedVar] = 1.0f;
	
    m_BlackBoard.setBattleHelper(battle);
    return CGameObject::init(typeId, ownerId, objId, battle);
}

void CRole::onExit()
{
    CGameObject::onExit();
    m_BlackBoard.clearBlackBoard();
    m_AIComponent = NULL;
    m_BuffComponent = NULL;
    m_SkillComponent = NULL;
    m_RoleComponent = NULL;
#ifndef RunningInServer
    m_DisplayNode = NULL;
#endif

}

void CRole::initRoleAttribute(CRoleModel *model)
{
    CHECK_RETURN_VOID(NULL != model);
    m_pModel = model;

    int offset = EAttributeIntBegin - EClassIntBegin;
    for (int i = EClassIntBegin + 1; i < EClassIntEnd; ++i)
    {
        m_IntAttributes[offset + i] = model->getClassInt(static_cast<EAttributeTypes>(i));
    }
    offset = EAttributeFloatEnd - EClassFloatEnd;
    for (int i = EClassFloatBegin + 1; i < EClassFloatEnd; ++i)
    {
        m_FloatAttributes[offset + i] = model->getClassFloat(static_cast<EAttributeTypes>(i));
    }

    m_IntAttributes[EAttributeHPPosX] = 0;
    m_IntAttributes[EAttributeHPPosY] = 0;
    m_IntAttributes[EAttributeHP] = m_IntAttributes[EAttributeMaxHP];
    // 菲迪说，初始蓝量为0，2016-5-25
    m_IntAttributes[EAttributeMP] = 0; //m_IntAttributes[EAttributeMaxMP];
    m_IntAttributes[EAttributeStrong] = m_IntAttributes[EAttributeMaxStrong];
    m_IntAttributes[EAttributeRage] = m_IntAttributes[EAttributeMaxRage];;
    m_IntAttributes[ECardLevel] = model->getLevel();
 
	float scale = m_FloatAttributes[EAttributeScale];
    modifyAttribute(EAttributeFireRange, floatToInt(getIntAttribute(EClassFireRange) * scale));
	const Role* role = model->getRoleComm();
	m_FireOffset = role->FireOffset * scale;
	m_FireOffset.x *= m_nDirection;
	m_HitOffset = role->HitOffset * scale;
	m_HitOffset.x *= m_nDirection;
	m_HeadOffset = role->HeadOffset * scale;
	m_HeadOffset.x *= m_nDirection;

    //dumpIntMap(m_IntAttributes);
    //dumpFloatMap(m_FloatAttributes);
}

bool CRole::initRoleComponents(const Role* role)
{
	// 由外部先创建角色组件
	CHECK_RETURN(NULL != m_RoleComponent);

    // 添加AI组件
    m_AIComponent = createComponent<CAIComponent>(EAIComponent);
    CHECK_RETURN(m_AIComponent);
    if (!m_AIComponent->init(role->AIID, this))
    {
        CComponentCache::getInstance()->freeComponent(m_AIComponent);
        KXLOGDEBUG("CRole AIComponent init fail roleID %d, AIID %d", role->ClassID, role->AIID);
        return false;
    }
    addLogicComponent(m_AIComponent);

	// 添加BUFF组件
	//m_BuffComponent = createComponent<CBuffComponent>(EBuffComponent);
    //CHECK_RETURN(m_BuffComponent);
    //if (!m_BuffComponent->init(this))
    //{
    //    CComponentCache::getInstance()->freeComponent(m_BuffComponent);
    //    KXLOGDEBUG("CRole AIComponent init fail roleID %d", role->ClassID);
    //    return false;
    //}
    //addLogicComponent(m_BuffComponent);

    // 如果技能配置不为空，添加技能组件
	if (role->Skill.size() > 0)
	{
		if (NULL == m_SkillComponent)
        {            
            m_SkillComponent = createComponent<CSkillComponent>(ESkillComponent);
            CHECK_RETURN(m_SkillComponent);
            if (!m_SkillComponent->init(this, role->Skill))
            {
                CComponentCache::getInstance()->freeComponent(m_SkillComponent);
                KXLOGDEBUG("CRole skillComponent init fail roleID %d", role->ClassID);
                return false;
            }
			addLogicComponent(m_SkillComponent);
		}
		else
		{
			CHECK_RETURN(m_SkillComponent->init(this, role->Skill));
            if (!m_SkillComponent->init(this, role->Skill))
            {
                CComponentCache::getInstance()->freeComponent(m_SkillComponent);
                KXLOGDEBUG("CRole skillComponent init fail roleID %d", role->ClassID);
                return false;
            }
		}		
	}
	return true;
}

// 初始化显示相关的组件（服务器为空实现）
bool CRole::initDisplayComponents(const Role* role)
{
#ifndef RunningInServer

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    // 用于在调试器中
    switch (m_RoleType)
    {
    case RT_HERO:
        setName("Summoner" + toolToStr(m_nGameObjectId));
        break;
    case RT_SOLDIER:
        setName("Hero" + toolToStr(m_nGameObjectId));
        break;
    case RT_MONSTER:
        setName("Monster" + toolToStr(m_nGameObjectId));
        break;
    case RT_BOSS:
        setName("Boss" + toolToStr(m_nGameObjectId));
        break;
    case RT_SUMMON:
        setName("Call" + toolToStr(m_nGameObjectId));
        break;

    case RT_NONE:
    default:
        break;
    }
#endif


	m_DisplayNode = Node::create();
	this->addChild(m_DisplayNode);
    getDisplayNode()->setScale(role->Scale);

	const SResPathItem* pResInfo = queryConfSResInfo(role->AnimationID);
    LOG("initDisplayComponents AnimationId %d", role->AnimationID);
    CHECK_RETURN(pResInfo);
	CAnimateComponent *pAnimateCom = NULL;

	// 骨骼动画
	if (RT_ARMATURE == pResInfo->ResType)
    {
		// 添加显示组件
		CArmatureComponent* armcom = new CArmatureComponent();
		if (!armcom->initAnimate(pResInfo->ResName, getDisplayNode()))
		{
			armcom->release();
            KXLOGERROR("error resName %s", pResInfo->ResName.c_str());
#ifndef RunningInServer
            if (CGame::getInstance()->isDebug())
            {
                cocos2d::MessageBox(pResInfo->ResName.c_str(), "error resName");
            }
#endif
			return false;
		}
		addComponent(armcom);
		armcom->release();
		pAnimateCom = armcom;
	}
	else if (RT_SPINE == pResInfo->ResType)
	{
		CSpineComponent* spinecom = new CSpineComponent();
        if (!spinecom->initAnimate(pResInfo->Path, getDisplayNode()))
		{
            spinecom->release();
            KXLOGERROR("error Path %s", pResInfo->Path.c_str());
#ifndef RunningInServer
            if (CGame::getInstance()->isDebug())
            {
                cocos2d::MessageBox(pResInfo->Path.c_str(), "error Path");
            }
#endif
			return false;
		}
		if (pResInfo->Skin.empty() == false)
		{
			spinecom->getSkeletonAnimation()->setSkin(pResInfo->Skin);
		}

		addComponent(spinecom);
		spinecom->release();
		pAnimateCom = spinecom;

        //设置色相,目前只有spine需要
        spinecom->setHSV(role->AnimationHSV);
	}
	else
    {
        KXLOGERROR("error type %d", pResInfo->ResType);
#ifndef RunningInServer
        if (CGame::getInstance()->isDebug())
        {
            cocos2d::MessageBox(String::createWithFormat("%d", pResInfo->ResType)->getCString(), "error type");
        }
#endif
		return false;
	}

	// 添加显示逻辑组件
	CRoleDisplayComponent* rolecom = new CRoleDisplayComponent();
	if (!rolecom->init(this, pAnimateCom, m_RoleComponent))
	{
		pAnimateCom->release();
		rolecom->release();
        KXLOGERROR("error obj %p, %p", pAnimateCom, m_RoleComponent);
#ifndef RunningInServer
        if (CGame::getInstance()->isDebug())
        {
            cocos2d::MessageBox(String::createWithFormat("%p %p", pAnimateCom, m_RoleComponent)->getCString(), "error obj");
        }
#endif
		return false;
	}
	addComponent(rolecom);
	rolecom->release();

	// 怪物名字设置
	switch (m_RoleType)
	{
	case RT_BOSS:
	//case RT_MONSTER:
        rolecom->getHPBar()->setName(getBattleHelper()->getComputerModel()->getUserName());
		break;
	case RT_HERO:
	//case RT_SOLDIER:
        rolecom->getHPBar()->setName(getBattleHelper()->getUserModel(getOwnerId())->getUserName());
		break;
	default:
		break;
	}

#endif

	// 服务器直接返回true
	return true;
}

bool CRole::haveBuff(int buffId)
{
    if (NULL != m_BuffComponent)
    {
        return m_BuffComponent->hasBuff(buffId);
    }

    return false;
}

bool CRole::addBuff(CRole *maker, int buffId, int addstack)
{
    if (NULL != m_BuffComponent)
    {
        m_BuffComponent->addBuff(maker, buffId, addstack);
        return true;
    }

    return false;
}

void CRole::delBuff(int buffId)
{
	if (NULL != m_BuffComponent)
	{
		m_BuffComponent->removeBuff(buffId);
	}
}

CSkill *CRole::getSkillWithID(int skillId)
{
    if (NULL != m_SkillComponent)
    {
        return m_SkillComponent->getSkillWithID(skillId);
    }

    return NULL;
}

CSkill *CRole::getSkillWithIndex(int skillIndex)
{
    if (NULL != m_SkillComponent)
    {
        return m_SkillComponent->getSkillWithIndex(skillIndex);
    }

    return NULL;
}

bool CRole::canExecuteSkillIndex(int skillIndex)
{
    if (NULL != m_SkillComponent)
    {
        return m_SkillComponent->canExecuteSkillIndex(skillIndex);
    } 

    return false;
}

bool CRole::executeSkillIndex(int skillIndex, Vec2 &pos)
{
    if (NULL != m_SkillComponent)
    {
        if (m_SkillComponent->canExecuteSkillIndex(skillIndex))
        {
            m_SkillComponent->executeSkillIndex(skillIndex, pos);
        }
        return true;
    }

    return false;
}

bool CRole::canExecuteSkill(int skillId)
{
    if (NULL != m_SkillComponent)
    {
        return m_SkillComponent->canExecuteSkill(skillId);
    }

    return false;
}

bool CRole::executeSkill(int skillId, Vec2 &pos)
{
    if (NULL != m_SkillComponent)
    {
        //注意指定位置
        if (m_SkillComponent->canExecuteSkill(skillId))
        {
            m_SkillComponent->executeSkill(skillId, pos);
        }
        return true;
    }

    return false;
}

bool CRole::executeSkill(int skillId, std::vector<CRole*> &targets, Vec2 &pos)
{
    if (NULL != m_SkillComponent)
    {
        //注意指定位置
        if (m_SkillComponent->canExecuteSkill(skillId))
        {
            m_SkillComponent->executeSkill(skillId, targets, pos);
        }
        return true;
    }

    return false;
}

CState* CRole::changeState(int stateId)
{
    if (NULL != m_RoleComponent)
    {
        return m_RoleComponent->changeState(stateId);
    }

    return NULL;
}

int CRole::currentState()
{
    if (NULL != m_RoleComponent)
    {
        return m_RoleComponent->getStateId();
    }

    return State_None;
}

void CRole::updateStat(EAttributeTypes stat)
{
    if (stat > EStatBegin && stat < EStatEnd)
    {
        m_IntAttributes[stat] += 1;
    }
}

bool CRole::updateAttribute(EAttributeTypes key, int v)
{
    std::map<int, int>::iterator iter = m_IntAttributes.find(key);
    if (iter == m_IntAttributes.end())
    {
        return false;
    }
    else
    {
        iter->second = limitAttribute(key, iter->second + v);
        return true;
    }
}

bool CRole::updateAttribute(EAttributeTypes key, float v)
{
    std::map<int, float>::iterator iter = m_FloatAttributes.find(key);
    if (iter == m_FloatAttributes.end())
    {
        return false;
    }
    else
    {
        iter->second = limitAttribute(key, iter->second + v);
        return true;
    }
}

bool CRole::checkAttribute(EAttributeTypes key, int& outv)
{
    std::map<int, int>::iterator iter = m_IntAttributes.find(key);
    if (iter != m_IntAttributes.end())
    {
        outv = iter->second;
        return true;
    }
    if (m_pModel->classIntKeyExist(key))
    {
        outv = m_pModel->getClassInt(key);
        return true;
    }
    return false;
}

bool CRole::checkAttribute(EAttributeTypes key, float& outv)
{
    std::map<int, float>::iterator iter = m_FloatAttributes.find(key);
    if (iter != m_FloatAttributes.end())
    {
        outv = iter->second;
        return true;
    }
    if (m_pModel->classFloatKeyExist(key))
    {
        outv = m_pModel->getClassFloat(key);
        return true;
    }
    return false;
}

int CRole::getIntAttribute(EAttributeTypes key)
{
    std::map<int, int>::iterator iter = m_IntAttributes.find(key);
    if (iter != m_IntAttributes.end())
    {
        return iter->second;
    }
    return m_pModel->getClassInt(key);
}

float CRole::getFloatAttribute(EAttributeTypes key)
{
    std::map<int, float>::iterator iter = m_FloatAttributes.find(key);
    if (iter != m_FloatAttributes.end())
    {
        return iter->second;
    }
    return m_pModel->getClassFloat(key);
}

int CRole::limitAttribute(EAttributeTypes key, int v)
{
    // 检查上下限
    switch (key)
    {
    case EAttributeExtraHP:
        limitValue(v, 0, 10000);
        break;
    case EAttributeSpeed:
        limitValue(v, 33, 400);
        break;
    /*case EAttributeSpeedVar:
        //if (v > 200) v = 200;
        break;
    case EAttributePPenetrateVar:
    case EAttributeMPenetrateVar:
        break;*/
    case EAttributeRageVar:
    case EAttributeRageRecoverVar:
    case EAttributeMPVar:
    case EAttributeMPRecoverVar:
        limitValue(v, -20000, 10000);
        break;
    case EAttributeMaxRage:
    case EAttributeMaxMP:
    case EAttributeMPRecover:
        limitValue(v, 0, 10000);
        break;
    case EAttributeMaxStrong:
    case EAttributeStrongRecover:
        limitValue(v, 0, 10000);
    case EAttributeStrongVar:
        limitValue(v, -20000, 10000);
        break;
    case EAttributeHP:
        limitValue(v, 0, m_IntAttributes[EAttributeMaxHP]);
        break;
    case EAttributeMP:
        limitValue(v, 0, m_IntAttributes[EAttributeMaxMP]);
        break;
    case EAttributeRage:
        limitValue(v, 0, m_IntAttributes[EAttributeMaxRage]);
        break;
    case EAttributeStrong:
        limitValue(v, 0, m_IntAttributes[EAttributeMaxStrong]);
        break;
    case EAttributeHaterd:
        limitValue(v, 0, 100);
        break;
    case EAttributeBeAoeable:
    case EAttributeBeAttackable:
    case EAttributeBeBuffable:
        limitValue(v, 0, 1);
        break;

    case EAttributePGuardVar:
    case EAttributeMGuardVar:
    case EAttributeDefendVar:
    case EAttributePAttackVar:
    case EAttributeMAttackVar:
        break;
    case EAttributeResustanceVar:
        limitValue(v, -2, 1);
        break;
    default:
        if (v < 0) { v = 0; }
        break;
    }
    return v;
}

float CRole::limitAttribute(EAttributeTypes key, float v)
{
    // 检查上下限
    switch (key)
    {
    case EAttributeX:
        m_Position.x = v;
        setPositionX(v);
        break;
    case EAttributeY:
        m_Position.y = v;
        setPositionY(v);
        break;
    /*case EAttributeScale:
    case EAttributeEffectScale:
    case EAttributeCritDamageVar:
        break;
    case EAttributeDefendVar:
    case EAttributeResustanceVar:
        //limitValue(v, -1.75f, 0.75f);
        break;
    case EAttributeCritVar:
        //limitValue(v, -200.0f, 100.0f);
        break;
    case EAttributeReboundVar:
    case EAttributeVampireVar:
        //limitValue(v, -150.0f, 50.0f);
        break;
    case EAttributeMissVar:
        //limitValue(v, -175.0f, 75.0f);
        break;*/
    case EAttributeDefend:
    case EAttributeResustance:
        limitValue(v, -1.0f, 1.0f);
        break;
    case EAttributeVampire:
    case EAttributeRebound:
    case EAttributeMiss:
    case EAttributeCrit:
        limitValue(v, 0.0f, 100.0f);
        break;
    default:
        if (v < 0.0f) { v = 0.0f; }
        break;
    }
    return v;
}

void CRole::initAttribute(EAttributeTypes key, int v)
{
    m_IntAttributes[key] = v;
}

void CRole::initAttribute(EAttributeTypes key, float v)
{
    m_FloatAttributes[key] = v;
}

bool CRole::canModifyAttribute(EAttributeTypes key)
{
    // 过滤不可修改
    switch (key)
    {
        // int 类型
        case EAttributePAttack:
        case EAttributeMAttack:
        case EAttributePGuard:
        case EAttributeMGuard:
        case EAttributePPenetrate:
        case EAttributeMPenetrate:
        case EAttributeAttackSpeed:
        case EAttributeMaxRage:
        case EAttributeRageRecover:
        //case EAttributeMaxMP:
        case EAttributeMaxStrong:
        case EAttributeStrongRecover:
        case EAttributeYmin:
        // float 类型
        case EAttributeDefend:
        case EAttributeResustance:
        case EAttributeVampire:
        case EAttributeRebound:
        case EAttributeMiss:
        case EAttributeCrit:
        case EAttributeCritDamage:
            LOG("Error You Can Not Modify Attribute %d At Count", key);
            return false;
        default:
            return true;
    }
}

void CRole::modifyAttribute(EAttributeTypes key, int v)
{
    m_IntAttributes[key] = limitAttribute(key, v);
}

void CRole::modifyAttribute(EAttributeTypes key, float v)
{
    m_FloatAttributes[key] = limitAttribute(key, v);
}

void CRole::changeOtherAttrbute(EAttributeTypes key)
{
    // 进行联动
    switch (key)
    {
        // 修改缩放导致的联动
        case EAttributeScale:
        {
            float scale = m_FloatAttributes[key];
#ifndef RunningInServer
            getDisplayNode()->setScale(scale);
#endif    
            modifyAttribute(EAttributeFireRange, (int)(getIntAttribute(EClassFireRange) * scale));
            //modifyAttribute(EAttributeHPLine, getIntAttribute(EClassHPLine) * scale);
            const Role* role = m_pModel->getRoleComm();
            m_FireOffset = role->FireOffset * scale;
            m_FireOffset.x *= m_nDirection;
            m_HitOffset = role->HitOffset * scale;
            m_HitOffset.x *= m_nDirection;
            m_HeadOffset = role->HeadOffset * scale;
            m_HeadOffset.x *= m_nDirection;
            break;
        }
        // 角色移动速度变化值
		case EAttributeSpeedVar:
		{	
			int var = m_IntAttributes[EAttributeSpeedVar];
			limitValue(var, -200, 200);
			modifyAttribute(EAttributeSpeed, getIntAttribute(EClassSpeed) + var);
			break;
		}
        // 角色物理攻击力变化值
        case EAttributePAttackVar:
            modifyAttribute(EAttributePAttack, getIntAttribute(EClassPAttack) + m_IntAttributes[EAttributePAttackVar]);
            break;
        // 角色魔法攻击力变化值
        case EAttributeMAttackVar:
            modifyAttribute(EAttributeMAttack, getIntAttribute(EClassMAttack) + m_IntAttributes[EAttributeMAttackVar]);
            break;
        // 角色物理护甲变化值
        case EAttributePGuardVar:
            modifyAttribute(EAttributePGuard, getIntAttribute(EClassPGuard) + m_IntAttributes[EAttributePGuardVar]);
            break;
        // 角色魔法护甲变化值
        case EAttributeMGuardVar:
            modifyAttribute(EAttributeMGuard, getIntAttribute(EClassMGuard) + m_IntAttributes[EAttributeMGuardVar]);
            break;
        // 角色物理穿透变化值
        case EAttributePPenetrateVar:
            modifyAttribute(EAttributePPenetrate, getIntAttribute(EClassPPenetrate) + m_IntAttributes[EAttributePPenetrateVar]);
            break;
        // 角色魔法穿透变化值
        case EAttributeMPenetrateVar:
            modifyAttribute(EAttributeMPenetrate, getIntAttribute(EClassMPenetrate) + m_IntAttributes[EAttributeMPenetrateVar]);
            break;
        // 角色攻击速率
        case EAttributeAttackSpeedVar:
			modifyAttribute(EAttributeAttackSpeed, getIntAttribute(EClassAttackSpeed) / m_FloatAttributes[EAttributeAttackSpeedVar]);
            break;
        // 角色怒气上限值变化值
        case EAttributeRageVar:
            modifyAttribute(EAttributeMaxRage, getIntAttribute(EClassRage) + m_IntAttributes[EAttributeRageVar]);
            break;
        // 角色怒气回复值变化值
        case EAttributeRageRecoverVar:
            modifyAttribute(EAttributeRageRecover, getIntAttribute(EClassRageRecover) + m_IntAttributes[EAttributeRageRecoverVar]);
            break;
        // 角色魔法值上限值变化值
        case EAttributeMPVar:
            modifyAttribute(EAttributeMaxMP, getIntAttribute(EClassMP) + m_IntAttributes[EAttributeMPVar]);
            break;
        // 角色魔法回复变化值
        case EAttributeMPRecoverVar:
            modifyAttribute(EAttributeMPRecover, getIntAttribute(EClassMPRecover) + m_IntAttributes[EAttributeMPRecoverVar]);
            break;
        // 角色霸体上限值变化值
        case EAttributeStrongVar:
            modifyAttribute(EAttributeMaxStrong, getIntAttribute(EClassStrong) + m_IntAttributes[EAttributeStrongVar]);
            break;
        // 角色霸体值恢复速率
        case EAttributeStrongRecoverVar:
            modifyAttribute(EAttributeStrongRecover, getIntAttribute(EClassStrongRecover) * m_IntAttributes[EAttributeStrongRecoverVar]);
            break;

        // Float Attributes ...
        // 角色暴击值变化值
        case EAttributeCritVar:
            modifyAttribute(EAttributeCrit, getFloatAttribute(EClassCrit) + m_FloatAttributes[EAttributeCritVar]);
            break;
        //角色暴击伤害变化值
		case EAttributeCritDamageVar:
		{
			float var = m_FloatAttributes[EAttributeCritDamageVar];
			limitValue(var, -200.0f, 100.0f);
			modifyAttribute(EAttributeCritDamage, getFloatAttribute(EClassCritDamage) + var);
			break;
		}
        //角色闪避值变化值
		case EAttributeMissVar:
		{
			float var = m_FloatAttributes[EAttributeMissVar];
			limitValue(var, -200.0f, 100.0f);
			modifyAttribute(EAttributeMiss, getFloatAttribute(EClassMiss) + var);
			break;
		}
        //角色反弹值变化值
		case EAttributeReboundVar:
		{
			float var = m_FloatAttributes[EAttributeReboundVar];
			limitValue(var, -200.0f, 100.0f);
			modifyAttribute(EAttributeRebound, getFloatAttribute(EClassRebound) + var);
			break;
		}
        //角色吸血值变化值
		case EAttributeVampireVar:
		{
			float var = m_FloatAttributes[EAttributeVampireVar];
			limitValue(var, -200.0f, 100.0f);
			modifyAttribute(EAttributeVampire, getFloatAttribute(EClassVampire) + var);
			break;
		} 
        // 角色魔法减伤变化值
		case EAttributeResustanceVar:
		{
			float var = m_FloatAttributes[EAttributeResustanceVar];
			limitValue(var, -2.0f, 1.0f);
			modifyAttribute(EAttributeResustance, getFloatAttribute(EClassResustance) + var);
			break;
		}
        // 角色物理减伤变化值
		case EAttributeDefendVar:
		{
			float var = m_FloatAttributes[EAttributeDefendVar];
			limitValue(var, -2.0f, 1.0f);
			modifyAttribute(EAttributeDefend, getFloatAttribute(EClassDefend) + var);
			break;
		}
        default:
            break;
    }
}

void CRole::setAttribute(EAttributeTypes key, int v)
{
    if (!canModifyAttribute(key))
    {
        return;
    }
    modifyAttribute(key, v);
    changeOtherAttrbute(key);
}

void CRole::setAttribute(EAttributeTypes key, float v)
{
    if (!canModifyAttribute(key))
    {
        return;
    }
    modifyAttribute(key, v);
    changeOtherAttrbute(key);
}

bool CRole::serialize(CBufferData& data)
{

	SERIALIZE_MATCH(data);

	if (!CGameObject::serialize(data))
	{
		return false;
	}

	// Attribute - 动态属性(int)
	for (int h = EAttributeBegin + 1; h <= EAttributeBeBuffable; ++h)
	{
		data.writeData(getIntAttribute((EAttributeTypes)h));
	}

	// Attribute - 动态属性(float)
	for (int i = EAttributeYmin; i < EAttributeEnd; ++i)
	{
		data.writeData(getFloatAttribute((EAttributeTypes)i));
	}

	// Attribute - int 属性
	for (int j = EAttributeIntBegin + 1; j < EAttributeIntEnd; ++j)
	{
		data.writeData(getIntAttribute((EAttributeTypes)j));
	}

	// Attribute - float 属性
	for (int k = EAttributeFloatBegin + 1; k < EAttributeFloatEnd; ++k)
	{
		data.writeData(getFloatAttribute((EAttributeTypes)k));
	}

	// Var 属性 - int 属性
	for (int m = EAttributeIntVarBegin + 1; m < EAttributeIntVarEnd; ++m)
	{
		data.writeData(getIntAttribute((EAttributeTypes)m));
	}

	// Var 属性 - float 属性
	for (int n = EAttributeFloatVarBegin + 1; n < EAttributeFloatVarEnd; ++n)
	{
		data.writeData(getFloatAttribute((EAttributeTypes)n));
	}

	// Stat属性 (int)
	for (int x = EStatBegin + 1; x <= EStatAttack; ++x)
	{
		data.writeData(getIntAttribute((EAttributeTypes)x));
	}

	data.writeData(m_RoleType);
	if (RT_HERO == m_RoleType)
	{
		for (int y = EHeroCrystalLevel; y <= EHeroMaxCrystal; ++y)
		{
			data.writeData(getIntAttribute((EAttributeTypes)y));
		}
		for (int z = EHeroCrystal; z <= EHeroCrystalSpeed; ++z)
		{
			data.writeData(getFloatAttribute((EAttributeTypes)z));
		}
	}

	//BOSS技能组件可能不存在
	if (!m_bSafeRemove)
	{
		// 黑板
		m_BlackBoard.serialize(data);
		// AI组件
		m_AIComponent->serialize(data);
		// BUFF组件
		m_BuffComponent->serialize(data);
		if (RT_BOSS != m_RoleType)
		{
			// 技能组件
			m_SkillComponent->serialize(data);
		}
		// 角色组件
		m_RoleComponent->serialize(data);
	}

	SERIALIZE_MATCH(data);

	return true;
}

bool CRole::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);

	if (!CGameObject::unserialize(data))
	{
		return false;
	}

	int iValue;
	float fValue;

	// Attribute - 动态属性(int)
	for (int h = EAttributeBegin + 1; h <= EAttributeBeBuffable; ++h)
	{
		data.readData(iValue);
		m_IntAttributes[h] = iValue;
	}

	// Attribute - 动态属性(float)
	for (int i = EAttributeYmin; i < EAttributeEnd; ++i)
	{
		data.readData(fValue);
		m_FloatAttributes[i] = fValue;
	}

	// Attribute - int 属性
	for (int j = EAttributeIntBegin + 1; j < EAttributeIntEnd; ++j)
	{
		data.readData(iValue);
		m_IntAttributes[j] = iValue;
	}

	// Attribute - float 属性
	for (int k = EAttributeFloatBegin + 1; k < EAttributeFloatEnd; ++k)
	{
		data.readData(fValue);
		m_FloatAttributes[k] = fValue;
	}

	float scale = m_FloatAttributes[EAttributeScale];
    modifyAttribute(EAttributeFireRange, floatToInt(getIntAttribute(EClassFireRange) * scale));
	const Role* role = m_pModel->getRoleComm();
	m_FireOffset = role->FireOffset * scale;
	m_FireOffset.x *= m_nDirection;
	m_HitOffset = role->HitOffset * scale;
	m_HitOffset.x *= m_nDirection;
	m_HeadOffset = role->HeadOffset * scale;
	m_HeadOffset.x *= m_nDirection;

	// Var 属性 - int 属性
	for (int m = EAttributeIntVarBegin + 1; m < EAttributeIntVarEnd; ++m)
	{
		data.readData(iValue);
		m_IntAttributes[m] = iValue;
	}

	// Var 属性 - float 属性
	for (int n = EAttributeFloatVarBegin + 1; n < EAttributeFloatVarEnd; ++n)
	{
		data.readData(fValue);
		m_FloatAttributes[n] = fValue;
	}

	// Stat属性 (int)
	for (int x = EStatBegin + 1; x <= EStatAttack; ++x)
	{
		data.readData(iValue);
		m_IntAttributes[x] = iValue;
	}

	data.readData(m_RoleType);
	if (RT_HERO == m_RoleType)
	{
		for (int y = EHeroCrystalLevel; y <= EHeroMaxCrystal; ++y)
		{
			data.readData(iValue);
			m_IntAttributes[y] = iValue;
		}
		for (int z = EHeroCrystal; z <= EHeroCrystalSpeed; ++z)
		{
			data.readData(fValue);
			m_FloatAttributes[z] = fValue;
		}
	}

	if (!m_bSafeRemove)
	{
		// 黑板
		m_BlackBoard.unserialize(data);
		// AI组件	
		m_AIComponent->unserialize(data);
		// BUFF组件
		m_BuffComponent->unserialize(data);
		if (RT_BOSS != m_RoleType)
		{
			// 技能组件
			m_SkillComponent->unserialize(data);
		}
		// 角色组件	
		m_RoleComponent->unserialize(data);
	}

#ifndef RunningInServer
	//Color3B color;
	//color.r = m_IntAttributes[EAttributeColorR];
	//color.g = m_IntAttributes[EAttributeColorG];
	//color.b = m_IntAttributes[EAttributeColorB];
	//getDisplayNode()->setColor(color);
#endif

	UN_SERIALIZE_MATCH(data);


	return true;
}
