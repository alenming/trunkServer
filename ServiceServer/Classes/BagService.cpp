#include "BagService.h"
#include "Protocol.h"
#include "BagProtocol.h"
#include "GameUserManager.h"
#include "ServiceDef.h"
#include "GameDef.h"

#include "ModelHelper.h"
#include "PropUseHelper.h"
#include "ConfGameSetting.h"
#include "CommOssHelper.h"
#include "ErrorCodeProtocol.h"
#include "KxCommManager.h"
#include "MemoryOperation.h"
#include "NoticeHelper.h"
#include "NoticeProtocol.h"

using namespace std;
const static int UnlockBagBox = 5;

void CBagService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
	case CMD_BAG_SALE_CS:
		processSaleService(uid, buffer, len, commun);
		break;
	case CMD_BAG_UNLOCK_CS:
		processUnlockService(uid, buffer, len, commun);
		break;
    case CMD_BAG_USE_CS:
        processUseService(uid, buffer, len, commun);
        break;
	case CMD_BAG_MAKEEQUIPMENT_CS:
		processMakeEquipmentService(uid, buffer, len, commun);
		break;
	case CMD_BAG_DECOMPOSEEQUIPMENT_CS:
		processDecomposeEquipmentService(uid, buffer, len, commun);
		break;
	default:
		break;
	}
}

void CBagService::processUnlockService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(len == sizeof(BagUnlockCS));
    BagUnlockCS *pUnlockCS = reinterpret_cast<BagUnlockCS*>(buffer);
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN_VOID(pGameUser != NULL);
    CBagModel *pBagModel = dynamic_cast<CBagModel*>(pGameUser->getModel(MODELTYPE_BAG));
    CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
    CHECK_RETURN_VOID(NULL != pBagModel && NULL != pUserModel);

    int haveDiamond = 0;
    int needDiamond = 0;
    int bagCapacity = 0;

    //判断是否最大容量
    CHECK_RETURN_VOID(pUserModel->GetUserFieldVal(USR_FD_BAGCAPACITY, bagCapacity));

    //计算目前该解锁第几行, 每行5个格
    int line = bagCapacity / UnlockBagBox;
    int addCapacity = 0;
    for (int i = 0; i < pUnlockCS->bagLine; ++i)
    {
        ++line;
        const IncreasePayItem *pPayConf = queryConfIncreasePay(line);
        if (pPayConf == NULL || pPayConf->BagCost < 0)
        {
            //次数超过最大次数
            ErrorCodeData CodeData;
            CodeData.nCode = ERROR_BAG_MAX_BAGNUM;
            CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
            return;
        }

        needDiamond += pPayConf->BagCost;
        addCapacity += UnlockBagBox;
    }
    //钻石是否满足 
    CHECK_RETURN_DEBUG_VOID(pUserModel->GetUserFieldVal(USR_FD_DIAMOND, haveDiamond));

    if (haveDiamond < needDiamond)
    {
        ErrorCodeData CodeData;
        CodeData.nCode = ERROR_DIAMOND_NOTENOUGH;
        CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
    }

    CHECK_RETURN_VOID(haveDiamond >= needDiamond);
    //增加背包容量
    if (!CModelHelper::addBagCapacity(uid, addCapacity))
    {
        KXLOGDEBUG("processUnlockService uid %d addBagCapacity failed!");
    }
    //消耗钻石
    if (!CModelHelper::addDiamond(uid, -1 * needDiamond))
    {
        KXLOGDEBUG("processUnlockService uid %d cost diamond = %d failed!", uid, needDiamond);
    }

    BagUnlockSC unlockSC;
    unlockSC.diamond = needDiamond;
    unlockSC.bagLine = pUnlockCS->bagLine;

    CKxCommManager::getInstance()->sendData(uid, CMD_BAG, CMD_BAG_UNLOCK_SC,
        reinterpret_cast<char *>(&unlockSC), sizeof(unlockSC));
    CCommOssHelper::userDiamondPayOss(uid, needDiamond, 0, addCapacity);
}

