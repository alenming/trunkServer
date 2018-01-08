#include "ConfActive.h"
#include "ConfAnalytic.h"

using namespace std;

CConfActiveTime::CConfActiveTime()
{
	//StringSplit();
}


CConfActiveTime::~CConfActiveTime()
{
}

bool CConfActiveTime::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	//如果有数据 
	while (loader.NextLine())
	{
		SConfActiveTime *pConfActiveTime = new SConfActiveTime;

		int nActiveID = loader.NextInt();
		pConfActiveTime->nActiveType = loader.NextInt();
        pConfActiveTime->nTitleLanguageID = loader.NextInt();
		pConfActiveTime->LvLimit = loader.NextInt();
		pConfActiveTime->nTimeType = loader.NextInt();
		pConfActiveTime->nStartTime = loader.NextInt();
		pConfActiveTime->nEndTime = loader.NextInt();
		pConfActiveTime->szIcon = loader.NextStr();
        pConfActiveTime->nButonLanguageID = loader.NextInt();

		m_Datas[nActiveID] = pConfActiveTime;
	}

	return true;
}

CConfActiveShop::CConfActiveShop()
{

}

CConfActiveShop::~CConfActiveShop()
{

}

bool CConfActiveShop::LoadCSV(const std::string& str)
{
	m_MapActiveShop.clear();
	m_MapActiveShopList.clear();
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	//如果有数据 
	while (loader.NextLine())
	{
		SConfActiveShopData ConfActiveShop;
		SConfActiveShopKey ShopKey;

		ShopKey.nActiveID = loader.NextInt();
		ShopKey.nGiftID = loader.NextInt();
		ConfActiveShop.nGiftID = ShopKey.nGiftID;
		
		VecInt Value;

		for (int i = 0; i < 4; i++)
		{
			CConfAnalytic::ToJsonInt(loader.NextStr(), Value);

            if ((int)Value.size() != 2)
			{
				continue;
			}

			ConfActiveShop.nGoodsID[i] = Value[0];
			ConfActiveShop.nGoodsNum[i] = Value[1];
			Value.clear();
		}

		CConfAnalytic::ToJsonInt(loader.NextStr(), Value);

        if ((int)Value.size() != 2)
		{
			return false;
		}

		ConfActiveShop.nGoldType = Value[0];
		ConfActiveShop.nPrice = Value[1];

		ConfActiveShop.nSaleRate = loader.NextInt();
		ConfActiveShop.nMaxBuyTimes = loader.NextInt();

		m_MapActiveShop[ShopKey] = ConfActiveShop;

		std::map<int, std::vector<SConfActiveShopData> >::iterator ator = m_MapActiveShopList.find(ShopKey.nActiveID);

		if (ator == m_MapActiveShopList.end())
		{
			std::vector<SConfActiveShopData> VectShopData;
			VectShopData.push_back(ConfActiveShop);
			m_MapActiveShopList[ShopKey.nActiveID] = VectShopData;
		}
		else
		{
			std::vector<SConfActiveShopData> &VectShopData = ator->second;
			VectShopData.push_back(ConfActiveShop);
		}
	}

	return true;
}

SConfActiveShopData* CConfActiveShop::GetActiveShopData(int nActiveID, int nGiftID)
{
	SConfActiveShopKey ShopKey;
	ShopKey.nActiveID = nActiveID;
	ShopKey.nGiftID = nGiftID;

	map<SConfActiveShopKey, SConfActiveShopData>::iterator ator = m_MapActiveShop.find(ShopKey);

	if (ator == m_MapActiveShop.end())
	{
		return NULL;
	}

	return &(ator->second);
}

std::vector<SConfActiveShopData>* CConfActiveShop::GetActiveVector(int nActiveID)
{
	std::map<int, std::vector<SConfActiveShopData> >::iterator ator = m_MapActiveShopList.find(nActiveID);

	if (ator == m_MapActiveShopList.end())
	{
		return NULL;
	}
	else
	{
		return &ator->second;
	}
}

CConfActiveExtraAdd::CConfActiveExtraAdd()
{

}

CConfActiveExtraAdd::~CConfActiveExtraAdd()
{

}

