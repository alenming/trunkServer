#include "UserService.h"
#include "UserProtocol.h"
#include "GameUserManager.h"
#include "ServiceDef.h"
#include "ConfGameSetting.h"
#include "TimeCalcTool.h"
#include "ModelHelper.h"
#include "CommOssHelper.h"
#include "ErrorCodeProtocol.h"
#include "KxCommManager.h"
#include "GameDef.h"
#include "PropUseHelper.h"
#include "KXServer.h"
#include "GiftModel.h"
#include "Protocol.h"
#include "KXServer.h"
#include "CommOssHelper.h"
#include "UnionProtocol.h"

using namespace KxServer;

#define RENAME_DIAMOND  10          // 改名字费用 10钻石
#define MONTH_CARD_COST 25          // 月卡25人民币
#define MONTHSECOND 2505600         // 29天秒数

void CUserService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
	case CMD_USER_BUY_CS:
		processBuyService(uid, buffer, len, commun);
		break;
	case CMD_USER_RENAME_CS:
		processRenameService(uid, buffer, len, commun);
		break;
    case CMD_USER_HEADICONCHANGE_CS:
        processHeadIconChange(uid, buffer, len, commun);
        break;
	case CMD_USER_SIGN_CS:
		processUserSignService(uid, buffer, len, commun);
		break;
	case CMD_USER_GIFTKEY_CS:
		processUserGiftKeyService(uid, buffer, len, commun);
		break;
	case CMD_USER_FIRSTPAY_CS:
		processUserFistPayService(uid, buffer, len, commun);
		break;
	case CMD_USR_FUND_CS:
		//processUserFundService(uid, buffer, len, commun);
		break;
	case CMD_USR_MODIFYPS_CS:
		processUsrPasswordModifyService(uid, buffer, len, commun);
		break;
    case CMD_USER_FORWARD_SS:
        processUserForwardDataService(uid, buffer, len, commun);
        break;
    default:
        break;
	}
}

