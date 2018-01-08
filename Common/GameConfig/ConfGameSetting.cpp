#include "ConfGameSetting.h"

bool CConfCardGambleSetting::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));

	while (pLoader.NextLine())
	{
        for (int i = 0; i < 4; ++i)
        {
			m_Data.SoldierLvUpScuccessRate.push_back(pLoader.NextInt());
        }

		m_Data.DiamondCardGamblePrice = pLoader.NextInt();
		m_Data.DiamondCardGamble10Price = pLoader.NextInt();
		Prob temp;
		for (int i = 1; i <= 5; ++i)
		{
			temp.Probability = pLoader.NextInt();
			temp.Ratio = pLoader.NextFloat();
			m_Data.RareRatios[i] = temp;
		}

		CConfAnalytic::ToJsonInt(pLoader.NextStr(), m_Data.FirstDrawCard);
		m_Data.exchangeRatio = pLoader.NextInt();

		break;
	}
	return true;
}

bool CConfIconSetting::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据
	while (pLoader.NextLine())
	{
		for (int i = 0; i < 6; ++i)
		{
			m_Data.EqIcon.push_back(pLoader.NextStr());
		}
		for (int i = 0; i < 4; ++i)
		{
			m_Data.RaceIcon.push_back(pLoader.NextStr());
		}
		for (int i = 0; i < 6; ++i)
		{
			m_Data.JobIcon.push_back(pLoader.NextStr());
		}
		break;
	}
	return true;
}

bool CConfSkillUpRateSetting::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据
	while (pLoader.NextLine())
	{
		SkillUpRateItem* rate = new SkillUpRateItem;
		rate->SkillLv = pLoader.NextInt();
		for (int i = 1; i <= 6; ++i)
		{
			rate->Rate[i] = pLoader.NextInt();
		}
		m_Datas[rate->SkillLv] = rate;
	}
	return true;
}

bool CConfSoldierLevelSetting::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据
	while (pLoader.NextLine())
	{
		SoldierLevelSettingItem* soldLevel = new SoldierLevelSettingItem;
		soldLevel->SoldierLv = pLoader.NextInt();
		soldLevel->LvUpCost = pLoader.NextInt();
		soldLevel->Exp = pLoader.NextInt();
		m_Datas[soldLevel->SoldierLv] = soldLevel;
	}
	return true;
}

bool CConfSoldierStarSetting::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据
	while (pLoader.NextLine())
	{
		SoldierStarSettingItem* soldierStar = new SoldierStarSettingItem;
		soldierStar->SoldierStar = pLoader.NextInt();
		soldierStar->TopLevel = pLoader.NextInt();
		soldierStar->UpStarLevel = pLoader.NextInt();
		soldierStar->UpStarCost = pLoader.NextInt();
        soldierStar->TurnCardCount = pLoader.NextInt();
        soldierStar->TurnFragCount = pLoader.NextInt();
		soldierStar->UpStarCount = pLoader.NextInt();
		m_Datas[soldierStar->SoldierStar] = soldierStar;
	}
	return true;
}

bool CConfSoldierRareSetting::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据
	while (pLoader.NextLine())
	{
		SoldierRareSettingItem* soldierRare = new SoldierRareSettingItem;
		soldierRare->Rare = pLoader.NextInt();
		soldierRare->UiRes = pLoader.NextStr();
		soldierRare->HeadboxRes = pLoader.NextStr();
        soldierRare->UnderboxRes = pLoader.NextStr();
        soldierRare->BorderboxRes = pLoader.NextStr();
        soldierRare->CircleboxRes = pLoader.NextStr();
		soldierRare->BigHeadboxRes = pLoader.NextStr();
        for (int i = 0; i < 6; i++)
        {
            soldierRare->JobsIcon.push_back(pLoader.NextStr());
        }
        soldierRare->HeadboxBgRes = pLoader.NextStr();
		soldierRare->JobBg = pLoader.NextStr();
		m_Datas[soldierRare->Rare] = soldierRare;
	}
	return true;
}

bool CConfTaskAchieveSetting::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据
	while (pLoader.NextLine())
	{
		m_Data.TaskFinishSound = pLoader.NextInt();
		m_Data.MainTaskIcon = pLoader.NextStr();
		m_Data.DailyTaskIcon = pLoader.NextStr();
		m_Data.AchieveFinishSound = pLoader.NextInt();
		break;
	}
	return true;
}

