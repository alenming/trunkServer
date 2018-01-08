#include "ShopService.h"
#include "ShopModel.h"
#include "GameUser.h"
#include "GameUserManager.h"
#include "ConfHall.h"
#include "GameDef.h"
#include "ShopProtocol.h"
#include "Protocol.h"
#include "ItemDrop.h"
#include "ConfGameSetting.h"
#include "KxMemPool.h"
#include "KxCommManager.h"
#include "ModelHelper.h"
#include "CommOssHelper.h"
#include <algorithm>
#include "ErrorCodeProtocol.h"
#include "PropUseHelper.h"
#include "UnionShopModel.h"

using namespace KxServer;

void CShopService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
		case CMD_SHOP_BUY_CS:
		{
			ProcessShopGoodsBuy(uid, buffer, len, commun);
		}
		break;
		case CMD_SHOP_REFRESH_CS:
		{
			ProcessShopFresh(uid, buffer, len, commun);
		}
		break;
		case CMD_UNIONSHOP_BUY_CS:
		{
			 processUnionShopBuy(uid, buffer, len, commun);
		}
		default:
		break;
	}
}

//商店物品购买
void CShopService::ProcessShopGoodsBuy(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_DEBUG_VOID(buffer != NULL && len == sizeof(ShopBuyCS));
    ShopBuyCS *pMsg = (ShopBuyCS *)buffer;
    int nShopNum = 0;
    int nShopGoodsID = 0;

    // 获取
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN_DEBUG_VOID(NULL != pGameUser);
    CShopModel *pShopModel = dynamic_cast<CShopModel*>(pGameUser->getModel(MODELTYPE_SHOP));
    CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
    CHECK_RETURN_DEBUG_VOID(NULL != pShopModel && NULL != pUserModel);

    // 获取商品列表中指定下标的物品信息
    CHECK_RETURN_DEBUG_VOID(pShopModel->GetShopGoodsInfo(pMsg->cShopID, pMsg->cIndex, nShopGoodsID, nShopNum));
    // 要买的物品数量大于0且小于等于商品的库存
    CHECK_RETURN_VOID(pMsg->itemNum > 0 && nShopNum >= pMsg->itemNum);
    // 获取这个商品详情
    const ShopGoodsConfigData *pGoodsData = queryConfShopData(nShopGoodsID);
    CHECK_RETURN_DEBUG_VOID(NULL != pGoodsData);

    int nField = 0;
    int nMoney = 0;
    ErrorCodeData CodeData; //如果异常 发送错误码
    int nPayFor = 0;		//记录日志变量
    int nTokenType = 0;
    switch (pGoodsData->nCoinType)
    {
    case BUY_GOLD_TYPE:					//金币购买类型
        nField = USR_FD_GOLD;
        CodeData.nCode = ERROR_GOLD_NOTENOUGH;
        nTokenType = GOLD_RESOURCE_TYPE;
        break;
    case BUY_DIAMOND_TYPE:					//钻石币
        nField = USR_FD_DIAMOND;
        CodeData.nCode = ERROR_DIAMOND_NOTENOUGH;
        break;
    default:
        return;
        break;
    }

    int nVipLv = 0;
    CHECK_RETURN_DEBUG_VOID(pUserModel->GetUserFieldVal(nField, nMoney));
	CShopData *conf = dynamic_cast<CShopData *>(CConfManager::getInstance()->getConf(CONF_SHOP));
	CHECK_RETURN_DEBUG_VOID(conf != NULL);
	ShopConfigData *pShopData = static_cast<ShopConfigData *>(conf->getData(pMsg->cShopID));
	CHECK_RETURN_DEBUG_VOID(pShopData);
	//金币商店才有VIP折扣
	VecInt::iterator iter = find(pShopData->VectType.begin(), pShopData->VectType.end(), BUY_GOLD_TYPE);
	//金钱不足(vip优惠折上折)
	int nCost = 0;
	int nSingleCost = 0;
	nSingleCost = static_cast<int>(pGoodsData->nCoinNum * pGoodsData->nSale / 100.0f);
	nCost = nSingleCost * pMsg->itemNum;
	if (nMoney < nCost)
	{
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return;
	}

	//添加物品
	DropItemInfo Info;
	bool bSendEmail = false;
	Info.id = pGoodsData->nGoodsID;
	Info.num = pMsg->itemNum;
	CHECK_RETURN_DEBUG_VOID(CPropUseHelper::getInstance()->AddItems(uid, Info, bSendEmail, SHOP_GAIN_REASON));

	//设置数据到模型中
	nShopNum = nShopNum - pMsg->itemNum;
	pShopModel->SetShopNum(pMsg->cShopID, pMsg->cIndex, nShopNum);
	switch (pGoodsData->nCoinType)
	{
		case BUY_GOLD_TYPE:					//金币购买类型
		{
			CModelHelper::addGold(uid, -1 * nCost);
		}
		break;
		case BUY_DIAMOND_TYPE:					//钻石币
		{
			CModelHelper::addDiamond(uid, -1 * nCost);
		}
		break;
		default:
			return;
			break;
	}

	//发送消息
	ShopBuySC BuySC;
	BuySC.cIndex = pMsg->cIndex;
	BuySC.nNum = pMsg->itemNum;
	CKxCommManager::getInstance()->sendData(uid, CMD_SHOP, CMD_SHOP_BUY_SC,
		reinterpret_cast<char *>(&BuySC), sizeof(BuySC));
	ActionCommonInfo CommInfo;
	CommInfo.param1 = pMsg->cShopID;
	CommInfo.param2 = pMsg->itemNum;
	// 购物任务
	CModelHelper::DispatchActionEvent(uid, ELA_BUY_ITEM_COUNT, &CommInfo, sizeof(CommInfo));

	//日志记录
	const PropItem *pPropConf = queryConfProp(pGoodsData->nGoodsID);
	CHECK_RETURN_DEBUG_VOID(NULL != pPropConf)
	switch (pPropConf->Type)
	{
		case PROPTYPE_EQUIP:
		case PROPTYPE_EQUIP_CREATE:
		{
			if (nTokenType != 0)
			{
				nPayFor = EQUIP_SHOP_PAYFOR_TYPE;
			}
			else
			{
				nPayFor = DIAMONDBUY_EQUIP_TYPE;
			}
		}
		break;
		case PROPTYPE_HEROCARD:		//英雄卡片
		{
			if (nTokenType != 0)
			{
				nPayFor = HERO_SHOP_PAYFOR_TYPE;
			}
			else
			{
				nPayFor = DIAMONDBUY_HERO_TYPE;
			}						
		}
		break;
		case PROPTYPE_SUMMONERCARD:	//召唤师卡片
		{
			if (nTokenType != 0)
			{
				nPayFor = SUMMER_SHOP_PAYFOR_TYPE;
			}
			else
			{
				nPayFor = DIAMONDBUY_SUMMER_TYPE;
			}				
		}
		break;
		case PROPTYPE_RESOURCE:
		//商店不存在资源类型购买
		return;
		break;
		default:
		{
			if (nTokenType != 0)
			{
				nPayFor = GOODS_SHOP_PAYFOR_TYPE;
			}
			else
			{
				nPayFor = DIAMONDBUY_GOODS_TYPE;
			}
		}
		break;
	}

	if (nTokenType == 0)
	{
		CCommOssHelper::userDiamondPayOss(uid, nCost, nPayFor, pMsg->itemNum, pGoodsData->nGoodsID);
	}
	else
	{
		CCommOssHelper::userTokenShopOss(uid, nTokenType, nCost, nPayFor, pMsg->itemNum, pGoodsData->nGoodsID);
	}
}

