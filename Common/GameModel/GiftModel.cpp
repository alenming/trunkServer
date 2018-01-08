#include "GiftModel.h"
#include "RedisStorer.h"
#include "Storage.h"
#include "StorageManager.h"
#include "ModelDef.h"
#include "ConfAnalytic.h"
#include "KXServer.h"

CGiftModel *	CGiftModel::m_pInstance = NULL;

CGiftModel::CGiftModel()
{
}


CGiftModel::~CGiftModel()
{
}

CGiftModel* CGiftModel::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CGiftModel();
	}

	return m_pInstance;
}

void CGiftModel::destroy()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CGiftModel::init(int nWorldID)
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGE_CHECK);
	if (m_pStorage == NULL)
	{
		return false;
	}

	m_nWorldID = nWorldID;
	return true;
}

bool CGiftModel::Refresh()
{
	return true;
}

bool CGiftModel::GetGiftData(int nUid, std::string strGift, int &nType, int &nParam, int &nGiftID, SGiftData &Data)
{
	Data.vectGoods.clear();
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pStorer == NULL)
	{
		return false;
	}

	std::string strMaskKey = ModelKey::GiftMaskKey(strGift);
	if (SUCCESS != pStorer->ExistKey(strMaskKey))
	{
		return false;
	}

	std::string strValue;
	if (SUCCESS != pStorer->GetString(strMaskKey,strValue))
	{
		return false;
	}
	VecInt VectValue;
	CConfAnalytic::ToJsonInt(strValue, VectValue);
	if (VectValue.size() != 3)
	{
		return false;
	}

	nType = VectValue[0];
	nParam = VectValue[1];
	nGiftID = VectValue[2];
	int nCode = VectValue[2];

	if (!CheckCodeIsUseFull(nUid, nCode))
	{
		return false;
	}

	//获取礼包数据
	std::string strCDKey = ModelKey::GiftKey(nCode);
	if (SUCCESS != pStorer->ExistKey(strCDKey))
	{
		return false;
	}
	std::string GiftContext;
	if (SUCCESS != pStorer->GetString(strCDKey, GiftContext))
	{
		return false;
	}

	//解析礼包数据
	std::vector<int> vectValue;
	SGoodsData GoodsData;
	CConfAnalytic::ToJsonInt(GiftContext, vectValue);
	if (vectValue.size() < 2)
	{
		return false;
	}
	for (unsigned int i = 0; i < vectValue.size() / 2; i++)
	{
		GoodsData.nGoodsID = vectValue[i * 2];
		GoodsData.nGoodsNum = vectValue[i * 2 + 1];
		Data.vectGoods.push_back(GoodsData);
	}

	return true;
}

bool CGiftModel::DelGiftData(int nUid,std::string strGift)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (NULL == pStorer)
	{
		return false;
	}

	std::string strMaskKey = ModelKey::GiftMaskKey(strGift);

	if (SUCCESS != pStorer->ExistKey(strMaskKey))
	{
		return false;
	}

	std::string strValue;
	if (SUCCESS != pStorer->GetString(strMaskKey, strValue))
	{
		return false;
	}

	VecInt VectValue;
	CConfAnalytic::ToJsonInt(strValue, VectValue);
	if (VectValue.size() != 3)
	{
		return false;
	}

	int nCode = VectValue[2];
	if (!SetCodeUseState(nUid, nCode))
	{
		return false;
	}

	if (SUCCESS != pStorer->DelKey(strMaskKey))
	{
		return false;
	}

	return true;
}

bool CGiftModel::modifyUsrPs(int nUid, int nChannelID, const char *strPs, int PsLen)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (redisStorer == NULL)
	{
		return false;
	}
	char szid[10] = { 0 };
	snprintf(szid, sizeof(szid), "%d", nUid);
	string AnySDKKey = ModelKey::AnySDKCheckKey(nChannelID, szid);
	redisStorer->SetString(AnySDKKey, strPs);
	
	string OpenKey = ModelKey::UserOpenKey(szid, nChannelID);
	if (SUCCESS != redisStorer->SetHashByField(OpenKey, 0, nUid))
	{
		return false;
	}
	
	return true;
}

//检查该礼包是否有用
bool CGiftModel::CheckCodeIsUseFull(int nUid,int nCode)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pStorer == NULL)
	{
		return false;
	}

	//检查角色是否使用过礼包码
	std::string strActorKey = ModelKey::GiftActorKey(nUid,m_nWorldID);
	std::set<int> setGift;
	if (SUCCESS != pStorer->GetSetAll(strActorKey, setGift))
	{
		return false;
	}

	if (setGift.find(nCode) != setGift.end())
	{
		return false;
	}

	return true;
}

bool CGiftModel::SetCodeUseState(int nUid, int nCode)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pStorer == NULL)
	{
		return false;
	}

	//检查角色是否使用过礼包码
	std::string strActorKey = ModelKey::GiftActorKey(nUid,m_nWorldID);

	if (SUCCESS != pStorer->SetAdd(strActorKey,nCode))
	{
		return false;
	}

	return true;
}


