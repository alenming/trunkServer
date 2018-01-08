#include "TaskAchieveHelper.h"
#include "Achievement.h"
#include "ModelDef.h"
#include "UnionModel.h"
#include "MailModel.h"
#include "MailProtocol.h"
#include "GameDef.h"
#include "ConfRole.h"
#include "ConfGameSetting.h"
#include "ModelHelper.h"
#include "Task.h"
#include "GameUserManager.h"
#include "ServiceDef.h"

using namespace std;

bool CTaskAchieveHelper::initTask(int uid)
{
	CTaskModel *model = dynamic_cast<CTaskModel*>(CModelHelper::getModel(uid, MODELTYPE_TASK));
    if (NULL == model)
    {
        return false;
    }

    std::map<int, TaskDBInfo>::iterator iter = model->GetTasks().begin();
    for (; iter != model->GetTasks().end(); ++iter)
    {
        // 登录检测重置任务数据
        checkTask(uid, iter->first, iter->second);
        instanceTask(uid, iter->first);
    }

    return true;
}

bool CTaskAchieveHelper::initAchieve(int uid)
{
	CAchievementModel *model = dynamic_cast<CAchievementModel*>(CModelHelper::getModel(uid, MODELTYPE_ACHIEVEMENT));
    if (NULL == model)
    {
        return false;
    }

    std::map<int, AchieveDBInfo>::iterator iter = model->GetAllAchievement().begin();
    for (; iter != model->GetAllAchievement().end(); ++iter)
    {
        // 实例化成就监听
        instanceAchieve(uid, iter->first);
    }

    return true;
}

bool CTaskAchieveHelper::addTask(int uid, int taskID)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    if (NULL == pGameUser)
    {
        return false;
    }

    const TaskItem *pTaskItem = queryConfTask(taskID);
    if (pTaskItem)
    {
		CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
        int nUserLv = 0;
        if (!pUserModel->GetUserFieldVal(USR_FD_USERLV, nUserLv))
        {
            return false;
        }

		CTaskModel *pTaskModel = dynamic_cast<CTaskModel*>(pGameUser->getModel(MODELTYPE_TASK));

        TaskDBInfo taskInfo;
        memset(&taskInfo, 0, sizeof(TaskDBInfo));
        if (pTaskItem->UnlockLv > nUserLv)
        {
            taskInfo.taskStatus = ETASK_UNACTIVE;
        }
        else if (ERTT_NON != pTaskItem->TaskReset)
        {
            taskInfo.resetTime = getTaskResetTimeStamp(taskID);
        }

        if (pTaskModel->AddTask(taskID, taskInfo))
        {
            return true;
        }
    }

    return false;
}

int CTaskAchieveHelper::getTaskResetTimeStamp(int taskID)
{
    int nResetTime = 0;
    const TaskItem *pTaskItem = queryConfTask(taskID);
    if (pTaskItem)
    {
        if (ERTT_DAY == pTaskItem->TaskReset)
        {
            nResetTime = calcWDayTimeStamp(pTaskItem->TaskResetParameters[0]);
        }
        else if (ERTT_WEEK == pTaskItem->TaskReset)
        {
            nResetTime = calcWDayTimeStamp(pTaskItem->TaskResetParameters[1]
                , pTaskItem->TaskResetParameters[0]);
        }
    }

    return nResetTime;
}

void CTaskAchieveHelper::checkTask(int uid, int taskID, TaskDBInfo &info)
{
    const TaskItem *pTaskItem = queryConfTask(taskID);
    if (NULL == pTaskItem)
    {
        KXLOGERROR("uid %d can't find task id %d when check task!", uid, taskID);
        return;
    }

    bool bSave = false;
    if (ETASK_UNACTIVE == info.taskStatus)
    {
        CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
		if (NULL == pUserModel)
		{
			KXLOGERROR("uid %d get user model error when check task1!", uid);
			return;
		}

        int nUserLv = 0;
        pUserModel->GetUserFieldVal(USR_FD_USERLV, nUserLv);
        if (pTaskItem->UnlockLv <= nUserLv)
        {
            bSave = true;
            info.taskStatus = ETASK_ACTIVE;
        }
    }
    else if (ERTT_NON != pTaskItem->TaskReset) 
    {
        int nCurStamp = (int)time(NULL);
        if (nCurStamp >= info.resetTime)
        {
            if (ETASK_FINISH == info.taskStatus)
            {
                // 已经完成的重置任务奖励通过邮件发送给玩家
            }

            bSave = true;
            info.taskVal = 0;
            info.taskStatus = ETASK_ACTIVE;
            info.resetTime = getTaskResetTimeStamp(taskID);
        }
    }

    if (bSave)
    {
		CTaskModel *pTaskModel = dynamic_cast<CTaskModel*>(CModelHelper::getModel(uid, MODELTYPE_TASK));
		if (NULL == pTaskModel)
		{
			KXLOGERROR("uid %d get task model error when check task2!", uid);
			return;
		}
        if (!pTaskModel->SetTask(taskID, info))
        {
            KXLOGERROR("CTaskAchieveHelper::checkTask, taskmodel settask fail, %d", taskID);
        }
    }
}