bool CConfActiveExtraAdd::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	//如果有数据 
	while (loader.NextLine())
	{
		SConfActiveExtraAdd *pDropData = new SConfActiveExtraAdd;
		int nActiveID = loader.NextInt();
		pDropData->ActivePic = loader.NextStr();
		pDropData->nLanguageID = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), pDropData->VectDropID);
		pDropData->nDropType = loader.NextInt();

		vector<string> VectStr;
		CConfAnalytic::StringSplit(loader.NextStr(),VectStr,"*");

		for (int i = 0; i < (int)VectStr.size(); i++)
		{
			VecFloat Value;
			CConfAnalytic::ToJsonFloat(VectStr[i], Value);

            if ((int)Value.size() != 4)
			{
				return false;
			}

			ExtraDropData Data;
			Data.nGoodsID = static_cast<int>(Value[0]);
			Data.nNumMinRate = static_cast<int>(Value[1]);
			Data.nNumMaxRate = static_cast<int>(Value[2]);
			Data.fDropRateAdd = Value[3];

			pDropData->MapDropChange[Data.nGoodsID] = Data;
		}

		VectStr.clear();
		CConfAnalytic::StringSplit(loader.NextStr(), VectStr, "*");

		for (int i = 0; i < (int)VectStr.size(); i++)
		{
			VecFloat Value;
			CConfAnalytic::ToJsonFloat(VectStr[i], Value);

			if (Value.size() != 4)
			{
				return false;
			}

			ExtraDropData Data;
			Data.nGoodsID = static_cast<int>(Value[0]);
			Data.nNumMinRate = static_cast<int>(Value[1]);
			Data.nNumMaxRate = static_cast<int>(Value[2]);
			Data.fDropRateAdd = Value[3];

			pDropData->MapAddDropType[Data.nGoodsID] = Data;
		}

		VectStr.clear();
		CConfAnalytic::StringSplit(loader.NextStr(), VectStr, "*");

		for (int i = 0; i < (int)VectStr.size(); i++)
		{
			VecFloat Value;
			CConfAnalytic::ToJsonFloat(VectStr[i], Value);

            if ((int)Value.size() != 4)
			{
				return false;
			}

			ExtraDropData Data;
			Data.nGoodsID = static_cast<int>(Value[0]);
			Data.nNumMinRate = static_cast<int>(Value[1]);
			Data.nNumMaxRate = static_cast<int>(Value[2]);
			Data.fDropRateAdd = Value[3];

			pDropData->MapAddDropExtra[Data.nGoodsID] = Data;
		}
		
		VecInt nValue;
		CConfAnalytic::ToJsonInt(loader.NextStr(), nValue);

        if ((int)nValue.size() == 2)
		{
			pDropData->nDropNumMin = nValue[0];
			pDropData->nDropNumMax = nValue[1];
		}
		else
		{
			pDropData->nDropNumMin = 0;
			pDropData->nDropNumMax = 0;
		}
		m_Datas[nActiveID] = pDropData;
	}

	return true;
}

CConfActiveTask::CConfActiveTask()
{

}
CConfActiveTask::~CConfActiveTask()
{

}