//处理公会商店购买
void CShopService::processUnionShopBuy(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CHECK_RETURN_DEBUG_VOID(buffer != NULL && len == sizeof(SUnionShopBuyCS));
	SUnionShopBuyCS *pMsg = (SUnionShopBuyCS *)buffer;
	CHECK_RETURN_DEBUG_VOID(pMsg->cGoodsNum > 0);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN_DEBUG_VOID(NULL != pGameUser);
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
	CHECK_RETURN_DEBUG_VOID(pUserModel != NULL && pUserUnionModel != NULL);

	int nUnionID = 0;
	CHECK_RETURN_DEBUG_VOID(pUserUnionModel->getUserUnionValue(USER_UNION_ID, nUnionID));
	CUnionShopModel *pUnionShopModel = new CUnionShopModel();
	pUnionShopModel->init(nUnionID);
	CUnionModel *pUnionModel = new CUnionModel();
	pUnionModel->init(nUnionID);
	int nShopNum = 0;
	pUnionShopModel->getUnionShopGoodsData(pMsg->sShopGoodsID, nShopNum);
	CHECK_RETURN_DEBUG_VOID(nShopNum >= pMsg->cGoodsNum);
	int nNum = -1 * static_cast<int>(pMsg->cGoodsNum);
	if (!pUnionShopModel->incrUnionShopGoodsNum(pMsg->sShopGoodsID,nNum))
	{
		delete pUnionShopModel;
		delete pUnionModel;
		return;
	}

	const ShopGoodsConfigData *pGoodsData = queryConfShopData(pMsg->sShopGoodsID);
	CHECK_RETURN_DEBUG_VOID(pGoodsData != NULL);
	int nMoney = 0;
	CHECK_RETURN_DEBUG_VOID(pUserModel->GetUserFieldVal(USR_FD_UNIONCONTRIB, nMoney));

	//金钱不足(vip优惠折上折)
	int nCost = 0;
	int nSingleCost = 0;
	nSingleCost = static_cast<int>(pGoodsData->nCoinNum * pGoodsData->nSale / 100.0f);
	nCost = nSingleCost * pMsg->cGoodsNum;
	if (nMoney < nCost)
	{
		ErrorCodeData CodeData; //如果异常 发送错误码
		CodeData.nCode = ERROR_UNIONCONTRIB_NOTENOUGH;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		delete pUnionShopModel;
		delete pUnionModel;
		return;
	}

	//添加物品
	DropItemInfo Info;
	bool bSendEmail = false;
	Info.id = pGoodsData->nGoodsID;
	Info.num = static_cast<int>(pMsg->cGoodsNum);
	std::vector<DropItemInfo> items;
	items.push_back(Info);
	CHECK_RETURN_DEBUG_VOID(CPropUseHelper::getInstance()->AddItems(uid, Info, bSendEmail, SHOP_GAIN_REASON));
	CModelHelper::addGuildContrib(uid, -1*nCost);
	//发送消息
	SUnionShopBuySC BuySC;
	BuySC.sShopGoodsID = pMsg->sShopGoodsID;
	BuySC.cGoodsNum = pMsg->cGoodsNum;
	BuySC.nBuyerId = uid;
	std::map<int, int> MemberMap = pUnionModel->GetMembers();
	for (std::map<int, int>::iterator ator = MemberMap.begin(); ator != MemberMap.end(); ++ator)
	{
		CKxCommManager::getInstance()->sendData(ator->first, CMD_SHOP, CMD_UNIONSHOP_BUY_SC,
			reinterpret_cast<char *>(&BuySC), sizeof(BuySC));
	}
	
	ActionCommonInfo CommInfo;
	CommInfo.param1 = BUY_UNION_TYPE;
	CommInfo.param2 = static_cast<int>(pMsg->cGoodsNum);
	// 购物任务
	CModelHelper::DispatchActionEvent(uid, ELA_BUY_ITEM_COUNT, &CommInfo, sizeof(CommInfo));
	//日志记录
	const PropItem *pPropConf = queryConfProp(pGoodsData->nGoodsID);
	CHECK_RETURN_DEBUG_VOID(NULL != pPropConf);
	int nPayFor = 0;
	switch (pPropConf->Type)
	{
		case PROPTYPE_EQUIP:
		case PROPTYPE_EQUIP_CREATE:
		{
			nPayFor = EQUIP_SHOP_PAYFOR_TYPE;
		}
		break;
		case PROPTYPE_HEROCARD:		//英雄卡片
		{
			nPayFor = HERO_SHOP_PAYFOR_TYPE;
		}
		break;
		case PROPTYPE_SUMMONERCARD:	//召唤师卡片
		{
			nPayFor = SUMMER_SHOP_PAYFOR_TYPE;
		}
		break;
		case PROPTYPE_RESOURCE:
		//商店不存在资源类型购买
		delete pUnionShopModel;
		delete pUnionModel;
		return;
		break;
		default:
		{
			nPayFor = GOODS_SHOP_PAYFOR_TYPE;
		}
		break;
	}

	CCommOssHelper::userTokenShopOss(uid,UNION_RESOURCE_TYPE,nCost, nPayFor, static_cast<int>(pMsg->cGoodsNum), pGoodsData->nGoodsID);
	delete pUnionShopModel;
	delete pUnionModel;
}

