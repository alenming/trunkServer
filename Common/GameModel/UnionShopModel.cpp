#include "UnionShopModel.h"
#include "StorageManager.h"
#include "ModelDef.h"

using namespace std;

CUnionShopModel::CUnionShopModel()
{
}


CUnionShopModel::~CUnionShopModel()
{
}

bool CUnionShopModel::init(int unionID)
{
	if (unionID == 0)
	{
		return false;
	}

	Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_SERVER);
	if (pStorage == NULL)
	{
		return false;
	}

	m_pStorage = pStorage;
	m_UnionShopKey = ModelKey::UnionShopKey(unionID);

	return Refresh();
}
// 刷新公会数据
bool CUnionShopModel::Refresh()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pStorer == NULL)
	{
		return false;
	}

	pStorer->GetHash(m_UnionShopKey, m_UnionShopGoods);
	return true;
}

//设置商店物品个数
bool CUnionShopModel::setUnionShopGoodsData(int nShopGoodsID, int &nGoodsNum)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pStorer == NULL)
	{
		return false;
	}

	map<int, int>::iterator ator = m_UnionShopGoods.find(nShopGoodsID);

	if (ator == m_UnionShopGoods.end())
	{
		m_UnionShopGoods[nShopGoodsID] = nGoodsNum;
	}

	if (SUCCESS != pStorer->SetHashByField(m_UnionShopKey, nShopGoodsID, nGoodsNum))
	{
		return false;
	}

	return true;
}

//获取商店物品个数
bool CUnionShopModel::getUnionShopGoodsData(int nShopGoodsID, int &nGoodsNum)
{
	map<int, int>::iterator ator = m_UnionShopGoods.find(nShopGoodsID);

	if (ator == m_UnionShopGoods.end())
	{
		nGoodsNum = 0;
		return false;
	}

	nGoodsNum = ator->second;
	return true;
}

//增加商店物品个数
bool CUnionShopModel::incrUnionShopGoodsNum(int nShopGoodsID, int nAddNum)
{
	if (nAddNum == 0)
	{
		return false;
	}

	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pStorer == NULL)
	{
		return false;
	}

	int val = nAddNum;
	if (SUCCESS != pStorer->IncreHashByField(m_UnionShopKey, nShopGoodsID, val))
	{
		return false;
	}

	//如果为负数，还原
	if (val < 0)
	{
		val = -1 * nAddNum;
		if (SUCCESS != pStorer->IncreHashByField(m_UnionShopKey, nShopGoodsID, val))
		{
			return false;
		}

		return false;
	}

	m_UnionShopGoods[nShopGoodsID] = val;
	return true;
}

//清空商品列表
bool CUnionShopModel::clearUnionShop()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pStorer == NULL)
	{
		return false;
	}

	if (SUCCESS != pStorer->DelKey(m_UnionShopKey))
	{
		return false;
	}
	m_UnionShopGoods.clear();
	return true;
}

// 批量处理
bool CUnionShopModel::setUnionShop(std::map<int, int> &kvs)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pStorer == NULL)
	{
		return false;
	}

	if (SUCCESS != pStorer->DelKey(m_UnionShopKey))
	{
		return false;
	}
	m_UnionShopGoods.clear();

	if (SUCCESS != pStorer->SetHash(m_UnionShopKey, kvs))
	{
		return false;
	}

	std::map<int, int>::iterator iter = kvs.begin();
	for (; iter != kvs.end(); ++iter)
	{
		m_UnionShopGoods[iter->first] = iter->second;
	}

	return true;
}


