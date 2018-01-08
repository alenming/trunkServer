#include "Dispatch.h"
#include "ConfManager.h"
#include "Hero.h"

CDispatch::CDispatch() :m_nPlayerCamp(0)
, m_nEnemyCamp(0)
, m_pBattleHelper(NULL)
, m_ConfStageItem(NULL)
{
	m_listDispatchClose.clear();
	m_listDispatchOpen.clear();
	m_listDispatchCache.clear();
	m_listDispatchInfo.clear();
}


CDispatch::~CDispatch()
{
}

bool CDispatch::init(int stageID, int playerID, int enemyID, CBattleHelper *battleHelper)
{
    // 关卡配置
    const StageConfItem *pStageConf = queryConfStage(stageID);
	CHECK_RETURN(NULL != pStageConf);

	// 关卡发兵配置
    CConfDispatch *pDispatchConf = queryConfDispatch(stageID);
	CHECK_RETURN(NULL != pDispatchConf);

	m_ConfStageItem = pStageConf;
    m_nPlayerCamp = battleHelper->getCampWithUid(playerID);
    // enemyID->camp
    m_nEnemyCamp = enemyID;//battleHelper->getCampWithUid(enemyID);
	m_pBattleHelper = battleHelper;

    // 关卡数据
	const std::map<int, void*> &mapDispatchData = pDispatchConf->getDatas();
	const std::vector<int>& vecGroup = pDispatchConf->getGroup();

	// 随机添加的分组
	int nRandGroup = 0;
	if (!vecGroup.empty())
	{
		nRandGroup = vecGroup[m_pBattleHelper->RandNum.random() % vecGroup.size()];
	}

	// 将分组为0的加载
	std::map<int, void*>::const_iterator iter = mapDispatchData.begin();
	for (; iter != mapDispatchData.end(); ++iter)
	{
		CDispatchItem *pDispatchItem = static_cast<CDispatchItem *>(iter->second);
		if (NULL != pDispatchItem)
		{
            // 分组为0和随机的分组全部加载
            if (0 == pDispatchItem->Group || nRandGroup == pDispatchItem->Group)
            {
                if (pDispatchItem->IsOpen)	// 默认开启
                {
                    m_listDispatchOpen.push_back(pDispatchItem);
                    continue;
                }
                // 需要加载但是关闭状态
                m_listDispatchClose.push_back(pDispatchItem);
            }
		}
	}

	return true;
}

void CDispatch::logicUpdate(float dt)
{
    if (!m_listDispatchInfo.empty())
    {
        std::list<int> lsCloseCondition;

        // 并行条件发兵
        std::list<SDispatchInfo>::iterator iterDispatchInfo = m_listDispatchInfo.begin();
        for (; iterDispatchInfo != m_listDispatchInfo.end();)
        {
            iterDispatchInfo->Delay -= dt;
            if (iterDispatchInfo->Delay <= 0)
            {
                // 延迟时间到,发兵
                execute(*iterDispatchInfo);
                // 加载新开启的条件
                VecInt::iterator iterConditionOpen = iterDispatchInfo->ConditionOpenID.begin();
                for (; iterConditionOpen != iterDispatchInfo->ConditionOpenID.end(); ++iterConditionOpen)
                {
                    // 由关闭列表转为开启列表
                    processCloseList(*iterConditionOpen);
                }

                // 关闭开启的条件
                VecInt::iterator iterConditionClose = iterDispatchInfo->ConditionCloseID.begin();
                for (; iterConditionClose != iterDispatchInfo->ConditionCloseID.end(); ++iterConditionClose)
                {
                    // 由开启或缓存列表转为关闭列表
                    processOpenList(*iterConditionClose);
                    lsCloseCondition.push_back(*iterConditionClose);
                }

                iterDispatchInfo = m_listDispatchInfo.erase(iterDispatchInfo);
            }
            else
            {
                ++iterDispatchInfo;
            }
        }

        // 将发兵列表中有关闭掉的开启条件去除
        if (!lsCloseCondition.empty())
        {
            std::list<int>::iterator iterClose = lsCloseCondition.begin();
            for (; iterClose != lsCloseCondition.end(); ++iterClose)
            {
                processDispatchList(*iterClose);
            }
        }
    }

    // 将缓存列表的数据插入开启列表的末尾
    if (!m_listDispatchCache.empty())
    {
        m_listDispatchOpen.splice(m_listDispatchOpen.end(), m_listDispatchCache);
        // 清掉缓存列表
        //m_listDispatchCache.clear();
    }
	
	// 遍历列表判断是否有满足条件
	std::list<CDispatchItem*>::iterator iter = m_listDispatchOpen.begin();
	for (; iter != m_listDispatchOpen.end();)
	{
		CDispatchItem *pDispatchItem = *iter;
		CHECK_RETURN_VOID(NULL != pDispatchItem);

		// 判断条件
		if (checkCondition(pDispatchItem->ConditionType, pDispatchItem->ConditionParam))
		{
			// 只需要延迟时间的列表
			m_listDispatchInfo.push_back(pDispatchItem->DispatchInfo);
			// 进入关闭列表
			m_listDispatchClose.push_back(pDispatchItem);
			// 从开启列表去除
			iter = m_listDispatchOpen.erase(iter);
 		}
 		else
 		{
 			++iter;
 		}
	}
}