void CUserService::processBuyService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(len == sizeof(UserBuyCS));

    CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
    CHECK_RETURN_VOID(NULL != pUserModel);

    int nUserLv = 0, nCurDiamond = 0;
    CHECK_RETURN_VOID(pUserModel->GetUserFieldVal(USR_FD_USERLV, nUserLv)
        && pUserModel->GetUserFieldVal(USR_FD_DIAMOND, nCurDiamond));
    
    int nCount = 0, nMultiple = 1, nBuyTimes = 0;
    bool bRlt = false;
    
    UserBuyCS *pBuyCS = reinterpret_cast<UserBuyCS*>(buffer);
    switch (pBuyCS->attrType)
    {
    case ATTR_GOLD:
    {
        CHECK_BREAK(pUserModel->GetUserFieldVal(USR_FD_BUYGOLDTIMES, nBuyTimes));
    
        nBuyTimes += 1;
        const UserLevelSettingItem *pUserLevelConf = queryConfUserLevel(nUserLv);
        const IncreasePayItem *pIncreasePayConf = queryConfIncreasePay(nBuyTimes);
		if (pIncreasePayConf == NULL)
		{
			//次数超过最大次数
			ErrorCodeData CodeData;
			CodeData.nCode = ERROR_ENERGYBUYTIMES_NOTENOUGH;
			CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
			return;
		}
        CHECK_BREAK(NULL != pUserLevelConf);
    
        // 钻石数量消耗是否足够&vip购买次数上限
        if (nCurDiamond >= pIncreasePayConf->GoldCost)
        {
            if (pUserLevelConf->BuyCoin.size() == 2)
            {
				int nTemp = pUserLevelConf->BuyCoin[1] - pUserLevelConf->BuyCoin[0];
                if (nTemp > 0)
                {
                    nCount = rand() % nTemp;
                }
				nCount += pUserLevelConf->BuyCoin[0];
            }
    
			int n = randRateIndex(pIncreasePayConf->GoldProb);
            nMultiple = n == 1 ? 2 : n == 2 ? 5 : n == 3 ? 10 : 1;
            int nTotalCoin = nCount * nMultiple;
            if (!CModelHelper::addDiamond(uid, -1 * pIncreasePayConf->GoldCost)
                || !CModelHelper::addGold(uid, nTotalCoin)
                || !pUserModel->SetUserFieldVal(USR_FD_BUYGOLDTIMES, nBuyTimes))
            {
				KXLOGERROR("uid %d set model error when buy gold!", uid);
            }

			bRlt = true;
            int nTimes = 1;
            CModelHelper::DispatchActionEvent(uid, ELA_BUY_GOLD_TIMES, &nTimes, sizeof(int));
			CCommOssHelper::userDiamondPayOss(uid, pIncreasePayConf->GoldCost, DIAMONDBUY_GOLD_TYPE, nTotalCoin);
        }
    }
    	break;
    case ATTR_DIAMOND:
    	break;
    case ATTR_VIP:
    case ATTR_MONTH_CARD:
    {
        CHECK_BREAK(pBuyCS->extend > 0);
        int nPay = 0;
		int nCardStamp = 0;
		CHECK_BREAK(pUserModel->GetUserFieldVal(USR_FD_MONTHCARDSTAMP, nCardStamp));
		// 月卡过期购买或者续费
		time_t nNow = time(NULL);
		if (nCardStamp < (int)nNow)
		{
			// 加上到0点时间
			int nStamp = CTimeCalcTool::nextTimeStampToZero(nNow, 0);
			nCardStamp = nStamp + MONTHSECOND * pBuyCS->extend;
		}
		else
		{
			nCardStamp += MONTHSECOND * pBuyCS->extend;
		}

		CHECK_BREAK(pUserModel->SetUserFieldVal(USR_FD_MONTHCARDSTAMP, nCardStamp));
		nPay = MONTH_CARD_COST;
		CHECK_BREAK(pUserModel->AddUserFieldVal(USR_FD_PAYMENT, nPay));
		CModelHelper::DispatchActionEvent(uid, ELA_RMVBCOST, &nPay, sizeof(nPay));
        bRlt = true;
    }
        break;
    default:
    	break;
    }

    UserBuySC buySC;
    buySC.result = bRlt ? 1 : 0;
    buySC.attrType = pBuyCS->attrType;
    buySC.addCount = nCount;
    buySC.extend = nMultiple;

	CKxCommManager::getInstance()->sendData(uid, CMD_USER, CMD_USER_BUY_SC,
        reinterpret_cast<char *>(&buySC), sizeof(buySC));
}

void CUserService::processRenameService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(NULL != buffer && NULL != commun && len <= sizeof(UserRenameCS) && len > 0);

    UserRenameCS *pUserRenameCS = reinterpret_cast<UserRenameCS*>(buffer);
    CHECK_RETURN_VOID(0 != strcmp("", pUserRenameCS->userName));

    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN_VOID(NULL != pGameUser);
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
    CHECK_RETURN_VOID(NULL != pUserModel);

    // 是否第一次改名字
    int nFree = -1;
    CHECK_RETURN_VOID(pUserModel->GetUserFieldVal(USR_FD_FREERENAME, nFree));

    int nCostRename = 0;
    if (COST_FREE == nFree)
    {
        CHECK_RETURN_VOID(pUserModel->SetUserFieldVal(USR_FD_FREERENAME, COST_OUTLAY));
    }
    else
    {
        nCostRename = RENAME_DIAMOND;
    }

    std::string newName = pUserRenameCS->userName;
    CHECK_RETURN_VOID(pUserModel->ModUserName(newName));
    if (nCostRename > 0)
    {
        int nCurDiamond = 0;
        CHECK_RETURN_VOID(pUserModel->GetUserFieldVal(USR_FD_DIAMOND, nCurDiamond));
        CHECK_RETURN_VOID(nCurDiamond >= nCostRename && CModelHelper::addDiamond(uid, -1 * nCostRename));
        CCommOssHelper::userDiamondPayOss(uid, nCostRename, DIAMONDRRENAME_TYPE, 0, 0);
    }

    UnionForwardSS unionForwardSS;
    memset(&unionForwardSS, 0, sizeof(UnionForwardSS));
    unionForwardSS.type = UNION_FORWARD_USERNAME;
    if (strlen(pUserRenameCS->userName) + 1 <= sizeof(unionForwardSS.szExtend))
    {
        memcpy(unionForwardSS.szExtend, pUserRenameCS->userName, strlen(pUserRenameCS->userName));
    }
    CKxCommManager::getInstance()->forwardData(uid, CMD_UNION, CMD_UNION_FORWARD_SS,
        reinterpret_cast<char*>(&unionForwardSS), sizeof(UnionForwardSS));

    // 下发改名结果
    UserRenameSC sUserRenameSC;
    sUserRenameSC.result = 1;
	CKxCommManager::getInstance()->sendData(uid, CMD_USER, CMD_USER_RENAME_SC,
        reinterpret_cast<char *>(&sUserRenameSC), sizeof(UserRenameSC));
}

