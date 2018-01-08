#include "ConfHall.h"

bool CConfAchieve::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));
	//如果有数据
	while (csvLoader.NextLine())
	{
		AchieveItem* item = new AchieveItem;
		item->ID = csvLoader.NextInt();
		csvLoader.NextStr();	//策划用
		item->Title = csvLoader.NextInt();
		item->Icon = csvLoader.NextStr();
		item->Desc = csvLoader.NextInt();
		item->UnLockLv = csvLoader.NextInt();
		item->Show = csvLoader.NextInt();
		item->FinishCondition = csvLoader.NextInt();
		for (int i = 0; i < 3; ++i)
		{
			item->FinishParameters.push_back(csvLoader.NextInt());
		}
		item->CompleteTimes = csvLoader.NextInt();
		item->Tips = csvLoader.NextInt();
		item->AwardExp = csvLoader.NextInt();
		item->AwardCoin = csvLoader.NextInt();
		item->AwardDiamond = csvLoader.NextInt();
		item->AwardEnergy = csvLoader.NextInt();
		CConfAnalytic::ToJsonID_Num(csvLoader.NextStr(), item->AwardItems);
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->EndStartID);
		// 映射成就的前置成就
		std::vector<int>::iterator iter = item->EndStartID.begin();
		for (; iter != item->EndStartID.end(); ++iter)
		{
			m_mapPreAchieve[*iter] = item->ID;
		}

		item->PosType = csvLoader.NextInt();
		item->AchieveStar = csvLoader.NextInt();
		item->CloseDisplay = csvLoader.NextInt();
		m_Datas[item->ID] = item;
	}
	return true;
}

int CConfAchieve::getPreAchieveID(int achieveID)
{
	std::map<int, int>::iterator iter = m_mapPreAchieve.find(achieveID);
	if (iter != m_mapPreAchieve.end())
	{
		return iter->second;
	}
	return 0;
}

bool CConfActivityInstance::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));

	while (csvLoader.NextLine())
	{
		ActivityInstanceItem* item = new ActivityInstanceItem;
		item->ID = csvLoader.NextInt();
		csvLoader.NextStr();
		item->Title = csvLoader.NextInt();
		item->Desc = csvLoader.NextInt();
		item->Pic = csvLoader.NextStr();
		item->Place = csvLoader.NextInt();
		item->Show = csvLoader.NextInt();
		item->Type = csvLoader.NextInt();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->PlaceTime);
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->StartTime);
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->EndTime);
		item->CompleteTimes = csvLoader.NextInt();
		item->BuyTimes = csvLoader.NextInt();
		item->RecoverType = csvLoader.NextInt();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->RecoverTime);
		item->RecoverParam = csvLoader.NextInt();
		for (int i = 0; i < 5; i++)
		{
			DiffcultItem diff;
			diff.DiffID = csvLoader.NextInt();
			diff.MaxLevel = csvLoader.NextInt();
			diff.BasicLevel = csvLoader.NextInt();
			diff.ExLevel = csvLoader.NextInt();
			item->Diffcult.push_back(diff);
		}
		m_Datas[item->ID] = item;
	}

	return true;
}

CConfCardGamble::~CConfCardGamble()
{
    MAP_CARDGAMEBLEITEM::iterator iter = m_GardGambleData.begin();
    for (; iter != m_GardGambleData.end(); ++iter)
    {
        if (iter->second.empty())
        {
            continue;
        }

        deleteAndClearVec(iter->second);
    }

    m_GardGambleData.clear();
}

bool CConfCardGamble::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));
	//如果有数据
	while (csvLoader.NextLine())
	{
		CardGambleItem* item = new CardGambleItem;
		item->ID = csvLoader.NextInt();
		item->Rare = csvLoader.NextInt();
		item->Star = csvLoader.NextInt();
		csvLoader.NextStr();	// 跳过
		item->Rate = csvLoader.NextInt();
		item->Ratio = csvLoader.NextInt();
		m_GardGambleData[item->Rare].push_back(item);
	}
	return true;
}