bool CDispatch::checkCondition(const int& conditionType, const int& conditionVal)
{
	bool bRlt = false;
	switch (conditionType)
	{
	case DC_Non:
		bRlt = true;
		break;
	case DC_PCHeroDistance:
		bRlt = checkPCHeroDistance(conditionVal);
		break;
	case DC_PCHeroHP:
		bRlt = checkPCHeroHP(conditionVal);
		break;
	case DC_CrystalLv:
		bRlt = checkCrystalLv(conditionVal);
		break;
	case DC_SoldierCount:
		bRlt = checkSoldierCount(conditionVal);
		break;
	case DC_SoldierMaxStar:
		bRlt = checkSoldierMaxStar(conditionVal);
		break;
	case DC_DispatchSoldierCount:
		bRlt = checkDispatchSoldierCount(conditionVal);
		break;
	case DC_DeadSoldierCount:
		bRlt = checkDeadSoldierCount(conditionVal);
		break;
	case DC_PCDeadSoldierCount:
		bRlt = checkPCDeadSoldierCount(conditionVal);
		break;
	case DC_DispatchSoldierType:
		bRlt = checkDispatchSoldierType(conditionVal);
		break;
	case DC_DispatchSoldierID:
		bRlt = checkDispatchSoldierID(conditionVal);
		break;
	default:
		break;
	}

	return bRlt;
}

void CDispatch::execute(const SDispatchInfo &info)
{
	// 第几个士兵,配置表从1开始,程序读取从0开始
	int nSoldierOrder = info.DispatchSoldierID - 1;

    if (nSoldierOrder < 0 || (int)m_ConfStageItem->Monsters.size() < info.DispatchSoldierID)
	{
		// 配置表有误
		return;
	}

    // 配置表信息
    m_pBattleHelper->createMonster(nSoldierOrder, info.DispatchLine);
}

bool CDispatch::checkPCHeroDistance(const int& conditionVal)
{
	CRole *pPCHero = m_pBattleHelper->getMainRole(m_nEnemyCamp);
	CHECK_RETURN(NULL != pPCHero);

	// 电脑英雄的位置
	Vec2 vPCHeroPos = pPCHero->getRealPosition();
	
	// 玩家士兵列表
	std::vector<CRole*> &vecPlayerSoldier = m_pBattleHelper->getRoleWithCamp(m_nPlayerCamp);
	std::vector<CRole*>::iterator iter = vecPlayerSoldier.begin();
	for (; iter != vecPlayerSoldier.end(); ++iter)
	{
		CRole *pRole = *iter;
		if (NULL == pRole)
		{
			continue;
		}

		// 英雄电脑指定范围内是否有玩家的士兵
		if (pRole->getRealPosition().distance(vPCHeroPos) <= conditionVal)
		{
			return true;
		}
	}

	return false;
}

bool CDispatch::checkPCHeroHP(const int& conditionVal)
{
	CRole *pPCHero = m_pBattleHelper->getMainRole(m_nEnemyCamp);
	CHECK_RETURN(NULL != pPCHero);

	float fHpPercent = pPCHero->getIntAttribute(EAttributeHP) * 1.0f / pPCHero->getIntAttribute(EAttributeMaxHP);
	if (fHpPercent <= conditionVal / 100.0f)
	{
		return true;
	}

	return false;
}

bool CDispatch::checkCrystalLv(const int& conditionVal)
{
	CRole *pPlayerHero = m_pBattleHelper->getMainRole(m_nPlayerCamp);
	CHECK_RETURN(NULL != pPlayerHero);

	if (pPlayerHero->getIntAttribute(EHeroCrystalLevel) >= conditionVal)
	{
		return true;
	}

	return false;
}

bool CDispatch::checkSoldierCount(const int& conditionVal)
{
	// 玩家士兵列表
	std::vector<CRole*> &vecPlayerSoldier = m_pBattleHelper->getRoleWithCamp(m_nPlayerCamp);
	// 在场数量
	if ((int)vecPlayerSoldier.size() >= conditionVal)
	{
		return true;
	}

	return false;
}