bool CTaskAchieveHelper::instanceTask(int uid, int taskID)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	if (NULL == pGameUser)
	{
		return false;
	}

    const TaskItem *pTaskItem = queryConfTask(taskID);
    if (NULL == pTaskItem || ELA_DAILY_ENERGY == pTaskItem->FinishCondition)
    {
        return false;
    }

	CTaskModel *pTaskModel = dynamic_cast<CTaskModel*>(CModelHelper::getModel(uid, MODELTYPE_TASK));
    TaskDBInfo info;
    if (!pTaskModel->GetTask(taskID, info))
    {
        return false;
    }

    if (EACHIEVE_STATUS_UNACTIVE != info.taskStatus
        && !needListenTask(pTaskItem->FinishCondition))
    {
        return true;
    }

    CTask *pTask = new CTask();
    if (NULL == pTask)
    {
        return false;
    }

    if (pTask->init(uid, taskID, info))
    {
        // 如果还没达到解锁等级(未激活)监听等级事件,否则监听对应的事件
        if (ETASK_UNACTIVE == info.taskStatus)
        {
            pGameUser->getUserActionManager()->AddActionListener(ELA_USER_LEVEL_UP, pTask);
        }
        else
        {
            pGameUser->getUserActionManager()->AddActionListener(pTaskItem->FinishCondition, pTask);
        }

        return true;
    }
    
    delete pTask;
    pTask = NULL;
    KXLOGERROR("create task, init error!!!");

    return false;
}

bool CTaskAchieveHelper::isFinishSpecialTask(int uid, int taskID, const TaskDBInfo& taskInfo)
{
    const TaskItem *pTaskItem = queryConfTask(taskID);
    CHECK_RETURN(NULL != pTaskItem);

    CHECK_RETURN(taskInfo.taskStatus != ETASK_GET);
    time_t nCurStamp = time(NULL);
    if (ELA_DAILY_ENERGY == pTaskItem->FinishCondition)
    {
        tm curTm;
        curTm = *(localtime(&nCurStamp));
        int nCurHour = curTm.tm_hour % 24;
        if (pTaskItem->FinishParameters.size() > 0
            && nCurHour >= pTaskItem->FinishParameters[0])
        {
            return true;
        }
    }
    else if (ELA_MONTH_CARD == pTaskItem->FinishCondition)
    {
		CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
        int nVipStamp = 0;
        CHECK_RETURN(pUserModel->GetUserFieldVal(USR_FD_MONTHCARDSTAMP, nVipStamp, true));
        if (nVipStamp >= nCurStamp)
        {
            return true;
        }
    }

    return false;
}

bool CTaskAchieveHelper::canFinishTask(int uid, int taskID)
{
    const TaskItem *pTaskItem = queryConfTask(taskID);
    CHECK_RETURN(NULL != pTaskItem);
    bool bFinish = false;

    switch (pTaskItem->FinishCondition)
    {
    case ELA_PASSED_STAGE:
    {					
		CStageModel *pStageModel = dynamic_cast<CStageModel*>(CModelHelper::getModel(uid, MODELTYPE_STAGE));
        if (pTaskItem->FinishParameters.size() > 0)
        {
            int curStage = 0;
            if (pTaskItem->FinishParameters[0] > 10000)
            {
                curStage = pStageModel->GetCurElite();
            }
            else
            {
                curStage = pStageModel->GetCurStage();
            }

            if (curStage >= pTaskItem->FinishParameters[0])
            {
                bFinish = true;
            }
        }
        break;
    }

    case ELA_OWN_HERO:
    {
        CHECK_RETURN(pTaskItem->FinishParameters.size() > 0);
        bFinish = canFinishCommon(uid, pTaskItem->FinishCondition,
            pTaskItem->CompleteTimes, pTaskItem->FinishParameters[0]);
        break;
    }

    default:
        break;
    }

    return bFinish;
}

