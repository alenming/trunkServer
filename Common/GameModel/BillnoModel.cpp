#include "BillnoModel.h"
#include "RedisStorer.h"
#include "StorageManager.h"
#include "ModelDef.h"
#include "GameDef.h"

using namespace std;

CBillnoModel * CBillnoModel::m_pInstance = NULL;

// 初始化
bool CBillnoModel::init()
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_USER);
	if (NULL == m_pStorage)
	{
		return false;
	}

	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));

	if (redisStorer == NULL)
	{
		return false;
	}

	string BillnoKey = ModelKey::UsrBillNoKey();

	if (SUCCESS != redisStorer->ExistKey(BillnoKey))
	{
		if (SUCCESS != redisStorer->SetHashByField(BillnoKey, 0,0))
		{
			return false;
		}
	}

	return true;
}

// 刷新关卡数据
bool CBillnoModel::Refresh()
{
	return true;
}

bool CBillnoModel::IncBillnoData(int &num)
{
	if (m_pStorage == NULL)
	{
		return false;
	}

	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));

	if (redisStorer == NULL)
	{
		return false;
	}

	string BillnoKey = ModelKey::UsrBillNoKey();

	if (SUCCESS != redisStorer->IncreHashByField(BillnoKey, 0, num))
	{
		num = -1;
		return false;
	}

	return true;
}

CBillnoModel * CBillnoModel::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CBillnoModel;
	}

	return m_pInstance;
}

void CBillnoModel::destroy()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CBillnoModel::CBillnoModel()
{
}


CBillnoModel::~CBillnoModel()
{
}
