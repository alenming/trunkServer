#include "AchieveService.h"
#include "AchievementProtocol.h"
#include "UserActionListener.h"
#include "GameUserManager.h"
#include "ServiceDef.h"
#include "GameDef.h"
#include "ModelHelper.h"
#include "PropUseHelper.h"
#include "ErrorCodeProtocol.h"
#include "KxCommManager.h"
#include "CommOssHelper.h"
#include "Protocol.h"
#include "TaskAchieveHelper.h"

using namespace std;
using namespace KxServer;

void CAchieveService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    switch (subcmd)
    {
    case CMD_ACHIEVEMENT_FINISH_CS:
        processFinishService(uid, buffer, len, commun);
        break;
    case CMD_ACHIEVEMENT_GAIN_CS:
        processAwardService(uid, buffer, len, commun);
        break;
    }
}

void CAchieveService::processFinishService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(len == sizeof(AchieveFinishCSC));

	CAchievementModel *pAchieveModel = dynamic_cast<CAchievementModel*>(CModelHelper::getModel(uid, MODELTYPE_ACHIEVEMENT));
    CHECK_RETURN_VOID(NULL != pAchieveModel);

    AchieveFinishCSC *pAchieveFinishCS = reinterpret_cast<AchieveFinishCSC*>(buffer);
    const AchieveItem *pAchieveItem = queryConfAchieve(pAchieveFinishCS->sachieveID);
    if (NULL == pAchieveItem)
    {
        KXLOGERROR("uid %d sachieveID %d processFinishService queryConfAchieve is NULL!", uid, pAchieveFinishCS->sachieveID);
        return;
    }

    AchieveDBInfo info;
    CHECK_RETURN_VOID(pAchieveModel->GetAchievementByID(pAchieveFinishCS->sachieveID, info));

    if (info.achieveStatus == EACHIEVE_STATUS_GET)
	{
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_ACHIEVE_HAVEGET;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return;
	}

    // 跨服务器任务信任前端
    if (ELA_PVP == pAchieveItem->FinishCondition
        || ELA_UNION_TASK == pAchieveItem->FinishCondition
        || ELA_USE_SAME_RACE == pAchieveItem->FinishCondition
        || ELA_PVP_KEEP_WIN == pAchieveItem->FinishCondition
        || ELA_PVP_SCORE == pAchieveItem->FinishCondition)
    {
        CHECK_RETURN_VOID(pAchieveFinishCS->extend >= 0);
        info.achieveVal = pAchieveFinishCS->extend;
        CHECK_RETURN_VOID(pAchieveModel->SetAchievement(pAchieveFinishCS->sachieveID, info));
    }

    CHECK_RETURN_VOID(CTaskAchieveHelper::canFinishAchieve(uid, pAchieveFinishCS->sachieveID, info.achieveVal));

    bool bRlt = false;
    // 隐藏成就
    if (TASK_HIDE == pAchieveItem->Show)
    {
        // 开启新任务(没有奖励)
        std::vector<int>::const_iterator iterEndStart = pAchieveItem->EndStartID.begin();
        for (; iterEndStart != pAchieveItem->EndStartID.end(); iterEndStart++)
        {
            CTaskAchieveHelper::addAchieve(uid, *iterEndStart, true);
        }
        // 完成则直接关闭
        if (pAchieveModel->RemoveAchievementByID(pAchieveFinishCS->sachieveID))
        {
            bRlt = true;
        }
    }
    else
    {
        info.achieveStatus = EACHIEVE_STATUS_FINISH;
        if (pAchieveModel->SetAchievement(pAchieveFinishCS->sachieveID, info))
        {
            bRlt = true;
        }
    }

    if (bRlt)
    {
        CKxCommManager::getInstance()->sendData(uid, CMD_ACHIEVEMENT, CMD_ACHIEVEMENT_FINISH_SC, 
			reinterpret_cast<char*>(pAchieveFinishCS), sizeof(AchieveFinishCSC));
    }
}