void CUserService::processHeadIconChange(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID (len == sizeof(UserHeadIconCSC))

    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN_VOID(NULL != pGameUser);
    CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
    CHECK_RETURN_VOID(NULL != pUserModel);
    CHeadModel *pHeadModel = dynamic_cast<CHeadModel*>(pGameUser->getModel(MODELTYPE_HEAD));
    CHECK_RETURN_VOID(NULL != pHeadModel);

    UserHeadIconCSC *pUserHeadIconCS = reinterpret_cast<UserHeadIconCSC*>(buffer);
	//配置表是否有该头像
    CHECK_RETURN_VOID(queryConfHasHeadIcon(pUserHeadIconCS->headIconID));
	//检查角色是否有该头像
	CHECK_RETURN_VOID(pHeadModel->IsHeadIDExist(pUserHeadIconCS->headIconID));

    int nPreHeadIconID = 0;
    if (pUserModel->GetUserFieldVal(USR_FD_HEADICON, nPreHeadIconID))
    {
        // 是否相同头像
        CHECK_RETURN_VOID(nPreHeadIconID != pUserHeadIconCS->headIconID);

        // 设置头像
        if (pUserModel->SetUserFieldVal(USR_FD_HEADICON, pUserHeadIconCS->headIconID))
        {
            UserHeadIconCSC sUserHeadIconSC;
            sUserHeadIconSC.headIconID = pUserHeadIconCS->headIconID;

			CKxCommManager::getInstance()->sendData(uid, CMD_USER, CMD_USER_HEADICONCHANGE_SC,
                reinterpret_cast<char *>(&sUserHeadIconSC), sizeof(UserHeadIconCSC));
        }
    }
}

