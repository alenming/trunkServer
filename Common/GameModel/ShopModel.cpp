#include "ShopModel.h"
#include "ModelDef.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ConfAnalytic.h"
#include "ConfGameSetting.h"
#include <time.h>
#include "ModelHelper.h"
#include "KXServer.h"

using namespace std;

CShopModel::CShopModel()
{
	for (map<int, ShopGoodsMap>::iterator ator = m_MapShopGoods.begin(); ator != m_MapShopGoods.end(); ++ator)
	{
		ator->second.clear();
	}
}


CShopModel::~CShopModel()
{
}

bool CShopModel::init(int uid)
{
	m_nUid = uid;
	m_szKey = ModelKey::ShopGoodsKey(m_nUid);
	m_szShopIDKey = ModelKey::ShopIDKey(m_nUid);
	m_ShopInfoKey = ModelKey::ShopInfoKey(m_nUid);
    m_ShopDiamondKey = ModelKey::pidKey(m_nUid);
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGE_SHOP);
	CHECK_RETURN(m_pStorage != NULL);

	Refresh();
	return true;
}

bool CShopModel::Refresh()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == pStorer)
	{
		return false;
	}

	m_MapShopGoods.clear();
	m_MapShopIDInfo.clear();
	m_MapShopInfo.clear();
	m_MapDiamond.clear();
	map<string, string> MapShopInfo;
	MapShopInfo.clear();
	SShopInfo Info;

	if (SUCCESS == pStorer->GetHash(m_szShopIDKey, MapShopInfo))
	{
		for (map<string, string>::iterator ator = MapShopInfo.begin(); ator != MapShopInfo.end(); ++ator)
		{
			int nShopID = atoi(ator->first.c_str());
			VecInt VectValue;
			CConfAnalytic::ToJsonInt(ator->second, VectValue);
			Info.nTimes = VectValue[0];
			Info.nFreshEndTime = VectValue[1];
			m_MapShopIDInfo[nShopID] = Info;
		}
	}
	
	if (SUCCESS != pStorer->ExistKey(m_ShopInfoKey))
	{
		SetFieldValue(enFreshStartTime, 0);
	}
	else
	{
		CHECK_RETURN(SUCCESS == pStorer->GetHash(m_ShopInfoKey, m_MapShopInfo));
	}

	pStorer->GetHash(m_ShopDiamondKey, m_MapDiamond);
	//获取商店数据
	MapShopInfo.clear();
	if (SUCCESS == pStorer->GetHash(m_szKey, MapShopInfo))
	{
		int nTimes = 0;
		int nFreshEndTime = 0;
		for (map<string, string>::iterator ator = MapShopInfo.begin(); ator != MapShopInfo.end(); ++ator)
		{
			int nShopID = atoi(ator->first.c_str());

			GetShopInfo(nShopID, nTimes, nFreshEndTime);
			CShopData *conf = dynamic_cast<CShopData *>(CConfManager::getInstance()->getConf(CONF_SHOP));
			CHECK_CONTINUE(conf != NULL);
			ShopConfigData *pData = static_cast<ShopConfigData *>(conf->getData(nShopID));
			CHECK_CONTINUE(pData != NULL);

			VecInt VectValue;
			CConfAnalytic::ToJsonInt(ator->second, VectValue);
			int			nIndex;						//商品在商店内的索引
			SShopGoods ShopGoods;
			ShopGoodsMap GoodsMap;

			for (int i = 0; i < (int)(VectValue.size()) / 3; i++)
			{
				nIndex = VectValue[i * 3];
				ShopGoods.nShopGoodsID = VectValue[1 + i * 3];
				ShopGoods.nGoodsNum = VectValue[2 + i * 3];

				if (GoodsMap.find(nIndex) == GoodsMap.end())
				{
					GoodsMap[nIndex] = ShopGoods;
				}
			}

			m_MapShopGoods[nShopID] = GoodsMap;
		}
	}

	return true;
}

bool CShopModel::SetShopNum(int ShopID, int nIndex, int ShopNum)
{
	map<int, ShopGoodsMap>::iterator ator = m_MapShopGoods.find(ShopID);

	if (ator == m_MapShopGoods.end())
	{
		return false;
	}

	ShopGoodsMap & MapGoods = ator->second;

	ShopGoodsMap::iterator iter = MapGoods.find(nIndex);
	CHECK_RETURN(iter != MapGoods.end());

	iter->second.nGoodsNum = ShopNum;

	if (!SaveShopToDB(ShopID))
	{
		return false;
	}

	return true;
}

bool CShopModel::GetShopGoodsInfo(int ShopID, int nIndex, int &nShopGoodsID, int &ShopNum)
{
	map<int, ShopGoodsMap>::iterator ator = m_MapShopGoods.find(ShopID);
	CHECK_RETURN(ator != m_MapShopGoods.end());

	ShopGoodsMap & MapGoods = ator->second;
	ShopGoodsMap::iterator iter = MapGoods.find(nIndex);
	CHECK_RETURN(iter != MapGoods.end())
	
	ShopNum = iter->second.nGoodsNum;
	nShopGoodsID = iter->second.nShopGoodsID;
	return true;
}