bool CTaskAchieveHelper::addAchieve(int uid, int achieveID, bool instance /*= false*/)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN(NULL != pGameUser);

    const AchieveItem *pAchieveItem = queryConfAchieve(achieveID);
    CHECK_RETURN(NULL != pAchieveItem);

    CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
    int nUserLv = 0;
    if (!pUserModel->GetUserFieldVal(USR_FD_USERLV, nUserLv))
    {
        return false;
    }

    bool bSave = true;
    AchieveDBInfo info;
    memset(&info, 0, sizeof(AchieveDBInfo));
    if (pAchieveItem->UnLockLv > nUserLv) // 等级未满足->未激活状态,需监听等级
    {
        info.achieveStatus = EACHIEVE_STATUS_UNACTIVE;
    }
    else if (canFinishAchieve(uid, achieveID)) // 新成就已经完成
    {
        if (TASK_HIDE == pAchieveItem->Show) // 隐藏的成就
        {
            bSave = false;
            std::vector<int>::const_iterator iter = pAchieveItem->EndStartID.begin();
            for (; iter != pAchieveItem->EndStartID.end(); ++iter)
            {
                addAchieve(uid, *iter);
            }
        }
        else
        {
            info.achieveStatus = EACHIEVE_STATUS_FINISH;
        }
    }

    if (ELA_ACCUMULATE_GOLD == pAchieveItem->FinishCondition)
    {
        int nAccumulateGold = 0;
        if (pUserModel->GetUserFieldVal(USR_FD_ACCUMULATEGOLD, nAccumulateGold))
        {
            info.achieveVal = nAccumulateGold;
        }
    }

    if (bSave)
    {
        CAchievementModel *pAchieveModel = dynamic_cast<CAchievementModel*>(pGameUser->getModel(MODELTYPE_ACHIEVEMENT));
        if (pAchieveModel->AddAchievement(achieveID, info))
        {
            if (instance)
            {
                instanceAchieve(uid, achieveID);
            }

            return true;
        }
    }

    return false;
}

bool CTaskAchieveHelper::instanceAchieve(int uid, int achieveID)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    if (NULL == pGameUser)
    {
        return false;
    }

    const AchieveItem *pAchieveItem = queryConfAchieve(achieveID);
    if (NULL == pAchieveItem)
    {
        KXLOGERROR("config can't find achieve id, %d", achieveID);
		return false;
    }

    CAchievementModel *pAchieveModel = dynamic_cast<CAchievementModel*>(pGameUser->getModel(MODELTYPE_ACHIEVEMENT));
    AchieveDBInfo info;
    if (!pAchieveModel->GetAchievementByID(achieveID, info))
    {
        return false;
    }

    // 非激活的成就都需要监听等级
    if (EACHIEVE_STATUS_UNACTIVE != info.achieveStatus)
    {
        // 完成的成就和领取的成就(前端显示用)不需要监听
        if (EACHIEVE_STATUS_FINISH == info.achieveStatus
            || EACHIEVE_STATUS_GET == info.achieveStatus)
        {
            return false;
        }
    }

    if (EACHIEVE_STATUS_UNACTIVE != info.achieveStatus 
        && !needListenAchieve(pAchieveItem->FinishCondition))
    {
        return true;
    }

    CAchievement *pAchieve = new CAchievement();
    if (NULL == pAchieve)
    {
        return false;
    }

    if (pAchieve->init(uid, achieveID, info))
    {
        // 如果还没达到解锁等级(未激活)监听等级事件,否则监听对应的事件
        if (EACHIEVE_STATUS_UNACTIVE == info.achieveStatus)
        {
            pGameUser->getUserActionManager()->AddActionListener(ELA_USER_LEVEL_UP, pAchieve);
        }
        else
        {
            pGameUser->getUserActionManager()->AddActionListener(pAchieveItem->FinishCondition, pAchieve);
        }

        return true;
    }
    else
    {
        KXLOGERROR("create achieve, init error!!!");
        delete pAchieve;
        return false;
    }
}

