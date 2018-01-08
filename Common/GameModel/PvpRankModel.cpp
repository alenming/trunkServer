#include "PvpRankModel.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "CommonHelper.h"
#include "UserModel.h"
#include "UnionModel.h"
#include "PvpModel.h"
#include "UserUnionModel.h"
#include "CommonHelper.h"
#include "TimeCalcTool.h"
#include "ConfGameSetting.h"
#include "GameDef.h"
#include <algorithm>

using namespace KxServer;
using namespace std;

bool RankDataCompare(const RankData &Data1, const RankData &Data2)
{
	if (Data1.source > Data2.source)
	{
		return true;
	}
	else if (Data1.source == Data2.source)
	{
		if (Data1.id < Data2.id)
		{
			return true;
		}
	}

	return false;
}

CPvpRankModel *	CPvpRankModel::m_pInstance = NULL;

CPvpRankModel::CPvpRankModel()
{
	clearData();
}

CPvpRankModel::~CPvpRankModel()
{
}

CPvpRankModel* CPvpRankModel::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CPvpRankModel();
	}

	return m_pInstance;
}

void CPvpRankModel::destroy()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CPvpRankModel::init(KxServer::KxBaseServer *pBaseServer)
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_PVP);
	if (NULL == m_pStorage)
	{
		return false;
	}

	m_PvpRankKey = ModelKey::PvpRankKey();
	m_ChampionKey = ModelKey::ChampionRankKey();

	m_pServer = pBaseServer;

	return true;
}

bool CPvpRankModel::Refresh()
{
	clearData();
	BuildPvpRank();
	BuildChampionRank();
	
	return true;
}

//添加角色到排行榜中
bool CPvpRankModel::AddPvpRank(int uid, int nScore)
{
	if (nScore == 0)
	{
		int nRank = 0;
		if (!GetPvpRank(uid, nRank))
		{
			return false;
		}
	}

	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	if (SUCCESS != pRedisStorer->Zadd(m_PvpRankKey, uid, nScore))
	{
		return false;
	}

	m_CurGetIndex++;

	if (m_CurGetIndex >= 100)
	{
		m_CurGetIndex = 0;

		if (SUCCESS != pRedisStorer->ZCount(m_PvpRankKey, m_CurPvpRankCount))
		{
			return false;
		}
	}

	if (m_CurPvpRankCount > MAX_RANK_NUM)
	{
		//超出1W名的去除，默认是按由小到大排序
		if (SUCCESS != pRedisStorer->ZDel(m_PvpRankKey, MAX_RANK_NUM, m_CurPvpRankCount))
		{
			return false;
		}

		m_CurPvpRankCount = MAX_RANK_NUM;
	}

	return true;
}

//添加竞标赛到排行榜中
bool CPvpRankModel::addChampionRank(int uid, int nScore)
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	int nCurExpireTime = 0;
	if (SUCCESS != pRedisStorer->TTL(m_ChampionKey, nCurExpireTime))
	{
		return false;
	}

	if (nCurExpireTime < 0)
	{
		if (SUCCESS != pRedisStorer->DelKey(m_ChampionKey))
		{
			return false;
		}

		if (SUCCESS != pRedisStorer->Zadd(m_ChampionKey, uid,nScore))
		{
			return false;
		}

		//距离下周二的时间
		int CurTime = (int)time(NULL);
		int nNextTime = CTimeCalcTool::nextTimeStamp(CurTime, 0, 0, 2);
		int nIterval = nNextTime - CurTime;
		if (nIterval < 0)
		{
			nIterval = nIterval + 7 * 24 * 3600;
		}

		if (SUCCESS != pRedisStorer->Expire(m_ChampionKey,nIterval))
		{
			return false;
		}

	}
	else
	{
		if (SUCCESS != pRedisStorer->Zadd(m_ChampionKey, uid, nScore))
		{
			return false;
		}
	}

	m_CurGetIndex++;

	if (m_CurGetIndex >= 100)
	{
		m_CurGetIndex = 0;

		if (SUCCESS != pRedisStorer->ZCount(m_ChampionKey, m_CurChampRankCount))
		{
			return false;
		}
	}

	if (m_CurChampRankCount > MAX_RANK_NUM)
	{
		//超出1W名的去除，默认是按由小到大排序
		if (SUCCESS != pRedisStorer->ZDel(m_ChampionKey, MAX_RANK_NUM, m_CurChampRankCount))
		{
			return false;
		}

		m_CurChampRankCount = MAX_RANK_NUM;
	}

	return true;
}

