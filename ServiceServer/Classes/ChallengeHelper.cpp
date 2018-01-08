#include "ChallengeHelper.h"
#include "ModelHelper.h"
#include "CommonHelper.h"

#include "CommStructs.h"
#include "GameUserManager.h"

#include "ConfStage.h"
#include "ConfGameSetting.h"

bool CChallengeHelper::canChallengeStage(int uid, int chapterId, int stageId, int times)
{
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	if (NULL == pGameUser)
	{
		KXLOGERROR("%s uid %d getGameUser == NULL!!", __FUNCTION__, uid);
		return false;
	}
	CStageModel *pStageModel = dynamic_cast<CStageModel*>(pGameUser->getModel(MODELTYPE_STAGE));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
	const StageConfItem *pStageConf = queryConfStage(stageId);

	if (NULL == pStageModel || NULL == pUserModel
		|| NULL == pChapterConf || pStageConf == NULL)
	{
		KXLOGERROR("uid %d get stage info error! conf is NULL chapter %d, stageId %d", uid, chapterId, stageId);
		return false;
	}

	//玩家体力
	//int needEnergy = CCommonHelper::getStageNeedEnergy(chapterId, stageId);
	//if (!CModelHelper::isEnergyEnough(uid, needEnergy, times))
	//{
	//	KXLOGERROR("uid %d challenge chapter %d stage %d not enough energy!", uid, chapterId, stageId);
	//	return false;
	//}
	// 等级是否满足
	if (!CModelHelper::isUserLevelEnough(uid, pChapterConf->UnlockLevel))
	{
		KXLOGERROR("uid %d challenge chapter %d stage %d not enough level! need %d", uid, chapterId, stageId, pChapterConf->UnlockLevel);
		return false;
	}
	//关卡进度
	if (pChapterConf->Type == CHAPTERTYPE_NORMAL)
	{
		if (pStageModel->GetChapterStatus(chapterId) >= CHAPTERSTATUS_UNLOCK)
		{
			int curStageId = pStageModel->GetCurStage();
			if (stageId > curStageId)
			{
				KXLOGERROR("uid %d stageId > curStageId, stageId %d, curStageId %d", uid, stageId, curStageId);
				return false;
			}
		}
		else
		{
			//版本更新检查
			if (pChapterConf->ExInfo.PrevID != 0 && pStageModel->GetChapterStatus(pChapterConf->ExInfo.PrevID) < CHAPTERSTATUS_FINISH)
			{
				KXLOGERROR("uid %d GetChapterStatus(pChapterConf->ExInfo.PrevID) < CHAPTERSTATUS_FINISH", uid);
				return false;
			}
			else
			{
				//且不是第一关
				if (pChapterConf->ExInfo.FirstStageID != stageId)
				{
					KXLOGERROR("uid %d pChapterConf->ExInfo.FirstStageID != stageId, lock and no first stage, stageId %d", uid, stageId);
					return false;
				}
				else
				{
					// 版本更新的第一关， 先设置为解锁
					if (!pStageModel->SetChapterStatus(chapterId, CHAPTERSTATUS_UNLOCK)
						|| !pStageModel->SetCurStage(stageId))
					{
						KXLOGERROR("uid %d update version or after level enough unlock normal chapter error!", uid);
					}
				}
			}
		}
	}
	else if (pChapterConf->Type == CHAPTERTYPE_ELITE)
	{
		//现在精英关卡关闭
		return false;
		if (pStageModel->GetEliteChapterStatus(chapterId) >= CHAPTERSTATUS_UNLOCK)
		{
			int lastStageId = pStageModel->GetCurElite();
			if (stageId > lastStageId)
			{
				KXLOGERROR("uid %d stageId > curStageId, stageId %d, lastStageId %d", uid, stageId, lastStageId);
				return false;
			}
		}
		else
		{
			//版本更新检查
			if (pChapterConf->ExInfo.PrevID != 0 && pStageModel->GetEliteChapterStatus(pChapterConf->ExInfo.PrevID) < CHAPTERSTATUS_FINISH)
			{
				KXLOGERROR("uid %d %s pStageModel->GetEliteChapterStatus(pChapterConf->ExInfo.PrevID) < CHAPTERSTATUS_FINISH", uid, __FUNCTION__);
				return false;
			}
			else
			{
				//且不是第一关
				if (pChapterConf->ExInfo.FirstStageID != stageId)
				{
					KXLOGERROR("uid %d pChapterConf->ExInfo.FirstStageID != stageId, lock and no first stage, elite stageId %d", uid, stageId);
					return false;
				}
				else
				{
					// 版本更新的第一关， 先设置基本信息
					if (!pStageModel->SetEliteChapterStatus(chapterId, CHAPTERSTATUS_UNLOCK)
						|| !pStageModel->SetCurElite(stageId))
					{
						KXLOGERROR("uid %d update version or after level enough unlock elite chapter error!", uid);
					}
				}
			}
		}

		//挑战次数用完
		//DBEliteStageInfo eliteInfo;
		//if (!CModelHelper::getEliteInfo(uid, chapterId, stageId, eliteInfo))
		//{
		//	KXLOGERROR("uid %d challenge elite get db info error!", uid);
		//	return false;
		//}
		//次数不够
		//if (eliteInfo.challengeTimes + times > pChapterConf->ChallengeTimes)
		//{
		//	KXLOGERROR("uid %d challenge elite has %d need %d times maxtimes %d not enough!",
		//		uid, eliteInfo.challengeTimes, times, pChapterConf->ChallengeTimes);
		//	return false;
		//}
	}

	return true;
}