bool CTaskAchieveHelper::canFinishAchieve(int uid, int achieveID, int times)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN(NULL != pGameUser);

    const AchieveItem *pAchieveItem = queryConfAchieve(achieveID);
    CHECK_RETURN(NULL != pAchieveItem);

    int nTemp = 0;
    bool bFinish = false;
    // 各类成就是否完成
    switch (pAchieveItem->FinishCondition)
    {
    case ELA_USER_LEVEL_UP:
    {    
        CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
		if (pUserModel->GetUserFieldVal(USR_FD_USERLV, nTemp))
		{
			if (pAchieveItem->FinishParameters.size() > 0
				&& nTemp >= pAchieveItem->FinishParameters[0])
			{
				bFinish = true;
			}
		}

        break;
    }
    case ELA_OWN_HERO:
    {
        CHECK_RETURN(pAchieveItem->FinishParameters.size() > 0);
        bFinish = canFinishCommon(uid, pAchieveItem->FinishCondition, 
            pAchieveItem->CompleteTimes, pAchieveItem->FinishParameters[0]);
        break;
    }
    case ELA_HAVE_HERO_X_STAR:
    {
        CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(pGameUser->getModel(MODELTYPE_HERO));
        CHECK_RETURN(pAchieveItem->FinishParameters.size() > 0);
        std::map<int, DBHeroAttr>& mapHerosInfo = pHeroModel->GetHeros();
		std::map<int, DBHeroAttr>::iterator iterHeros = mapHerosInfo.begin();
        for (; iterHeros != mapHerosInfo.end(); ++iterHeros)
        {
            if (iterHeros->second.star >= pAchieveItem->FinishParameters[0])
            {
                nTemp += 1;
                if (nTemp >= pAchieveItem->CompleteTimes)
                {
                    bFinish = true;
                    break;
                }
            }
        }
        break;
    }
    case ELA_HERO_UPGRADE_STAR_TO_X:
        CHECK_RETURN(pAchieveItem->FinishParameters.size() > 0);
        bFinish = canFinishHeroAchieve(uid, pAchieveItem->FinishCondition, pAchieveItem->FinishParameters[0]);
        break;
    case ELA_HERO_UPGRADE_SKILL_TO_X:
    case ELA_OWN_DIFFEREN_HERO:
        bFinish = canFinishHeroAchieve(uid, pAchieveItem->FinishCondition, pAchieveItem->CompleteTimes);
        break;

    case ELA_OWN_EQUIP:
    {
	    CEquipModel *pEquipModel = dynamic_cast<CEquipModel*>(pGameUser->getModel(MODELTYPE_EQUIP));
        std::map<int, SEquipInfo>& mapEquips = pEquipModel->GetEquipsInfo();
		std::map<int, SEquipInfo>::iterator iterEquip = mapEquips.begin();
        for (; iterEquip != mapEquips.end(); iterEquip++)
        {
            const PropItem *pPropItem = queryConfProp(iterEquip->second.nItemID);
            const EquipmentItem *pEquipmentItem = queryConfEquipment(iterEquip->second.nItemID);
            if (pPropItem && pEquipmentItem && pAchieveItem->FinishParameters.size() > 0)
            {
                if ((0 == pAchieveItem->FinishParameters[0] && 0 == pAchieveItem->FinishParameters[1]) // 任意颜色任意部位
                    || (0 == pAchieveItem->FinishParameters[0] && pEquipmentItem->Parts == pAchieveItem->FinishParameters[1]) // 任意颜色特定部位
                    || (pPropItem->Quality == pAchieveItem->FinishParameters[0] && 0 == pAchieveItem->FinishParameters[1])
                    || (pPropItem->Quality == pAchieveItem->FinishParameters[0] && pEquipmentItem->Parts == pAchieveItem->FinishParameters[1]))
                {
                    // 满足品质、部位装备的件数
                    nTemp += 1;
                    if (nTemp >= pAchieveItem->CompleteTimes)
                    {
                        bFinish = true;
                        break;
                    }
                }
            }
        }

        break;
    }
    case ELA_OWN_SUMMONER_COUNT:
    {
        CSummonModel *pSummonModel = dynamic_cast<CSummonModel*>(pGameUser->getModel(MODELTYPE_SUMMONER));
        nTemp = pSummonModel->GetAllSummon().size();
        if (nTemp >= pAchieveItem->CompleteTimes)
        {
            bFinish = true;
        }

        break;
    }
    case ELA_OWN_ASSIGN_SUMMONER:
    {
    CSummonModel *pSummonModel = dynamic_cast<CSummonModel*>(pGameUser->getModel(MODELTYPE_SUMMONER));
        CHECK_RETURN(pAchieveItem->FinishParameters.size() > 0);
		std::vector<int>::iterator iterSummon = find(pSummonModel->GetAllSummon().begin(), pSummonModel->GetAllSummon().end(), pAchieveItem->FinishParameters[0]);
        if (iterSummon != pSummonModel->GetAllSummon().end())
        {
            bFinish = true;
        }

        break;
    }
    case ELA_STAGE_STAR:
    {
        CStageModel *pStageModel = dynamic_cast<CStageModel*>(pGameUser->getModel(MODELTYPE_STAGE));
		std::map<int, int>& mapNormalChapter = pStageModel->GetChapterStatusMap();
		std::map<int, int>::iterator iterNormalChapter = mapNormalChapter.begin();
		for (; iterNormalChapter != mapNormalChapter.end(); ++iterNormalChapter)
		{
			if (iterNormalChapter->second >= CHAPTERSTATUS_UNLOCK)
			{
				nTemp += CModelHelper::getChapterRewardStar(uid, iterNormalChapter->first);
				if (nTemp >= pAchieveItem->CompleteTimes)
				{
					bFinish = true;
					break;
				}
			}
			else
			{
				break;
			}
		}

		CHECK_BREAK(!bFinish);
        std::map<int, int>& mapEliteChapter = pStageModel->GetEliteChapterStatusMap();
		std::map<int, int>::iterator iterEliteChapter = mapEliteChapter.begin();
		for (; iterEliteChapter != mapEliteChapter.end(); ++iterEliteChapter)
		{
			if (iterEliteChapter->second >= CHAPTERSTATUS_UNLOCK)
			{
				nTemp += CModelHelper::getChapterRewardStar(uid, iterEliteChapter->first);
				if (nTemp >= pAchieveItem->CompleteTimes)
				{
					bFinish = true;
					break;
				}
			}
			else
			{
				break;
			}
		}
        break;
    }
    case ELA_ACCUMULATE_GOLD:
    {
        CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
		int nAccumulateGold = 0;
		if (pUserModel->GetUserFieldVal(USR_FD_ACCUMULATEGOLD, nAccumulateGold))
		{
			bFinish = nAccumulateGold >= pAchieveItem->CompleteTimes;
		}
        break;
    }
    case ELA_HAVE_UNION:
    {
        CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
		int nUnionId = 0;
        if (pUserUnionModel->getUserUnionValue(USER_UNION_ID, nUnionId) && nUnionId > 0)
		{
			bFinish = true;
		}
		break;
    }
	case ELA_PVP_SCORE:
	{
		CPvpModel *pPvpModel = dynamic_cast<CPvpModel *>(pGameUser->getModel(MODELTYPE_PVP));
		if (pPvpModel == NULL)
		{
			return false;
		}
		int integral = 0;
		if (pPvpModel->GetPvpField(PVP_FD_INTEGRAL, integral, true))
		{
			bFinish = integral >= pAchieveItem->CompleteTimes;
		}
		break;
	}
	case ELA_USE_SAME_RACE:
	{
		bFinish = times >= pAchieveItem->CompleteTimes;
		break;
	}
	case ELA_PVP_KEEP_WIN:
	{
		CPvpModel *pPvpModel = dynamic_cast<CPvpModel *>(pGameUser->getModel(MODELTYPE_PVP));
		if (pPvpModel == NULL)
		{
			return false;
		}
		int winTimes = 0;
		if (pPvpModel->GetPvpField(PVP_FD_HISTORYCONTINUSWINTIMES, winTimes))
		{
			bFinish = winTimes >= pAchieveItem->CompleteTimes;
		}
		break;
	}
    case ELA_PVP_RANK:
	{
        if (pAchieveItem->FinishParameters.size() < 2)
        {
            return false;
        }

		CPvpModel *pPvpModel = dynamic_cast<CPvpModel *>(pGameUser->getModel(MODELTYPE_PVP));
		if (pPvpModel == NULL)
		{
			return false;
		}

		int rank = 0;
        if (0 == pAchieveItem->FinishParameters[0])
        {
            pPvpModel->GetPvpField(PVP_FD_HISTORYHIGHESTRANK, rank);
        }
        else
        {
            pPvpModel->GetPvpField(CPN_FD_HISTORYHIGHESTRANK, rank);
        }

        if (rank > 0 && rank <= pAchieveItem->FinishParameters[1])
        {
            bFinish = true;
        }

		break;
	}
    default:
        break;
    }

    return bFinish;
}

