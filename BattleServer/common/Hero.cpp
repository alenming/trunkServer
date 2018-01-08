#include "Hero.h"
#include "HeroComponent.h"
#include "ConfManager.h"
#include "Soldier.h"
#include "ComponentCache.h"

#ifndef RunningInServer
#include "ArmatureComponent.h"
#endif 
using namespace std;

CHero::CHero() 
	:m_SoldierCards(NULL)
{
    m_RoleType = RT_HERO;
    m_nObjectType = EObjectHero;

#ifndef RunningInServer
    m_nWithholdValue = 0;
    m_bStartSkillExecute = false;
    m_fCurSkillExecutingTime = 0;
    m_fMaxSKillExecutingTime = 0;
#endif
}

CHero::~CHero()
{
    for (map<int, CRole*>::iterator iter = m_SingleMap.begin();
        iter != m_SingleMap.end(); ++iter)
    {
        SAFE_RELEASE(iter->second);
    }
}

bool CHero::init(int typeId, int ownerId, int objId, CBattleHelper* battle)
{
    CHECK_RETURN(CRole::init(typeId, ownerId, objId, battle));

    CPlayerModel* userModel = dynamic_cast<CPlayerModel*>(battle->getUserModel(ownerId));
    CHECK_RETURN(NULL != userModel);
	
    const HeroConfItem* confItem = queryConfHero(typeId);
    CHECK_RETURN(NULL != confItem);

    // 获取英雄配置
    initRoleAttribute(userModel->getRoleModel());
    initAttribute(EAttributeCamp, userModel->getCamp());
    initAttribute(EHeroCrystalSpeedParam, confItem->CrystalSpeedPrarm);
	
	// 根据模型初始化士兵卡片列表
    m_SoldierCards = &userModel->getSoldierCards();

    // 添加BUFF组件 
    m_BuffComponent = createComponent<CBuffComponent>(EBuffComponent);
    CHECK_RETURN(m_BuffComponent);
    if (!m_BuffComponent->init(this))
    {
        CComponentCache::getInstance()->freeComponent(m_BuffComponent);
        KXLOGDEBUG("CHero buffComponent init fail typeId %d", typeId);
        return false;
    }
    addLogicComponent(m_BuffComponent);

	// 初始化角色组件
    m_RoleComponent = createComponent<CHeroComponent>(EHeroComponent);
    CHECK_RETURN(m_RoleComponent);
    if (!m_RoleComponent->init(confItem->Common.StatusID))
    {
        CComponentCache::getInstance()->freeComponent(m_RoleComponent);
        KXLOGDEBUG("CHero roleComponent init fail typeId %d, statusID", typeId, confItem->Common.StatusID);
        return false;
    }
	addLogicComponent(m_RoleComponent);
	// 添加英雄的UI技能（PlayerSkill）, 需要先添加, 防止技能错位
	if (confItem->PlayerSkill.size() > 0)
	{
		if (NULL == m_SkillComponent)
        {            
            m_SkillComponent = createComponent<CSkillComponent>(ESkillComponent);
            CHECK_RETURN(m_SkillComponent);
            if (!m_SkillComponent->init(this, confItem->PlayerSkill))
            {
                CComponentCache::getInstance()->freeComponent(m_SkillComponent);
                KXLOGDEBUG("CHero skillComponent init fail typeId %d", typeId);
                return false;
            }
			addLogicComponent(m_SkillComponent);
		}
	}
	// 初始化其他逻辑组件
	CHECK_RETURN(initRoleComponents(&confItem->Common));
	// 初始化显示组件
	CHECK_RETURN(initDisplayComponents(&confItem->Common));

	CConfBase* conf = CConfManager::getInstance()->getConf(CONF_CRYSTAL);
	const CrystalConfItem* crystalItem = queryConfCrystal(1);
	CHECK_RETURN(NULL != crystalItem);
	// 初始等级
	int crystalLevel = 1;
	// 水晶当前数量	
	float crystalNum = 150.0f;
	// 最大水晶数
	int crystalMax = crystalItem->Max;
	// 恢复速度
	float crystalSpeed = crystalItem->Speed;
	
    if (m_pBattle->getBattleType() == EBATTLE_TOWERTEST)
    {
		// 爬塔试炼没有水晶等级的概念——需要写入水晶状态，水晶等级为满级
		// 爬塔试炼Ext1为初始化的水晶总量
        crystalLevel = conf->getDatas().size();
		crystalNum = static_cast<float>(m_pBattle->getRoom()->getExt1());
		crystalMax = 0;
		crystalSpeed = 0.0f;
    }
	else if (m_pBattle->getBattleType() == EBATTLE_GOLDTEST 
		|| m_pBattle->getBattleType() == EBATTLE_GUIDE)
	{
		// 金币试炼默认满级
		crystalLevel = conf->getDatas().size();
		const CrystalConfItem* crystalItem = queryConfCrystal(crystalLevel);
		CHECK_RETURN(NULL != crystalItem);
		
		crystalNum = crystalItem->Max * 1.0f;
		crystalMax = crystalItem->Max;
		crystalSpeed = crystalItem->Speed;
	}

	initAttribute(EHeroCrystalLevel, crystalLevel);
	initAttribute(EHeroCrystal, crystalNum);
	initAttribute(EHeroMaxCrystal, crystalMax);
	initAttribute(EHeroCrystalSpeed, crystalSpeed);
	// 额外信息处理
	// 是否有buff需要添加到自己身上
	if (m_pBattle->getBattleType() != EBATTLE_PVP)
	{
		const std::vector<BuffData> &buffs = m_pBattle->getRoom()->getStageBuffs();
		for (std::vector<BuffData>::const_iterator iter = buffs.begin(); 
			iter != buffs.end(); ++iter)
		{
			if (iter->Target == 0 || iter->Target == 1)
			{
				addBuff(this, iter->BuffId, iter->BuffLv);
			}
		}
	}
	
    return true;
}