void CBagService::processSaleService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(len == sizeof(BagSaleCS));
    BagSaleCS *pSaleCS = reinterpret_cast<BagSaleCS*>(buffer);
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN_VOID(pGameUser != NULL);

    CBagModel *pBagModel = dynamic_cast<CBagModel*>(pGameUser->getModel(MODELTYPE_BAG));
    CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
    CHECK_RETURN_VOID(pBagModel != NULL);
    CHECK_RETURN_VOID(pUserModel != NULL);

    int itemConfId = 0, itemVal = 0, haveCount = 0, saleCount = 0, gold = 0;

    if (!pBagModel->GetItem(pSaleCS->saleId, itemVal))
    {
        ErrorCodeData CodeData;
        CodeData.nCode = ERROR_BAG_ITEMNOTEXIST;
        CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
        return;
    }

    if (PROPTYPE_EQUIP == CModelHelper::getItemType(pSaleCS->saleId))
    {
        itemConfId = itemVal;
        saleCount = 1;
        haveCount = 1;
    }
    else
    {
        itemConfId = pSaleCS->saleId;
        haveCount = itemVal;
        saleCount = pSaleCS->saleCount;
    }

    const PropItem *pItemConf = queryConfProp(itemConfId);
    if (NULL == pItemConf)
    {
        KXLOGERROR("processSaleService uid %d itemConfId %d is NULL", uid, itemConfId);
        return;
    }

    if (pItemConf->SellPrice < 0 || saleCount <= 0 || haveCount < saleCount)
    {
        // 出售个数为0或拥有个数少与出售个数
        // 不可出售
        KXLOGDEBUG("%s pItemConf->SellPrice < 0 || saleCount <= 0 || haveCount < saleCount", __FUNCTION__);
        return;
    }

    gold = pItemConf->SellPrice * saleCount;
    // 保护玩家权益,先加钱
    if (!CModelHelper::addGold(uid, gold))
    {
        KXLOGDEBUG("processSaleService uid %d, addGold count=%d failed!", uid, gold);
    }
    // 移除物品
    if (!CModelHelper::removeItem(uid, pSaleCS->saleId, pSaleCS->saleCount))
    {
        KXLOGDEBUG("processSaleService uid %d, removeItem id = %d failed!", uid, pSaleCS->saleId);
    }

    BagSaleSC bagSaleSC;
    bagSaleSC.gold = gold;
    bagSaleSC.saleId = pSaleCS->saleId;
    bagSaleSC.saleCount = pSaleCS->saleCount;

    CKxCommManager::getInstance()->sendData(uid, CMD_BAG, CMD_BAG_SALE_SC,
        reinterpret_cast<char *>(&bagSaleSC), sizeof(bagSaleSC));
}

void CBagService::processUseService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CHECK_RETURN_VOID(len >= sizeof(BagUseCS));
	BagUseCS *pBagUseCS = reinterpret_cast<BagUseCS*>(buffer);
	const PropItem* pPropItem = queryConfProp(pBagUseCS->id);
    if (NULL == pPropItem)
    {
        KXLOGERROR("processUseService uid %d pPropItemId %d is NULL", uid, pBagUseCS->id);
        return;
    }
    int propId = pBagUseCS->id;
    int count = pBagUseCS->val;
    int haveCount = 0;
    if (count < 1)
    {
        KXLOGERROR("processUseService uid %d count %d", uid, count);
        return;
    }
    
	CBagModel *pBagModel = dynamic_cast<CBagModel*>(CModelHelper::getModel(uid, MODELTYPE_BAG));
	CHECK_RETURN_DEBUG_VOID(pBagModel != NULL);
	if (!pBagModel->GetItem(propId, haveCount) || haveCount < count || count > 99)
	{
		return;
	}

	std::vector<DropItemInfo> items;
	if (CPropUseHelper::getInstance()->bagUseProp(uid, pBagUseCS->id, pBagUseCS->val, items))
	{
		//内存空间计算
		int nSendLen = sizeof(BagSaleSC) + items.size() * sizeof(DropItemInfo);
		char *pSendData = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(nSendLen));

		BagUseSC* pBagUseSC = reinterpret_cast<BagUseSC*>(pSendData);
		pBagUseSC->count = items.size();

		DropItemInfo *pPropsInfo = reinterpret_cast<DropItemInfo*>(pBagUseSC+1);
		for (std::vector<DropItemInfo>::iterator iter = items.begin();
			iter != items.end(); ++iter)
		{
			//if (iter->id == )
			memcpy(pPropsInfo, &(*iter), sizeof(DropItemInfo));
			pPropsInfo++;
		}

		CKxCommManager::getInstance()->sendData(uid, CMD_BAG, CMD_BAG_USE_SC,
			pSendData, nSendLen);

		KxServer::kxMemMgrRecycle(pSendData, nSendLen);
		CModelHelper::removeItem(uid, propId, count);
	}
}

inline int getQualityIndex(int randNum, int randArray[], int arraySize)
{
    for (int i = 0; i < arraySize; ++i)
    {
        if (randNum < randArray[i])
        {
            return i;
        }
    }
    return 0;
}