bool CConfEquipment::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));
	//如果有数据 
	while (csvLoader.NextLine())
	{
		EquipmentItem* equipment = new EquipmentItem;
		equipment->ID = csvLoader.NextInt();
		csvLoader.NextStr();	//这一列不需要解析
		equipment->Suit = csvLoader.NextInt();
		equipment->Level = csvLoader.NextInt();
		equipment->Parts = csvLoader.NextInt();
        equipment->Rank = csvLoader.NextInt();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), equipment->Vocation);
		for (int i = 0; i < 4; ++i)
		{
			DecompositMaterial decomposit;
			decomposit.Decomposit = csvLoader.NextInt();
			decomposit.DecompositionParam = csvLoader.NextInt();
			if (0 != decomposit.Decomposit)
			{
				equipment->Decomposit.push_back(decomposit);
			}
		}

		equipment->Gold = csvLoader.NextInt();
		VecVecInt extEff;
		CConfAnalytic::ToVecVecInt(csvLoader.NextStr(), extEff);
		for (size_t i = 0; i < (int)extEff.size(); ++i)
		{
            if ((int)extEff[i].size() == 2)
			{
				equipment->ExtEffect.Buff[extEff[i][0]] = extEff[i][1];
			}
		}
		VecInt talentInfo;
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), talentInfo);
        if ((int)talentInfo.size() == 2)
		{
			equipment->ExtEffect.SoliderId = talentInfo[0];
			equipment->ExtEffect.SoliderStart = talentInfo[1];
		}
		m_Datas[equipment->ID] = equipment;
	}
	return true;
}

bool CConfEqupmentCreate::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));
	//如果有数据 
	while (csvLoader.NextLine())
	{
		EquipPropCreate* pEquipCreate = new EquipPropCreate;
		EffectData Data;
		VecInt VectValue;

		int nEquipCreateID = csvLoader.NextInt();
		csvLoader.NextStr();	//这一列不需要解析
		pEquipCreate->nEquipID = csvLoader.NextInt();

		//基础属性读取
		for (int i = 0; i < MAX_BASE_PROP; i++)
		{
			memset(&Data, 0, sizeof(Data));
			VectValue.clear();
			Data.nEffectID = csvLoader.NextInt();

			if (Data.nEffectID == 0)
			{
				csvLoader.NextStr();
				csvLoader.NextStr();
				continue;
			}

			CConfAnalytic::ToJsonInt(csvLoader.NextStr(), VectValue);

            if ((int)VectValue.size() != 2)
			{
				return false;
			}

			Data.nMinValue = VectValue[0];
			Data.nMaxValue = VectValue[1];

			pEquipCreate->VectBaseProp.push_back(Data);

			csvLoader.NextStr();	//这一列不需要解析
		}

		//附加属性读取
		for (int i = 0; i < MAX_EXTRA_PROP; i++)
		{
			memset(&Data, 0, sizeof(Data));
			VectValue.clear();
			Data.nEffectID = csvLoader.NextInt();							//效果

			if (Data.nEffectID == 0)
			{
				csvLoader.NextStr();
				csvLoader.NextStr();
				continue;
			}

			Data.nWeight = csvLoader.NextInt();								//权值
			CConfAnalytic::ToJsonInt(csvLoader.NextStr(), VectValue);		//效果数值

            if ((int)VectValue.size() != 2)
			{
				return false;
			}

			Data.nMinValue = VectValue[0];
			Data.nMaxValue = VectValue[1];

			pEquipCreate->VectExtraProp.push_back(Data);

			csvLoader.NextStr();	//这一列不需要解析
		}

		m_Datas[nEquipCreateID] = pEquipCreate;
	}
	return true;
}

bool CConfProp::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));
	//如果有数据 
	while (csvLoader.NextLine())
	{
		PropItem* prop = new PropItem;
		prop->ID = csvLoader.NextInt();
		csvLoader.NextStr();	//这一列不解析
		prop->Name = csvLoader.NextInt();
		prop->Desc = csvLoader.NextInt();
		prop->Icon = csvLoader.NextStr();
		prop->Quality = csvLoader.NextInt();
		prop->Type = csvLoader.NextInt();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), prop->TypeParam);
		prop->SellPrice = csvLoader.NextInt();
		prop->UseLevel = csvLoader.NextInt();
		// 快速前往
		CConfAnalytic::ToVecVecInt(csvLoader.NextStr(), prop->QuickToStage);
		prop->Ratio = csvLoader.NextFloat();
		prop->BagLabel = csvLoader.NextInt();
		m_Datas[prop->ID] = prop;
	}
	return true;
}