void CAchieveService::processAwardService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(len == sizeof(AchieveGainCS));
	CAchievementModel *pAchieveModel = dynamic_cast<CAchievementModel*>(CModelHelper::getModel(uid, MODELTYPE_ACHIEVEMENT));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
	CBagModel *pBagModel = dynamic_cast<CBagModel*>(CModelHelper::getModel(uid, MODELTYPE_BAG));
	CHECK_RETURN_VOID(NULL != pAchieveModel && NULL != pUserModel && NULL != pBagModel);
 
    AchieveGainCS *pAchieveGainCS = reinterpret_cast<AchieveGainCS*>(buffer);
	const AchieveItem *pAchieveItem = queryConfAchieve(pAchieveGainCS->sachieveID);
    if (NULL == pAchieveItem)
    {
        KXLOGERROR("processAwardService queryConfAchieve is NULL, uid %d sachieveID %d", uid, pAchieveGainCS->sachieveID);
        return;
    }
 
    AchieveDBInfo info;
	CHECK_RETURN_VOID(pAchieveModel->GetAchievementByID(pAchieveGainCS->sachieveID, info));

    if (info.achieveVal < pAchieveItem->CompleteTimes
        && EACHIEVE_STATUS_FINISH != info.achieveStatus)
    {
        ErrorCodeData CodeData;
        CodeData.nCode = ERROR_ACHIEVE_NOTFINISH;
        CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
        return;
    }

    if (TASK_HIDE == pAchieveItem->CloseDisplay)
    {
		CHECK_RETURN_VOID(pAchieveModel->RemoveAchievementByID(pAchieveGainCS->sachieveID));
    }
    else
    {
        info.achieveVal = 0;
        info.achieveStatus = EACHIEVE_STATUS_GET;
		CHECK_RETURN_VOID(pAchieveModel->SetAchievement(pAchieveGainCS->sachieveID, info));
    }

    // 开启新成就
    std::vector<int>::const_iterator iterEndStar = pAchieveItem->EndStartID.begin();
    for (; iterEndStar != pAchieveItem->EndStartID.end(); ++iterEndStar)
    {
        CTaskAchieveHelper::addAchieve(uid, *iterEndStar, true);
    }

    std::vector<DropItemInfo> rewardItems;
    std::vector<ID_Num>::const_iterator iterItems = pAchieveItem->AwardItems.begin();
    for (; iterItems != pAchieveItem->AwardItems.end(); iterItems++)
    {
        DropItemInfo info;
        info.id = iterItems->ID;
        info.num = iterItems->num;
        rewardItems.push_back(info);
    }

    CPropUseHelper *pPropUseHelper = CPropUseHelper::getInstance();
    pPropUseHelper->AddResource(RESOURCEID_GOLD, pAchieveItem->AwardCoin, rewardItems);
    pPropUseHelper->AddResource(RESOURCEID_DIAMOND, pAchieveItem->AwardDiamond, rewardItems);
    pPropUseHelper->AddResource(RESOURCEID_EXP, pAchieveItem->AwardExp, rewardItems);
    pPropUseHelper->AddResource(RESOURCEID_ENERGY, pAchieveItem->AwardEnergy, rewardItems);

	pPropUseHelper->AddItems(uid, rewardItems, true, ACHIEVE_GAIN_REASON);

    // 封包
    int bufSize = sizeof(AchieveGainSC)+rewardItems.size() * sizeof(DropItemInfo);
    char *buf = static_cast<char*>(kxMemMgrAlocate(bufSize));
    AchieveGainSC *pAchieveGainSC = reinterpret_cast<AchieveGainSC *>(buf);
    pAchieveGainSC->cCount = 0;
    pAchieveGainSC->achieveID = pAchieveGainCS->sachieveID;

    // 偏移量
    int nOffset = sizeof(AchieveGainSC);
    DropItemInfo *pRewardItem = reinterpret_cast<DropItemInfo*>(pAchieveGainSC + 1);

    for (unsigned int i = 0; i < rewardItems.size(); ++i)
    {
        memcpy(pRewardItem, &(rewardItems[i]), sizeof(DropItemInfo));

		if (rewardItems[i].id == RESOURCEID_DIAMOND)
		{
			CCommOssHelper::userDiamondGetOss(uid, rewardItems[i].num, TASK_DIAMOND_GET_REASON);
		}
		else if (rewardItems[i].id == RESOURCEID_ENERGY)
		{
			CCommOssHelper::userTokenShopOss(uid, TASK_PAYLER_TYPE,0, NONE_PAY_TYPE, rewardItems[i].num, rewardItems[i].num);
		}

        pAchieveGainSC->cCount++;
        pRewardItem++;
        nOffset += sizeof(DropItemInfo);
    }

    // 发送数据后回收
    CKxCommManager::getInstance()->sendData(uid, CMD_ACHIEVEMENT, CMD_ACHIEVEMENT_GAIN_SC, buf, nOffset);
    kxMemMgrRecycle(buf, bufSize);
}