void CBagService::processMakeEquipmentService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(len == sizeof(MakeEquipmentCS));
    CBagModel *pBagModel = dynamic_cast<CBagModel*>(CModelHelper::getModel(uid, MODELTYPE_BAG));
    CHECK_RETURN_VOID(pBagModel);
    CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
    CHECK_RETURN_VOID(pBagModel);
    MakeEquipmentCS* pMakeEquipCS = reinterpret_cast<MakeEquipmentCS*>(buffer);

    // 配置判断
    CConfEquipmentForCast *castconf = dynamic_cast<CConfEquipmentForCast *>(
        CConfManager::getInstance()->getConf(CONF_EQUIP_CAST));
    CHECK_RETURN_VOID(castconf);
    SEquipForCast *cast = reinterpret_cast<SEquipForCast *>(castconf->getData(pMakeEquipCS->row));
    CHECK_RETURN_VOID(cast);
    if (NULL == cast || pMakeEquipCS->eqLevel != cast->nLevel
        || pMakeEquipCS->eqPart != cast->nPart || pMakeEquipCS->eqVocation != cast->nVocation)
    {
        KXLOGERROR("processMakeEquipmentService Error part %d level %d vocation %d",
        pMakeEquipCS->eqPart, pMakeEquipCS->eqLevel, pMakeEquipCS->eqVocation);
        return;
    }

    if (CModelHelper::getBagStoreCapacity(uid) < 1)
    {
        ErrorCodeData CodeData;
        CodeData.nCode = ERROR_BAG_ISNOTENOUGH;
        CKxCommManager::getInstance()->sendData(uid,
            CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
        return;
    }

    do 
    {
        // 判断打造条件是否满足
        int itemIndex = MAX_EQUIP_ITEM_INDEX;
        int equipCreateIndex = 0;
        int needGold = 0;
        int percent = rand() % 100;
        if (!pMakeEquipCS->isDelicate)
        {
            // 如果不是精品打造，不需要最后一个物品
            itemIndex -= 1;
            needGold = cast->nGoldSpend[0];
            equipCreateIndex = getQualityIndex(percent, cast->nNormalMakePercent,
                sizeof(cast->nNormalMakePercent) / sizeof(int));
        }
        else
        {
            needGold = cast->nGoldSpend[1];
            equipCreateIndex = getQualityIndex(percent, cast->nDelicateMakePercent,
                sizeof(cast->nDelicateMakePercent) / sizeof(int));
        }

        bool isEnough = true;
        for (int i = 0; i < itemIndex; ++i)
        {
            if (0 != cast->nSynthesisItemID[i])
            {
                int nCount = 0;
                if (!pBagModel->GetItem(cast->nSynthesisItemID[i], nCount)
                    || nCount < cast->nSynthesisItemCount[i])
                {
                    isEnough = false;
                    break;
                }
            }
        }
        if (!isEnough)
        {
            break;
        }

        int nUsrGold = 0;
        pUserModel->GetUserFieldVal(USR_FD_GOLD, nUsrGold);
        if (nUsrGold < needGold)
        {
            break;
        }

        // 所有检查都通过了，钱够，材料够，背包够，扣物品加装备
        SEquipInfo equipData;
        CHECK_RETURN_VOID(CModelHelper::addEquip(uid, cast->nEquipmentCreateID[equipCreateIndex], equipData));
        for (int i = 0; i < itemIndex; i++)
        {
            if (0 != cast->nSynthesisItemID[i])
            {
                CModelHelper::removeItem(uid, cast->nSynthesisItemID[i], cast->nSynthesisItemCount[i]);
            }
        }
        CModelHelper::addGold(uid, -1 * needGold);

        // 回复给玩家
        CKxCommManager::getInstance()->sendData(uid, CMD_BAG, CMD_BAG_EQUIP_MAKE_SC,
            reinterpret_cast<char*>(&equipData), sizeof(equipData), commun);

        const PropItem *pPropConf = queryConfProp(equipData.nItemID);
        CHECK_RETURN_DEBUG_VOID(pPropConf != NULL);
        ActionCommonInfo CommInfo;
        CommInfo.param1 = pPropConf->Quality;
        CommInfo.param2 = 1;
        //装备打造任务
        CModelHelper::DispatchActionEvent(uid, ELA_EQUIPBUILD, &CommInfo, sizeof(CommInfo));

        //通知全服打造装备
        if (EITEM_QUALITY_PURPLE <= pPropConf->Quality)
        {
            int len = sizeof(NoticeSC)+CNoticeHelper::getStringIntInfoLen(pUserModel->GetName().length() + 1);
            char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(len));
            NoticeSC *pNoticeSC = reinterpret_cast<NoticeSC*>(buff);
            pNoticeSC->noticeCount = 1;
            CNoticeHelper::getStringIntNoticeInfo(buff + sizeof(NoticeSC),
                EITEM_QUALITY_GOLD == pPropConf->Quality ? NOTICE_USER_FORGE_GOLD : NOTICE_USER_FORGE_PURPLE,
                pUserModel->GetName(), equipData.nItemID);
            CNoticeHelper::noticeAll(uid, buff, len, commun);
            KxServer::kxMemMgrRecycle(buff, len);
        }
        return;
    } while (false);

    ErrorCodeData CodeData;
    CodeData.nCode = ERROR_BAG_GOODSNOTENOUGH;
    CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
}

