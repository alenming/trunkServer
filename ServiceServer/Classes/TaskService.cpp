#include "TaskService.h"
#include "TaskProtocol.h"
#include "Protocol.h"
#include "GameUserManager.h"
#include "ServiceDef.h"
#include "KxMemPool.h"
#include "GameDef.h"
#include "ConfHall.h"
#include "ModelHelper.h"
#include "PropUseHelper.h"
#include "CommOssHelper.h"
#include "KxCommManager.h"
#include "TaskAchieveHelper.h"

using namespace std;
using namespace KxServer;

void CTaskService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    switch (subcmd)
    {
    case CMD_TASK_FINISH_CS:
        processFinishService(uid, buffer, len, commun);
        break;
    case CMD_TASK_AWARD_CS:
        processAwardService(uid, buffer, len, commun);
        break;
    }
}

void CTaskService::processFinishService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    // 暂时没有需要前端单独处理的状态类任务
    CHECK_RETURN_VOID(len == sizeof(TaskFinishCSC));
	CTaskModel *pTaskModel = dynamic_cast<CTaskModel*>(CModelHelper::getModel(uid, MODELTYPE_TASK));
	CHECK_RETURN_VOID(NULL != pTaskModel);

    TaskFinishCSC *pTaskFinishCS = reinterpret_cast<TaskFinishCSC*>(buffer);
    const TaskItem *pTaskItem = queryConfTask(pTaskFinishCS->taskID);
    if (NULL == pTaskItem)
    {
        KXLOGERROR("uid %d taskID %d processFinishService queryConfTask is NULL!", uid, pTaskFinishCS->taskID);
        return;
    }

    TaskDBInfo taskInfo;
    CHECK_RETURN_VOID(pTaskModel->GetTask(pTaskFinishCS->taskID, taskInfo));
    
    // 检测任务,是否过时间
    CTaskAchieveHelper::checkTask(uid, pTaskFinishCS->taskID, taskInfo);
    // 跨服务器任务信任前端
    if (ELA_PVP == pTaskItem->FinishCondition
        || ELA_UNION_TASK == pTaskItem->FinishCondition
        || ELA_UNION_LIVENESS == pTaskItem->FinishCondition
        || ELA_UNION_DISPATCH_MERCENARY == pTaskItem->FinishCondition
        || ELA_UNION_USE_MERCENARY == pTaskItem->FinishCondition)
    {
        CHECK_RETURN_VOID(pTaskFinishCS->extend >= 0);
        taskInfo.taskVal = pTaskFinishCS->extend;
        CHECK_RETURN_VOID(pTaskModel->SetTask(pTaskFinishCS->taskID, taskInfo));
    }
    else if (CTaskAchieveHelper::canFinishTask(uid, pTaskFinishCS->taskID))
    {
        taskInfo.taskStatus = ETASK_FINISH;
        CHECK_RETURN_VOID(pTaskModel->SetTask(pTaskFinishCS->taskID, taskInfo));
    }
    // 回发任务数据
	CKxCommManager::getInstance()->sendData(uid, CMD_TASK, CMD_TASK_FINISH_SC,
		reinterpret_cast<char*>(pTaskFinishCS), sizeof(TaskFinishCSC));
}