bool CConfActiveTask::LoadCSV(const std::string& str)
{
	m_MapActiveTask.clear();
	m_MapActiveCondition.clear();
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	//如果有数据 
	while (loader.NextLine())
	{
		SConfActiveTaskKey ActiveTaskKey;
		SConfActiveTaskData ActiveTaskData;
		
		ActiveTaskKey.nActiveID = loader.NextInt();
		ActiveTaskKey.nTaskID = loader.NextInt();
		loader.NextStr();

		ActiveTaskData.ActiveIcon = loader.NextStr();
		ActiveTaskData.nActiveLanguangeID = loader.NextInt();
		ActiveTaskData.nFinishCondition = loader.NextInt();
		ActiveTaskData.nConditionParam[0] = loader.NextInt();
		ActiveTaskData.nConditionParam[1] = loader.NextInt();
		ActiveTaskData.nRewardDimand = loader.NextInt();
		ActiveTaskData.nRewardGold = loader.NextInt();

		vector<string> VectStr;
		CConfAnalytic::StringSplit(loader.NextStr(), VectStr, "*");
		memset(ActiveTaskData.nRewardGoodsID, 0, sizeof(ActiveTaskData.nRewardGoodsID));
		memset(ActiveTaskData.nRewardGoodsNum, 0, sizeof(ActiveTaskData.nRewardGoodsNum));

		for (int i = 0; i < (int)VectStr.size(); i++)
		{
			VecInt Value;
			CConfAnalytic::ToJsonInt(VectStr[i], Value);

			if ((int)Value.size() != 2)
			{
				return false;
			}
			ActiveTaskData.nRewardGoodsID[i] = Value[0];
			ActiveTaskData.nRewardGoodsNum[i] = Value[1];
		}
        
		ActiveTaskData.nRewardEnergy = loader.NextInt();
		ActiveTaskData.ActivePic = loader.NextStr();
		ActiveTaskData.szGoTo = loader.NextStr();
		ActiveTaskData.nActiveTaskType = loader.NextInt();

		m_MapActiveTask[ActiveTaskKey] = ActiveTaskData;
		m_MapActiveCondition[ActiveTaskKey.nActiveID] = ActiveTaskData.nFinishCondition;

		map<int, map<int, SConfActiveTaskData> >::iterator ator = m_MapActiveDataByActiveID.find(ActiveTaskKey.nActiveID);

		if (ator == m_MapActiveDataByActiveID.end())
		{
			map<int, SConfActiveTaskData> MapTaskData;
			MapTaskData[ActiveTaskKey.nTaskID] = ActiveTaskData;
			m_MapActiveDataByActiveID[ActiveTaskKey.nActiveID] = MapTaskData;
		}
		else
		{
			map<int, SConfActiveTaskData> &MapTaskData = ator->second;
			MapTaskData[ActiveTaskKey.nTaskID] = ActiveTaskData;
		}
	}

	return true;
}

int CConfActiveTask::GetTaskActiveCondition(int nActiveID)
{
	std::map<int, int>::iterator ator = m_MapActiveCondition.find(nActiveID);

	if (ator == m_MapActiveCondition.end())
	{
		return 0;
	}

	return ator->second;
}

SConfActiveTaskData* CConfActiveTask::GetTaskActiveTaskData(int nActiveID, int nActiveTaskID)
{
	SConfActiveTaskKey ActiveTaskKey;
	ActiveTaskKey.nActiveID = nActiveID;
	ActiveTaskKey.nTaskID = nActiveTaskID;

	map<SConfActiveTaskKey, SConfActiveTaskData>::iterator ator = m_MapActiveTask.find(ActiveTaskKey);

	if (ator == m_MapActiveTask.end())
	{
		return NULL;
	}
	
	return &(ator->second);
}

map<int, SConfActiveTaskData>* CConfActiveTask::GetTaskActiveTaskDataByActiveID(int nActiveID)
{
	map<int, map<int, SConfActiveTaskData> >::iterator ator = m_MapActiveDataByActiveID.find(nActiveID);

	if (ator == m_MapActiveDataByActiveID.end())
	{
		return NULL;
	}

	return &ator->second;
}

bool CConf7DayActive::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	//如果有数据 
	while (loader.NextLine())
	{
		S7DayActive *pData = new S7DayActive;
		int nDay = loader.NextInt();
		loader.NextStr();
		loader.NextStr();
		loader.NextStr();
		VecInt nValue;
		CConfAnalytic::ToJsonInt(loader.NextStr(), nValue);
		if (nValue.size() != 2)
		{
			return false;
		}

		pData->nStartDay = nValue[0];
		pData->nEndDay = nValue[1];

		for (int i = 0; i < 3; i++)
		{
			S7DayActiveData DayActiveData;
			DayActiveData.nActiveType = loader.NextInt();
			DayActiveData.nActiveID = loader.NextInt();
			pData->VectActive.push_back(DayActiveData);
		}

		m_Datas[nDay] = pData;
	}

	return true;
}

bool CConfMonthActive::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	//如果有数据 
	while (loader.NextLine())
	{
		SConfActiveTaskKey Key;
		SConfMonthActiveData ActiveData;

		Key.nActiveID = loader.NextInt();
		Key.nTaskID = loader.NextInt();
		ActiveData.sMonthPic = loader.NextStr();
		ActiveData.nDayLimit = loader.NextInt();
		ActiveData.nDiamondReward = loader.NextInt();
		ActiveData.nPid = loader.NextInt();
		m_MapActiveMonth[Key] = ActiveData;

		map<int,map<int, SConfMonthActiveData> >::iterator ator = m_MapActiveDataByActiveID.find(Key.nActiveID);
		if (ator == m_MapActiveDataByActiveID.end())
		{
			map<int, SConfMonthActiveData> MapActiveData;
			MapActiveData[Key.nTaskID] = ActiveData;
			m_MapActiveDataByActiveID[Key.nActiveID] = MapActiveData;
		}
		else
		{
			map<int, SConfMonthActiveData> &MapActiveData = ator->second;
			MapActiveData[Key.nTaskID] = ActiveData;
		}
	}

	return true;
}

