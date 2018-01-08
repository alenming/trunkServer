#include "ComponentCache.h"

#include "KxCSComm.h"
#include "AIComponent.h"
#include "BuffComponent.h"
#include "BulletComponent.h"
#include "CallComponent.h"
#include "HeroComponent.h"
#include "LogicComponent.h"
#include "RoleComponent.h"
#include "SkillComponent.h"

CComponentCache* CComponentCache::m_pInstance = NULL;

CComponentCache::CComponentCache()
{

}

CComponentCache* CComponentCache::getInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new (std::nothrow) CComponentCache();
    }
    return m_pInstance;
}

void CComponentCache::destory()
{
    if (m_pInstance)
    {
        std::map<EComponentType, std::list<CLogicComponent*> >::iterator iter = m_pInstance->m_ComponentCache.begin();
        for (; iter != m_pInstance->m_ComponentCache.end(); ++iter)
        {
            std::list<CLogicComponent*>::iterator listIter = iter->second.begin();
            for (; listIter != iter->second.end(); ++listIter)
            {
                SAFE_RELEASE((*listIter));
            }
        }
        m_pInstance->m_ComponentCache.clear();
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

CLogicComponent* CComponentCache::getComponent(EComponentType type)
{
    CLogicComponent* pComponent = NULL;

    if (m_ComponentCache.find(type) != m_ComponentCache.end()
        && !m_ComponentCache[type].empty())
    {
        pComponent = *m_ComponentCache[type].begin();
        m_ComponentCache[type].erase(m_ComponentCache[type].begin());
        return pComponent;
    }

    switch (type)
    {
    case ELogicComponent:
        pComponent = new CLogicComponent();
        break;
    case EAIComponent:
        pComponent = new CAIComponent();
        break;
    case EBuffComponent:
        pComponent = new CBuffComponent();
        break;
    case EBulletComponent:
        pComponent = new CBulletComponent();
        break;
    case ERoleComponent:
        pComponent = new CRoleComponent();
        break;
    case ESkillComponent:
        pComponent = new CSkillComponent();
        break;
    case ECallComponent:
        pComponent = new CCallComponent();
        break;
    case EHeroComponent:
        pComponent = new CHeroComponent();
        break;
    default:
        break;
    }
    return pComponent;
}

void CComponentCache::freeComponent(CLogicComponent* pComponent)
{
    if (NULL == pComponent)
    {
        return;
    }
    
    EComponentType type = pComponent->getComponentType();
    m_ComponentCache[type].push_back(pComponent);
}