bool CConfDropProp::LoadCSV(const std::string& str)
{
	m_ShowDropPropItem.clear();
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	//如果有数据 
	while (loader.NextLine())
	{
		DropPropItem *pItem = new DropPropItem;
		pItem->DropRuleID = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->MeanwhileDropNum);
		pItem->IsCrit = loader.NextInt();
		pItem->IsRepeat = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->ExtraDropRuleID);

		for (int i = 0; i < 7; ++i)
		{
			std::vector<int> temp;
			CConfAnalytic::ToJsonInt(loader.NextStr(), temp);
			if (temp.size() < 3)
			{
				continue;
			}
			DropCurrencyData dropCurrency;
			dropCurrency.CurrencyId = temp[0];
			dropCurrency.LowerLimit = temp[1];
			dropCurrency.UpperLimit = temp[2];
			pItem->DropCurrencys.push_back(dropCurrency);
		}

		for (int i = 0; i < 30; ++i)
		{
			int dropId = loader.NextInt();
			if (dropId == 0)
			{
				break;
			}
			pItem->DropIDs.push_back(DropIdData());
			pItem->DropIDs[i].DropID = dropId;
			pItem->DropIDs[i].DropRate = loader.NextInt();
			CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->DropIDs[i].DropNum);
		}
		DropPropItem ShowItem;
		ShowItem = (*pItem);
		m_ShowDropPropItem[pItem->DropRuleID] = ShowItem;
		m_Datas[pItem->DropRuleID] = pItem;
	}
	return true;
}

DropPropItem* CConfDropProp::GetRealDropPropItem(int nDropID)
{
	std::map<int, DropPropItem>::iterator ator = m_ShowDropPropItem.find(nDropID);

	if (ator == m_ShowDropPropItem.end())
	{
		return NULL;
	}

	return &(ator->second);
}

bool CConfDropProp::ReSetShowDropPropItem()
{
	m_ShowDropPropItem.clear();

	for (std::map<int, void*>::iterator ator = m_Datas.begin(); ator != m_Datas.end(); ++ator)
	{
		DropPropItem PropData;
		PropData = *(DropPropItem*)(ator->second);
		m_ShowDropPropItem[ator->first] = PropData;
	}

	return true;
}

bool CConfDropProp::SetShowDropPropItem(int nDropID, DropPropItem &Item)
{
	std::map<int, DropPropItem>::iterator ator = m_ShowDropPropItem.find(nDropID);

	if (ator == m_ShowDropPropItem.end())
	{
		return false;
	}

	ator->second = Item;
	return true;
}

bool CConfMail::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));
	//如果有数据
	while (csvLoader.NextLine())
	{
		MailItem* item = new MailItem;
		item->ID = csvLoader.NextInt();
		csvLoader.NextStr(); //备注
		item->Topic = csvLoader.NextInt();
		item->Sender = csvLoader.NextInt();
		item->Content = csvLoader.NextInt();
		item->LiveTime = csvLoader.NextInt();

		m_Datas[item->ID] = item;
	}

	return true;
}

bool CConfSoldierUpRate::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));

	VecVecInt vec2Temp;
	//如果有数据
	while (csvLoader.NextLine())
	{
		SoldierUpRateItem* item = new SoldierUpRateItem;
		item->SoldierID = csvLoader.NextInt();
		csvLoader.NextStr();	// 这一列不读
        item->DefaultStar = csvLoader.NextInt();
		item->TopStar = csvLoader.NextInt();

        // 快速前往
        CConfAnalytic::ToVecVecInt(csvLoader.NextStr(), item->QuickToStage);
        item->Source = csvLoader.NextInt();

		m_Datas[item->SoldierID] = item;
	}
	return true;
}

bool CConfSuit::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));
	//如果有数据 
	while (csvLoader.NextLine())
	{
		SuitItem* suit = new SuitItem;
		suit->ID = csvLoader.NextInt();
		suit->Name = csvLoader.NextInt();
		suit->Desc = csvLoader.NextInt();
		for (int i = 1; i <= 6; ++i)
		{
			int eqID = csvLoader.NextInt();
			if (0 != eqID)
			{
				suit->Eq[i] = eqID;
			}
		}
		Ability eqEffectTemp;
		for (int i = 2; i <= 6; ++i)
		{
			eqEffectTemp.AbilityID = csvLoader.NextInt();
			eqEffectTemp.AbilityParam = csvLoader.NextInt();
			suit->SuitExtEffect[i] = EquipmentEffect();
			VecVecInt extEff;
			CConfAnalytic::ToVecVecInt(csvLoader.NextStr(), extEff);
			for (size_t j = 0; j < extEff.size(); ++j)
			{
                if ((int)extEff[j].size() == 2)
				{
					suit->SuitExtEffect[i].Buff[extEff[j][0]] = extEff[j][1];
				}
			}
			VecInt talentInfo;
			CConfAnalytic::ToJsonInt(csvLoader.NextStr(), talentInfo);
            if ((int)talentInfo.size() == 2)
			{
				suit->SuitExtEffect[i].SoliderId = talentInfo[0];
				suit->SuitExtEffect[i].SoliderStart = talentInfo[1];
			}

			eqEffectTemp.AbilityDesc = csvLoader.NextInt();
			if (0 != eqEffectTemp.AbilityID)
			{
				suit->SuitAbility[i] = eqEffectTemp;
			}
		}
		m_Datas[suit->ID] = suit;
	}
	return true;
}