int CChallengeHelper::getChapterStageLevel(int chapterId, int stageId)
{
	int stageLevel = 1;
	const StageInfo *pStageInfo = queryStageInfoInChapter(chapterId, stageId);
	if (pStageInfo == NULL)
	{
		return 0;
	}
	if (pStageInfo->ID.size() >= 2)
	{
		stageLevel = pStageInfo->ID[1];
	}
	return stageLevel;
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
	pUUModel->getUserUnionValue(USER_UNION_ID, unionId);
	if (pUUModel == NULL)
	{
		return false;
	}

	CUnionMercenaryModel *pMercenaryModel = new CUnionMercenaryModel();
	pMercenaryModel->init(unionId);
	UnionMercenaryInfo *pMerInfo = pMercenaryModel->GetSingleMercenaryInfo(mercenary);
	if (NULL == pMerInfo)
	{
		delete pMercenaryModel;
		return false;
	}

	merInfo = *pMerInfo;
	delete pMercenaryModel;
	return CModelHelper::canMercenaryUse(uid, mercenary);
}

bool CChallengeHelper::canChallengeGoldTest(int uid, int &stageId, int &stageLv)
{
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	if (NULL == pGameUser)
	{
		KXLOGERROR("%s uid %d getGameUser == NULL!!", __FUNCTION__, uid);
		return false;
	}
	//先刷新一下金币试练信息
	CModelHelper::updateGoldTestInfo(uid);

	time_t nowStamp = time(NULL);
	tm *pCurTm = localtime(&nowStamp);
	int wday = pCurTm->tm_wday == 0 ? 7 : pCurTm->tm_wday;

	CGoldTestModel *pGoldTestModel = dynamic_cast<CGoldTestModel*>(pGameUser->getModel(MODELTYPE_GOLDTEST));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));

	const GoldTestConfItem *pConf = queryConfGoldTest(wday);
	if (NULL == pConf || NULL == pUserModel || NULL == pGoldTestModel)
	{
		KXLOGERROR("user %d model or config is NULL!!", uid);
		return false;
	}

    int userLv = 0;
	if (!pUserModel->GetUserFieldVal(USR_FD_USERLV, userLv))
	{
		KXLOGERROR("%s user %d get user lv error!!", __FUNCTION__, uid);
		return false;
	}

    //次数判断
    int userTimes = 0;
	if (!pGoldTestModel->GetFieldNum(GT_FD_USETIMES, userTimes))
	{
		KXLOGERROR("%s user %d get gold trials time error!!", __FUNCTION__, uid);
		return false;
	}

	if (userTimes >= pConf->Frequency)
	{
		KXLOGERROR("%s user %d get gold trials time not enough!!", __FUNCTION__, uid);
		return false;
	}

    stageId = pConf->Stage;
    stageLv = pConf->StageLevel + userLv;
    return true;
}