bool CHero::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);

	//序列化基类数据
	CRole::serialize(data);

	//序列化唯一派发
	int length1 = m_SingleMap.size();
	data.writeData(length1);
	for (std::map<int, CRole*>::iterator iter = m_SingleMap.begin(); iter != m_SingleMap.end(); ++iter)
	{
		data.writeData(iter->second->getObjectId());
	}

	//序列化士兵卡片模型
	int length2 = m_SoldierCards->size();
	data.writeData(length2);
	for (std::vector<CSoldierModel*>::iterator iter = m_SoldierCards->begin(); iter != m_SoldierCards->end(); ++iter)
	{
		(*iter)->serialize(data);
	}

	SERIALIZE_MATCH(data);

	return true;
}

bool CHero::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);

	//反序列化基类数据
	CRole::unserialize(data);

	//反序列化唯一派发
	int soldierCounts;
	int objID;
	CRole* targetTemp = NULL;

	releaseAndClearMap(m_SingleMap);
	data.readData(soldierCounts);
	for (int i = 0; i < soldierCounts; ++i)
	{
		data.readData(objID);
		//根据objID获取目标实体
		targetTemp = m_pBattle->getRoleByObjectId(objID);
		targetTemp->retain();
		m_SingleMap[targetTemp->getTypeId()] = targetTemp;
	}
	
	//反序列化士兵卡片模型
	int solderCardCount;
	data.readData(solderCardCount);
	CHECK_RETURN(m_SoldierCards->size()==solderCardCount);
	for (int i = 0; i < solderCardCount; ++i)
	{
		(*m_SoldierCards)[i]->unserialize(data);
	}

	UN_SERIALIZE_MATCH(data);

#ifndef RunningInServer
    m_nWithholdValue = 0;
    m_bStartSkillExecute = false;
    m_fCurSkillExecutingTime = 0;
    m_fMaxSKillExecutingTime = 0;
#endif

	return true;
}

bool CHero::executeSkillIndex(int skillIndex, Vec2 pos)
{
    m_pBattle->pEventManager->raiseEvent(0xfff2, NULL);
    return CRole::executeSkillIndex(skillIndex, pos);
}

bool CHero::executeSkill(int skillId, Vec2 pos)
{
    m_pBattle->pEventManager->raiseEvent(0xfff2, NULL);
    return CRole::executeSkill(skillId, pos);
}

bool CHero::canUpgradeCrystal()
{
    // 获取当前级别的配置
    int cur = getIntAttribute(EHeroCrystalLevel);
    const CrystalConfItem* cf = queryConfCrystal(cur);
    CHECK_RETURN(cf);

    // 获取下一级别的配置（获取不到说明到顶级了）
    int level = getIntAttribute(EHeroCrystalLevel) + 1;
    const CrystalConfItem* conf = queryConfCrystal(level);
    CHECK_RETURN(conf);

    return canUseCrystal(cf->Price);
}

bool CHero::upgradeCrystal()
{
    // 获取当前级别的配置
    int cur = getIntAttribute(EHeroCrystalLevel);
    const CrystalConfItem* cf = queryConfCrystal(cur);
    CHECK_RETURN(cf);

    // 获取下一级别的配置（获取不到说明到顶级了）
    int level = getIntAttribute(EHeroCrystalLevel) + 1;
    const CrystalConfItem* conf = queryConfCrystal(level);
    CHECK_RETURN(conf);

    // 消耗升级所需的水晶
    CHECK_RETURN(useCrystal(cf->Price));

    // 升级水晶，调整水晶上限，水晶恢复速度
    setAttribute(EHeroCrystalLevel, level);
    setAttribute(EHeroMaxCrystal, conf->Max);
    setAttribute(EHeroCrystalSpeed, conf->Speed);
    return true;
}

bool CHero::canUseCrystal(int crystal)
{
#ifndef RunningInServer
    return (floatToInt(getFloatAttribute(EHeroCrystal) - m_nWithholdValue) >= crystal);
#else
    return (floatToInt(getFloatAttribute(EHeroCrystal)) >= crystal);
#endif
}