bool CTaskAchieveHelper::canFinishHeroAchieve(int uid, int actID, int compelteTimes)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    if (NULL == pGameUser)
    {
        return false;
    }

	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(pGameUser->getModel(MODELTYPE_HERO));
    bool bFinish = false;
    if (ELA_HERO_UPGRADE_STAR_TO_X == actID)
    {
        std::map<int, DBHeroAttr>& mapHerosInfo = pHeroModel->GetHeros();
		std::map<int, DBHeroAttr>::iterator iterHeros = mapHerosInfo.begin();
        for (; iterHeros != mapHerosInfo.end(); ++iterHeros)
        {
            if (iterHeros->second.star >= compelteTimes)
            {
                bFinish = true;
                break;
            }
        }
    }
    else if (ELA_HERO_UPGRADE_SKILL_TO_X == actID)
    {
    }
    else if (ELA_OWN_DIFFEREN_HERO == actID) // 拥有不同的英雄,配置表ID
    {
        if (pHeroModel->GetHeros().size() >= static_cast<unsigned int>(compelteTimes))
        {
            bFinish = true;
        }
    }

    return bFinish;
}

bool CTaskAchieveHelper::canFinishCommon(int uid, int type, int compelteTimes, int finishParam)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN(NULL != pGameUser);

    int nTemp = 0;
    switch (type)
    {
    case ELA_OWN_HERO:
    {
        CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(pGameUser->getModel(MODELTYPE_HERO));
        std::map<int, DBHeroAttr>& mapHerosInfo = pHeroModel->GetHeros();
		std::map<int, DBHeroAttr>::iterator iterHeros = mapHerosInfo.begin();
        for (; iterHeros != mapHerosInfo.end(); ++iterHeros)
        {
            if (iterHeros->second.level >= finishParam)
            {
                nTemp += 1;
                if (nTemp >= compelteTimes)
                {
                    return true;
                }
            }
        }
        break;
    }
    default:
        break;
    }

    return false;
}