//刷新商店
bool CShopModel::AddShopData(int ShopID, ShopGoodsMap &GoodsMap)
{
	//有数据清除数据
	std::map<int, ShopGoodsMap>::iterator ator = m_MapShopGoods.find(ShopID);

	if (ator != m_MapShopGoods.end())
	{
		ator->second.clear();
		m_MapShopGoods.erase(ShopID);
	}

	m_MapShopGoods[ShopID] = GoodsMap;

	if (!SaveShopToDB(ShopID))
	{
		return false;
	}
	return true;
}

bool CShopModel::SetShopInfo(int nShopID, int nTimes, int nFreshTime)
{
	map<int, SShopInfo>::iterator ator = m_MapShopIDInfo.find(nShopID);
	if (ator != m_MapShopIDInfo.end())
	{
		ator->second.nTimes = nTimes;
		ator->second.nFreshEndTime = nFreshTime;
	}
	else
	{
		SShopInfo Info;
		Info.nTimes = nTimes;
		Info.nFreshEndTime = nFreshTime;
		m_MapShopIDInfo[nShopID] = Info;
	}

	if (!SaveShopIDToDB(nShopID))
	{
		return false;
	}

	return true;
}

bool CShopModel::GetShopInfo(int nShopID, int &nTimes, int &nFreshTime)
{
	map<int, SShopInfo>::iterator ator = m_MapShopIDInfo.find(nShopID);
	CHECK_RETURN(ator != m_MapShopIDInfo.end());

	nTimes = ator->second.nTimes;
	nFreshTime = ator->second.nFreshEndTime;
	return true;
}

ShopGoodsMap* CShopModel::GetShopData(int nShopID)
{
	std::map<int, ShopGoodsMap>::iterator ator = m_MapShopGoods.find(nShopID);
	CHECK_RETURN_NULL(ator != m_MapShopGoods.end());
	
	return &ator->second;
}

bool CShopModel::SetFieldValue(int nField, int nValue)
{
	m_MapShopInfo[nField] = nValue;

	return SaveShopInfoToDB(nField, nValue);
}

bool CShopModel::GetFieldValue(int nField, int &nValue)
{
	std::map<int, int>::iterator ator = m_MapShopInfo.find(nField);

	if (ator != m_MapShopInfo.end())
	{
		nValue = ator->second;
		return true;
	}

	nValue = 0;
	return false;
}

bool CShopModel::SaveShopToDB(int ShopID)
{
	map<int, ShopGoodsMap>::iterator ator = m_MapShopGoods.find(ShopID);

	if (ator == m_MapShopGoods.end())
	{
		return false;
	}

	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == pStorer)
	{
		return false;
	}

	ShopGoodsMap &GoodsMap = ator->second;
	memset(m_szBuffer, 0, sizeof(m_szBuffer));
	int nLen = snprintf(m_szBuffer,sizeof(m_szBuffer),"[");

	for (ShopGoodsMap::iterator iter = GoodsMap.begin(); iter != GoodsMap.end(); ++iter)
	{
		const int &nIndex = iter->first;
		SShopGoods &Goods = iter->second;

		if (iter == GoodsMap.begin())
		{
			nLen += snprintf(m_szBuffer + nLen, sizeof(m_szBuffer)-nLen, "%d+%d+%d", nIndex, Goods.nShopGoodsID, Goods.nGoodsNum);
		}
		else
		{
			nLen += snprintf(m_szBuffer + nLen, sizeof(m_szBuffer)-nLen, "+%d+%d+%d", nIndex, Goods.nShopGoodsID, Goods.nGoodsNum);
		}
	}

	nLen += snprintf(m_szBuffer + nLen, sizeof(m_szBuffer)-nLen, "]");

	if (SUCCESS == pStorer->SetHashByField(m_szKey, ShopID,m_szBuffer, nLen))
	{
		return true;
	}

	return false;
}

bool CShopModel::SaveShopIDToDB(int ShopID)
{
	map<int, SShopInfo>::iterator ator = m_MapShopIDInfo.find(ShopID);

	if (ator == m_MapShopIDInfo.end())
	{
		return false;
	}

	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == pStorer)
	{
		return false;
	}

	memset(m_szBuffer, 0, sizeof(m_szBuffer));
	int nLen = snprintf(m_szBuffer,sizeof(m_szBuffer),"[%d+%d]",ator->second.nTimes,ator->second.nFreshEndTime);

	if (SUCCESS == pStorer->SetHashByField(m_szShopIDKey, ShopID, m_szBuffer, nLen))
	{
		return true;
	}

	return false;
}

bool CShopModel::SaveShopInfoToDB(int nField,int nValue)
{
	map<int, int>::iterator ator = m_MapShopInfo.find(nField);

	if (ator == m_MapShopInfo.end())
	{
		return false;
	}

	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == pStorer)
	{
		return false;
	}

	if (SUCCESS == pStorer->SetHashByField(m_ShopInfoKey, nField,nValue))
	{
		return true;
	}

	return false;
}
