#include "PvpRobotHelper.h"
#include "PvpProtocol.h"
#include "Protocol.h"
#include "ConfHall.h"
#include "ConfArena.h"
#include "CommStructs.h"
#include "PvpRankModel.h"
#include "ActiveHelper.h"
#include "KxCommManager.h"
#include "PvpSettleResult.h"

#include "ModelHelper.h"

bool CRobotHelper::canChallengeRobot(int uid, int &robotId)
{
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(uid, MODELTYPE_PVP));
	CHECK_RETURN(NULL != pPvpModel);

	pPvpModel->GetPvpRobotId(robotId);
	if (robotId == 0)
	{
		KXLOGERROR("user %d didn't be dispatched robot!", uid);
		return false;
	}
	return true;
}

bool CRobotHelper::canChallengeRobot(int uid, int summonerId, std::vector<int>& heroList)
{
	CSummonModel *pSummonerModel = dynamic_cast<CSummonModel*>(CModelHelper::getModel(uid, MODELTYPE_SUMMONER));
	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(CModelHelper::getModel(uid, MODELTYPE_HERO));
	
	CHECK_RETURN(NULL != pSummonerModel && NULL != pHeroModel);

	if (!pSummonerModel->HaveSummoner(summonerId))
	{
		KXLOGERROR("user %d check summoner error!", uid);
		return false;
	}

	std::vector<int>::const_iterator iter = heroList.begin();
	for (; iter != heroList.end(); ++iter)
	{
		int heroId = *iter;
		if (heroId != 0 && !pHeroModel->ExsitHero(heroId))
		{
			KXLOGERROR("user %d check hero error!", uid);
			return false;
		}
	}
	return true;
}

int CRobotHelper::getRoomDataLength(int uid, int summonerId, std::vector<int>& heroList)
{
	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(CModelHelper::getModel(uid, MODELTYPE_HERO));
	if (pHeroModel == NULL)
	{
		return 0;
	}

	int length = sizeof(RoomData);
	length += sizeof(PlayerData);

	length += sizeof(HeroCardData);
	length += sizeof(SoldierCardData)* heroList.size();
	for (unsigned int i = 0; i < heroList.size(); ++i)
	{
		int heroId = heroList[i];
		DBHeroAttr heroInfo;
		if (!pHeroModel->GetHero(heroId, heroInfo))
		{
			continue;
		}

		for (int i = WEAPON; i < TREASURE + 1; ++i)
		{
			if (heroInfo.equipId[i] > 0)
			{
				length += sizeof(SoldierEquip);
			}
		}
	}

	return length;
}

bool CRobotHelper::getRobotBattleData(int uid, int robotId, int summonerId, std::vector<int>& heroList, char *data, int &len)
{
	//玩家信息
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(CModelHelper::getModel(uid, MODELTYPE_HERO));
	CEquipModel *pEquipModel = dynamic_cast<CEquipModel*>(CModelHelper::getModel(uid, MODELTYPE_EQUIP));
	CHECK_RETURN(NULL != pUserModel && NULL != pHeroModel && NULL != pEquipModel);
	//机器人信息
	const SArenaRobotItem *pRobotConf = queryConfArenaRobotItem(robotId);
	if (NULL == pRobotConf)
	{
		KXLOGERROR("%d get robot config error, robotid=%d!", uid, robotId);
		return false;
	}
	//房间信息
	RoomData *pRoomData = reinterpret_cast<RoomData *>(data);
	pRoomData->BattleType = EBATTLE_PVPROBOT;
	pRoomData->StageId = pRobotConf->stageID[0];
	pRoomData->StageLevel = pRobotConf->stageID[1];;
	pRoomData->Ext1 = 0;
	pRoomData->Ext2 = 0;
	pRoomData->BuffCount = 0;
	
	len += sizeof(RoomData);

	pRoomData->PlayerCount = 1;
	PlayerData *pPlayerData = reinterpret_cast<PlayerData *>(pRoomData + 1);
	pPlayerData->UserId = uid;
	pUserModel->GetUserFieldVal(USR_FD_USERLV, pPlayerData->UserLv);
	pPlayerData->Camp = /*ECamp_Blue*/1;
	pPlayerData->OuterBonusCount = 0;
	pPlayerData->SoldierCount = heroList.size();
	pPlayerData->MecenaryCount = 0;
	pPlayerData->Identity = pUserModel->getIdentity();;
	std::string name = pUserModel->GetName();
	memset(pPlayerData->UserName, 0, sizeof(pPlayerData->UserName));
	memcpy(pPlayerData->UserName, name.c_str(), strlen(name.c_str()));
	len += sizeof(PlayerData);

	//玩家召唤师信息
	HeroCardData *pHeroCard = reinterpret_cast<HeroCardData*>(data + len);
	pHeroCard->HeroId = summonerId;

	len += sizeof(HeroCardData);

	//玩家英雄信息
	for (unsigned int i = 0; i < heroList.size(); ++i)
	{
		SoldierCardData *pSoldierCard = reinterpret_cast<SoldierCardData*>(data + len);
		int heroId = heroList[i];
		DBHeroAttr heroInfo;
		if (!pHeroModel->GetHero(heroId, heroInfo))
		{
			continue;
		}

		pSoldierCard->SoldierId = heroId;
		pSoldierCard->SoldierExp = heroInfo.exp;
		pSoldierCard->SoldierLv = heroInfo.level;
		pSoldierCard->SoldierStar = heroInfo.star;
		memcpy(pSoldierCard->SoldierTalent, heroInfo.talent, sizeof(heroInfo.talent));

		pSoldierCard->EquipCnt = 6;
		len += sizeof(SoldierCardData);
		//装备信息
		pSoldierCard->EquipCnt = 0;
		for (int i = 0; i < 6; ++i)
		{
			int equipId = heroInfo.equipId[i];
			if (equipId <= 0)
			{
				continue;
			}
			SoldierEquip *pEquipData = reinterpret_cast<SoldierEquip*>(data + len);
			SEquipInfo *pEquipInfo = pEquipModel->GetEquipData(equipId);
			if (pEquipInfo == NULL)
			{
				KXLOGERROR("%d get equip info is NULL, equip id=%d when get robot room data", uid, equipId);
				return false;
			}
			pEquipData->confId = pEquipInfo->nItemID;
			for (int i = 0; i < MAX_EQUIP_EFFECT_NUM; i++)
			{
				pEquipData->cEffectID[i] = pEquipInfo->cEffectID[i];
				pEquipData->sEffectValue[i] = pEquipInfo->sEffectValue[i];
			}

			pSoldierCard->EquipCnt += 1;
			len += sizeof(SoldierEquip);
		}
	}

	return true;
}