bool CTaskAchieveHelper::needListenTask(int actID)
{
    switch (actID)
    {
    case ELA_USER_LEVEL_UP:
    case ELA_PASS_STAGE:
    case ELA_HERO_EQUIP:
    case ELA_HERO_LEVEL_UP:
    case ELA_HERO_UPGRADE_STAR:
    case ELA_HERO_UPGRADE_SKILL:
    case ELA_DRAW_CARD:
    case ELA_USE_EXP_BOOK:
    case ELA_BUY_GOLD_TIMES:
    case ELA_HERO_TEST:
    case ELA_GOLD_TEST:
    case ELA_TOWER_TEST_FLOOR:
    case ELA_PVP:
    case ELA_EQUIPBUILD:
        return true;
    default:
        break;
    }

    return false;
}

bool CTaskAchieveHelper::needListenAchieve(int actID)
{
    switch (actID)
    {
    case ELA_USER_LEVEL_UP:
    case ELA_ACCUMULATE_GOLD:
    case ELA_HERO_LEVEL_UP:
    case ELA_HERO_TEST:
    case ELA_GOLD_TEST:
    case ELA_BUY_ITEM_COUNT:
    case ELA_USE_SAME_RACE:
    case ELA_DRAW_CARD:
    case ELA_BUY_GOLD_TIMES:
        return true;
    default:
        break;
    }

    return false;
}