bool CDispatch::checkSoldierMaxStar(const int& conditionVal)
{
	CConfSoldier *pConfSoldier
		= static_cast<CConfSoldier*>(CConfManager::getInstance()->getConf(CONF_SOLDIER));
	CHECK_RETURN(NULL != pConfSoldier);

	// 玩家士兵列表
	std::vector<CRole*> &vecPlayerSoldier = m_pBattleHelper->getRoleWithCamp(m_nPlayerCamp);
	std::vector<CRole*>::iterator iter = vecPlayerSoldier.begin();
	for (; iter != vecPlayerSoldier.end(); ++iter)
	{
        int nConfigID = 0;
        int nStar = 0;
        if (!getSoldierInfo(*iter, nConfigID, nStar))
        {
            continue;
        }

		// 士兵卡片属性
		SoldierConfItem *pSoldierItem 
            = static_cast<SoldierConfItem *>(pConfSoldier->getData(nConfigID, nStar));
		// 士兵星级
		if (pSoldierItem && pSoldierItem->Star >= conditionVal)
		{
			return true;
		}
	}

	return false;
}

bool CDispatch::checkDispatchSoldierCount(const int& conditionVal)
{
	if (m_pBattleHelper->isEnoughDispatchCount(conditionVal))
	{
		return true;
	}

	return false;
}

bool CDispatch::checkDeadSoldierCount(const int& conditionVal)
{
	if (m_pBattleHelper->isEnoughDeadSoldierCount(m_nPlayerCamp, conditionVal))
	{
		return true;
	}

	return false;
}

bool CDispatch::checkPCDeadSoldierCount(const int& conditionVal)
{
	if (m_pBattleHelper->isEnoughDeadSoldierCount(m_nEnemyCamp, conditionVal))
	{
		return true;
	}

	return false;
}

bool CDispatch::checkDispatchSoldierType(const int& conditionVal)
{
	// 玩家士兵列表
	std::vector<CRole*> &vecPlayerSoldier = m_pBattleHelper->getRoleWithCamp(m_nPlayerCamp);
	std::vector<CRole*>::iterator iter = vecPlayerSoldier.begin();
	for (; iter != vecPlayerSoldier.end(); ++iter)
	{
        int nConfigID = 0;
        int nStar = 0;
        if (!getSoldierInfo(*iter, nConfigID, nStar))
        {
            continue;
        }

        if (isSoldierType(nConfigID, nStar, conditionVal))
		{
            return true;
		}
	}

	return false;
}

bool CDispatch::checkDispatchSoldierID(const int& conditionVal)
{
	// 玩家士兵列表
	std::vector<CRole*> &vecPlayerSoldier = m_pBattleHelper->getRoleWithCamp(m_nPlayerCamp);
	std::vector<CRole*>::iterator iter = vecPlayerSoldier.begin();
	for (; iter != vecPlayerSoldier.end(); ++iter)
	{
        int nConfigID = 0;
        int nStar = 0;
        if (!getSoldierInfo(*iter, nConfigID, nStar))
        {
            continue;
        }

		// 士兵ID
        if (nConfigID == conditionVal)
		{
			return true;
		}
	}

	return false;
}

void CDispatch::processCloseList(const int &conditionID)
{
	// 说明:如果该条件不在关闭列表中,两种可能:
	// 1、不在加载范围中  2、在开启列表中(无效)
	std::list<CDispatchItem*>::iterator iter = m_listDispatchClose.begin();
	for (; iter != m_listDispatchClose.end(); ++iter)
	{
		CDispatchItem *pDispatch = *iter;
		if (NULL == pDispatch)
		{
			continue;
		}

		if (pDispatch->ID == conditionID)
		{
			// 缓存列表中,会加入开启列表
			m_listDispatchCache.push_back(pDispatch);
			// 从关闭列表中去掉
			m_listDispatchClose.erase(iter);

			break;
		}
	}
}

void CDispatch::processOpenList(const int &conditionID)
{
	std::list<CDispatchItem*>::iterator iter = m_listDispatchOpen.begin();
	for (; iter != m_listDispatchOpen.end(); ++iter)
	{
		CDispatchItem *pDispatch = *iter;
		if (NULL == pDispatch)
		{
			continue;
		}

		if (pDispatch->ID == conditionID)
		{
			// 加入关闭列表
			m_listDispatchClose.push_back(pDispatch);
			// 从开启列表去掉
			m_listDispatchOpen.erase(iter);

			// 条件只存在其中一张列表
			return;
		}
	}

	std::list<CDispatchItem*>::iterator iterCache = m_listDispatchCache.begin();
	for (; iterCache != m_listDispatchCache.end(); ++iterCache)
	{
		CDispatchItem *pDispatch = *iterCache;
		if (NULL == pDispatch)
		{
			continue;
		}

		if (pDispatch->ID == conditionID)
		{
			// 加入关闭列表
			m_listDispatchClose.push_back(pDispatch);
			// 从开启列表去掉
			m_listDispatchCache.erase(iterCache);

			// 条件只存在其中一张列表
			return;
		}
	}
}