bool CChallengeHelper::isEnoughGoldTestChest(int uid, int chest)
{
    if (chest <= 0 || chest > 31)
    {
		KXLOGERROR("%s user %d chest <= 0 || chest > 31", __FUNCTION__, uid);
        return false;
    }

	int chestFlag = 0;
	int damage = 0;
	CGoldTestModel *pGoldTestModel = dynamic_cast<CGoldTestModel*>(CModelHelper::getModel(uid, MODELTYPE_GOLDTEST));
	if (NULL == pGoldTestModel || !pGoldTestModel->GetFieldNum(GT_FD_CHESTFLAG, chestFlag) 
		|| !pGoldTestModel->GetFieldNum(GT_FD_DAMAGE, damage))
	{
		KXLOGERROR("%s user %d get gold test model data error!", __FUNCTION__, uid);
		return false;
	}

	// 需要的伤害数量
	CConfGoldTestChest *conf = dynamic_cast<CConfGoldTestChest *>(CConfManager::getInstance()->getConf(CONF_GOLD_TEST_CHEST));
	std::map<int, void *> mapItem = conf->getDatas();

	std::map<int, void *>::iterator iter = mapItem.find(chest);
	if (iter == mapItem.end())
	{
		KXLOGERROR("%s user %d can't find config of gold test chest %d!", __FUNCTION__, uid, chest);
		return false;
	}

	GoldTestChestConfItem *pChestConf = static_cast<GoldTestChestConfItem*>(iter->second);
	if (pChestConf->Damage > damage)
	{
		KXLOGERROR("%s user %d can't get chest damage not enough!", __FUNCTION__, uid);
		return false;
	}

	chest -= 1;
	int nFlag = 0x1 << chest;
	if (nFlag & chestFlag)
	{
		KXLOGERROR("%s user %d chest has got!", __FUNCTION__, uid);
		return false;
	}
	return true;
}

bool CChallengeHelper::canChallengeHeroTest(int uid, int instanceId, int diff)
{
	//刷新英雄试练
	CModelHelper::updateHeroTestInfo(uid);

	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN(NULL != pGameUser);
	CHeroTestModel *pHeroTestModel = dynamic_cast<CHeroTestModel*>(pGameUser->getModel(MODELTYPE_HEROTEST));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CHECK_RETURN(NULL != pHeroTestModel && NULL != pUserModel);

	const TimeRecoverItem *pRecoverConf = queryConfTimeRecoverSetting();
	const HeroTestItem* pHeroTestConf = queryConfHeroTest(instanceId);
	CHECK_RETURN(NULL != pHeroTestConf && NULL != pRecoverConf);
	//难度是否合法
	CHECK_RETURN(diff >= 0 && diff < (int)pHeroTestConf->Diff.size());

	// 指定的英雄试练今日是否开启
	bool bOpening = false;
	time_t nowTime = time(NULL);
	tm *pCurTm = localtime(&nowTime);
	int wday = pCurTm->tm_wday == 0 ? 7 : pCurTm->tm_wday;

	int hour = pRecoverConf->AllTimeReset / 60;
	int min = pRecoverConf->AllTimeReset % 60;

	for (unsigned int i = 0; i < pHeroTestConf->Time.size(); ++i)
	{
		//比如5点开启, 大于今天5点并小于明天的5点, 24小时内
		int nConfWDay = pHeroTestConf->Time[i];
		int nConfNextWDay = pHeroTestConf->Time[i] == 7 ? 1 : pHeroTestConf->Time[i] + 1;

		if ((wday == nConfWDay && pCurTm->tm_hour >= hour && pCurTm->tm_min >= min)
			|| (wday == nConfNextWDay && pCurTm->tm_hour < hour && pCurTm->tm_min < min))
		{
			bOpening = true;
			break;
		}
	}
	// 是否开启
	CHECK_RETURN(bOpening);
	// 等级限制
	int nUserLv = 0;
	CHECK_RETURN(pUserModel->GetUserFieldVal(USR_FD_USERLV, nUserLv));
	CHECK_RETURN(nUserLv >= pHeroTestConf->Diff[diff].UnlockLevel);
	// 挑战次数到达上限
	if (pHeroTestModel->GetChallengeTimes(instanceId) >= pHeroTestConf->Times)
	{
// 		ErrorCodeData CodeData;
// 		CodeData.nCode = ERROR_SHILIANTIMES_NOTENOUGH;
// 		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		KXLOGERROR("%s user %d challenge hero test but times was limited!", __FUNCTION__, uid);
		return false;
	}

	return true;
}