SConfMonthActiveData * CConfMonthActive::getConfMonthData(int nActiveID, int nMonthCardID)
{
	SConfActiveTaskKey Key;
	Key.nActiveID = nActiveID;
	Key.nTaskID = nMonthCardID;
	std::map<SConfActiveTaskKey, SConfMonthActiveData>::iterator ator = m_MapActiveMonth.find(Key);

	if (ator == m_MapActiveMonth.end())
	{
		return NULL;
	}

	return &ator->second;
}

std::map<int, SConfMonthActiveData>* CConfMonthActive::GetActiveMonthDataByActiveID(int nActiveID)
{
	std::map<int, std::map<int, SConfMonthActiveData> >::iterator ator = m_MapActiveDataByActiveID.find(nActiveID);

	if (ator == m_MapActiveDataByActiveID.end())
	{
		return NULL;
	}

	return &ator->second;
}

bool CConfExChangeActive::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str, 3));
	//如果有数据 
	while (loader.NextLine())
	{
		SConfActiveTaskKey Key;
		SConfExChangeActiveData ExChangeData;

		Key.nActiveID = loader.NextInt();
		Key.nTaskID = loader.NextInt();

		vector<string> VectStr;
		CConfAnalytic::StringSplit(loader.NextStr(), VectStr, "*");
		for (int i = 0; i < (int)VectStr.size(); i++)
		{
			VecInt Value;
			CConfAnalytic::ToJsonInt(VectStr[i], Value);

			if ((int)Value.size() != 2)
			{
				return false;
			}

			ExChangeData.VecGoods[0].push_back(Value[0]);
			ExChangeData.VecGoods[1].push_back(Value[1]);
		}
		ExChangeData.nTimes = loader.NextInt();
		loader.NextStr();
		VectStr.clear();
		CConfAnalytic::StringSplit(loader.NextStr(), VectStr, "*");
		for (int i = 0; i < (int)VectStr.size(); i++)
		{
			VecInt Value;
			CConfAnalytic::ToJsonInt(VectStr[i], Value);

			if ((int)Value.size() != 2)
			{
				return false;
			}

			ExChangeData.VecRewardGoods[0].push_back(Value[0]);
			ExChangeData.VecRewardGoods[1].push_back(Value[1]);
		}

		m_MapActiveExChange[Key] = ExChangeData;

		map<int, map<int, SConfExChangeActiveData> >::iterator ator = m_MapActiveExChangeByActiveID.find(Key.nActiveID);
		if (ator == m_MapActiveExChangeByActiveID.end())
		{
			map<int, SConfExChangeActiveData> MapActiveData;
			MapActiveData[Key.nTaskID] = ExChangeData;
			m_MapActiveExChangeByActiveID[Key.nActiveID] = MapActiveData;
		}
		else
		{
			map<int, SConfExChangeActiveData> &MapActiveData = ator->second;
			MapActiveData[Key.nTaskID] = ExChangeData;
		}
	}

	return true;
}

SConfExChangeActiveData * CConfExChangeActive::getConfExChangeData(int nActiveID, int nTaskID)
{
	SConfActiveTaskKey Key;
	Key.nActiveID = nActiveID;
	Key.nTaskID = nTaskID;
	std::map<SConfActiveTaskKey, SConfExChangeActiveData>::iterator ator = m_MapActiveExChange.find(Key);

	if (ator == m_MapActiveExChange.end())
	{
		return NULL;
	}

	return &ator->second;
}

std::map<int, SConfExChangeActiveData>* CConfExChangeActive::GetActiveExChangeyActiveID(int nActiveID)
{
	std::map<int, std::map<int, SConfExChangeActiveData> >::iterator ator = m_MapActiveExChangeByActiveID.find(nActiveID);

	if (ator == m_MapActiveExChangeByActiveID.end())
	{
		return NULL;
	}

	return &ator->second;
}