void CDispatch::processDispatchList(const int &conditionID)
{
	std::list<SDispatchInfo>::iterator iterDispatch = m_listDispatchInfo.begin();
	for (; iterDispatch != m_listDispatchInfo.end(); ++iterDispatch)
	{
		VecInt::iterator iterOpen = iterDispatch->ConditionOpenID.begin();
		for (; iterOpen != iterDispatch->ConditionOpenID.end(); )
		{
			if (*iterOpen == conditionID)
			{
				iterOpen = iterDispatch->ConditionOpenID.erase(iterOpen);
			}
			else
			{
				++iterOpen;
			}
		}
	}
}

bool CDispatch::getSoldierInfo(CRole *role, int &confID, int &star)
{
    if (NULL == role)
    {
        return false;
    }

    int nOwerID = role->getOwnerId();
    CPlayerModel *pPlayerModel = dynamic_cast<CPlayerModel *>(m_pBattleHelper->getUserModel(nOwerID));
    if (NULL == pPlayerModel)
    {
        return false;
    }

    CSoldierModel* pSoldierModel = pPlayerModel->getSoldierCard(role->getTypeId());
    if (NULL == pSoldierModel)
    {
        return false;
    }

    confID = pSoldierModel->getSoldId();
    star = pSoldierModel->getStar();

    return true;
}

bool CDispatch::isSoldierType(int confid, int star, int type)
{
    const SoldierConfItem* pSoldierItem = queryConfSoldier(confid, star);
    // 士兵卡片属性
    CHECK_RETURN(NULL != pSoldierItem);

    bool b = false;
    if (type >= EST_VOCATION_BEGIN) // 职业判断
    {
        ERaceType raceType = RACE_TYPE_NON;
        if (EST_RACE_HUMAN == type)
        {
            raceType = RACE_TYPE_HUMAN;
        }
        else if (EST_RACE_GHOST == type)
        {
            raceType = RACE_TYPE_GHOST;
        }
        else
        {
            raceType = RACE_TYPE_OTHER;
        }

        if (raceType == pSoldierItem->Common.Vocation)
        {
            b = true;
        }
    }
    else if (type >= EST_FIRE_BEGIN) // 攻击方式
    {
        EFireType fireType = FIRE_TYPE_NON;
        if (EST_FIRE_NEAR == type)
        {
            fireType = FIRE_TYPE_NEAR;
        }
        else
        {
            fireType = FIRE_TYPE_FAR;
        }

        if (fireType == pSoldierItem->Common.AttackType)
        {
            b = true;
        }
    }
    else if (type >= EST_SEX_BEGIN) // 性别判断
    {
        ESexType sexType = SEX_TYPE_NON;
        if (EST_SEX_MALE == type)
        {
            sexType = SEX_TYPE_MALE;
        }
        else if (EST_SEX_SHEMALE == type)
        {
            sexType = SEX_TYPE_SHEMALE;
        }
        else
        {
            sexType = SEX_TYPE_NEUTER;
        }

        if (sexType == pSoldierItem->Common.Sex)
        {
            b = true;
        }
    }
    else if (type >= EST_RACE_BEGIN) // 种族判断
    {
        EVocationType vocationType = VOCATION_TYPE_NON;
        if (EST_VOCATION_SOLDIER == type)
        {
            vocationType = VOCATION_TYPE_SOLDIER;
        }
        else if (EST_VOCATION_ASSASSIN == type)
        {
            vocationType = VOCATION_TYPE_ASSASSIN;
        }
        else if (EST_VOCATION_SHOOTER == type)
        {
            vocationType = VOCATION_TYPE_SHOOTER;
        }
        else if (EST_VOCATION_MAGE == type)
        {
            vocationType = VOCATION_TYPE_MAGE;
        }
        else if (EST_VOCATION_ASSISTANT == type)
        {
            vocationType = VOCATION_TYPE_ASSISTANT;
        }
        else if (EST_VOCATION_TANK == type)
        {
            vocationType = VOCATION_TYPE_BODYGUARD;
        }

        if (vocationType == pSoldierItem->Common.Vocation)
        {
            b = true;
        }
    }

    return b;
}