bool CConfSaleSummoner::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));
	//如果有数据 
	while (csvLoader.NextLine())
	{
		SaleSummonerConfItem* saleItem = new SaleSummonerConfItem;
		saleItem->ID = csvLoader.NextInt();
		saleItem->Type = csvLoader.NextInt();
		saleItem->Num = csvLoader.NextInt();
		saleItem->SummonerMusic = csvLoader.NextInt();
		saleItem->NewLabel = csvLoader.NextInt();
		saleItem->Head_Name = csvLoader.NextStr();
		saleItem->Bg_Name = csvLoader.NextStr();
		saleItem->Bg_Texture = csvLoader.NextStr();
		saleItem->HeadID = csvLoader.NextInt();
		m_Datas[saleItem->ID] = saleItem;
	}
	return true;
}

bool CConfTask::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));
	//如果有数据
	while (csvLoader.NextLine())
	{
		TaskItem* item = new TaskItem;
		item->ID = csvLoader.NextInt();
		csvLoader.NextStr();	//策划用, 不解析
		item->Title = csvLoader.NextInt();
		item->Type = csvLoader.NextInt();
		item->Icon = csvLoader.NextStr();
		item->Desc = csvLoader.NextInt();
		item->UnlockLv = csvLoader.NextInt();
		item->Show = csvLoader.NextInt();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->QuickTo);
		item->FinishCondition = csvLoader.NextInt();
		for (int i = 0; i < 3; ++i)
		{
			item->FinishParameters.push_back(csvLoader.NextInt());
		}
		item->CompleteTimes = csvLoader.NextInt();
		item->Tips = csvLoader.NextInt();
		item->AwardExp = csvLoader.NextInt();
		item->AwardCoin = csvLoader.NextInt();
		item->AwardDiamond = csvLoader.NextInt();
		item->AwardEnergy = csvLoader.NextInt();
		item->AwardFlashcard = csvLoader.NextInt();
		CConfAnalytic::ToJsonID_Num(csvLoader.NextStr(), item->AwardItems);
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->EndStartID);
		item->TaskReset = csvLoader.NextInt();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->TaskResetParameters);
		m_Datas[item->ID] = item;
	}
	return true;
}

bool CConfOutterBonus::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str, 3));

	EnhanceCondition condition;
	//如果有数据
	while (csvLoader.NextLine())
	{
		OutterBonusItem* item = new OutterBonusItem;
		item->ID = csvLoader.NextInt();
		csvLoader.NextStr();	//策划用, 不解析
		item->Name = csvLoader.NextInt();
		item->Desc = csvLoader.NextInt();
		// 加成条件
		for (int i = 0; i < 4; i++)
		{
			condition.Type = csvLoader.NextInt();
			condition.Param.clear();
			CConfAnalytic::ToJsonInt(csvLoader.NextStr(), condition.Param);
			if (condition.Type != 0)
			{
				item->EnhanceConditions.push_back(condition);
			}
		}
		// 加成效果
		for (int j = 0; j < 4; j++)
		{
			EnhanceValue enhancesValue;
			enhancesValue.EffectId = csvLoader.NextInt();
			enhancesValue.Param = csvLoader.NextInt();
			enhancesValue.EffectLanID = csvLoader.NextInt();
			if (enhancesValue.EffectId != 0)
			{
				item->Enhances.push_back(enhancesValue);
			}
		}

		item->Pic = csvLoader.NextStr();
		item->PicS = csvLoader.NextStr();

		m_Datas[item->ID] = item;
	}
	return true;
}

