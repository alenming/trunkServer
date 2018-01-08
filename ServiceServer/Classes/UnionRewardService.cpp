#include "UnionRewardService.h"
#include "Protocol.h"
#include "UnionExpiditionProtocol.h"
#include "UnionProtocol.h"
#include "KxCSComm.h"
#include "GameUserManager.h"
#include "KxCommManager.h"
#include "PropUseHelper.h"
#include "CommOssHelper.h"

using namespace std;

void CUnionExpiditionRewardService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
		case CMD_UNIONEXPIDITION_REWARDGET_CS:
		{
			processExpiditionRewardGet(uid, buffer, len, commun);
		}
		break;
		default:
		break;
	}
}

//处理公会远征伤害排行奖励领取
void CUnionExpiditionRewardService::processExpiditionRewardGet(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN_DEBUG_VOID(NULL != pGameUser);
	CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
	CHECK_RETURN_DEBUG_VOID(pUserUnionModel != NULL);
	pUserUnionModel->Refresh();

	std::vector<int> RewardValue = pUserUnionModel->getUserExpiditionValue();
	
	if (RewardValue.size() != 0)
	{
		//添加物品
		std::vector<DropItemInfo> items;
		DropItemInfo Info;
		bool bSendEmail = true;

		//发送消息应答
		int nSendLength = RewardValue.size() * sizeof(int)+sizeof(UnionExpiditionRewardGetSC);
		char *pSendData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nSendLength));
		UnionExpiditionRewardGetSC *pRewardGetSC = reinterpret_cast<UnionExpiditionRewardGetSC *>(pSendData);
		pRewardGetSC->nNum = RewardValue.size();
		int *pRewardID = (int*)(pRewardGetSC + 1);
		for (int i = 0; i < RewardValue.size(); i++)
		{
			Info.id = RewardValue[i];
			Info.num = 1;
			items.push_back(Info);
			*pRewardID = RewardValue[i];
			pRewardID++;
		}

		CHECK_RETURN_DEBUG_VOID(CPropUseHelper::getInstance()->AddItems(uid, items, bSendEmail,UNIONSTAGEBOX_GAION_REASON));
		RewardValue.clear();
		pUserUnionModel->setUserExpiditionValue(RewardValue);
		CKxCommManager::getInstance()->sendData(uid, CMD_UNIONEXPIDITION, CMD_UNIONEXPIDITION_REWARDGET_SC, pSendData, nSendLength);
		KxServer::kxMemMgrRecycle(pSendData, nSendLength);
	}
}

void CUnionRewardService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    switch (subcmd)
	{
		case CMD_UNION_WELFARE_SS:
            processWelfareReward(uid, buffer, len, commun);
		    break;
		default:
		    break;
	}
}

void CUnionRewardService::processWelfareReward(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(len == sizeof(UnionWelfareSS));
    UnionWelfareSS *pUnionWelfareSS = reinterpret_cast<UnionWelfareSS*>(buffer);

    //获得配置物品
    std::vector<DropItemInfo> dropItems;
    CItemDrop::Drop(pUnionWelfareSS->dropId, dropItems);
    CPropUseHelper::getInstance()->AddItems(uid, dropItems, true, UNIONACTIVEBOX_GAION_REASON);

    int bufSize = sizeof(UnionWelfareSC)+dropItems.size()*sizeof(DropItemInfo);
    char *buf = static_cast<char*>(KxServer::kxMemMgrAlocate(bufSize));
    UnionWelfareSC *pUnionWelfareSC = reinterpret_cast<UnionWelfareSC *>(buf);
    pUnionWelfareSC->welfareType = pUnionWelfareSS->welfareType;
    pUnionWelfareSC->itemCount = 0;

    DropItemInfo *pDropItemInfo = reinterpret_cast<DropItemInfo*>(pUnionWelfareSC + 1);
    for (unsigned int i = 0; i < dropItems.size(); i++)
    {
        ++pUnionWelfareSC->itemCount;
        memcpy(pDropItemInfo, &dropItems[i], sizeof(DropItemInfo));

        if (dropItems[i].id == RESOURCEID_DIAMOND)
        {
            CCommOssHelper::userDiamondGetOss(uid, dropItems[i].num, TASK_DIAMOND_GET_REASON);
        }
        else if (dropItems[i].id == RESOURCEID_ENERGY)
        {
            CCommOssHelper::userTokenShopOss(uid, TASK_PAYLER_TYPE, 0, NONE_PAY_TYPE, dropItems[i].num, dropItems[i].id);
        }

        pDropItemInfo += 1;
    }

    CKxCommManager::getInstance()->sendData(uid, CMD_UNION, CMD_UNION_WELFARE_SC, buf, bufSize);
    KxServer::kxMemMgrRecycle(buf, bufSize);
}