bool CConfUserLevelSetting::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据
	while (pLoader.NextLine())
	{
		UserLevelSettingItem* user = new UserLevelSettingItem;
		user->Level = pLoader.NextInt();
		user->Exp = pLoader.NextInt();
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), user->BuyCoin);
        user->SummonerHP = pLoader.NextInt();

		m_Datas[user->Level] = user;

		// 玩家最高等级
		if (m_nUserMaxLv < user->Level)
		{
			m_nUserMaxLv = user->Level;
		}
	}
	return true;
}

bool CConfNewPlayerSetting::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));
	while (csvLoader.NextLine())
	{
		VecVecInt temp;
		NewPlayerItem *item = new NewPlayerItem;
		item->NewbieType = csvLoader.NextInt();
		item->HeadId = csvLoader.NextInt();
		item->UserLv = csvLoader.NextInt();
		item->UserExp = csvLoader.NextInt();
		item->Gold = csvLoader.NextInt();
		item->Diamond = csvLoader.NextInt();
		item->InitBagCapacity = csvLoader.NextInt();
		item->MaxBagCapacity = csvLoader.NextInt();
		item->MaxHeroCapacity = csvLoader.NextInt();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->SummonerIds);
		//items
		temp.clear();
		CConfAnalytic::ToVecVecInt(csvLoader.NextStr(), temp);
		for (VecVecInt::iterator iterItem = temp.begin();
			iterItem != temp.end(); ++iterItem)
		{
			if ((int)iterItem->size() >= 2)
			{
				item->Items[(*iterItem)[0]] = (*iterItem)[1];
			}
		}
		//equips
		temp.clear();
		CConfAnalytic::ToVecVecInt(csvLoader.NextStr(), temp);
		for (VecVecInt::iterator iterEquip = temp.begin();
			iterEquip != temp.end(); ++iterEquip)
		{
            if ((int)temp.size() >= 2)
			{
				item->Equips[(*iterEquip)[0]] = (*iterEquip)[1];
			}
		}
		//heros
		temp.clear();
		CConfAnalytic::ToVecVecInt(csvLoader.NextStr(), temp);
		for (VecVecInt::iterator iterHero = temp.begin();
			iterHero != temp.end(); ++iterHero)
		{
            if ((int)temp.size() >= 2)
			{
				NewPlayerHeroInfo newHero;
				newHero.heroId = (*iterHero)[0];
				newHero.heroStarLv = (*iterHero)[1];
				item->Heros.push_back(newHero);
			}
		}
		// summoner to team group
		item->SummonerInGroup = csvLoader.NextInt();
		// task
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->Tasks);
		// achieve
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->Achieves);
		// chapters
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->Chapters);
		// guide
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->Guides);
		// head
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->Heads);
		item->UserName = csvLoader.NextStr();
		// chestId
        CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->ChestIds);
        // robotId
        CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->RobotIds);
		m_Datas[item->NewbieType] = item;
	}
	return true;
}

bool CConfTowerTestSetting::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));

	while (csvLoader.NextLine())
	{
		TowerTestSettingItem* item = new TowerTestSettingItem;
		item->FirstCrystal = csvLoader.NextInt();
		item->nStartWeekDay = csvLoader.NextInt();
		item->nInterval = csvLoader.NextInt();

		if (item->nInterval > /*WEEKSECOND*/604800)
		{
			return false;
		}

		m_Datas[1] = item;
	}
	return true;
}

bool CConfChapterSetting::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));

	while (csvLoader.NextLine())
	{
		m_Data.NormalLastChapter = csvLoader.NextInt();
		m_Data.EliteLastChapter = csvLoader.NextInt();

		break;
	}

	return true;
}

bool CConfChatSetting::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));

	while (csvLoader.NextLine())
	{
		m_Data.ChatUnlockLv = csvLoader.NextInt();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), m_Data.RecoverTime);
		m_Data.RecoverTimes = csvLoader.NextInt();
		m_Data.SpeedTimesLimit = csvLoader.NextInt();
		m_Data.WordNumLimit = csvLoader.NextInt();
		m_Data.IntervalTime = csvLoader.NextInt();

		break;
	}

	return true;
}