bool CConfGoldTest::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

	//如果有数据 
	while (csvLoader.NextLine())
	{
		GoldTestConfItem* item = new GoldTestConfItem;
		item->WeekNum = csvLoader.NextInt();
		item->Stage = csvLoader.NextInt();
		item->StageDesc = csvLoader.NextInt();
		item->Frequency = csvLoader.NextInt();
		item->StageLevel = csvLoader.NextInt();
		CConfAnalytic::ToJsonFloat(csvLoader.NextStr(), item->Param);
		m_Datas[item->WeekNum] = item;
	}

	return true;
}

bool CConfGoldTestChest::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

	//如果有数据 
	while (csvLoader.NextLine())
	{
		GoldTestChestConfItem* item = new GoldTestChestConfItem;
		csvLoader.NextStr();
		item->Level = csvLoader.NextInt();
		item->Gold = csvLoader.NextInt();
		item->Damage = csvLoader.NextInt();
		m_Datas[item->Level] = item;
	}

	return true;
}

bool CConfHeroTest::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据
	while (pLoader.NextLine())
	{
		HeroTestItem* item = new HeroTestItem;
		pLoader.NextInt();
		item->ID = pLoader.NextInt();
		pLoader.NextInt();
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), item->Time);
		item->Occupation = pLoader.NextInt();
		item->Times = pLoader.NextInt();
		item->Desc = pLoader.NextInt();
		item->UpDesc = pLoader.NextInt();
		item->Pic = pLoader.NextStr();
		item->Title = pLoader.NextInt();
		item->Diff.reserve(5);
		for (int i = 0; i < 5; i++)
		{
			DiffcultItem diff;
			diff.UnlockLevel = pLoader.NextInt();
			diff.DiffID = pLoader.NextInt();
			CConfAnalytic::ToJsonInt(pLoader.NextStr(), diff.Pic);
			diff.MaxLevel = pLoader.NextInt();
			diff.BasicLevel = pLoader.NextInt();
			diff.ExLevel = pLoader.NextInt();
			item->Diff.push_back(diff);
		}
		m_Datas[item->ID] = item;
	}
	return true;
}

bool CConfTowerFloor::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据
	m_nMaxFloor = 0;
	while (pLoader.NextLine())
	{
		TowerFloorItem* item = new TowerFloorItem;

		item->ID = pLoader.NextInt();
		item->MaxLevel = pLoader.NextInt();
		item->BasicLevel = pLoader.NextInt();
		item->EXLevel = pLoader.NextInt();
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), item->StageID);
		item->Place = pLoader.NextInt();
		item->Drop = pLoader.NextInt();

		m_nMaxFloor = item->ID;
		m_Datas[item->ID] = item;
	}
	return true;
}

bool CConfTowerBuff::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据
	while (pLoader.NextLine())
	{
		TowerBuffItem* item = new TowerBuffItem;
		item->ID = pLoader.NextInt();
		for (int i = 0; i < 3; i++)
		{
			TowerBuffInfo info;
			info.BuffID = pLoader.NextInt();
			info.Cost = pLoader.NextInt();
			item->Buff.push_back(info);
		}
		item->Max = pLoader.NextInt();
		item->Min = pLoader.NextInt();
		m_Datas[item->ID] = item;
#ifdef RunningInServer
		for (int i = item->Min; i <= item->Max; ++i)
		{
			m_mapFloorsBuffId[i].push_back(item->ID);
		}
#endif
	}
	return true;
}

#ifdef RunningInServer
std::vector<int> &CConfTowerBuff::getFloorsBuff(int id)
{
	return m_mapFloorsBuffId[id];
}
#endif

bool CConfTowerRank::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据
	while (pLoader.NextLine())
	{
		TowerRankItem* item = new TowerRankItem;
		item->ID = pLoader.NextInt();
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), item->Rank);
		VecInt VecGoodsID;
		VecInt VecGoodsNum;
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), VecGoodsID);
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), VecGoodsNum);
		if (VecGoodsID.size() != VecGoodsNum.size())
		{
			return false;
		}

		for (unsigned int i = 0; i < VecGoodsID.size(); i++)
		{
			TowerRankInfo info;
			info.ID = VecGoodsID[i];
			info.Num = VecGoodsNum[i];
			item->Item.push_back(info);
		}

		m_Datas[item->ID] = item;
	}
	return true;
}

CConfArenaReward::CConfArenaReward()
: m_pDayWinItem(NULL)
, m_pDayContinusWinItem(NULL)
, m_pDayBattleItem(NULL)
{
}

