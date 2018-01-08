#include "Boss.h"
#include "ComponentCache.h"

CBoss::CBoss()
{
	m_RoleType = RT_BOSS;
	m_nObjectType = EObjectBoss;
}

CBoss::~CBoss()
{
}

bool CBoss::init(int bossid, int ownerId, int objId, CBattleHelper *battle)
{
	CHECK_RETURN(CRole::init(bossid, ownerId, objId, battle));

    CComputerModel* computer = battle->getComputerModel();
    CHECK_RETURN(computer);
    CRoleModel* bossModel = computer->getRoleModel();
    CHECK_RETURN(bossModel);

    //初始化通用属性
    initRoleAttribute(bossModel);
    // 卡片属性初始化
	initAttribute(EAttributeCamp, ECamp_Red);

    // 添加BUFF组件
    m_BuffComponent = createComponent<CBuffComponent>(EBuffComponent);
    CHECK_RETURN(m_BuffComponent);
    if (!m_BuffComponent->init(this))
    {
        CComponentCache::getInstance()->freeComponent(m_BuffComponent);
        KXLOGDEBUG("CBoss buffComponent init fail bossid %d", bossid);
        return false;
    }
    addLogicComponent(m_BuffComponent);

	// 初始化角色组件
    m_RoleComponent = createComponent<CRoleComponent>(ERoleComponent);
    CHECK_RETURN(m_RoleComponent);
    if (!m_RoleComponent->init(m_pModel->getRoleComm()->StatusID))
    {
        CComponentCache::getInstance()->freeComponent(m_RoleComponent);
        KXLOGDEBUG("CBoss roleComponent init fail bossid %d", bossid);
        return false;
    }
	addLogicComponent(m_RoleComponent);
	// 初始化其他逻辑组件
    CHECK_RETURN(initRoleComponents(m_pModel->getRoleComm()));
	// 初始化显示组件
    CHECK_RETURN(initDisplayComponents(m_pModel->getRoleComm()));

	// 额外信息处理
	// 是否有buff需要添加到自己身上
	if (m_pBattle->getBattleType() != EBATTLE_PVP)
	{
		const std::vector<BuffData> &buffs = m_pBattle->getRoom()->getStageBuffs();
		for (std::vector<BuffData>::const_iterator iter = buffs.begin();
			iter != buffs.end(); ++iter)
		{
			if (iter->Target == 0 || iter->Target == 2)
			{
				addBuff(this, iter->BuffId, iter->BuffLv);
			}
		}
	}

	return true;
}

void CBoss::modifyAttribute(EAttributeTypes key, int v)
{
    // 金币试炼需要统计BOSS掉血的数量
    if (key == EAttributeHP
		&& (m_pBattle->getBattleType() == EBATTLE_GOLDTEST || m_pBattle->getBattleType() == EBATTLE_UNIONEXPEDITION))
    {
        // 只统计扣血
        int HitHp = m_IntAttributes[key] - v;
        if (HitHp > 0)
        {
            // 显示层会根据SettleAccountModel的值刷新显示
            m_pBattle->getSettleAccountModel()->addHitBossHP(HitHp);
        }
    }
    m_IntAttributes[key] = limitAttribute(key, v);

}