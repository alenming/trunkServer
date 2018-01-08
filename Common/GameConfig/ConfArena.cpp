#include "ConfArena.h"

bool CConfArenaRank::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		ArenaRankItem *item = new ArenaRankItem;
		item->ArenaLevel = loader.NextInt();
		loader.NextStr();
		CConfAnalytic::ToJsonInt(loader.NextStr(), item->GNRank);
		item->GNPic = loader.NextStr();
		item->MMR_K = loader.NextFloat();
		item->MMR_kx = loader.NextFloat();
		CConfAnalytic::ToJsonInt(loader.NextStr(), item->MMR_Range);
		item->Arena_K = loader.NextFloat();
		m_Datas[item->ArenaLevel] = item;
	}

	return true;
}

bool CConfArenaTraining::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		loader.NextStr();
		int computerId = loader.NextInt();
		int stageId = loader.NextInt();

		m_mapArenaTraining[computerId] = stageId;
	}

	return true;
}

bool CConfArenaRobot::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		SArenaRobotItem *pItem = new SArenaRobotItem;
		pItem->index = loader.NextInt();
		pItem->robotType = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->robotAP);
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->robotLevel);
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->stageID);
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->robotTime);

		if (pItem->robotType == 1)
		{
			m_RobotEasy.push_back(pItem);
		}
		else if (pItem->robotType == 2)
		{
			m_RobotNormal.push_back(pItem);
		}
        else if (pItem->robotType == 3)
		{
			m_RobotAdvance.push_back(pItem);
		}
		m_Datas[pItem->index] = pItem;
	}
	return true;
}

CConfArenaRobotName::~CConfArenaRobotName()
{
	deleteAndClearVec(m_vecPvpRobotName);
}

bool CConfArenaRobotName::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		ArenaRobotNameItem *item = new ArenaRobotNameItem;
		item->IndexId = loader.NextInt();
		loader.NextStr();
		item->robotName = loader.NextStr();

		m_vecPvpRobotName.push_back(item);
	}

	return true;
}

bool CConfArenaScrollBar::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		ArenaScollNameItem *item = new ArenaScollNameItem;
		item->ComputerID = loader.NextInt();
		loader.NextStr();
		//item->ComputerPic = loader.NextStr();
		item->ComputerName = loader.NextInt();

		m_Datas[item->ComputerID] = item;
	}
	return true;
}

bool CConfArenaSetting::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	while (loader.NextLine())
	{
		ArenaSettingItem * pItem = new ArenaSettingItem;
		pItem->ChestPrice = loader.NextInt();
		pItem->ChestHalfPriceNum = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->Low);
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->Mid);
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->High);
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->LowTime);
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->MidTime);
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->HighTime);
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->LowWaitingTime);
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->MidWaitingTime);
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->HighWaitinghTime);
		pItem->LimitWaitingTime = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->WinTimesLimit);
		
		for (int i = 0; i < 8; i++)
		{
			ArenaWinDrop winDrop;
			CConfAnalytic::ToJsonInt(loader.NextStr(), winDrop.WinLevelLimit);
			CConfAnalytic::ToJsonInt(loader.NextStr(), winDrop.DropId);
			if (winDrop.WinLevelLimit.size() > 0)
			{
				pItem->WinDrops.push_back(winDrop);
			}
		}

		pItem->ActivityChestRate = loader.NextInt();
		pItem->ChestRefreshTime = loader.NextInt();
		pItem->CriticalPoint = loader.NextInt();
		
		int chestOrder = loader.NextInt();
		//配表有100项
		while (chestOrder != 0)
		{
			pItem->ChestOrders.push_back(chestOrder);
			chestOrder = loader.NextInt();
		}
		m_Datas[0] = pItem;
		break;
	}
	return true;
}

bool CConfArenaChest::LoadCSV(const std::string& str) 
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	while (loader.NextLine())
	{
		ArenaChestItem *pItem = new ArenaChestItem;
		pItem->ChestId = loader.NextInt();
		loader.NextStr();
		pItem->ChestType = loader.NextInt();
		pItem->ChestLevel = loader.NextInt();
		pItem->ChestQuality = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), pItem->ChestAppearLevel);
		pItem->ChestWeight = loader.NextInt();
		pItem->ChestName = loader.NextInt();
		pItem->ChestDesc = loader.NextInt();
		pItem->ChestIcon1 = loader.NextStr();
		pItem->ChestIcon2 = loader.NextStr();
		pItem->ChestDropId = loader.NextInt();
		m_Datas[pItem->ChestId] = pItem;
		// 还需要处理一下品质列表
		m_Chests[pItem->ChestType][pItem->ChestLevel].push_back(pItem);
	}
	return true;
}

const std::vector<ArenaChestItem*>& CConfArenaChest::getAreanChest(int type, int level)
{
	return m_Chests[type][level];
}

bool CConfArenaLevel::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));

	while (loader.NextLine())
	{
		int arenaMode = loader.NextInt();
		loader.NextStr();
		int unlockLv = loader.NextInt();

		m_mapArenaLevel[arenaMode] = unlockLv;
	}

	return true;
}

int CConfArenaLevel::getUnlockLevel(int mode)
{
	std::map<int, int>::iterator iter = m_mapArenaLevel.find(mode);
	if (iter != m_mapArenaLevel.end())
	{
		return iter->second;
	}

	return 0;
}