CConfArenaReward::~CConfArenaReward()
{
	if (NULL != m_pDayWinItem)
	{
		delete m_pDayWinItem;
		m_pDayWinItem = NULL;
	}
	if (NULL != m_pDayContinusWinItem)
	{
		delete m_pDayContinusWinItem;
		m_pDayContinusWinItem = NULL;
	}
	if (NULL != m_pDayBattleItem)
	{
		delete m_pDayBattleItem;
		m_pDayBattleItem = NULL;
	}

    deleteAndClearVec(m_RankRewards);
    deleteAndClearVec(m_CampionRankRewards);
}

bool CConfArenaReward::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		ArenaRewardItem *item = new ArenaRewardItem;
		item->Reward_ID = loader.NextInt();
		loader.NextStr();
		item->Reward_Type = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), item->Type_Parameter);
		item->WinNum_Pic = loader.NextStr();
		item->WinNum_Text = loader.NextInt();
		item->Award_Coin = loader.NextInt();
		item->Award_Diamond = loader.NextInt();
		item->Award_PvpCoin = loader.NextInt();
		item->Award_Flashcard = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), item->AwardPic);
		item->Award_Items = loader.NextInt();

		if (0 == item->Reward_Type)
		{
			//日累计胜场奖励
			m_pDayWinItem = item;
		}
		else if (1 == item->Reward_Type)
		{
			//日连胜场奖励
			m_pDayContinusWinItem = item;
		}
		else if (4 == item->Reward_Type)
		{
			//日战斗场次奖励
			m_pDayBattleItem = item;
		}
        else if (3 == item->Reward_Type)
        {
            // 锦标赛排行奖励
            m_CampionRankRewards.push_back(item);
        }
        else if (2 == item->Reward_Type)
		{
			//公平竞技排行奖励
			m_RankRewards.push_back(item);
		}
        else
        {
            LOG("Arena Reward type:%d error", item->Reward_Type);
            delete item;
            return false;
        }
	}

	return true;
}


bool CConfArenaTask::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		ArenaTaskItem *item = new ArenaTaskItem;
		item->Task_ID = loader.NextInt();
		loader.NextStr();
		item->Task_Type = loader.NextInt();
		item->Task_Pic = loader.NextStr();
		item->Task_Text = loader.NextInt();
		item->Complete_Times = loader.NextInt();
		item->Award_Exp = loader.NextInt();
		item->Award_Coin = loader.NextInt();
		item->Award_Diamond = loader.NextInt();
		item->Award_Energy = loader.NextInt();
		item->Award_PvpCoin = loader.NextInt();
		item->Award_Flashcard = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), item->Award_Items);
		item->DropID = loader.NextInt();
		item->IsOpen = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), item->End_StartID);
		item->IsReset = loader.NextInt();
		m_Datas[item->Task_ID] = item;
	}

	return true;
}

bool CConfAnimationPlayOrder::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据
	std::vector<std::string> temp;
	while (pLoader.NextLine())
	{
		AnimationPlayOrderItem* item = new AnimationPlayOrderItem;
		item->ResID = pLoader.NextInt();
		pLoader.NextStr();	//策划看
		for (int i = 0; i < 9; ++i)
		{
			temp.clear();
			CConfAnalytic::ToJsonStr(pLoader.NextStr(), temp);
			if (!temp.empty())
			{
				item->VecAnimations.push_back(temp);
			}
		}
		m_Datas[item->ResID] = item;
	}
	return true;
}

bool CConfHallStanding::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));

	while (pLoader.NextLine())
	{
		HallStandingItem* item = new HallStandingItem;
		item->SpotOrder = pLoader.NextInt();
		CConfAnalytic::ToVec2(pLoader.NextStr(), item->Position);
		item->ZOrder = pLoader.NextInt();
		m_Datas[item->SpotOrder] = item;
	}

	return true;
}

bool CShopGoodsData::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		ShopGoodsConfigData *pData = new ShopGoodsConfigData;
		pData->nShopGoodsID = loader.NextInt();
		loader.NextStr();		//跳过说明行
		pData->nShopID = loader.NextInt();
		pData->nGoodsID = loader.NextInt();
		pData->nGoodsNum = loader.NextInt();
		pData->nCoinType = loader.NextInt();
		pData->nCoinNum = loader.NextInt();
		pData->nWeight = loader.NextInt();
		pData->nSale = loader.NextInt();
		pData->nKind = loader.NextInt();
		pData->nFreshMinLev = loader.NextInt();
		pData->nFreshMaxLev = loader.NextInt();
		m_Datas[pData->nShopGoodsID] = pData;

		std::map<int, std::list<ShopGoodsConfigData*> >::iterator ator = m_MapShopID.find(pData->nShopID);

		if (ator == m_MapShopID.end())
		{
			std::list<ShopGoodsConfigData*> ShopGoodsList;
			ShopGoodsList.push_back(pData);
			m_MapShopID[pData->nShopID] = ShopGoodsList;
		}
		else
		{
			std::list<ShopGoodsConfigData*> &ShopGoodsList = ator->second;
			ShopGoodsList.push_back(pData);
		}
	}

	return true;
}