void CBagService::processDecomposeEquipmentService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(len >= sizeof(DecomposeEquimentCS));
    DecomposeEquimentCS* decomposeCS = reinterpret_cast<DecomposeEquimentCS*>(buffer);
    if (decomposeCS->equipCount <= 0 || decomposeCS->equipCount > 10)
    {
        return;
    }
    CHECK_RETURN_VOID(len == decomposeCS->equipCount * sizeof(EquipInfo) + sizeof(DecomposeEquimentCS));
    CEquipModel* equipModel = dynamic_cast<CEquipModel*>(CModelHelper::getModel(uid, MODELTYPE_EQUIP));
    CHECK_RETURN_VOID(equipModel != NULL);
    CUserModel *userModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
    CHECK_RETURN_VOID(userModel != NULL);

    std::map<int, int> mapWillGetItems;
    std::map<int, int> decomposeEquips;
    std::map<int, SEquipInfo>& equips = equipModel->GetEquipsInfo();
    CConfEquipment* pConfEquipment = dynamic_cast<CConfEquipment *>(
        CConfManager::getInstance()->getConf(CONF_EQUIPMENT));
    CHECK_RETURN_VOID(pConfEquipment != NULL);

    EquipInfo* equipInfo = reinterpret_cast<EquipInfo*>(buffer + sizeof(DecomposeEquimentCS));
    int needGold = 0;
    for (int i = 0; i < decomposeCS->equipCount; ++i)
    {
        // 在配置和装备模型中检查，无此装备
        EquipmentItem* stEquipment = static_cast<EquipmentItem *>(pConfEquipment->getData(
            equipInfo->equipConfID));
        if (stEquipment == NULL)
        {
            return;
        }
        if (equips.find(equipInfo->equipID) == equips.end())
        {
            return;
        }
        // 统计要分解的合法装备
        decomposeEquips[equipInfo->equipID] = equipInfo->equipConfID;
        // 统计所需的金币、获得的物品
        needGold += stEquipment->Gold;
        for (std::vector<DecompositMaterial>::iterator vecit = stEquipment->Decomposit.begin();
            vecit != stEquipment->Decomposit.end(); ++vecit)
        {
            mapWillGetItems[(*vecit).Decomposit] += (*vecit).DecompositionParam;
        }
        ++equipInfo;
    }

    // 钱不够
    int money = 0;
    if (!userModel->GetUserFieldVal(USR_FD_GOLD, money)
        || money < needGold)
    {
        return;
    }

    // 删除装备
    for (map<int, int>::iterator iter = decomposeEquips.begin();
        iter != decomposeEquips.end(); ++iter)
    {
        if (!CModelHelper::removeItem(uid, iter->first, 1))
        {
            KXLOGDEBUG("processDecomposeEquipmentService uid %d remove %d faile", uid, iter->first);
        }
    }

    // 获得材料并扣钱
    if (!CModelHelper::addGold(uid, -1 * needGold))
    {
        KXLOGDEBUG("processDecomposeEquipmentService uid %d addGold %d faile", uid, -1 * needGold);
    }

    int count = mapWillGetItems.size();
    CBufferData scBuffer;
    scBuffer.init(128);
    scBuffer.writeData(needGold);
    scBuffer.writeData(count);
    // 添加物品
    std::vector<DropItemInfo> vecDropItem;
    for (map<int, int>::iterator iter = mapWillGetItems.begin();
        iter != mapWillGetItems.end(); ++iter)
    {
        DropItemInfo stDropItem = {};
        stDropItem.id = iter->first;
        stDropItem.num = iter->second;
        vecDropItem.push_back(stDropItem);
        scBuffer.writeData(iter->first);
        scBuffer.writeData(iter->second);
    }

    if (!CPropUseHelper::getInstance()->AddItems(uid, vecDropItem, true, MAKE_GAIN_REASON))
    {
        KXLOGERROR("processDecomposeEquipmentService %d addItems faile", uid);
    }

	CKxCommManager::getInstance()->sendData(uid,
        CMD_BAG, CMD_BAG_EQUIP_DECOMPOSE_SC, scBuffer.getBuffer(), scBuffer.getDataLength(), commun);
}