void CRobotHelper::setModelWinRobot(int uid, int robotId)
{
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(uid, MODELTYPE_PVP));
	CPvpRankModel *pPvpRankModel = CPvpRankModel::getInstance();
	CHECK_RETURN_VOID(NULL != pPvpModel);
	//机器人信息
	const SArenaRobotItem *pRobotConf = queryConfArenaRobotItem(robotId);
	if (NULL == pRobotConf)
	{
		KXLOGERROR("%d get robot config error, robotid=%d!", uid, robotId);
		return;
	}

    if (pRobotConf->robotAP.empty())
    {
        KXLOGERROR("%d get robot config error, robotid=%d, robotAP is empty!", uid, robotId);
        return;
    }

	std::map<int, int> pvpValue;
	pvpValue[PVP_FD_MMR] = 0;
	pvpValue[PVP_FD_DANCONTINUSWINTIMES] = 0;
	pvpValue[PVP_FD_INTEGRAL] = 0;
	pvpValue[PVP_FD_TOTALWINTIMES] = 0;

	if (!pPvpModel->GetPvpField(pvpValue))
	{
		KXLOGERROR("% get pvp field error, when set model win robot!", uid);
		return;
	}

	int rankNow = 0;
	int index = rand() % pRobotConf->robotAP.size();
	int changeMMR = CPvpSettleResult::calcMMR(pvpValue[PVP_FD_MMR], pvpValue[PVP_FD_MMR], pvpValue[PVP_FD_DANCONTINUSWINTIMES], 1);
	int changeInte = CPvpSettleResult::calcIntegral(pvpValue[PVP_FD_INTEGRAL], pvpValue[PVP_FD_INTEGRAL]+ pRobotConf->robotAP[index], 1);

	// 结算后有多少积分
	pvpValue[PVP_FD_INTEGRAL] += changeInte;
	if (pvpValue[PVP_FD_INTEGRAL] < 0)
	{
		pvpValue[PVP_FD_INTEGRAL] = 0;
	}

	pPvpRankModel->AddPvpRank(uid, pvpValue[PVP_FD_INTEGRAL]);
	pPvpRankModel->GetPvpRank(uid, rankNow);

	CPvpSettleResult::setWinnerPvpModel(PVPROOMTYPE_PVP, uid, changeMMR, changeInte, rankNow);
}

void CRobotHelper::setModelLoseRobot(int uid, int robotId)
{
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(uid, MODELTYPE_PVP));
	CPvpRankModel *pPvpRankModel = CPvpRankModel::getInstance();
	CHECK_RETURN_VOID(NULL != pPvpModel);
	//机器人信息
	const SArenaRobotItem *pRobotConf = queryConfArenaRobotItem(robotId);
	if (NULL == pRobotConf)
	{
		KXLOGERROR("%d get robot config error, robotid=%d!", uid, robotId);
		return;
	}

    if (pRobotConf->robotAP.empty())
    {
        KXLOGERROR("%d get robot config error, robotid=%d, robotAP is empty!", uid, robotId);
        return;
    }

	std::map<int, int> pvpValue;
	pvpValue[PVP_FD_MMR] = 0;
	pvpValue[PVP_FD_DANCONTINUSWINTIMES] = 0;
	pvpValue[PVP_FD_INTEGRAL] = 0;

	if (!pPvpModel->GetPvpField(pvpValue))
	{
		KXLOGERROR("% get pvp field error, when set model lose robot!", uid);
		return;
	}

	int rankNow = 0;
	int index = rand() % pRobotConf->robotAP.size();
	int changeMMR = CPvpSettleResult::calcMMR(pvpValue[PVP_FD_MMR], pvpValue[PVP_FD_MMR], pvpValue[PVP_FD_DANCONTINUSWINTIMES], 0);
	int changeInte = CPvpSettleResult::calcIntegral(pvpValue[PVP_FD_INTEGRAL], pvpValue[PVP_FD_INTEGRAL] + pRobotConf->robotAP[index], 0);

	// 失败者结算后有多少积分
    if (changeInte < 0)
    {
        if (pvpValue[PVP_FD_INTEGRAL] >= PVP_INTEGRAL_BOUNDARY)
        {
            pvpValue[PVP_FD_INTEGRAL] += changeInte;
            if (pvpValue[PVP_FD_INTEGRAL] < 0)
            {
                pvpValue[PVP_FD_INTEGRAL] = 0;
            }
        }
    }
    else
    {
        pvpValue[PVP_FD_INTEGRAL] += changeInte;
    }

	pPvpRankModel->AddPvpRank(uid, pvpValue[PVP_FD_INTEGRAL]);
	pPvpRankModel->GetPvpRank(uid, rankNow);

	CPvpSettleResult::setLoserPvpModel(PVPROOMTYPE_PVP, uid, changeMMR, changeInte, rankNow);
}
