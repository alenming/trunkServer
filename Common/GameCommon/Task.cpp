#include "Task.h"
#include "GameUserManager.h"
#include "TaskAchieveHelper.h"
#include "CommonHelper.h"
#include "ServiceDef.h"

CTask::CTask() :m_nUserID(0)
, m_nTaskID(0)
, m_pTaskModel(NULL)
, m_pTaskItemConf(NULL)
{

}

CTask::~CTask()
{

}

bool CTask::init(int userID, int taskID, const TaskDBInfo& info)
{
    m_pTaskItemConf = queryConfTask(taskID);
    if (NULL == m_pTaskItemConf)
    {
        return false;
    }

    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(userID);
    if (NULL == pGameUser)
    {
        return false;
    }

    m_pTaskModel = dynamic_cast<CTaskModel*>(pGameUser->getModel(MODELTYPE_TASK));
    m_nUserID = userID;
    m_nTaskID = taskID;
    m_TaskInfo = info;

    return true;
}

bool CTask::onAction(int actID, void *data, int len)
{
    // 任务(非重置)可以领取或者已经完成清除
    if (ERTT_NON == m_pTaskItemConf->TaskReset)
    {
        if (ETASK_GET == m_TaskInfo.taskStatus
          || ETASK_FINISH == m_TaskInfo.taskStatus)
            return true;
    }
    else
    {
        // 监听任务可能过了重置时间,重置任务数据
        CTaskAchieveHelper::checkTask(m_nUserID, m_nTaskID, m_TaskInfo);
        if (ETASK_GET == m_TaskInfo.taskStatus
            || ETASK_FINISH == m_TaskInfo.taskStatus)
            return false;
    }

    bool bFinish = false;
    switch (actID)
    {
    case ELA_USER_LEVEL_UP:
        return onUserLevelUpAction(actID, data, len);
    case ELA_PASS_STAGE:
        bFinish = onPassStageAction(actID, data, len);
        break;
    case ELA_HERO_EQUIP:
        bFinish = onHeroEquipAction(actID, data, len);
        break;
    case ELA_HERO_LEVEL_UP:
        bFinish = onHeroLevelUpAction(actID, data, len);
        break;
    case ELA_HERO_UPGRADE_STAR:
        bFinish = onHeroStarAction(actID, data, len);
        break;
    case ELA_HERO_UPGRADE_SKILL:
        bFinish = onHeroSkillAction(actID, data, len);
        break;
    case ELA_DRAW_CARD:
        bFinish = onDrawCardAction(actID, data, len);
        break;
    case ELA_USE_EXP_BOOK:
        bFinish = onUseExpBookAction(actID, data, len);
        break;
    case ELA_BUY_GOLD_TIMES:
        bFinish = onBuyGoldAction(actID, data, len);
        break;
    case ELA_HERO_TEST:
        bFinish = onHeroTestAction(actID, data, len);
        break;
    case ELA_GOLD_TEST:
        bFinish = onGoldTestAction(actID, data, len);
        break;
    case ELA_TOWER_TEST_FLOOR:
        bFinish = onTowerFloorAction(actID, data, len);
        break;
    case ELA_PVP:
        bFinish = onPvpAction(actID, data, len);
        break;
    case ELA_EQUIPBUILD:
        bFinish = onEquipAction(actID, data, len);
        break;
    }

    if (bFinish && ERTT_NON != m_pTaskItemConf->TaskReset)
    {
        return false;
    }

    return bFinish;
}

bool CTask::onUserLevelUpAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));

    int *pUserLv = static_cast<int*>(data);
    if (m_pTaskItemConf->UnlockLv <= *pUserLv)
    {
        if (ERTT_NON != m_pTaskItemConf->TaskReset)
        {
            m_TaskInfo.resetTime = CTaskAchieveHelper::getTaskResetTimeStamp(m_nTaskID);
        }

        m_TaskInfo.taskVal = 0;
        m_TaskInfo.taskStatus = ETASK_ACTIVE;
        if (m_pTaskModel->SetTask(m_nTaskID, m_TaskInfo))
        {
            // 当成一个新的任务
            CTaskAchieveHelper::instanceTask(m_nUserID, m_nTaskID);

            return true;
        }
    }

    return false;
}

bool CTask::onPassStageAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(ActionCommonInfo));

    if (EPARAM_COUNT_ONE > m_pTaskItemConf->FinishParameters.size())
    {
        return false;
    }

    ActionCommonInfo *pTaskPassStage = static_cast<ActionCommonInfo*>(data);
    if (m_pTaskItemConf->FinishParameters[0] > 0
        && pTaskPassStage->param1 == m_pTaskItemConf->FinishParameters[0]) // 通关指定关卡
    {
        return canFinish(pTaskPassStage->param2);
    }
    else if (0 == m_pTaskItemConf->FinishParameters[0] // 通关任意关卡
        || m_pTaskItemConf->FinishParameters[0]
            == CCommonHelper::getStageTypeToTask(pTaskPassStage->param1)) // 普通精英
    {
        return canFinish(pTaskPassStage->param2);
    }

    return false;
}

