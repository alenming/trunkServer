#include "Monster.h"
#include "ComponentCache.h"

CMonster::CMonster()
{
	m_RoleType = RT_MONSTER;
	m_nObjectType = EObjectMonster;
}

CMonster::~CMonster()
{
}

bool CMonster::init(int monsterid, int ownerId, int objId, CBattleHelper *battle)
{
	CHECK_RETURN(CRole::init(monsterid, ownerId, objId, battle));
    CComputerModel* computer = battle->getComputerModel();
    CHECK_RETURN(computer);
    CRoleModel* monsterModel = computer->getMonsterModel(monsterid);
    CHECK_RETURN(monsterModel);

    //初始化通用属性
    initRoleAttribute(monsterModel);
	// 卡片属性初始化
    initAttribute(EAttributeCamp, ECamp_Red);

    // 添加BUFF组件
    m_BuffComponent = createComponent<CBuffComponent>(EBuffComponent);
    CHECK_RETURN(m_BuffComponent);
    if (!m_BuffComponent->init(this))
    {
        CComponentCache::getInstance()->freeComponent(m_BuffComponent);
        KXLOGDEBUG("CMonster buffComponent init fail monsterid %d", monsterid);
        return false;
    }
    addLogicComponent(m_BuffComponent);

	//初始化角色组件
    m_RoleComponent = createComponent<CRoleComponent>(ERoleComponent);
    CHECK_RETURN(m_RoleComponent);
    if (!m_RoleComponent->init(m_pModel->getRoleComm()->StatusID))
    {
        CComponentCache::getInstance()->freeComponent(m_RoleComponent);
        KXLOGDEBUG("CMonster roleComponent init fail monsterid %d, statusID %d", 
            monsterid, m_pModel->getRoleComm()->StatusID);
        return false;
    }
    addLogicComponent(m_RoleComponent);
	// 初始化其他逻辑组件
	CHECK_RETURN(initRoleComponents(m_pModel->getRoleComm()));
	// 初始化显示组件
    CHECK_RETURN(initDisplayComponents(m_pModel->getRoleComm()));
	return true;
}
