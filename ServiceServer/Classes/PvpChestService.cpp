#include "PvpChestService.h"
#include "Protocol.h"
#include "PvpChestProtocol.h"
#include "KxCommManager.h"
#include "KxCSComm.h"
#include "ModelHelper.h"
#include "PvpModel.h"
#include "PvpTaskModel.h"
#include "UserModel.h"
#include "ItemDrop.h"
#include "PropUseHelper.h"
#include "CommOssHelper.h"
#include "ConfArena.h"

void CPvpChestService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
    case CMD_PVPCHEST_REFRESH_CS:
        processRefreshChest(uid, buffer, len, commun);
		break;

    case CMD_PVPCHEST_BUY_CS:
        processBuyChest(uid, buffer, len, commun);
		break;

    case CMD_PVPCHEST_OPEN_CS:
        processOpenChest(uid, buffer, len, commun);
        break;

    case CMD_PVPCHEST_OPENATONCE_CS:
        processOpenChestAtOnce(uid, buffer, len, commun);
        break;

	default:
		break;
	}
}

void CPvpChestService::processRefreshChest(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CModelHelper::refreshPvpChest(uid);
}

void CPvpChestService::processBuyChest(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    // 判断宝箱是否已满(需计算恢复的宝箱)
	if (CModelHelper::isPvpChestFull(uid))
	{
		KXLOGERROR("%d pvp chest was full! can't buy again!", uid);
		return;
	}
    // 判断钻石是否够
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(uid, MODELTYPE_PVP));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
	CHECK_RETURN_VOID(NULL != pPvpModel && NULL != pUserModel);
	
	int diamond = 0;
	int dayBuyTimes = 0;
	
	if (!pPvpModel->GetPvpField(PVPCOMM_FD_DAYBUYCHESTTIMES, dayBuyTimes) ||
		!pUserModel->GetUserFieldVal(USR_FD_DIAMOND, diamond))
	{
		KXLOGERROR("%d get model data error!", uid);
		return;
	}
	dayBuyTimes += 1;
	int price = queryPvpChestPrice(dayBuyTimes);
	if (price > diamond)
	{
		KXLOGERROR("%d buy chest fail, because diamond not enough!", uid);
		return;
	}

    int chestId = CModelHelper::addPvpChest(uid);
	if (chestId < 0)
	{
		KXLOGERROR("%d add pvp chest fail!", uid);
		return;
	}
	// 次数+1
	if (!pPvpModel->SetPvpField(PVPCOMM_FD_DAYBUYCHESTTIMES, dayBuyTimes))
	{
		KXLOGERROR("%d set pvp field:PVPCOMM_FD_DAYBUYCHESTTIMES error when buy pvp chest!", uid);
		return;
	}
    // 如果宝箱已经满了
    if (CModelHelper::isPvpChestFull(uid))
    {
        if (!pPvpModel->SetPvpField(PVPCOMM_FD_LASTCHESTGENTIME, 0))
        {
            KXLOGERROR("%d set pvp field:PVPCOMM_FD_LASTCHESTGENTIME1 error when buy pvp chest!", uid);
            return;
        }
    }

    // 扣除钻石
    CModelHelper::addDiamond(uid, -1 * price);
	CCommOssHelper::userDiamondPayOss(uid, price, DIAMONDBUYPVPCHEST_TYPE, 0, 0);

	PvpChestBuySC buySC;
	buySC.chestId = chestId;
	buySC.costDiamond = price;

	CKxCommManager::getInstance()->sendData(uid,
        CMD_PVPCHEST, CMD_PVPCHEST_BUY_SC, reinterpret_cast<char*>(&buySC), sizeof(buySC));
}

void CPvpChestService::processOpenChest(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(NULL != buffer && len == sizeof(PvpChestOpenCS));
    
    CPvpModel *pPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(uid, MODELTYPE_PVP));
    CHECK_RETURN_VOID(NULL != pPvpModel);

	PvpChestOpenCS *pPvpChestOpenCS = reinterpret_cast<PvpChestOpenCS*>(buffer);
	int chestId = pPvpChestOpenCS->chestId;
    int chestStatus = 0; 
    if (!pPvpModel->GetPvpField(PVPCOMM_FD_CHESTSTATUS, chestStatus, true))
	{
		KXLOGERROR("%d get pvp field error when open pvp chest!", uid);
		return;
	}
	// 不能领
	if (chestStatus == 0)
	{
		KXLOGERROR("%d chestStatus == 0, can't get chest reward!", uid);
		return;
	}
	// 宝是否有宝箱
	int newChestId = CModelHelper::popPvpChest(uid);
	if (newChestId == 0)
	{
		KXLOGERROR("%d get DB chest id is 0, not chest in pool!", uid);
		return;
	}

	const ArenaChestItem *pChestConf = queryConfArenaChestItem(newChestId);
	if (NULL == pChestConf)
	{
		KXLOGERROR("%d chest conf in NULL, chest id = %d!", uid, newChestId);
		return;
	}
	//掉落物品
	std::vector<DropItemInfo> dropItems;
	CItemDrop::Drop(pChestConf->ChestDropId, dropItems);
	//增加物品
	bool bAdd = CPropUseHelper::getInstance()->AddItems(uid, dropItems, true, PVPBOX_GAIN_REASON);
	if (bAdd)
	{
		for (unsigned int i = 0; i < dropItems.size(); i++)
		{
			if (dropItems[i].id == RESOURCEID_DIAMOND)
			{
				CCommOssHelper::userDiamondGetOss(uid, dropItems[i].num, PVP_DIAMOND_GET_REASON);
			}
		}
	}
	
	int nSendLen = sizeof(PvpChestOpenSC) + dropItems.size() * sizeof(DropItemInfo);
	char *pSendData = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(nSendLen)); 
	
	PvpChestOpenSC *pOpenSC = reinterpret_cast<PvpChestOpenSC*>(pSendData);
	pOpenSC->chestId = newChestId;
	pOpenSC->rewardCount = dropItems.size();

	DropItemInfo *pPropInfo = reinterpret_cast<DropItemInfo *>(pOpenSC + 1);
	for (std::vector<DropItemInfo>::iterator iter = dropItems.begin();
		iter != dropItems.end(); ++iter)
	{
		memcpy(pPropInfo, &(*iter), sizeof(DropItemInfo));
		pPropInfo += 1;
	}

    //设置宝箱状态
    if (!pPvpModel->SetPvpField(PVPCOMM_FD_CHESTSTATUS, 0))
    {
        KXLOGERROR("%d set pvp field:PVPCOMM_FD_CHESTSTATUS error when open pvp chest!", uid);
        return;
    }

    int nLastChestGenTime = 0;
    pPvpModel->GetPvpField(PVPCOMM_FD_LASTCHESTGENTIME, nLastChestGenTime);
    if (nLastChestGenTime == 0)
    {
        if (!pPvpModel->SetPvpField(PVPCOMM_FD_LASTCHESTGENTIME, (int)time(NULL)))
        {
            KXLOGERROR("%d set pvp field:PVPCOMM_FD_LASTCHESTGENTIME2 error when open pvp chest!", uid);
            return;
        }
    }

	CKxCommManager::getInstance()->sendData(uid, 
		CMD_PVPCHEST, CMD_PVPCHEST_OPEN_SC, pSendData, nSendLen);
	//内存回收
	KxServer::kxMemMgrRecycle(pSendData, nSendLen);
}

void CPvpChestService::processOpenChestAtOnce(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{

}