bool CTask::onHeroEquipAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));

    if (EPARAM_COUNT_ONE > m_pTaskItemConf->FinishParameters.size())
    {
        return false;
    }

    bool bRlt = false;
    if (0 == m_pTaskItemConf->FinishParameters[0])
    {
        bRlt = true;
    }
    else
    {
        int *pEquipPart = static_cast<int*>(data);
        if (*pEquipPart == m_pTaskItemConf->FinishParameters[0])
        {
            bRlt = true;
        }
    }

    if (bRlt)
    {
        return canFinish();
    }

    return false;
}

bool CTask::onHeroLevelUpAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));
    if (EPARAM_COUNT_ONE > m_pTaskItemConf->FinishParameters.size())
    {
        return false;
    }

    int *pHeroLv = static_cast<int*>(data);
    if (0 == m_pTaskItemConf->FinishParameters[0]
        || *pHeroLv >= m_pTaskItemConf->FinishParameters[0])
    {
        return canFinish();
    }

    return false;
}

bool CTask::onHeroStarAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));

    if (EPARAM_COUNT_ONE > m_pTaskItemConf->FinishParameters.size())
    {
        return false;
    }

    bool bRlt = false;
    if (0 == m_pTaskItemConf->FinishParameters[0])
    {
        bRlt = true;
    }
    else
    {
        int *pHeroStar = static_cast<int*>(data);
        if (*pHeroStar == m_pTaskItemConf->FinishParameters[0])
        {
            bRlt = true;
        }
    }

    if (bRlt)
    {
        return canFinish();
    }

    return false;
}

bool CTask::onHeroSkillAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));

    if (EPARAM_COUNT_ONE > m_pTaskItemConf->FinishParameters.size())
    {
        return false;
    }

    bool bRlt = false;
    if (0 == m_pTaskItemConf->FinishParameters[0])
    {
        bRlt = true;
    }
    else
    {
        int *pHeroSkillLv = static_cast<int*>(data);
        if (*pHeroSkillLv == m_pTaskItemConf->FinishParameters[0])
        {
            bRlt = true;
        }
    }

    if (bRlt)
    {
        return canFinish();
    }

    return false;
}

bool CTask::onDrawCardAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));
    // 抽卡次数
    int *pDrawCardCount = static_cast<int*>(data);
    return canFinish(*pDrawCardCount);
}

bool CTask::onUseExpBookAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));
    // 道具ID
    int *pItemID = static_cast<int*>(data);
    const PropItem* pPropItem = queryConfProp(*pItemID);
    if (pPropItem)
    {
        if (PROPTYPE_EXPBOOK == pPropItem->Type)
        {
            return canFinish();
        }
    }

    return false;
}

bool CTask::onBuyGoldAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));
    // 次数
    int *pBuyGoldTimes = static_cast<int*>(data);
    return canFinish(*pBuyGoldTimes);
}

bool CTask::onHeroTestAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));
    if (EPARAM_COUNT_ONE > m_pTaskItemConf->FinishParameters.size())
    {
        return false;
    }

    int *pDiff = static_cast<int*>(data);
    if (*pDiff >= m_pTaskItemConf->FinishParameters[1])
    {
        return canFinish();
    }

    return false;
}

bool CTask::onGoldTestAction(int actID, void *data, int len)
{
    return canFinish();
}

bool CTask::onTowerFloorAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(int));
    // 层数
    int *pFloor = static_cast<int*>(data);
    return canFinish(*pFloor);
}

bool CTask::onPvpAction(int actID, void *data, int len)
{
    return canFinish();
}

bool CTask::onEquipAction(int actID, void *data, int len)
{
    CHECK_RETURN(NULL != data && len == sizeof(ActionCommonInfo));
    ActionCommonInfo *pTaskPassStage = static_cast<ActionCommonInfo*>(data);
    
    return canFinish(pTaskPassStage->param2);
}

bool CTask::canFinish(int val /*= 1*/)
{
    m_TaskInfo.taskVal += val;
    if (m_TaskInfo.taskVal >= m_pTaskItemConf->CompleteTimes)
    {
        m_TaskInfo.taskStatus = ETASK_FINISH;
        // 隐藏任务
        if (TASK_HIDE == m_pTaskItemConf->Show)
        {
            // 开启新任务(没有奖励)
            std::vector<int>::const_iterator iterEndStart = m_pTaskItemConf->EndStartID.begin();
            for (; iterEndStart != m_pTaskItemConf->EndStartID.end(); iterEndStart++)
            {
                if (CTaskAchieveHelper::addTask(m_nUserID, *iterEndStart))
                {
                    CTaskAchieveHelper::instanceTask(m_nUserID, *iterEndStart);
                }
            }
            // 完成则直接关闭
            if(m_pTaskModel->RemoveTask(m_nTaskID))
            {
                return true;
            }
        }
    }

    if (!m_pTaskModel->SetTask(m_nTaskID, m_TaskInfo))
    {
        // 存储失败处理
    }

    return m_TaskInfo.taskStatus == ETASK_FINISH;
}
