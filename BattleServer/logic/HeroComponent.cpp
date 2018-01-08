#include "HeroComponent.h"

using namespace std;

CHeroComponent::CHeroComponent()
{
    m_eComponentType = EHeroComponent;
}


CHeroComponent::~CHeroComponent()
{
}

void CHeroComponent::onEnter()
{
    CRoleComponent::onEnter();

    m_pHero = dynamic_cast<CHero*>(getOwner());
    m_pSodlierCards = m_pHero->getSoldierCards();
}

void CHeroComponent::onExit()
{
    CRoleComponent::onExit();
    m_pHero = NULL;
}

void CHeroComponent::logicUpdate(float delta)
{
    CRoleComponent::logicUpdate(delta);

    // ø®∆¨CDº∆À„
    for (vector<CSoldierModel*>::iterator iter = m_pSodlierCards->begin();
        iter != m_pSodlierCards->end(); ++iter)
    {
        CSoldierModel* card = *iter;
        // ø®∆¨Œ¥±ªÀ¯◊°
        if (!card->IsLock && card->MaxCD > card->CurCD)
        {
            card->CurCD += delta;
            if (card->CurCD > card->MaxCD)
            {
                card->CurCD = card->MaxCD;
            }
        }
    }

    // ∏¸–¬Œ®“ª
    m_pHero->checkSingo();

    // ≈¿À˛ ‘¡∂≤ªº∆À„ÀÆæßª÷∏¥
    if (m_pHero->getBattleHelper()->getBattleType() != EBATTLE_TOWERTEST)
    {
        // ÀÆæßª÷∏¥º∆À„
        float curCrystal = m_pHero->getFloatAttribute(EHeroCrystal);
        int maxCrystal = m_pHero->getIntAttribute(EHeroMaxCrystal);
        if (curCrystal < maxCrystal)
        {
            float curSpeedParam = m_pHero->getFloatAttribute(EHeroCrystalSpeedParam);
            float curSpeed = m_pHero->getFloatAttribute(EHeroCrystalSpeed);
            curCrystal += curSpeed * curSpeedParam * delta;
            if (curCrystal > maxCrystal)
            {
                curCrystal = static_cast<float>(maxCrystal);
            }
            m_pHero->setAttribute(EHeroCrystal, curCrystal);
			//LOG("crystal recover curCrystal : %f, curSpeedParam: %f, curSpeed: %f ", curCrystal, curSpeedParam, curSpeed);
        }
    }

#ifndef RunningInServer
    m_pHero->setCurSkillExecutingTime(delta);
#endif
}
