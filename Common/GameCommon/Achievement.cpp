#include "Achievement.h"
#include "GameUserManager.h"
#include "TaskAchieveHelper.h"
#include "ServiceDef.h"

CAchievement::CAchievement() :m_nUserID(0)
, m_nAchieveID(0)
, m_pAchieveModel(NULL)
, m_pAchieveItemConf(NULL)
{
    memset(&m_AchieveInfo, 0, sizeof(AchieveDBInfo));
}

CAchievement::~CAchievement()
{
}

bool CAchievement::init(int uid, int achieveID, AchieveDBInfo &info)
{
    m_pAchieveItemConf = queryConfAchieve(achieveID);
    if (NULL == m_pAchieveItemConf)
    {
        return false;
    }

    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    if (NULL == pGameUser)
    {
        return false;
    }

	m_pAchieveModel = dynamic_cast<CAchievementModel*>(pGameUser->getModel(MODELTYPE_ACHIEVEMENT));
    if (NULL == m_pAchieveModel)
    {
        return false;
    }

    m_nUserID = uid;
    m_nAchieveID = achieveID;
    memcpy(&m_AchieveInfo, &info, sizeof(AchieveDBInfo));

    return true;
}

bool CAchievement::onAction(int actID, void *data, int len)
{
    switch (actID)
    {
    case ELA_USER_LEVEL_UP:
        return onUserLevelUpAction(actID, data, len);
    case ELA_ACCUMULATE_GOLD:
        return onAccumulateGoldAction(actID, data, len);
    case ELA_HERO_LEVEL_UP:
        return onUpgradeHeroLvAction(actID, data, len);
    case ELA_HERO_TEST:
        return onHeroTestAction(actID, data, len);
    case ELA_GOLD_TEST:
        return onGoldTestAction(actID, data, len);
    case ELA_BUY_ITEM_COUNT:
        return onShopBuyAction(actID, data, len);
    case ELA_USE_SAME_RACE:
        return onUseSameRaceAction(actID, data, len);
    case ELA_DRAW_CARD:
        return onDrawCardAction(actID, data, len);
    case ELA_BUY_GOLD_TIMES:
        return onBuyGoldAction(actID, data, len);
    default:
        break;
    }

    return false;
}

bool CAchievement::onUserLevelUpAction(int actID, void *data, int len)
{
    if (NULL == data || len != sizeof(int))
    {
        return false;
    }

    // 监听等级事件有两种情况,1、成就完成条件,2、未解锁的成就
    int *pUserLv = static_cast<int*>(data);
    if (m_pAchieveItemConf->UnLockLv <= *pUserLv)
    {
        if (EACHIEVE_STATUS_UNACTIVE == m_AchieveInfo.achieveStatus)
        {
            // 激活
            m_AchieveInfo.achieveVal = 0;
            m_AchieveInfo.achieveStatus = EACHIEVE_STATUS_ACTIVE;
            if (CTaskAchieveHelper::canFinishAchieve(m_nUserID, m_nAchieveID))
            {
                return true;
            }
            
            if (m_pAchieveModel->SetAchievement(m_nAchieveID, m_AchieveInfo))
            {
                CTaskAchieveHelper::instanceAchieve(m_nUserID, m_nAchieveID);
                return true;
            }
        }
        else
        {
            return canFinishAchieve(*pUserLv);
        }
    }

    return false;
}

bool CAchievement::onAccumulateGoldAction(int actID, void *data, int len)
{
    if (NULL == data || len != sizeof(int))
    {
        return false;
    }

    // 获得金币的数量
    int *pGoldCount = static_cast<int*>(data);

    return canFinishAchieve(*pGoldCount);
}

bool CAchievement::onUpgradeHeroLvAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));
    if (EPARAM_COUNT_ONE > m_pAchieveItemConf->FinishParameters.size())
    {
        return false;
    }

    int *pHeroLv = static_cast<int*>(data);
    if (0 == m_pAchieveItemConf->FinishParameters[0]
        || *pHeroLv >= m_pAchieveItemConf->FinishParameters[0])
    {
        return canFinishAchieve();
    }

    return false;
}

bool CAchievement::onHeroTestAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));
    if (EPARAM_COUNT_ONE > m_pAchieveItemConf->FinishParameters.size())
    {
        return false;
    }

    int *pDiff = static_cast<int*>(data);
    if (*pDiff >= m_pAchieveItemConf->FinishParameters[1])
    {
        return canFinishAchieve();
    }

    return false;
}

bool CAchievement::onGoldTestAction(int actID, void *data, int len)
{
    return canFinishAchieve();
}

bool CAchievement::onShopBuyAction(int actID, void *data, int len)
{
	CHECK_RETURN(NULL != data && len == sizeof(ActionCommonInfo));
	ActionCommonInfo *pCommInfo = static_cast<ActionCommonInfo*>(data);
    //int *pCount = static_cast<int*>(data);
	return canFinishAchieve(pCommInfo->param2);
}

bool CAchievement::onUseSameRaceAction(int actID, void *data, int len)
{
    return canFinishAchieve();
}

bool CAchievement::onDrawCardAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));
    // 抽卡次数
    int *pDrawCardCount = static_cast<int*>(data);
    return canFinishAchieve(*pDrawCardCount);
}

bool CAchievement::onBuyGoldAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));
    // 次数
    int *pBuyGoldTimes = static_cast<int*>(data);
    return canFinishAchieve(*pBuyGoldTimes);
}

bool CAchievement::canFinishAchieve(int val)
{
    m_AchieveInfo.achieveVal += val;
    if (m_AchieveInfo.achieveVal >= m_pAchieveItemConf->CompleteTimes)
    {
        m_AchieveInfo.achieveStatus = EACHIEVE_STATUS_FINISH;
        // 隐藏成就
        if (TASK_HIDE == m_pAchieveItemConf->Show)
        {
            // 开启新任务(没有奖励)
            std::vector<int>::const_iterator iterEndStart = m_pAchieveItemConf->EndStartID.begin();
            for (; iterEndStart != m_pAchieveItemConf->EndStartID.end(); iterEndStart++)
            {
                CTaskAchieveHelper::addAchieve(m_nUserID, *iterEndStart, true);
            }
            // 完成则直接关闭
            if (m_pAchieveModel->RemoveAchievementByID(m_nAchieveID))
            {
                return true;
            }
        }
    }

    if (!m_pAchieveModel->SetAchievement(m_nAchieveID, m_AchieveInfo))
    {
        // 处理
    }

    return EACHIEVE_STATUS_FINISH == m_AchieveInfo.achieveStatus;
}

bool CAchievement::finishHideAchieve()
{
    return false;
}