//处理商店刷新
void CShopService::ProcessShopFresh(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CHECK_RETURN_DEBUG_VOID(buffer != NULL && len == sizeof(ShopRefreshCS));
	ShopRefreshCS *pMsg = (ShopRefreshCS *)buffer;
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN_DEBUG_VOID(NULL != pGameUser);
	CShopModel *pShopModel = dynamic_cast<CShopModel*>(pGameUser->getModel(MODELTYPE_SHOP));
    CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CHECK_RETURN_DEBUG_VOID(NULL != pShopModel && NULL != pUserModel);

	// 校验商店刷新时间
	CModelHelper::checkShopFreshTimes(uid);

    int nUserLv = 0;
	int nTimes = 0;
	int nFreshEndTimes = 0;
	int nCurTimes = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
	CHECK_RETURN_DEBUG_VOID(pShopModel->GetShopInfo(pMsg->cShopID, nTimes, nFreshEndTimes));
	CShopData *conf = dynamic_cast<CShopData *>(CConfManager::getInstance()->getConf(CONF_SHOP));
	CHECK_RETURN_DEBUG_VOID(conf != NULL);
	ShopConfigData *pData = static_cast<ShopConfigData *>(conf->getData(pMsg->cShopID));
	CHECK_RETURN_DEBUG_VOID(pData != NULL);
	CHECK_RETURN_DEBUG_VOID(pData->nTimeInterval != 0);
	pUserModel->GetUserFieldVal(USR_FD_USERLV, nUserLv);

	if (pMsg->cShopType == EAUTOFRESH_TYPE)
	{
		// 时间没到
		CHECK_RETURN_DEBUG_VOID(nFreshEndTimes <= nCurTimes);
        // 刷新间隔时间
        int freshDt = (pData->nTimeInterval * 60);
        // 下次刷新时间
        nFreshEndTimes = nCurTimes + (freshDt - ((nCurTimes - nFreshEndTimes) % freshDt));
		ShopGoodsMap GoodsMap;
        CHECK_RETURN_DEBUG_VOID(CModelHelper::FreshShop(uid, pMsg->cShopID, nUserLv, GoodsMap));
		pShopModel->AddShopData(pMsg->cShopID, GoodsMap);
		pShopModel->SetShopInfo(pMsg->cShopID, nTimes, nFreshEndTimes);
	}
	else
	{
		nTimes = nTimes + 1;
		int nCurDamand = 0;
		CHECK_RETURN_DEBUG_VOID(pUserModel->GetUserFieldVal(USR_FD_DIAMOND, nCurDamand));
		const IncreasePayItem *pPayItem = queryConfIncreasePay(nTimes);
		if (pPayItem == NULL || pPayItem->FreshShopCost == 0)
		{
			ErrorCodeData CodeData;
			CodeData.nCode = ERROR_SHOPFRESHTIMES_NOTENOUGH;
			CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
			return;
		}
		
		if (pPayItem->FreshShopCost > nCurDamand)
		{
			ErrorCodeData CodeData;
			CodeData.nCode = ERROR_DIAMOND_NOTENOUGH;
			CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
			return;
		}

		ShopGoodsMap GoodsMap;
        CHECK_RETURN_DEBUG_VOID(CModelHelper::FreshShop(uid, pMsg->cShopID, nUserLv, GoodsMap));
		pShopModel->AddShopData(pMsg->cShopID, GoodsMap);
		pShopModel->SetShopInfo(pMsg->cShopID, nTimes, nFreshEndTimes);
		CModelHelper::addDiamond(uid, -1 * pPayItem->FreshShopCost);

		//日志记录
		CCommOssHelper::userDiamondPayOss(uid, pPayItem->FreshShopCost, DIAMONDRESET_SHOP_TYPE, pMsg->cShopID);

		ActionCommonInfo CommInfo;
		CommInfo.param1 = pMsg->cShopID;
		CommInfo.param2 = 1;
		//刷新商店任务
		CModelHelper::DispatchActionEvent(uid, ELA_SHOPFRESHTIMES, &CommInfo, sizeof(CommInfo));
	}

    // 告知玩家刷新结果
	ShopGoodsMap* pGoodsMap = pShopModel->GetShopData(pMsg->cShopID);
	CHECK_RETURN_DEBUG_VOID(NULL != pGoodsMap);

	int nLen = sizeof(ShopRefreshSC)+pGoodsMap->size() * sizeof(SSendShopGoodsData);
	char *pSendData = reinterpret_cast<char *>(kxMemMgrAlocate(nLen));
	ShopRefreshSC *pRefreshSC = (ShopRefreshSC *)pSendData;
	pRefreshSC->cCurCount = 0;
	pRefreshSC->nNextFreshTime = nFreshEndTimes;
	pRefreshSC->cShopID = pMsg->cShopID;
	pRefreshSC->nTimes = nTimes;
	pRefreshSC->cType = pMsg->cShopType;
	SSendShopGoodsData *pGoodsData = (SSendShopGoodsData *)(pRefreshSC+1);
	for (ShopGoodsMap::iterator ator = pGoodsMap->begin(); ator != pGoodsMap->end(); ++ator)
	{
		SShopGoods &CurGoods = ator->second;
		const ShopGoodsConfigData *pTemplateGoods = queryConfShopData(CurGoods.nShopGoodsID);
		CHECK_CONTINUE(NULL != pTemplateGoods);
		pGoodsData->sGoodsShopID = CurGoods.nShopGoodsID;
		pGoodsData->nGoodsNum = CurGoods.nGoodsNum;
		pGoodsData->nCoinNum = pTemplateGoods->nCoinNum;
		pGoodsData->cCoinType = pTemplateGoods->nCoinType;
		pGoodsData->nGoodsID = pTemplateGoods->nGoodsID;
		pGoodsData->cIndex = ator->first;
		pGoodsData->cSale = pTemplateGoods->nSale;
        ++pRefreshSC->cCurCount;
        ++pGoodsData;
	}
	
	CKxCommManager::getInstance()->sendData(uid, CMD_SHOP, CMD_SHOP_REFRESH_SC, pSendData, nLen);
	kxMemMgrRecycle(pSendData, nLen);
}