bool CHero::useCrystal(int crystal)
{
    // 判断额外水晶和普通水晶的总量是否满足
    int curCrystal = floatToInt(getFloatAttribute(EHeroCrystal));
    if (curCrystal < crystal)
    {
        return false;
    }
    curCrystal -= crystal;
    setAttribute(EHeroCrystal, curCrystal * 1.0f);
    // 爬塔试炼需要统计水晶消耗
    //if (m_pBattle->getBattleType() == EBATTLE_TOWERTEST)
    //{
        m_pBattle->getSettleAccountModel()->addCostCrystal(crystal);
    //}
    return true;
}

bool CHero::isCrystalMaxLevel()
{
    int level = getIntAttribute(EHeroCrystalLevel) + 1;
    const CrystalConfItem* conf = queryConfCrystal(level);
    return NULL == conf;
}

int CHero::getCrystalCost(int level)
{
    const CrystalConfItem* conf = queryConfCrystal(level);
    if (NULL == conf)
    {
        return 0;
    }
    else
    {
        return conf->Price;
    }
}

bool CHero::canUseSoldierCard(int soldierId)
{
    CSoldierModel* soldierCard = getSoldierCard(soldierId);
    CHECK_RETURN(soldierCard);

    // 判断锁
    CHECK_RETURN(!soldierCard->IsLock);
    // 判断唯一
    CHECK_RETURN(!soldierCard->IsSingo || !isSoldierSingle(soldierId))
    // 判断CD
    CHECK_RETURN(soldierCard->CurCD >= soldierCard->MaxCD);
    // 判断消耗
    CHECK_RETURN(canUseCrystal(soldierCard->CurCost));

    return true;
}

bool CHero::createSoldier(int soldierId, int line /*= -1*/)
{
    CHECK_RETURN(canUseSoldierCard(soldierId));

    CSoldierModel* soldierCard = getSoldierCard(soldierId);
    CHECK_RETURN(useCrystal(soldierCard->CurCost));
    soldierCard->CurCD = 0;
    CSoldier* soldier = m_pBattle->createSoldier(getOwnerId(), soldierId, line);
    CHECK_RETURN(soldier);

    if (soldierCard->IsSingo)
    {
        SAFE_RETAIN(soldier);
        m_SingleMap[soldierId] = soldier;
    }
    return true;
}

bool CHero::isSoldierCardLock(int id)
{
    CSoldierModel* card = getSoldierCard(id);
	CHECK_RETURN(card);
	return card->IsLock;
}

bool CHero::isSoldierCardBuff(int id)
{
	return false;
}

bool CHero::isSoldierCardDebuff(int id)
{
	return false;
}

bool CHero::isSoldierCardBinding(int id)
{
	return false;
}

float CHero::getSoldierCardCDPercent(int id)
{
    CSoldierModel* card = getSoldierCard(id);
	CHECK_RETURN(card);
	return card->CurCD / card->MaxCD;
}

void CHero::checkSingo()
{
    for (map<int, CRole*>::iterator iter = m_SingleMap.begin();
        iter != m_SingleMap.end();)
    {
        // 如果对象被移除或死亡，从容器中移除
        if (iter->second->getRoleComponent() == NULL
            || iter->second->getRoleComponent()->getStateId() == State_Death)
        {
            SAFE_RELEASE(iter->second);
            m_SingleMap.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}

bool CHero::isSoldierCardSingle(int id)
{
    CSoldierModel* card = getSoldierCard(id);
    CHECK_RETURN(card);
    return card->IsSingo;
}

bool CHero::isSoldierSingle(int soldierId)
{
    map<int, CRole*>::iterator iter = m_SingleMap.find(soldierId);
    if (iter == m_SingleMap.end()
        || iter->second->getRoleComponent() == NULL
        || iter->second->getRoleComponent()->getStateId() == State_Death)
    {
        return false;
    }

    return true;
}

#ifndef RunningInServer
// 设置水晶的预扣值
void CHero::setWithholdValue(int value)
{
    if (value > 0)
    {
        m_nWithholdValue += value;
    }
    else
    {
        m_nWithholdValue = 0;
    }
}

// 设置公共技能执行时间
void CHero::startSkillExecute(bool flag)
{
    m_bStartSkillExecute = flag;
}

void CHero::setCurSkillExecutingTime(float time)
{
    if (m_bStartSkillExecute)
    {
        if (m_fCurSkillExecutingTime > m_fMaxSKillExecutingTime)
        {
            m_bStartSkillExecute = false;
            m_fCurSkillExecutingTime = 0;
            m_fMaxSKillExecutingTime = 0;
        }
        else
        {
            m_fCurSkillExecutingTime += time;
        }
    }
}

void CHero::setMaxSkillExecutingTime(float time)
{
    if (0 == m_fMaxSKillExecutingTime)
    {
        m_fCurSkillExecutingTime = 0;
        m_fMaxSKillExecutingTime = 1.5f;
        if (time > m_fMaxSKillExecutingTime)
        {
            m_fMaxSKillExecutingTime = time;
        }
    }
}

float CHero::getCommonCDPercent()
{
    if (m_fMaxSKillExecutingTime > 0)
    {
        return m_fCurSkillExecutingTime / m_fMaxSKillExecutingTime;
    }

    return 0.0f;
}

#endif