std::list<ShopGoodsConfigData*>* CShopGoodsData::GetShopList(int nShopID)
{
	std::map<int, std::list<ShopGoodsConfigData*> >::iterator ator = m_MapShopID.find(nShopID);

	if (ator == m_MapShopID.end())
	{
		return NULL;
	}

	return &ator->second;
}

bool CShopData::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		ShopConfigData *pData = new ShopConfigData;
		int nShopID = loader.NextInt();
		loader.NextStr();				//说明
		pData->nShopName = loader.NextInt();				//名称
		pData->strShopIcon = loader.NextStr();
		CConfAnalytic::ToJsonInt(loader.NextStr(), pData->VectNum);
		pData->nLevLimit = loader.NextInt();
		pData->nTimeInterval = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), pData->VectType);
		m_Datas[nShopID] = pData;
	}

	return true;
}

bool CConfDiamondShop::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		DiamondShopConfigData *pData = new DiamondShopConfigData;
		pData->nGoodsID = loader.NextInt();
		pData->strPicName = loader.NextStr();
		pData->nNameLanID = loader.NextInt();
		pData->nDescLanID = loader.NextInt();
		pData->nPrice = loader.NextInt();
		pData->nDiamond = loader.NextInt();
		m_Datas[pData->nGoodsID] = pData;
	}
	return true;
}


bool CConfDaySign::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		SCheckInDayConfig DaySign;
		int nSignID = loader.NextInt();
		int nMonth = loader.NextInt();
		loader.NextStr();
		DaySign.nGoodsID = loader.NextInt();
		DaySign.nGoodsNum = loader.NextInt();
		DaySign.nShowNum = loader.NextInt();
		//DaySign.nVipDoubleNeed = loader.NextInt();

		std::map<int, std::map<int, SCheckInDayConfig> >::iterator ator = m_MapYearSign.find(nMonth);

		if (ator == m_MapYearSign.end())
		{
			std::map<int, SCheckInDayConfig> MapMonthSign;
			MapMonthSign[nSignID] = DaySign;
			m_MapYearSign[nMonth] = MapMonthSign;
		}
		else
		{
			std::map<int, SCheckInDayConfig> &MapMonthSign = ator->second;
			MapMonthSign[nSignID] = DaySign;
		}
	}

	return true;
}

SCheckInDayConfig * CConfDaySign::GetMonthSignDay(int nMonth, int nSignID)
{
	std::map<int, std::map<int, SCheckInDayConfig> >::iterator ator = m_MapYearSign.find(nMonth);

	if (ator == m_MapYearSign.end())
	{
		return NULL;
	}
	else
	{
		std::map<int, SCheckInDayConfig> &MapMonthSign = ator->second;
		std::map<int, SCheckInDayConfig>::iterator iter = MapMonthSign.find(nSignID);
		if (iter == MapMonthSign.end())
		{
			return NULL;
		}
		
		return &iter->second;
	}
}

bool CConfConDaySign::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		SConCheckInConfig *pItem = new SConCheckInConfig;
		int nSignID = loader.NextInt();
		loader.NextStr();
		pItem->DayNeeds = loader.NextInt();

		for (int i = 0; i < 3; i++)
		{
			pItem->nGoodsID[i] = loader.NextInt();
			pItem->nGoodsNum[i] = loader.NextInt();
			pItem->nShowNum[i] = loader.NextInt();
		}

		m_Datas[nSignID] = pItem;
	}

	return true;
}

bool CConfFirstPay::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		VecInt vectValue;
		CConfAnalytic::ToJsonInt(loader.NextStr(),vectValue);

		for (int i = 0; i < (int)vectValue.size() / 2; i++)
		{
			m_FirstPayData.vectGoodsID.push_back(vectValue[i * 2]);
			m_FirstPayData.vectGoodsNum.push_back(vectValue[i * 2 + 1]);
		}

		m_FirstPayData.nGrowGiftPrice = loader.NextInt();
		m_FirstPayData.nGiftDiamonds = loader.NextInt();
		m_FirstPayData.nGetTimes = loader.NextInt();
		return true;
	}

	return false;
}

