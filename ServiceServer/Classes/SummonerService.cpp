#include "SummonerService.h"
#include "Protocol.h"
#include "SummonerProtocol.h"
#include "ServiceDef.h"
#include "GameUserManager.h"
#include "ConfHall.h"
#include "ModelHelper.h"
#include "ConfGameSetting.h"
#include "ConfRole.h"
#include "CommOssHelper.h"
#include "KxCommManager.h"

void CSummonerService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
	case CMD_SUM_BUY_CS:
		processBuySummoner(uid, buffer, len, commun);
		break;
	}
}

void CSummonerService::processBuySummoner(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CHECK_RETURN_VOID(len == sizeof(SumBuyCS));
	SumBuyCS *pBuyCS = reinterpret_cast<SumBuyCS*>(buffer);

	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN_VOID(NULL != pGameUser);
	CSummonModel *pSumModel = dynamic_cast<CSummonModel*>(pGameUser->getModel(MODELTYPE_SUMMONER));
	CHECK_RETURN_VOID(NULL != pSumModel);
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CHECK_RETURN_VOID(NULL != pUserModel);
	CHeadModel *pHeadModel = dynamic_cast<CHeadModel*>(pGameUser->getModel(MODELTYPE_HEAD));
	CHECK_RETURN_VOID(NULL != pHeadModel);

	int usrField = 0;
	int haveMoney = 0;

	// 1. 购买的召唤师是否已拥有 
	std::vector<int>& summoners = pSumModel->GetAllSummon();
	std::vector<int>::iterator ator = find(summoners.begin(), summoners.end(), pBuyCS->summonerId);
	if (ator != summoners.end())
	{
		KXLOGDEBUG("%s summoners.find(pBuyCS->summonerId) != summoners.end()", __FUNCTION__);
		return;
	}

    const SaleSummonerConfItem *pSaleSumConf = queryConfSaleSummoner(pBuyCS->summonerId);
    CHECK_RETURN_DEBUG_VOID(NULL != pSaleSumConf);
	CHECK_RETURN_DEBUG_VOID(queryConfHasHeadIcon(pSaleSumConf->HeadID));

	// 2. 货币是否足够
	if (pSaleSumConf->Type == MONEY_DIAMOND)
	{
		usrField = USR_FD_DIAMOND;
	}
	else
	{
		usrField = USR_FD_GOLD;
	}

    CHECK_RETURN_DEBUG_VOID(pUserModel->GetUserFieldVal(usrField, haveMoney));

    // 货币足够并且达到vip限制
	if (haveMoney >= pSaleSumConf->Num)
	{
		// 先添加物品
        CHECK_RETURN_DEBUG_VOID(pSumModel->AddSummon(pBuyCS->summonerId));
		//添加头像
		CHECK_RETURN_DEBUG_VOID(pHeadModel->AddHeadID(pSaleSumConf->HeadID));
		if (usrField == USR_FD_DIAMOND)
		{
			CHECK_RETURN_DEBUG_VOID(CModelHelper::addDiamond(uid, -1 * pSaleSumConf->Num));
			CCommOssHelper::userDiamondPayOss(uid, pSaleSumConf->Num, DIAMONDBUY_SUMMER_TYPE, 1, pBuyCS->summonerId);
		}
		else
		{
			CHECK_RETURN_DEBUG_VOID(pUserModel->AddUserFieldVal(usrField, -1 * pSaleSumConf->Num));
		}

		CModelHelper::DispatchActionEvent(uid, ELA_OWN_SUMMONER_COUNT, NULL, 0);
		CCommOssHelper::userGoodsGainOss(uid, SUMMONER_GAION_REASON, pBuyCS->summonerId, 1);

		SumBuySC buySC;
		buySC.moneyType = pSaleSumConf->Type;
		buySC.money = pSaleSumConf->Num;
		CKxCommManager::getInstance()->sendData(uid, CMD_SUMMONER, CMD_SUM_BUY_SC,
			reinterpret_cast<char *>(&buySC), sizeof(buySC));
	}
}