//获取角色排行
//Day 标识获取哪天的排行，如果为0，则是现行排行
bool CPvpRankModel::GetPvpRank(int id, int &Rank, int Day)
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	string Key;

	if (Day == 0)
	{
		Key = m_PvpRankKey;
	}
	else
	{
		Key = ModelKey::PvpBakRankKey(Day);
	}

	if (SUCCESS != pRedisStorer->ZRank(Key, id, Rank))
	{
		Rank = 0;
		return false;
	}

	return true;
}

//获取竞标赛排行
bool CPvpRankModel::getChampionRank(int id, int &Rank, int Day)
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	string Key;
	if (Day == 0)
	{
		Key = m_ChampionKey;
	}
	else
	{
		Key = ModelKey::ChampionBakRankKey(Day);
	}

	if (SUCCESS != pRedisStorer->ZRank(m_ChampionKey, id, Rank))
	{
		Rank = 0;
		return false;
	}

	return true;
}

//获取角色积分
bool CPvpRankModel::GetRankData(int id, int &Score, int Day)
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	string Key;

	if (Day == 0)
	{
		Key = m_PvpRankKey;
	}
	else
	{
		Key = ModelKey::PvpBakRankKey(Day);
	}

	if (SUCCESS == pRedisStorer->ZScore(Key, id, Score))
	{
		return true;
	}

	Score = 0;
	return false;
}

//拷贝数据到备份key中,同时删除无效备份Key
bool CPvpRankModel::bakPvpData()
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}
	//当前天排行数据
	//时间过了零点，是第二天了,后面添加60秒保证夸天
	int nCurTime = time(NULL) + 60;
	int nCurDay = CCommonHelper::getTotalDay(nCurTime) - 1;
	string BakKey = ModelKey::PvpBakRankKey(nCurDay);

	if (SUCCESS == pRedisStorer->ExistKey(BakKey))
	{
		return false;
	}

	vector<RankData> VectRankData;
	pRedisStorer->ZRange(m_PvpRankKey, VectRankData, 0, MAX_RANK_NUM);

	//备份数据排序重设置分数，保证分数的唯一性
	for (vector<RankData>::iterator riter = VectRankData.begin(); riter != VectRankData.end();++riter)
	{
		pRedisStorer->Zadd(BakKey, riter->id, riter->source);
	}

	int nExpireTime = 0;
	if (SUCCESS != pRedisStorer->TTL(BakKey, nExpireTime))
	{
		return false;
	}

	//-1时，表示key值没有设置过期时间
	//1表示key值已经过期
	if (nExpireTime < 0)
	{
		time_t nCurTime = time(NULL);
		nExpireTime = CTimeCalcTool::nextTimeStampToZero(nCurTime, 0) + 7 * 24 * 60 * 60 - (int)nCurTime;

		if (SUCCESS != pRedisStorer->Expire(BakKey, nExpireTime))
		{
			return false;
		}
	}

	return true;
}

//保存锦标赛周一时的排行数据
bool CPvpRankModel::bakChampionRank()
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}
	//当前天排行数据
	//时间过了零点，是第二天了
	//距离周二的时间点
	int nExpireTime = 0;
	time_t nCurTime = time(NULL);
	int nNextTime = CTimeCalcTool::nextTimeStamp(nCurTime, 0, 0, 2);
	int nKeyDay = CCommonHelper::getTotalDay(nNextTime);
	string Key = ModelKey::ChampionBakRankKey(nKeyDay);
	vector<RankData> VectRankData;
	if (SUCCESS != pRedisStorer->ZRange(m_ChampionKey, VectRankData, 0, MAX_RANK_NUM))
	{
		return false;
	}
	//备份数据排序重设置分数，保证分数的唯一性
	for (vector<RankData>::iterator riter = VectRankData.begin(); riter != VectRankData.end(); ++riter)
	{
		pRedisStorer->Zadd(Key, riter->id, riter->source);
	}

	if (SUCCESS != pRedisStorer->TTL(Key, nExpireTime))
	{
		return false;
	}

	//-1时，表示key值没有设置过期时间
	//1表示key值已经过期
	if (nExpireTime < 0)
	{
		nExpireTime = CTimeCalcTool::nextTimeStampToZero(nCurTime, 0) + 30 * 24 * 60 * 60 - (int)nCurTime;

		if (SUCCESS != pRedisStorer->Expire(Key, nExpireTime))
		{
			return false;
		}
	}

	return true;
}

bool CPvpRankModel::setPvpRewardFlag(int uid, int nDay)
{
	if (uid == 0)
	{
		return false;
	}

	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	std::string RewardKey = ModelKey::PvpRankReward(uid);
	std::set<int> RewardFlag;
	if (SUCCESS != pRedisStorer->GetSetAll(RewardKey, RewardFlag))
	{
		return false;
	}

	if (RewardFlag.find(nDay) != RewardFlag.end())
	{
		return false;
	}

	if (SUCCESS != pRedisStorer->SetAdd(RewardKey,nDay))
	{
		return false;
	}

	return true;
}