//处理玩家签到
void CUserService::processUserSignService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CHECK_RETURN_VOID(buffer != NULL && commun != NULL)
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN_VOID(pGameUser != NULL);
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CHECK_RETURN_VOID(pUserModel != NULL);

	int nValue = 0;
	pUserModel->GetUserFieldVal(USR_FD_DAYFLAG, nValue);
	//已经签到过了
	if (nValue != 0)
	{
		return;
	}

	//获取当月累计签到天数
	int nCurMonthDay = 0;
	//获取累计签到次数
	int nSignTimes = 0;
	//获取总累计签到天数
	int nTotalDay = 0;
	pUserModel->GetUserFieldVal(USR_FD_MONTHDAY, nCurMonthDay);
	pUserModel->GetUserFieldVal(USR_FD_SIGNDAY, nTotalDay);
	pUserModel->GetUserFieldVal(USR_FD_INCSIGNFLAG, nSignTimes);

	//获取当月签到信息
	time_t now = time(NULL);
	tm NowTm;
	stlocaltime(&now, &NowTm);

	nCurMonthDay += 1;
	nTotalDay += 1;
	
	const SCheckInDayConfig *pCheckDayConfig = queryCheckInDaySign(NowTm.tm_mon+1, nCurMonthDay);
	const SConCheckInConfig *pConCheckInConfig = queryConCheckInSign(nSignTimes+1);

	std::vector<DropItemInfo> items;
	DropItemInfo Info;

	//没有超过26天，计算当天签到奖励
	if (pCheckDayConfig != NULL)
	{
		Info.id = pCheckDayConfig->nGoodsID;
		Info.num = pCheckDayConfig->nGoodsNum;
		items.push_back(Info);
	}
	
	// 累积奖励
	if (pConCheckInConfig != NULL)
	{
		if (pConCheckInConfig->DayNeeds <= nTotalDay)
		{
			for (int i = 0; i < 3; i++)
			{
				Info.id = pConCheckInConfig->nGoodsID[i];
				Info.num = pConCheckInConfig->nGoodsNum[i];
				items.push_back(Info);
			}

			nSignTimes += 1;
		}
	}

	//添加物品
	if (CPropUseHelper::getInstance()->AddItems(uid, items, true, SIGN_GAIN_REASON))
	{
		for (size_t i = 0; i < items.size(); i++)
		{
			if (items[i].id == RESOURCEID_DIAMOND)
			{
				CCommOssHelper::userDiamondGetOss(uid, items[i].num, SIGN_DIAMOND_GET_REASON);
			}
		}
	}

	if (!pUserModel->SetUserFieldVal(USR_FD_MONTHDAY, nCurMonthDay)
		|| !pUserModel->SetUserFieldVal(USR_FD_DAYFLAG, 1)
		|| !pUserModel->SetUserFieldVal(USR_FD_INCSIGNFLAG, nSignTimes)
		|| !pUserModel->SetUserFieldVal(USR_FD_SIGNDAY, nTotalDay))
	{
		KXLOGERROR("uid %d set user field error!", uid);
	}

	SUserSignSC SignSC;
	SignSC.nFlag = 1;
	CKxCommManager::getInstance()->sendData(uid, CMD_USER, CMD_USER_SIGN_SC,
		reinterpret_cast<char *>(&SignSC), sizeof(SignSC));
}

//处理玩家礼包Key
void CUserService::processUserGiftKeyService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	if (NULL == buffer || len != sizeof(SUserGiftKeyCS))
	{
		return;
	}

	SUserGiftKeyCS *pMsg = reinterpret_cast<SUserGiftKeyCS *>(buffer);

	//获取礼包数据
	SGiftData giftData;
	int nType = 0;
	int nParam = 0;
	int nGiftID = 0;
	if (!CGiftModel::getInstance()->GetGiftData(uid, pMsg->GiftKey, nType, nParam, nGiftID, giftData))
	{
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_GIFTCODE_INVALID;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return;
	}
	//条件判定
	switch (nType)
	{
		case eSpecPlatfType:
		{
			if (pMsg->cPatform != nParam)
			{
				ErrorCodeData CodeData;
				CodeData.nCode = ERROR_GIFTPLATFORM_INVALID;
				CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
				return;
			}
		}
		break;
		case eSpecChannelType:
		{
			if (pMsg->nChannelID != nParam)
			{
				ErrorCodeData CodeData;
				CodeData.nCode = ERROR_GIFTCHANNELID_INVALID;
				CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
				return;
			}
		}
		break;
		case eSpecServerType:
		{
			//服务器
// 			KxBaseServer *pServer = KxBaseServer::getNewInstance();
// 			int nWorldID = 0;
// 			if (pServer != NULL)
// 			{
// 				int nServerID = pServer->GetServerID();
// 				const CommServerData *pServerData = CCommServerConfig::getInstance()->GetData(nServerID);
// 
// 				if (pServerData != NULL)
// 				{
// 					nWorldID = pServerData->nWordID;
// 				}
// 			}
// 
// 			if (nWorldID != nParam)
// 			{
// 				ErrorCodeData CodeData;
// 				CodeData.nCode = ERROR_GIFTSERVERID_INVALID;
// 				CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
// 				return;
// 			}
		}
		break;
	}

	std::vector<DropItemInfo> VectItem;
	DropItemInfo Info;
	for (size_t i = 0; i < giftData.vectGoods.size(); i++)
	{
		Info.id = giftData.vectGoods[i].nGoodsID;
		Info.num = giftData.vectGoods[i].nGoodsNum;
		VectItem.push_back(Info);
	}

	//添加物品
	if (CPropUseHelper::getInstance()->AddItems(uid, VectItem, true, CDKEY_GAIN_REASON))
	{
        for (int i = 0; i < static_cast<int>(VectItem.size()); i++)
		{
			if (VectItem[i].id == RESOURCEID_DIAMOND)
			{
				CCommOssHelper::userDiamondGetOss(uid, VectItem[i].num, ITEMUSE_DIAMOND_GET_REASON);
			}
		}
	}

	//删除使用后的Key
	if (!CGiftModel::getInstance()->DelGiftData(uid,pMsg->GiftKey))
	{
		return;
	}

	int nBuffSize = sizeof(SUserGiftKeySC)+VectItem.size() * sizeof(SGoodsData);
	char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(nBuffSize));

	SUserGiftKeySC *pGiftKeySC = (SUserGiftKeySC *)buff;
	pGiftKeySC->nNum = VectItem.size();
	pGiftKeySC++;
	SGoodsData *pGoodsData = (SGoodsData *)pGiftKeySC;
	for (size_t i = 0; i < giftData.vectGoods.size(); i++)
	{
		memcpy(pGoodsData, &(giftData.vectGoods[i]), sizeof(SGoodsData));
		pGoodsData++;
	}

	CCommOssHelper::UserGiftGetOss(uid, pMsg->nChannelID, nGiftID, pMsg->GiftKey);
	//发送消息
	CKxCommManager::getInstance()->sendData(uid, CMD_USER, CMD_USER_GIFTKEY_SC,
		buff,nBuffSize);
	KxServer::kxMemMgrRecycle(buff,nBuffSize);
}