bool CConfIncreasePaymentPrice::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));

	//如果有数据
	while (pLoader.NextLine())
	{
		IncreasePayItem* payItem = new IncreasePayItem;
		payItem->BuyTimes = pLoader.NextInt();
		payItem->GoldCost = pLoader.NextInt();
        for (int i = 0; i < 4; ++i)
        {
            payItem->GoldProb.push_back(pLoader.NextInt());
        }

        CConfAnalytic::ToJsonInt(pLoader.NextStr(), payItem->EnergyCost);
		payItem->ChallengeCost = pLoader.NextInt();
		payItem->TowerTreasureCost = pLoader.NextInt();
		payItem->FreshShopCost = pLoader.NextInt();
		payItem->BagCost = pLoader.NextInt();
		m_Datas[payItem->BuyTimes] = payItem;
	}
	return true;
}

bool CConfItemCrit::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));

	while (csvLoader.NextLine())
	{
		ItemCrit *pItem = new ItemCrit;
		pItem->ItemType = csvLoader.NextInt();
		pItem->ItemQuality = csvLoader.NextInt();
		pItem->DoubleCrit = csvLoader.NextInt();
		pItem->FourfoldCrit = csvLoader.NextInt();

		int key = pItem->ItemType * 100 + pItem->ItemQuality;
		m_Datas[key] = pItem;
	}
	return true;
}

bool CConfSystemHeadIcon::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));

	while (csvLoader.NextLine())
	{
        int nIconID = csvLoader.NextInt();
        SSystemHeadIconItem* pIconItem = new SSystemHeadIconItem;
        pIconItem->IconName = csvLoader.NextStr();
        pIconItem->IconTips = csvLoader.NextInt();

        m_mapHeadIcom[nIconID] = *pIconItem;
	}
	return true;
}

bool CConfSystemHeadIcon::hasHeadIcon(int headID)
{
    return m_mapHeadIcom.find(headID) != m_mapHeadIcom.end();
}

bool CConfStageSetting::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));

	while (csvLoader.NextLine())
	{
		m_Data.WinMusic = csvLoader.NextInt();
		m_Data.LostMusic = csvLoader.NextInt();

		break;
	}
	return true;
}

bool CConfItemLevelSetting::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));

	while (csvLoader.NextLine())
	{
		ItemLevelSettingItem *item = new ItemLevelSettingItem;
		item->ItemLevel = csvLoader.NextInt();
		item->ItemFrame = csvLoader.NextStr();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->Color);

		m_Datas[item->ItemLevel] = item;
	}

	return true;
}

bool CConfOutterBonusSetting::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));

	while (csvLoader.NextLine())
	{
		OutterBonusSetting *item = new OutterBonusSetting;
		item->ID = csvLoader.NextInt();
		csvLoader.NextStr();//备注
		item->AttributeID = CConfManager::getInstance(
			)->getRoleAttributeID(csvLoader.NextStr());
		item->Method = csvLoader.NextInt();

		m_Datas[item->ID] = item;
	}

	return true;
}

bool CEquipSet::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		int nQuality = loader.NextInt();
		int nPropNum = loader.NextInt();
		m_MapQuality[nQuality] = nPropNum;
	}

	return true;
}

int CEquipSet::GetPropNum(int nQuality)
{
	std::map<int, int>::iterator ator = m_MapQuality.find(nQuality);

	if (ator == m_MapQuality.end())
	{
		return 0;
	}

	return ator->second;
}

bool CConfPvpGradingChange::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	while (loader.NextLine())
	{
		VecInt vec;
		CConfAnalytic::ToJsonInt(loader.NextStr(), vec);
        if ((int)vec.size() < 2)
			continue;

		PvpGradingChange item;
		item.ChangeMMRLower = vec[0];
		item.ChangeMMRUpper = vec[1];
		item.ChangePoint = loader.NextInt();
		m_vecGradingChange.push_back(item);
	}
	
	return true;
}

std::vector<PvpGradingChange> &CConfPvpGradingChange::GetGradingChange()
{
	return m_vecGradingChange;
}

bool CConfPvpGradingInitial::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	while (loader.NextLine())
	{
		VecInt vec;
		CConfAnalytic::ToJsonInt(loader.NextStr(), vec);
        if ((int)vec.size() < 2)
			continue;

		PvpGradingInitial item;
		item.PlayerLevelLower = vec[0];
		item.PlayerLevelUpper = vec[1];
		item.StartPoint = loader.NextInt();
		item.StartMMR = loader.NextInt();
		m_vecGradingInitial.push_back(item);
	}

	return true;
}

