#include "ConfUnion.h"
#include "ConfAnalytic.h"

bool CConfUnionLevel::LoadCSV(const std::string& str)
{
    CCsvLoader pLoader;
    CHECK_RETURN(loadCsv(pLoader, str, 3));
    //如果有数据
    while (pLoader.NextLine())
    {
        UnionLevelItem* item = new UnionLevelItem;
        item->UnionLv = pLoader.NextInt();
        pLoader.NextStr(); //备注
        item->ViceChairmanNum = pLoader.NextInt();
        item->MemberLimit = pLoader.NextInt();
        item->ActiveMin = pLoader.NextInt();
        item->ActiveReward = pLoader.NextInt();
        item->RewardID = pLoader.NextInt();
        item->ActiveSReward = pLoader.NextInt();
        item->SRewardID = pLoader.NextInt();
        item->UpLevelCost = pLoader.NextInt();
        item->DownLevelCost = pLoader.NextInt();
        item->UnActiveReduce = pLoader.NextInt();

        m_Datas[item->UnionLv] = item;
    }
    return true;
}

bool CConfUnion::LoadCSV(const std::string& str)
{
    CCsvLoader pLoader;
    CHECK_RETURN(loadCsv(pLoader, str, 3));
    //如果有数据
    while (pLoader.NextLine())
    {
        m_Data.UnLockLv = pLoader.NextInt();
        m_Data.CostCoin = pLoader.NextInt();
        m_Data.AuditTime = pLoader.NextInt();
        m_Data.ApplyCD = pLoader.NextInt();
        m_Data.ApplyCount = pLoader.NextInt();
		m_Data.ChangeNameCost = pLoader.NextInt();
		m_Data.nExpiditionTimes = pLoader.NextInt();
		m_Data.nRewardSendTime = pLoader.NextInt();
        break;
    }
    return true;
}

bool CConfExpeditionWorld::LoadCSV(const std::string& str)
{
    CCsvLoader loader;
    CHECK_RETURN(loadCsv(loader, str.c_str(), 3));

    //如果有数据 
    while (loader.NextLine())
    {
        SExpeditonWorldItem* pWorldData = new SExpeditonWorldItem;
        pWorldData->nWorldID = loader.NextInt();
        pWorldData->nUnLockLv = loader.NextInt();
        pWorldData->strIcon = loader.NextStr();

		VecInt vecValue;
        CConfAnalytic::ToJsonInt(loader.NextStr(), vecValue);
        for (size_t i = 0; i < vecValue.size();)
        {
            SMapWeight data;
			data.mapId = vecValue[i++];
			data.weight = vecValue[i++];
            pWorldData->vecMap.push_back(data);
        }

        pWorldData->nFightTime = loader.NextInt();
        pWorldData->nColdTime = loader.NextInt();
        pWorldData->strName = loader.NextStr();
        pWorldData->strDesc = loader.NextStr();
        m_Datas[pWorldData->nWorldID] = pWorldData;
    }

    return true;
}

bool CConfExpeditionMap::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

    //如果有数据 
    while (csvLoader.NextLine())
    {
        SExpeditionMapData *pExpeditionMapData = new SExpeditionMapData;
        pExpeditionMapData->nStageMapID = csvLoader.NextInt();
        pExpeditionMapData->nUnlockLv = csvLoader.NextInt();
        pExpeditionMapData->nTotal = csvLoader.NextInt();
        CConfAnalytic::ToJsonInt(csvLoader.NextStr(), pExpeditionMapData->vecStartStages);
        pExpeditionMapData->nMapName = csvLoader.NextInt();
        pExpeditionMapData->strMapScence = csvLoader.NextInt();
		pExpeditionMapData->nUnionPrestige = csvLoader.NextInt();
        pExpeditionMapData->nWinGoodsID = csvLoader.NextInt();
        pExpeditionMapData->nGoodsID[0] = csvLoader.NextInt();
        pExpeditionMapData->nGoodsID[1] = csvLoader.NextInt();
        pExpeditionMapData->nGoodsID[2] = csvLoader.NextInt();
        pExpeditionMapData->nGoodsID[3] = csvLoader.NextInt();
        CConfAnalytic::ToJsonInt(csvLoader.NextStr(), pExpeditionMapData->vecShopGoods);
		VecInt nValue;
        for (int i = 0; i < MAX_MAP_INDEX; i++)
        {
            nValue.clear();
            CConfAnalytic::ToJsonInt(csvLoader.NextStr(), nValue);
            if (nValue.size() < 2)
            {
				break;
            }
			pExpeditionMapData->stageList[i].nStageID = nValue[0];
			pExpeditionMapData->stageList[i].nStageLv = nValue[1];
            pExpeditionMapData->stageList[i].nDescID = csvLoader.NextInt();
            pExpeditionMapData->stageList[i].nTitleID = csvLoader.NextInt();
            pExpeditionMapData->stageList[i].strThumbnail = csvLoader.NextStr();
            pExpeditionMapData->stageList[i].strBackground = csvLoader.NextStr();
            pExpeditionMapData->stageList[i].strHeadIcon = csvLoader.NextStr();
            pExpeditionMapData->stageList[i].nHeadDesc = csvLoader.NextInt(); 
            pExpeditionMapData->stageList[i].nHeadName = csvLoader.NextInt();
            pExpeditionMapData->stageList[i].strHeadRes = csvLoader.NextStr();
            pExpeditionMapData->stageList[i].strHeadTag = csvLoader.NextStr();
            CConfAnalytic::ToJsonInt(csvLoader.NextStr(), pExpeditionMapData->stageList[i].unlockIndexs);
            pExpeditionMapData->stageList[i].nColor = csvLoader.NextInt();
            CConfAnalytic::ToJsonInt(csvLoader.NextStr(), pExpeditionMapData->stageList[i].StageMapBuff);

			if (pExpeditionMapData->stageList[i].StageMapBuff.size() != 0
				&& pExpeditionMapData->stageList[i].StageMapBuff.size() != 3)
			{
				return false;
			}
		
            pExpeditionMapData->stageList[i].nBossHp = csvLoader.NextInt();
        }

        m_Datas[pExpeditionMapData->nStageMapID] = pExpeditionMapData;
    }

    return true;
}

bool CConfUnionBadge::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

    //如果有数据 
    while (csvLoader.NextLine())
    {
        int nBadgeId = csvLoader.NextInt();
        std::string strBadgePng = csvLoader.NextStr();

        m_MapUnionBadge[nBadgeId] = strBadgePng;
    }

    return true;
}

bool CConfUnionDrop::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str.c_str(), 3));

	//如果有数据 
	while (loader.NextLine())
	{
		UnionShopDropData *pShopData = new UnionShopDropData;
		pShopData->DropRuleID = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), pShopData->MeanwhileDropNum);
		for (int i = 0; i < 30; ++i)
		{
			int dropId = loader.NextInt();
			if (dropId == 0)
			{
				break;
			}
			pShopData->DropIDs.push_back(UnionDropIdData());
			pShopData->DropIDs[i].DropID = dropId;
			pShopData->DropIDs[i].DropRate = loader.NextInt();
			CConfAnalytic::ToJsonInt(loader.NextStr(), pShopData->DropIDs[i].DropNum);
		}

		m_Datas[pShopData->DropRuleID] = pShopData;
	}
	return true;

}

