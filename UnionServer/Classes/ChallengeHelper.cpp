#include "ChallengeHelper.h"
#include "ModelHelper.h"
#include "CommonHelper.h"

#include "CommStructs.h"
#include "GameUserManager.h"

#include "ConfStage.h"
#include "ConfGameSetting.h"
#include "UnionManager.h"

bool CChallengeHelper::checkChallengeTeam(int uid, int summonerId, const std::vector<int> &heroList)
{
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	if (NULL == pGameUser)
	{
		KXLOGERROR("%s uid %d getGameUser == NULL!!", __FUNCTION__, uid);
		return false;
	}

	CSummonModel *pSummonerModel = dynamic_cast<CSummonModel*>(pGameUser->getModel(MODELTYPE_SUMMONER));
	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(pGameUser->getModel(MODELTYPE_HERO));
	CHECK_RETURN(pSummonerModel && pHeroModel);

	if (!pSummonerModel->HaveSummoner(summonerId))
	{
		KXLOGERROR("%s user %d check summoner error!!", __FUNCTION__, uid);
		return false;
	}

	std::vector<int>::const_iterator iter = heroList.begin();
	for (; iter != heroList.end(); ++iter)
	{
		int heroId = *iter;
		if (heroId != 0 && !pHeroModel->ExsitHero(heroId))
		{
			KXLOGERROR("%s user %d check hero error!!", __FUNCTION__, uid);
			return false;
		}
	}
	return true;
}

bool CChallengeHelper::getBattleData(int uid, int summonerId, std::vector<int>& heroList, UnionMercenaryInfo *pMerInfo, BattleRoomInfo &roomInfo, char *data, int &length)
{
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	if (NULL == pGameUser)
	{
		KXLOGERROR("%s uid %d getGameUser == NULL!!", __FUNCTION__, uid);
		return false;
	}
	//房间信息
	RoomData *pRoomData = reinterpret_cast<RoomData *>(data);
	pRoomData->BattleType = roomInfo.battleType;
	pRoomData->StageId = roomInfo.stageId;
	pRoomData->StageLevel = roomInfo.stageLv;
	pRoomData->Ext1 = roomInfo.ext1;
	pRoomData->Ext2 = roomInfo.ext2;
	length += sizeof(RoomData);
	// 战斗内buff, 跟战斗内buff一样
	pRoomData->BuffCount = roomInfo.buffs.size();
	for (std::vector<BuffData>::iterator iter = roomInfo.buffs.begin();
		iter != roomInfo.buffs.end(); ++iter)
	{
		BuffData *pBuffData = reinterpret_cast<BuffData*>(data + length);
		memcpy(pBuffData, &(*iter), sizeof(BuffData));
		length += sizeof(BuffData);
	}
	//玩家信息
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CHECK_RETURN(pUserModel != NULL);
	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(pGameUser->getModel(MODELTYPE_HERO));
	CHECK_RETURN(pHeroModel != NULL);
	CEquipModel *pEquipModel = dynamic_cast<CEquipModel*>(pGameUser->getModel(MODELTYPE_EQUIP));
	CHECK_RETURN(pEquipModel != NULL);

	pRoomData->PlayerCount = 1;
	PlayerData *pPlayerData = reinterpret_cast<PlayerData *>(data + length);
	pPlayerData->Camp = /*ECamp_Blue*/1;
	pPlayerData->UserId = uid;
	pPlayerData->Identity = pUserModel->getIdentity();
	pUserModel->GetUserFieldVal(USR_FD_USERLV, pPlayerData->UserLv);
	std::string name = pUserModel->GetName();
	memset(pPlayerData->UserName, 0, sizeof(pPlayerData->UserName));
	strncpys(pPlayerData->UserName, name.c_str(), strlen(name.c_str()));
	length += sizeof(PlayerData);
	// 战斗外buff, 爬塔buff类型
	pPlayerData->OuterBonusCount = roomInfo.outerBuffs.size();
	for (std::vector<int>::iterator iter = roomInfo.outerBuffs.begin();
		iter != roomInfo.outerBuffs.end(); ++iter)
	{
		int * pOuterBuffId = (int *)(data + length);
		*pOuterBuffId = *iter;
		length += sizeof(int);
	}
	//玩家召唤师信息
	HeroCardData *pHeroCard = reinterpret_cast<HeroCardData*>(data + length);
	pHeroCard->HeroId = summonerId;
	length += sizeof(HeroCardData);
	//玩家英雄信息
	pPlayerData->SoldierCount = heroList.size();
	pPlayerData->MecenaryCount = 0;
	for (unsigned int i = 0; i < heroList.size(); ++i)
	{
		SoldierCardData *pSoldierCard = reinterpret_cast<SoldierCardData*>(data + length);
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
		length += sizeof(SoldierCardData);
		//装备信息
		pSoldierCard->EquipCnt = 0;
		for (int i = WEAPON; i < TREASURE + 1; ++i)
		{
			int equipId = heroInfo.equipId[i];
			if (equipId <= 0)
			{
				continue;
			}
			SoldierEquip *pEquipData = reinterpret_cast<SoldierEquip*>(data + length);
			SEquipInfo *pEquipInfo = pEquipModel->GetEquipData(equipId);
			if (pEquipInfo == NULL)
			{
				KXLOGERROR("CChallengeRoom::getRoomData pEquipInfo == NULL");
				continue;
			}
			pEquipData->confId = pEquipInfo->nItemID;
			for (int i = 0; i < MAX_EQUIP_EFFECT_NUM; i++)
			{
				pEquipData->cEffectID[i] = static_cast<unsigned char>(pEquipInfo->cEffectID[i]);
				pEquipData->sEffectValue[i] = static_cast<unsigned short>(pEquipInfo->sEffectValue[i]);
			}

			pSoldierCard->EquipCnt += 1;
			length += sizeof(SoldierEquip);
		}
	}
	//佣兵
	if (NULL != pMerInfo)
	{
        pPlayerData->MecenaryCount = 1;
        SoldierCardData *pSoldierCard = reinterpret_cast<SoldierCardData*>(data + length);
        memcpy(pSoldierCard, &pMerInfo->stSoldierCardData, sizeof(SoldierCardData));
        if (pSoldierCard->SoldierLv > pPlayerData->UserLv + 15)
        {
            pSoldierCard->SoldierLv = pPlayerData->UserLv + 15;
        }

        length += sizeof(SoldierCardData);
        pSoldierCard->EquipCnt = 0;
        for (int i = 0; i < EQUIPMENTSMAX; ++i)
        {
            SoldierEquip *pSolderEquip = reinterpret_cast<SoldierEquip*>(data + length);
            if (pMerInfo->stEquipments[i].confId != 0)
            {
                pSoldierCard->EquipCnt += 1;
                memcpy(pSolderEquip, &pMerInfo->stEquipments[i], sizeof(SoldierEquip));
                length += sizeof(SoldierEquip);
            }
        }
	}

	return true;
}