int CChallengeHelper::getHeroTestStageLevel(int uid, int instanceId, int diff)
{
	const HeroTestItem* pHeroTestConf = queryConfHeroTest(instanceId);
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
	if (NULL == pUserModel || NULL == pHeroTestConf)
	{
		KXLOGERROR("%s user %d user model or hero test config is NULL!", __FUNCTION__, uid);
		return 1;
	}

	int stageLevel = 0;
	int userLevel = 0;
	if (!pUserModel->GetUserFieldVal(USR_FD_USERLV, userLevel))
	{
		KXLOGERROR("%s user %d get user level error!", __FUNCTION__, uid);
		return 1;
	}

	stageLevel = userLevel + pHeroTestConf->Diff[diff].ExLevel;
	if (stageLevel > pHeroTestConf->Diff[diff].MaxLevel)
	{
		stageLevel = pHeroTestConf->Diff[diff].MaxLevel;
	}
	else if (stageLevel < pHeroTestConf->Diff[diff].BasicLevel)
	{
		stageLevel = pHeroTestConf->Diff[diff].BasicLevel;
	}

	return stageLevel;
}

bool CChallengeHelper::canChallengeTowerTest(int uid, int floor)
{
	CTowerTestModel *pTowerTestModel = dynamic_cast<CTowerTestModel*>(CModelHelper::getModel(uid, MODELTYPE_TOWERTEST));
	if (pTowerTestModel == NULL)
	{
		KXLOGDEBUG("%s user %d pTowerTestModel == NULL", __FUNCTION__, uid);
		return false;
	}
	//更新玩家爬塔
	CModelHelper::updateTowerTestInfo(uid);

	int curFloor = pTowerTestModel->GetTowerTestField(TOWER_FD_FLOOR);
	int state = pTowerTestModel->GetTowerTestField(TOWER_FD_FLOORSTATE);
	int maxFloor = queryMaxTowerFloor();

	// 如果不处于挑战状态不允许挑战
	if (state != FLOORSTATE_FIGHTING)
	{
		KXLOGERROR("%s user %d challenge tower test but state %d != FLOORSTATE_FIGHTING!", __FUNCTION__, uid, state);
// 		ErrorCodeData CodeData;
// 		CodeData.nCode = ERROR_BATTLE_STATEERROR;
// 		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return false;
	}
	// 当前楼层最高
	if (floor != curFloor || floor > maxFloor)
	{
		KXLOGERROR("%s user %d challenge tower test but floor %d was limited!", __FUNCTION__, uid, floor);
// 		ErrorCodeData CodeData;
// 		CodeData.nCode = ERROR_BATTLE_MAXTOWER;
// 		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
		return false;
	}

	//是否处于能挑战爬塔的时间
	const TowerTestSettingItem * pSettingConf = queryConfTowerSetting();
	CHECK_RETURN(NULL != pSettingConf);
	int nCurTime = static_cast<time_t>(time(NULL));
	int nStartTime = CTimeCalcTool::curDayTimeZero(pSettingConf->nStartWeekDay);
	int nEndTime = nStartTime + pSettingConf->nInterval * 3600;

	//爬塔未开启
	if (nCurTime < nStartTime || nCurTime > nEndTime)
	{
		return false;
	}

	return true;
}

int CChallengeHelper::getTowerTestStageLevel(int uid, int floor)
{
	const TowerFloorItem *pTowerFloorConf = queryConfTowerFloor(floor);
	if (NULL == pTowerFloorConf)
	{
		KXLOGDEBUG("%s user %d get tower %d config is NULL!", __FUNCTION__, uid, floor);
		return false;
	}

	CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
	int userLevel = 0;
	if (!pUserModel->GetUserFieldVal(USR_FD_USERLV, userLevel))
	{
		KXLOGERROR("%s user %s get user model error!", __FUNCTION__, uid);
		return 1;
	}

	int stageLevel = pTowerFloorConf->EXLevel + userLevel;
	if (stageLevel > pTowerFloorConf->MaxLevel)
	{
		stageLevel = pTowerFloorConf->MaxLevel;
	}
	else if (stageLevel < pTowerFloorConf->BasicLevel)
	{
		stageLevel = pTowerFloorConf->BasicLevel;
	}
	return stageLevel;
}

bool CChallengeHelper::canChallengeSpecialTest(int uid)
{
	return true;
}
