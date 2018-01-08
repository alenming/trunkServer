#include "EquipModel.h"
#include "RedisStorer.h"
#include "Storage.h"
#include "StorageManager.h"
#include "ModelDef.h"
#include "ConfAnalytic.h"
#include "KXServer.h"

using namespace std;

CEquipModel::CEquipModel()
{

}

CEquipModel::~CEquipModel()
{

}

bool CEquipModel::init(int uid)
{
	m_nUid = uid;
	m_szKey = ModelKey::EquipKey(uid);
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_EQUIP);
	if (m_pStorage == NULL)
	{
		return false;
	}
	Refresh();
	return true;
}

bool CEquipModel::Refresh()
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		map<string, string> MapEquipInfo;
		if (SUCCESS == pStorer->GetHash(m_szKey, MapEquipInfo))
		{
			for (map<string, string>::iterator ator = MapEquipInfo.begin(); ator != MapEquipInfo.end(); ++ator)
			{
				VecInt VectValue;
				CConfAnalytic::ToJsonInt(ator->second, VectValue);

				if (VectValue.size() != 18)
				{
					continue;
				}

				SEquipInfo EquipData;

				EquipData.nItemID = VectValue[0];
				EquipData.cMainPropNum = VectValue[1];
				EquipData.nDnycEquipID = atoi(ator->first.c_str());

				for (int i = 0; i < MAX_EQUIP_EFFECT_NUM; i++)
				{
					EquipData.cEffectID[i] = VectValue[2 + i * 2];
					EquipData.sEffectValue[i] = VectValue[3 + i * 2];
				}

				m_EquipsInfo[EquipData.nDnycEquipID] = EquipData;
			}

			return true;
		}
	}

	return false;
}

std::map<int, SEquipInfo>& CEquipModel::GetEquipsInfo()
{
	return m_EquipsInfo;
}

bool CEquipModel::AddEquip(SEquipInfo &EquipData)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		char szBuffer[256] = {};
		int nLen = 0;
		int nMaxLen = sizeof(szBuffer);
		nLen = snprintf(szBuffer,nMaxLen,"[%d+%d", EquipData.nItemID,EquipData.cMainPropNum);
		for (int i = 0; i < MAX_EQUIP_EFFECT_NUM; i++)
		{
			nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "+%d+%d", EquipData.cEffectID[i], EquipData.sEffectValue[i]);
		}
		nLen += snprintf(szBuffer + nLen, nMaxLen - nLen, "]");

		if (SUCCESS == pStorer->SetHashByField(m_szKey, EquipData.nDnycEquipID, szBuffer, nLen))
        {
			m_EquipsInfo[EquipData.nDnycEquipID] = EquipData;
            return true;
        }
	}
	return false;
}

bool CEquipModel::RemoveEquip(int equipId)
{
	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL != pStorer)
	{
		std::map<int,SEquipInfo>::iterator iter = m_EquipsInfo.find(equipId);
		if (iter != m_EquipsInfo.end())
		{
			if (SUCCESS == pStorer->DelHashByField(m_szKey, equipId))
			{
				m_EquipsInfo.erase(iter);
				return true;
			}
		}
	}
	return false;
}

SEquipInfo* CEquipModel::GetEquipData(int equipId, bool bNew)
{
	std::map<int, SEquipInfo>::iterator ator = m_EquipsInfo.find(equipId);

	if (ator != m_EquipsInfo.end())
	{
		if (bNew)
		{
			SEquipInfo EquipData;
			if (!GetRealEquipDataFromDB(equipId, &EquipData))
			{
				return NULL;
			}

			memcpy(&ator->second, &EquipData, sizeof(SEquipInfo));
		}

		return &m_EquipsInfo[equipId];
	}
	else
	{
		SEquipInfo EquipData;
		if(!GetRealEquipDataFromDB(equipId, &EquipData))
		{
			return NULL;
		}
		else
		{
			m_EquipsInfo[equipId] = EquipData;
			return &m_EquipsInfo[equipId];
		}
	}

	return NULL;
}

bool CEquipModel::GetRealEquipDataFromDB(int equipId, SEquipInfo* pEquipInfo)
{
	if (pEquipInfo == NULL)
	{
		return false;
	}

	CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(m_nUid));
	if (NULL == pStorer)
	{
		return false;
	}

	string SStrEqupInfo;

	if (SUCCESS != pStorer->GetHashByField(m_szKey, equipId, SStrEqupInfo))
	{
		return false;
	}

	VecInt VectValue;
	CConfAnalytic::ToJsonInt(SStrEqupInfo,VectValue);

	if (VectValue.size() != 18)
	{
		return false;
	}

	pEquipInfo->nItemID = VectValue[0];
	pEquipInfo->cMainPropNum = VectValue[1];
	pEquipInfo->nDnycEquipID = equipId;

	for (int i = 0; i < MAX_EQUIP_EFFECT_NUM; i++)
	{
		pEquipInfo->cEffectID[i] = VectValue[2 + i * 2];
		pEquipInfo->sEffectValue[i] = VectValue[3 + i * 2];
	}

	return true;
}