//首冲礼包领取
void CUserService::processUserFistPayService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN_VOID(pGameUser != NULL);

	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CHECK_RETURN_VOID(pUserModel != NULL);
	int nValue = 0;
	CHECK_RETURN_VOID(pUserModel->GetUserFieldVal(USR_FD_PAYMENT,nValue,true));
	CHECK_RETURN_VOID(nValue != 0);
	CHECK_RETURN_VOID(pUserModel->GetUserFieldVal(USR_FD_FIRSTPAYFLAG, nValue));
	CHECK_RETURN_VOID(nValue == 0);
	const SFirstPayData *pFirstPayData = queryFirstPayData();
    CHECK_RETURN_VOID(NULL != pFirstPayData);
	//领取
	std::vector<DropItemInfo> vectItems;

	for (size_t i = 0; i < pFirstPayData->vectGoodsID.size(); i++)
	{
		DropItemInfo Info;
		Info.id = pFirstPayData->vectGoodsID[i];
		Info.num = pFirstPayData->vectGoodsNum[i];
		vectItems.push_back(Info);
	}

	CPropUseHelper::getInstance()->AddItems(uid, vectItems, true, FIRSTPAY_GAIN_REASON);
	CHECK_RETURN_VOID(pUserModel->SetUserFieldVal(USR_FD_FIRSTPAYFLAG,1));
	SUserFirstPaySC PaySC;
	PaySC.nFlag = 1;
	CKxCommManager::getInstance()->sendData(uid, CMD_USER, CMD_USER_FIRSTPAY_SC, (char*)&PaySC, sizeof(PaySC));
}

