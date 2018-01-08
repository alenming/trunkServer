#include "CallSoldier.h"
#include "ComponentCache.h"

CCallSoldier::CCallSoldier()
: m_pCallModel(NULL)
{
    m_RoleType = RT_SUMMON;
    m_nObjectType = EObjectCall;
}

CCallSoldier::~CCallSoldier()
{
    SAFE_DELETE(m_pCallModel);
}

bool CCallSoldier::init(int callid, int level, int ownerId, int objId, CBattleHelper *battle)
{
    int camp = static_cast<int>(ECamp_Neutral);
    CHECK_RETURN(CRole::init(callid, ownerId, objId, battle));
    //召唤物配置
    m_pCallConf = queryConfCall(callid);
    CHECK_RETURN(NULL != m_pCallConf);

    // 用户召唤
    CBattlePlayerModel* userModel = battle->getUserModel(ownerId);
	if (NULL != userModel)
	{
		camp = userModel->getCamp();
	}
    // BOSS召唤
    else if (EDefaultNpc == ownerId)
    {
        camp = ECamp_Red;
    }

    if (CAMP_ENEMY == m_pCallConf->RoleType)
    {
        camp = battle->getEnmeyCamp(camp);
    }

    // 移动方向是1 向右, 移动方向是2 向左, 否则只有是蓝方才向右
    if (m_pCallConf->RoleMoveDirection == 1)
    {
        m_nDirection = 1;
    }
    else if (m_pCallConf->RoleMoveDirection == 2)
    {
        m_nDirection = -1;
    }
    else
    {
        m_nDirection = camp == ECamp_Blue ? 1 : -1;
    }

	//召唤物的等级
    level = m_pCallConf->CardCurrentLevel == 0 ? level : m_pCallConf->CardCurrentLevel;

    CRoleModel* m_pCallModel = new CRoleModel();
    CHECK_RETURN(m_pCallModel->init(RT_SUMMON, level, &(m_pCallConf->Common), battle->getBattleType()));

    //初始化召唤物属性
    initRoleAttribute(m_pCallModel);
    // 卡片属性初始化
    initAttribute(EAttributeCamp, camp);

    // 添加BUFF组件
    m_BuffComponent = createComponent<CBuffComponent>(EBuffComponent);
    CHECK_RETURN(m_BuffComponent);
    if (!m_BuffComponent->init(this))
    {
        CComponentCache::getInstance()->freeComponent(m_BuffComponent);
        KXLOGDEBUG("CCallSoldier buffComponent init fail callid %d, callLv %d", callid, level);
        return false;
    }
    addLogicComponent(m_BuffComponent);

    //初始化召唤组件
    CCallComponent* com = createComponent<CCallComponent>(ECallComponent);
    CHECK_RETURN(com);
    if (!com->init(m_pModel->getRoleComm()->StatusID, m_pCallConf))
    {
        CComponentCache::getInstance()->freeComponent(com);
        KXLOGDEBUG("CCallSoldier callComponent init fail callid %d, callLv %d", callid, level);
        return false;
    }
 	m_RoleComponent = com;
	addLogicComponent(com);

    // 初始化其他逻辑组件
    CHECK_RETURN(initRoleComponents(m_pModel->getRoleComm()));
    // 初始化显示组件
    CHECK_RETURN(initDisplayComponents(m_pModel->getRoleComm()));
    return true;
}