bool CConfBDActive::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	while (loader.NextLine())
	{
		SBDActiveKey Key;
		SBDActiveData Data;

		Key.nActiveID	= loader.NextInt();
		Key.nTaskID		= loader.NextInt();

		loader.NextStr();
		loader.NextStr();
		loader.NextStr();

		Data.nActiveType = loader.NextInt();
		Data.nUserLimit = loader.NextInt();
		Data.nConditionParam = loader.NextInt();

		std::vector<std::string> VectStr;
		CConfAnalytic::StringSplit(loader.NextStr(), VectStr, "*");
		for (int i = 0; i < (int)VectStr.size(); i++)
		{
			VecFloat Value;
			CConfAnalytic::ToJsonFloat(VectStr[i], Value);

			if ((int)Value.size() != 2)
			{
				return false;
			}

			Data.vectGoodsID.push_back(Value[0]);
			Data.vectGoodsNum.push_back(Value[1]);
		}

		m_MapBDActive[Key] = Data;
	}

	return true;
}

SBDActiveData*	CConfBDActive::getDBActiveData(int nActiveID, int nTaskID)
{
	SBDActiveKey Key;
	Key.nActiveID = nActiveID;
	Key.nTaskID = nTaskID;
	std::map<SBDActiveKey, SBDActiveData>::iterator ator = m_MapBDActive.find(Key);

	if (ator == m_MapBDActive.end())
	{
		return NULL;
	}

	return &ator->second;
}

bool CConfTalent::LoadCSV(const std::string& str)
{
    CCsvLoader loader;
    CHECK_RETURN(loadCsv(loader, str, 3));

    while (loader.NextLine())
    {
        STalentData *pTalentData = new STalentData;
        pTalentData->TalentID = loader.NextInt();
        loader.NextStr();
        pTalentData->TalentName = loader.NextInt();
        pTalentData->TalentDes = loader.NextInt();
        pTalentData->TalentPic = loader.NextStr();

        for (int i = 0; i < 4; i++)
        {
            int nOutterBonusId = loader.NextInt();
            int nOutterBonusVal = loader.NextInt();
            if (nOutterBonusId > 0)
            {
                pTalentData->OutterBonus[nOutterBonusId] = nOutterBonusVal;
            }
        }

        CConfAnalytic::ToJsonID_Num(loader.NextStr(), pTalentData->BuffId);

        m_Datas[pTalentData->TalentID] = pTalentData;
    }

    return true;
}

bool CConfTalentArrange::LoadCSV(const std::string& str)
{
    CCsvLoader loader;
    CHECK_RETURN(loadCsv(loader, str, 3));

    while (loader.NextLine())
    {
        STalentArrangeData *pTalentArrangeData = new STalentArrangeData;
        pTalentArrangeData->ArrangeID = loader.NextInt();
        CConfAnalytic::ToVecVecInt(loader.NextStr(), pTalentArrangeData->FloorTalent);

        m_Datas[pTalentArrangeData->ArrangeID] = pTalentArrangeData;
    }

    return true;
}

bool CConfPVPShare::LoadCSV(const std::string& str)
{
    CCsvLoader loader;
    CHECK_RETURN(loadCsv(loader, str, 3));

    while (loader.NextLine())
    {
        m_Data.ReplayShowCount = loader.NextInt();
        m_Data.ReplayShareCD = loader.NextInt();
        m_Data.ReplayShareCount = loader.NextInt();
        m_Data.BattleShareCount = loader.NextInt();
        m_Data.RefreshCD = loader.NextInt();
        m_Data.ReplayShowRank = loader.NextInt();
        m_Data.ShareDescLength = loader.NextInt();
        m_Data.Desc = loader.NextInt();
        break;
    }

    return true;
}

bool CConfPVPUpload::LoadCSV(const std::string& str)
{
    CCsvLoader loader;
    CHECK_RETURN(loadCsv(loader, str, 3));

    while (loader.NextLine())
    {
        m_Data.AutoUploadHP = loader.NextInt();
        m_Data.ApplyRank = loader.NextInt();
        m_Data.ApplyCount = loader.NextInt();
        m_Data.AutoUploadRank = loader.NextInt();
        break;
    }

    return true;
}