int CChallengeHelper::getBattleDataLength(int uid, int summonerId, std::vector<int> &heroList, UnionMercenaryInfo *pMerInfo, int buffSize, int outerBonusSize)
{
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	if (NULL == pGameUser)
	{
		KXLOGERROR("uid %d getGameUser == NULL when getBattleDataLength()!", uid);
		return false;
	}

	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(pGameUser->getModel(MODELTYPE_HERO));
	CHECK_RETURN(pHeroModel != NULL);
	int length = sizeof(RoomData);
	length += sizeof(PlayerData);
	length += sizeof(BuffData)* buffSize;
	length += sizeof(int)* outerBonusSize;

	length += sizeof(HeroCardData);
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
		length += sizeof(SoldierCardData);
	}

	if (NULL != pMerInfo)
	{
		length += sizeof(SoldierCardData);
		for (int i = 0; i < EQUIPMENTSMAX; ++i)
		{
			if (pMerInfo->stEquipments[i].confId != 0)
			{
				length += sizeof(SoldierEquip);
			}
		}
	}

	return length;
}


int CChallengeHelper::getStageStar(int uid, int stageId, ChallengeBattleInfo &finishData, std::vector<int> &starReason)
{
	int star = 1;
	// 关卡配表
	const StageConfItem* pStageConf = queryConfStage(stageId);
	if (NULL == pStageConf)
	{
		KXLOGERROR("%s user %d get stage %d conf is NULL", __FUNCTION__, uid, stageId);
		return 0;
	}
	starReason.resize(2);
	starReason[0] = 0;
	starReason[1] = 0;
	// 第一颗星
	switch (pStageConf->WinStar1)
	{
	case 1:
		if (finishData.hpPercent >= pStageConf->WinStar1Param)
		{
			star += 1;
			starReason[0] = pStageConf->WinStar1;
		}
		break;
	case 2:
		if (finishData.timeCost <= pStageConf->WinStar1Param)
		{
			star += 1;
			starReason[0] = pStageConf->WinStar1;
		}
		break;
	case 3:
		if (finishData.crystalCost <= pStageConf->WinStar1Param)
		{
			star += 1;
			starReason[0] = pStageConf->WinStar1;
		}
		break;
	default:
		break;
	}
	// 第2颗星
	switch (pStageConf->WinStar2)
	{
	case 1:
		if (finishData.hpPercent >= pStageConf->WinStar2Param)
		{
			star += 1;
			starReason[1] = pStageConf->WinStar2;
		}
		break;
	case 2:
		if (finishData.timeCost <= pStageConf->WinStar2Param)
		{
			star += 1;
			starReason[1] = pStageConf->WinStar2;
		}
		break;
	case 3:
		if (finishData.crystalCost <= pStageConf->WinStar2Param)
		{
			star += 1;
			starReason[1] = pStageConf->WinStar2;
		}
		break;
	default:
		break;
	}
	return star;
}

bool CChallengeHelper::getMercenaryInfo(int uid, int mercenary, UnionMercenaryInfo& merInfo)
{
	int unionId = 0;
	CUserUnionModel* pUUModel = dynamic_cast<CUserUnionModel *>(CModelHelper::getModel(uid, MODELTYPE_USERUNION));
	CHECK_RETURN(pUUModel != NULL);
	pUUModel->getUserUnionValue(USER_UNION_ID, unionId);
	if (pUUModel == NULL)
	{
		return false;
	}

	CUnion *pUnion = CUnionManager::getInstane()->getUnion(unionId);
	CHECK_RETURN(pUnion != NULL);

	CUnionMercenaryModel *pMercenaryModel = pUnion->getMercenaryModel();
	CHECK_RETURN(pMercenaryModel != NULL);
	UnionMercenaryInfo *pMerInfo = pMercenaryModel->GetSingleMercenaryInfo(mercenary);
	if (NULL == pMerInfo)
	{
		return false;
	}

	merInfo = *pMerInfo;
	return true;
}