void CTaskService::processAwardService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(len == sizeof(TaskAwardCS));

	CTaskModel *pTaskModel = dynamic_cast<CTaskModel*>(CModelHelper::getModel(uid, MODELTYPE_TASK));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
	CBagModel *pBagModel = dynamic_cast<CBagModel*>(CModelHelper::getModel(uid, MODELTYPE_BAG));
	CHECK_RETURN_VOID(NULL != pBagModel && NULL != pTaskModel && NULL != pUserModel);

    TaskAwardCS *pTaskAwardCS = reinterpret_cast<TaskAwardCS*>(buffer);
    const TaskItem *pTaskItem = queryConfTask(pTaskAwardCS->taskID);
    if (NULL == pTaskItem)
    {
        KXLOGERROR("uid %d taskID %d processAwardService queryConfTask is NULL!", uid, pTaskAwardCS->taskID);
        return;
    }
    // 任务信息
    TaskDBInfo taskInfo;
    CHECK_RETURN_VOID(pTaskModel->GetTask(pTaskAwardCS->taskID, taskInfo));
    // 检测任务,是否过时间
    CTaskAchieveHelper::checkTask(uid, pTaskAwardCS->taskID, taskInfo);

    // 如果完成任务领取奖励
    if (taskInfo.taskVal < pTaskItem->CompleteTimes
        && taskInfo.taskStatus != ETASK_FINISH
        && !CTaskAchieveHelper::isFinishSpecialTask(uid, pTaskAwardCS->taskID, taskInfo))
    {
        return;
    }

    // 领取奖励的任务不是重置任务删除,否则设置为领取状态
    if (ERTT_NON == pTaskItem->TaskReset)
    {
        // 删除非重置的任务
        CHECK_RETURN_VOID(pTaskModel->RemoveTask(pTaskAwardCS->taskID));
    }
    else
    {
        taskInfo.taskVal = 0;
        taskInfo.taskStatus = ETASK_GET;
        CHECK_RETURN_VOID(pTaskModel->SetTask(pTaskAwardCS->taskID, taskInfo));
    }

    // 开启任务
    vector<int>::const_iterator iterEndStart = pTaskItem->EndStartID.begin();
    for (; iterEndStart != pTaskItem->EndStartID.end(); iterEndStart++)
    {
        if (CTaskAchieveHelper::addTask(uid, *iterEndStart))
        {
            CTaskAchieveHelper::instanceTask(uid, *iterEndStart);
        }
    }

    std::vector<DropItemInfo> rewardItems;
    std::vector<ID_Num>::const_iterator iterItems = pTaskItem->AwardItems.begin();
    for (; iterItems != pTaskItem->AwardItems.end(); iterItems++)
    {
        DropItemInfo info;
        info.id = iterItems->ID;
        info.num = iterItems->num;
        rewardItems.push_back(info);
    }

    CPropUseHelper *pPropUseHelper = CPropUseHelper::getInstance();
    pPropUseHelper->AddResource(RESOURCEID_GOLD, pTaskItem->AwardCoin, rewardItems);
    pPropUseHelper->AddResource(RESOURCEID_DIAMOND, pTaskItem->AwardDiamond, rewardItems);
    pPropUseHelper->AddResource(RESOURCEID_EXP, pTaskItem->AwardExp, rewardItems);
    pPropUseHelper->AddResource(RESOURCEID_ENERGY, pTaskItem->AwardEnergy, rewardItems);
    pPropUseHelper->AddResource(RESOURCEID_FLASHCARD, pTaskItem->AwardFlashcard, rewardItems);

	pPropUseHelper->AddItems(uid, rewardItems, true, TASK_GAIN_REASON);

    // 封包
    int bufSize = sizeof(TaskAwardSC)+rewardItems.size()*sizeof(DropItemInfo);
    char *buf = static_cast<char*>(kxMemMgrAlocate(bufSize));
    TaskAwardSC *pTaskAwardSC = reinterpret_cast<TaskAwardSC *>(buf);
    pTaskAwardSC->taskID = pTaskAwardCS->taskID;
    pTaskAwardSC->awardCount = 0;

    // 偏移量
    int nOffset = sizeof(TaskAwardSC);
    for (unsigned int i = 0; i < rewardItems.size(); i++)
    {
        DropItemInfo *pDropItemInfo = reinterpret_cast<DropItemInfo*>(buf + nOffset);
        pTaskAwardSC->awardCount++;
        memcpy(pDropItemInfo, &rewardItems[i], sizeof(DropItemInfo));
        nOffset += sizeof(DropItemInfo);

        if (rewardItems[i].id == RESOURCEID_DIAMOND)
        {
            CCommOssHelper::userDiamondGetOss(uid, rewardItems[i].num, TASK_DIAMOND_GET_REASON);
        }
        else if (rewardItems[i].id == RESOURCEID_ENERGY)
        {
            CCommOssHelper::userTokenShopOss(uid, TASK_PAYLER_TYPE, 0, NONE_PAY_TYPE, rewardItems[i].num, rewardItems[i].id);
        }
    }

    // 发送数据后回收
    CKxCommManager::getInstance()->sendData(uid, CMD_TASK, CMD_TASK_AWARD_SC, buf, nOffset);
    kxMemMgrRecycle(buf, bufSize);
}