std::vector<PvpGradingInitial> &CConfPvpGradingInitial::GetGradingInitial()
{
	return m_vecGradingInitial;
}

bool CConfPvpMatchTime::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	while (loader.NextLine())
	{
		VecInt vec;
		CConfAnalytic::ToJsonInt(loader.NextStr(), vec);
        if ((int)vec.size() < 2)
			continue;

		PvpMatchTime item;
		item.MatchOnlineLower = vec[0];
		item.MatchOnlineUpper = vec[1];
		item.MatchTime = loader.NextInt();
		m_vecMatchTime.push_back(item);
	}
	return true;
}

std::vector<PvpMatchTime> &CConfPvpMatchTime::GetMatchTime()
{
	return m_vecMatchTime;
}

bool CConfPvpSetting::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	while (loader.NextLine())
	{
		m_PvpSetting.GradingNum = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), m_PvpSetting.ArenaDay);
		CConfAnalytic::ToJsonInt(loader.NextStr(), m_PvpSetting.ArenaTime);
		m_PvpSetting.CriticalPoint = loader.NextInt();
		return true;
	}
	return false;
}

bool CConfTimeRecover::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	while (loader.NextLine())
    {
        TimeRecoverItem *item = new TimeRecoverItem;
		item->AllTimeReset = loader.NextInt();
		m_Datas[1] = item;
	}
	return true;
}

bool CConfSoundEffectSetting::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    CHECK_RETURN(loadCsv(csvLoader, str, 3));

    while (csvLoader.NextLine())
    {
        m_Data.MissChannel = csvLoader.NextInt();
        m_Data.VolumeDecayRate = csvLoader.NextInt();

        break;
    }

    return true;
}


bool CConfSysAutoName::LoadCSV(const std::string& str)
{
    CCsvLoader loader;
    CHECK_RETURN(loadCsv(loader, str, 3));

    while (loader.NextLine())
    {
        int id = loader.NextInt();
        int type = loader.NextInt();
        std::string content = loader.NextStr();
        m_mapAutoName[type].push_back(content);
    }
    return true;
}

bool CConfUnionMercenaryPrize::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 2));

	while (csvLoader.NextLine())
	{
		SMercenaryPrize *pPrize = new SMercenaryPrize;
		int index = csvLoader.NextInt();
		pPrize->TimeProfit = csvLoader.NextInt();
		pPrize->MaxTime = csvLoader.NextInt();
		pPrize->EmployedProfit = csvLoader.NextInt();
		pPrize->MaxEmployed = csvLoader.NextInt();

		m_Datas[index] = pPrize;
	}
	return true;
}

bool CConfEquipmentForCast::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	int rowCount = 1;

	while (loader.NextLine())
	{
		SEquipForCast* eq_cast = new SEquipForCast();
		eq_cast->nPart = loader.NextInt();
		eq_cast->nLevel = loader.NextInt();
		eq_cast->nVocation = loader.NextInt();
		loader.NextStr();
		loader.NextInt();
		loader.NextStr();
		for (int i = 0; i < 5; i++)
		{
			eq_cast->nSynthesisItemID[i] = loader.NextInt();
			eq_cast->nSynthesisItemCount[i] = loader.NextInt();
		}
		eq_cast->nGoldSpend[0] = loader.NextInt();
		eq_cast->nGoldSpend[1] = loader.NextInt();

		eq_cast->nEquipmentCreateID[0] = loader.NextInt();
		eq_cast->nNormalMakePercent[0] = loader.NextInt();
		eq_cast->nDelicateMakePercent[0] = loader.NextInt();

		for (int i = 1; i < 5; i++)
		{
			eq_cast->nEquipmentCreateID[i] = loader.NextInt();
			eq_cast->nNormalMakePercent[i] = loader.NextInt() + eq_cast->nNormalMakePercent[i - 1];
			eq_cast->nDelicateMakePercent[i] = loader.NextInt() + eq_cast->nDelicateMakePercent[i - 1];
		}
		m_Datas[rowCount++] = eq_cast;
	}
	return true;
}