bool CPvpRankModel::getPvpRewardFlag(int uid, int nDay)
{
	if (uid == 0)
	{
		return false;
	}

	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	std::string RewardKey = ModelKey::PvpRankReward(uid);
	std::set<int> RewardFlag;
	if (SUCCESS != pRedisStorer->GetSetAll(RewardKey, RewardFlag))
	{
		return false;
	}

	if (RewardFlag.find(nDay) != RewardFlag.end())
	{
		return false;
	}

	return true;
}

bool CPvpRankModel::cleanPvpRewardFlag(int uid, int nStartDay)
{
	if (uid == 0)
	{
		return false;
	}

	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	std::string RewardKey = ModelKey::PvpRankReward(uid);
	std::set<int> RewardFlag;
	if (SUCCESS != pRedisStorer->GetSetAll(RewardKey, RewardFlag))
	{
		return false;
	}

	if (RewardFlag.size() < MAX_FLAG_NUM)
	{
		return false;
	}

	for (std::set<int>::iterator ator = RewardFlag.begin(); ator != RewardFlag.end();)
	{
		if ((*ator) < nStartDay)
		{
			RewardFlag.erase(ator++);
			if (SUCCESS != pRedisStorer->SetDel(RewardKey, (const char*)&(*ator), sizeof(int)))
			{
				continue;
			}
		}
		else
		{
			ator++;
		}
	}

	return true;
}

bool CPvpRankModel::BuildPvpRank()
{
	m_ShowPvpRank.clear();
	string strUserKey;
	string strPvpKey;
	string UnionKey;
    string strUserUnionKey;
	string strDBKey;

	SPvpRankData PvpRankData;
	SPvpRankKey  PvpRankKey;
	int nUnionID = 0;

	CRedisStorer *pRedisStorer = dynamic_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	if (SUCCESS != pRedisStorer->ZCount(m_PvpRankKey, m_CurPvpRankCount))
	{
		return false;
	}

	vector<RankData> pvpRankData;
	if (SUCCESS != pRedisStorer->ZRange(m_PvpRankKey, pvpRankData, 0, MAX_RANK_SHOW_NUM - 1))
	{
		return false;
	}

	for (vector<RankData>::iterator ator = pvpRankData.begin(); ator != pvpRankData.end(); ++ator)
	{
		CRedisStorer *pStorer = dynamic_cast<CRedisStorer*>(m_pStorage->GetStorer(ator->id));
		if (NULL == pStorer)
		{
			continue;
		}

		PvpRankKey.nUid = ator->id;
		PvpRankKey.nScore = ator->source;
		PvpRankKey.nMMR = 0;
		PvpRankData.nHeadID = 0;
		PvpRankData.nLevel = 0;
		PvpRankData.UnionName = "";
		PvpRankData.UserName = "";
		PvpRankData.cBDType = 0;
		PvpRankData.cBDLev = 0;
		strUserKey	= ModelKey::UsrKey(ator->id);
		strPvpKey = ModelKey::PvpKey(ator->id);
        strUserUnionKey = ModelKey::UserUnionKey(ator->id);
		strDBKey = ModelKey::ckExtraDataKey(ator->id);
		nUnionID = 0;

		//用户名
		pStorer->GetHashByField(strUserKey, USR_FD_USERNAME, PvpRankData.UserName);
		//头像
		pStorer->GetHashByField(strUserKey, USR_FD_HEADICON, PvpRankData.nHeadID);
		//等级
		pStorer->GetHashByField(strUserKey, USR_FD_USERLV, PvpRankData.nLevel);

		std::string strQQExtraData;
		//没有蓝钻
		if (SUCCESS == pStorer->GetString(strDBKey, strQQExtraData))
		{
			strQQExtraData = "[" + strQQExtraData + "]";
			//有蓝钻，数据必须合法
			VecInt VectValue;
			CConfAnalytic::ToJsonInt(strQQExtraData, VectValue);
			if (VectValue.size() != 3)
			{
				return false;
			}

			PvpRankData.cBDType = VectValue[0];
			PvpRankData.cBDLev = VectValue[1];
		}

		//公会ID
		pStorer->GetHashByField(strUserUnionKey, USER_UNION_ID, nUnionID);
		if (nUnionID != 0)
		{
			UnionKey = ModelKey::UnionKey(nUnionID);
			pStorer->GetHashByField(UnionKey, UNION_FIELD_UNIONNAME, PvpRankData.UnionName);
		}

		//PVP MMR
		pStorer->GetHashByField(strPvpKey, PVP_FD_MMR, PvpRankKey.nMMR);
		m_ShowPvpRank[PvpRankKey] = PvpRankData;
	}

	if (m_CurPvpRankCount >= MAX_RANK_NUM)
	{
		vector<RankData> lastRankData;
		if (SUCCESS != pRedisStorer->ZRange(m_PvpRankKey, lastRankData, 10000, 10000)) //获取最后一个排行榜数据
		{
			return false;
		}

		if (lastRankData.size() != 0)
		{
			m_CurPvpMinScore = lastRankData[0].source;
		}
	}
	else
	{
		m_CurPvpMinScore = 0;
	}

	return true;
}