//基金消息处理
void CUserService::processUserFundService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN_VOID(pGameUser != NULL);
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CHECK_RETURN_VOID(pUserModel != NULL);
	int nValue = 0;
	/*CHECK_RETURN_VOID(pUserModel->GetUserFieldVal(USR_FD_FUND_STARTTIME, nValue,true));*/
	const SFirstPayData *pFirstPayData = queryFirstPayData();
	int nCurTime = time(NULL);
	SUserFundCS FundCS;
	if (nValue == 0)
	{
		////购买基金
		//int nHaveDiamond = 0;
		//int nVipLv = 0;
		////CHECK_RETURN_VOID(pUserModel->GetUserFieldVal(USR_FD_VIP, nVipLv, true))
		//CHECK_RETURN_VOID(pUserModel->GetUserFieldVal(USR_FD_DIAMOND, nHaveDiamond,true));
		////CHECK_RETURN_VOID(nVipLv >= 2);
		//CHECK_RETURN_VOID(nHaveDiamond >= pFirstPayData->nGrowGiftPrice);
		//CHECK_RETURN_VOID(pUserModel->IncreaseFieldVal(USR_FD_DIAMOND,-1*pFirstPayData->nGrowGiftPrice));
		//int nStartTime = CTimeCalcTool::nextTimeStamp(nCurTime, 0, 0);
		//CHECK_RETURN_VOID(pUserModel->SetUserFieldVal(USR_FD_FUND_STARTTIME, nStartTime));
		//CHECK_RETURN_VOID(pUserModel->SetUserFieldVal(USR_FD_FUND_GETTIME, nCurTime));
		//FundCS.nBuyTime = nStartTime;
		//FundCS.nGetTime = nCurTime;
		return;
	}
	//else
	//{
	//	//领取基金
	//	int nRewardTime = 0;
	//	int nStartTime = 0;
	//	CHECK_RETURN_VOID(pUserModel->GetUserFieldVal(USR_FD_FUND_GETTIME, nRewardTime, true));
	//	CHECK_RETURN_VOID(pUserModel->GetUserFieldVal(USR_FD_FUND_STARTTIME, nStartTime, true));

	//	time_t now = nCurTime;
	//	time_t prew = (time_t)nRewardTime;
	//	tm PrevTm;
	//	stlocaltime(&prew, &PrevTm);
	//	tm NowTm;
	//	stlocaltime(&now, &NowTm);

	//	if (PrevTm.tm_yday >= NowTm.tm_yday)
	//	{
	//		//已经领取过了
	//		return;
	//	}

	//	if (now >= nStartTime + pFirstPayData->nGetTimes * 24 * 3600)
	//	{
	//		//已经过期了
	//		return;
	//	}

	//	//添加基金
	//	CHECK_RETURN_VOID(pUserModel->IncreaseFieldVal(USR_FD_DIAMOND,pFirstPayData->nGiftDiamonds));
	//	//设置基金领取时间
	//	CHECK_RETURN_VOID(pUserModel->SetUserFieldVal(USR_FD_FUND_GETTIME,now));
	//	FundCS.nBuyTime = nStartTime;
	//	FundCS.nGetTime = nCurTime;
	//}

	CKxCommManager::getInstance()->sendData(uid, CMD_USER, CMD_USR_FUND_SC, (char*)&FundCS, sizeof(FundCS));
}

void CUserService::processUserForwardDataService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(len == sizeof(int));

    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN_VOID(pGameUser != NULL);
    CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
    CHECK_RETURN_VOID(pUserUnionModel != NULL);

    pUserUnionModel->alterUserUnionValue(USER_UNION_ID, *reinterpret_cast<int*>(buffer));
}

//修改玩家账号对应密码
void CUserService::processUsrPasswordModifyService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CHECK_RETURN_VOID((NULL != buffer) && (NULL != commun) && (len == sizeof(SUsrModifyPsCS)));
	SUsrModifyPsCS *pMsg = reinterpret_cast<SUsrModifyPsCS *>(buffer);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN_VOID(pGameUser != NULL);
	CHECK_RETURN_VOID(CGiftModel::getInstance()->modifyUsrPs(uid, pMsg->nChannelID, pMsg->Ps, sizeof(pMsg->Ps)));
	
	SUsrModifyPsSC PsSC;
	PsSC.nFlag = 1;
	CKxCommManager::getInstance()->sendData(uid, CMD_USER, CMD_USR_MODIFYPS_SC, (char*)&PsSC, sizeof(PsSC));
}