bool CPvpRankModel::BuildChampionRank()
{
	m_ShowChampionRank.clear();
	string strUserKey;
	string strPvpKey;
	string UnionKey;
    string strUserUnionKey;
	string strDBKey;

	SPvpRankData PvpRankData;
	SPvpRankKey  PvpRankKey;
	int nUnionID = 0;

	CRedisStorer *pRedisStorer = dynamic_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	if (SUCCESS != pRedisStorer->ZCount(m_ChampionKey, m_CurChampRankCount))
	{
		return false;
	}

	vector<RankData> championRankData;
	if (SUCCESS != pRedisStorer->ZRange(m_ChampionKey, championRankData, 0, MAX_RANK_SHOW_NUM - 1))
	{
		return false;
	}


	for (vector<RankData>::iterator ator = championRankData.begin(); ator != championRankData.end(); ++ator)
	{
		CRedisStorer *pStorer = dynamic_cast<CRedisStorer*>(m_pStorage->GetStorer(ator->id));
		if (NULL == pStorer)
		{
			continue;
		}

		PvpRankKey.nUid = ator->id;
		PvpRankKey.nScore = ator->source;
		PvpRankKey.nMMR = 0;
		PvpRankData.nHeadID = 0;
		PvpRankData.nLevel = 0;
		PvpRankData.UnionName = "";
		PvpRankData.UserName = "";
		PvpRankData.cBDType = 0;
		PvpRankData.cBDLev = 0;
		nUnionID = 0;
		strUserKey = ModelKey::UsrKey(ator->id);
		strPvpKey = ModelKey::PvpKey(ator->id);
        strUserUnionKey = ModelKey::UserUnionKey(ator->id);
		strDBKey = ModelKey::ckExtraDataKey(ator->id);

		//用户名
		pStorer->GetHashByField(strUserKey, USR_FD_USERNAME, PvpRankData.UserName);
		//头像
		pStorer->GetHashByField(strUserKey, USR_FD_HEADICON, PvpRankData.nHeadID);
		//等级
		pStorer->GetHashByField(strUserKey, USR_FD_USERLV, PvpRankData.nLevel);
		
		std::string strQQExtraData;
		//没有蓝钻
		if (SUCCESS == pStorer->GetString(strDBKey, strQQExtraData))
		{
			strQQExtraData = "[" + strQQExtraData + "]";
			//有蓝钻，数据必须合法
			VecInt VectValue;
			CConfAnalytic::ToJsonInt(strQQExtraData, VectValue);
			if (VectValue.size() != 3)
			{
				return false;
			}

			PvpRankData.cBDType = VectValue[0];
			PvpRankData.cBDLev = VectValue[1];
		}

		//公会ID
		pStorer->GetHashByField(strUserUnionKey, USER_UNION_ID, nUnionID);
		if (nUnionID != 0)
		{
			UnionKey = ModelKey::UnionKey(nUnionID);

			if (SUCCESS != pStorer->GetHashByField(UnionKey, UNION_FIELD_UNIONNAME, PvpRankData.UnionName))
			{
				continue;
			}
		}
		//PVP MMR
		pStorer->GetHashByField(strPvpKey, CPN_FD_MMR, PvpRankKey.nMMR);

		m_ShowChampionRank[PvpRankKey] = PvpRankData;
	}

	if (m_CurChampRankCount >= MAX_RANK_NUM)
	{
		vector<RankData> lastRankData;
		if (SUCCESS != pRedisStorer->ZRange(m_ChampionKey, lastRankData, 10000, 10000)) //获取最后一个排行榜数据
		{
			return false;
		}

		if (lastRankData.size() != 0)
		{
			m_CurChampMinScore = lastRankData[0].source;
		}
	}
	else
	{
		m_CurChampMinScore = 0;
	}

	return true;
}

void CPvpRankModel::clearData()
{
	m_CurPvpRankCount = 0;			//当前排行榜人数
	m_CurPvpMinScore = 0;		//当前排行榜最小人数
	m_CurGetIndex = 0;			//操作次数延迟
	m_ShowPvpRank.clear();
	m_ShowChampionRank.clear();
	m_CurChampRankCount = 0;	//当前竞技排行榜人数
	m_CurChampMinScore = 0;
	m_CurGetIndex = 0;
	m_ChampionIDSet.clear();
}
