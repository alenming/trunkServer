#include "RankModel.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "UserModel.h"
#include "UnionModel.h"
#include "TowerTestModel.h"
#include "UserUnionModel.h"
#include "TimeCalcTool.h"
#include "ConfGameSetting.h"
#include "GameDef.h"

using namespace std;

CRankModel *	CRankModel::m_pInstance = NULL;

CRankModel::CRankModel()
{
	clearData();
}


CRankModel::~CRankModel()
{
}

CRankModel* CRankModel::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CRankModel();
	}

	return m_pInstance;
}

void CRankModel::destroy()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CRankModel::init()
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_SERVER);
	if (NULL == m_pStorage)
	{
		return false;
	}

	return Refresh();
}

bool CRankModel::Refresh()
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	string strCountKey = ModelKey::RankCountKey(TOWER_RANK_TYPE); //爬塔计数Key
	m_CurTimeCount = MAX_TOWERRANK_INDEX;
	pRedisStorer->GetString(strCountKey, m_CurTimeCount);
	if(m_CurTimeCount <= 0)
	{
		m_CurTimeCount = MAX_TOWERRANK_INDEX;
	}

	clearData();
	BuildLevelRank();
	BuildTowerRank();
	BuildUnionRank();

	return true;
}

//增加数据类型
bool CRankModel::AddRankData(int RType, int id, int Score)
{
	if (id == 0)
	{
		return false;
	}

	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	string Key = ModelKey::RankKey(RType);
	//设置爬塔过期
	if (RType == TOWER_RANK_TYPE)
	{
		string strCountKey = ModelKey::RankCountKey(TOWER_RANK_TYPE); //爬塔计数Key
		int nCurExpireTime = 0;
		if (SUCCESS != pRedisStorer->TTL(Key, nCurExpireTime))
		{
			return false;
		}

		if (nCurExpireTime < 0)
		{
			m_CurTimeCount = MAX_TOWERRANK_INDEX;
			if (SUCCESS != pRedisStorer->DelKey(Key))
			{
				return false;
			}
			//如果是爬塔，亿值上存储塔层数，后面连接进入顺序由大到小，
			Score = Score * TOWER_LEVEL_POSITIONNUM + m_CurTimeCount;
			m_CurTimeCount--;
			if (SUCCESS != pRedisStorer->Zadd(Key, id, Score))
			{
				return false;
			}

			pRedisStorer->SetString(strCountKey, m_CurTimeCount);
			
			int CurTime = (int)time(NULL);
			int recoverStamp = 0;
			const TowerTestSettingItem * pSettingConf = queryConfTowerSetting();
			if (NULL != pSettingConf)
			{
				recoverStamp = CTimeCalcTool::nextTimeStamp(CurTime, 0, 0, pSettingConf->nStartWeekDay);
			}

			int nIterval = recoverStamp - CurTime;
			if (SUCCESS != pRedisStorer->Expire(Key, nIterval))
			{
				return false;
			}
		}
		else
		{
			//如果是爬塔，千万值上存储塔层数，后面连接进入顺序由大到小，
			Score = Score * TOWER_LEVEL_POSITIONNUM + m_CurTimeCount;
			m_CurTimeCount--;
			if (SUCCESS != pRedisStorer->Zadd(Key, id, Score))
			{
				return false;
			}

			pRedisStorer->SetString(strCountKey, m_CurTimeCount);

		}
	}
	else
	{
		if (SUCCESS != pRedisStorer->Zadd(Key, id, Score))
		{
			return false;
		}
	}

	m_CurGetIndex[RType - 1]++;

	if (m_CurGetIndex[RType - 1] >= 100)
	{
		m_CurGetIndex[RType - 1] = 0;

		if (SUCCESS != pRedisStorer->ZCount(Key, m_CurRankCount[RType - 1]))
		{
			return false;
		}
	}

	if (m_CurRankCount[RType - 1] > MAX_RANK_NUM)
	{
		//超出1W名的去除，默认是按由小到大排序
		if (SUCCESS != pRedisStorer->ZDel(Key, MAX_RANK_NUM, m_CurRankCount[RType - 1]))
		{
			return false;
		}

		m_CurRankCount[RType - 1] = MAX_RANK_NUM;

		std::vector<RankData> VectRankData;
		if (SUCCESS != pRedisStorer->ZRange(Key, VectRankData, MAX_RANK_NUM, MAX_RANK_NUM)) //获取最后一名的排名
		{
			return false;
		}

		if (VectRankData.size() != 0)
		{
			m_CurMinScore[RType - 1] = VectRankData[0].source;
		}
	}

	return true;
}

//获取排行
bool CRankModel::GetRankData(int RType, int id, int &Score)
{
	CRedisStorer *pRedisStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	string Key = ModelKey::RankKey(RType);

	if (SUCCESS == pRedisStorer->ZRank(Key, id, Score))
	{
		return true;
	}

	Score = 0;
	return true;
}

//删除排行数据
bool CRankModel::DelRankData(int RType)
{
	CRedisStorer *pRedisStorer = reinterpret_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (pRedisStorer == NULL)
	{
		return false;
	}

	string Key = ModelKey::RankKey(RType);
	if (SUCCESS != pRedisStorer->DelKey(Key))
	{
		return false;
	}

	return true;
}

bool CRankModel::getUserRankID(int uid,int& value)
{
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(uid));
	if (pRedisStorer == NULL)
	{
		return false;
	}

    return SUCCESS == pRedisStorer->GetHashByField(ModelKey::UserUnionKey(uid), USER_UNION_ID, value);
}


bool CRankModel::BuildLevelRank()
{
	string strUserKey;      //保存redis中用户信息的key
	string UnionKey;        //保存redis中公会信息的key
    string strUserUnionKey; //保存redis中用户的公会信息的key
	string strDBKey;		//蓝钻信息Key

	SLevelRankData LevelRankData;//表示排行榜中一个用户的信息
	SLevelRankKey  LevelRankKey;//排行榜中的索引
	int nUnionID = 0;//用户的公会ID

	CRedisStorer *pRedisStorer = dynamic_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (NULL == pRedisStorer)
	{
		KXLOGDEBUG("Get RedisStorer Error by BuildLevelRank");
		return false;
	}
	string levelRankKey =  ModelKey::RankKey(LEVEL_RANK_TYPE);//得到等级排行榜在redis中的key

	vector<RankData> levelRankData;//用来存放读取的等级排行榜数据

	if (SUCCESS != pRedisStorer->ZCount(levelRankKey, m_CurRankCount[LEVEL_RANK_TYPE - 1]))//获取数量
	{
		KXLOGDEBUG("Redis Get LevelRankCount Error");
		return false;
	}

	if (SUCCESS != pRedisStorer->ZRange(levelRankKey, levelRankData, 0, MAX_RANK_SHOW_NUM - 1)) //获取前50排名的全部数据
	{
		KXLOGDEBUG("Redis Get LevelRankData Error");
		return false;
	}

	m_LevelShowRankMap.clear();

	for (vector<RankData>::iterator ator = levelRankData.begin(); ator != levelRankData.end(); ++ator)
	{
		CRedisStorer *pStorer = dynamic_cast<CRedisStorer*>(m_pStorage->GetStorer(ator->id));
		if (NULL == pStorer)
		{
			continue;
		}

		LevelRankKey.nUid = ator->id;
		LevelRankKey.nLevel = ator->source;
		LevelRankData.nHeadID = 0;
		LevelRankData.UnionName = "";
		LevelRankData.UserName = "";
		LevelRankData.cBDType = 0;
		LevelRankData.cBDLev = 0;
		nUnionID = 0;
		strUserKey = ModelKey::UsrKey(ator->id);
        strUserUnionKey = ModelKey::UserUnionKey(ator->id);
		strDBKey = ModelKey::ckExtraDataKey(ator->id);

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

			LevelRankData.cBDType = VectValue[0];
			LevelRankData.cBDLev = VectValue[1];
		}

		//用户名
		pStorer->GetHashByField(strUserKey, USR_FD_USERNAME, LevelRankData.UserName);
		//头像
		pStorer->GetHashByField(strUserKey, USR_FD_HEADICON, LevelRankData.nHeadID);
		//公会ID
		pStorer->GetHashByField(strUserUnionKey, USER_UNION_ID, nUnionID);
		if (nUnionID != 0)
		{
			UnionKey = ModelKey::UnionKey(nUnionID);
			pStorer->GetHashByField(UnionKey, UNION_FIELD_UNIONNAME, LevelRankData.UnionName);
		}
		m_LevelShowRankMap[LevelRankKey] = LevelRankData;
	}


	if (m_CurRankCount[LEVEL_RANK_TYPE - 1] >= MAX_RANK_NUM)
	{
		vector<RankData> lastRankData;
		if (SUCCESS != pRedisStorer->ZRange(levelRankKey, lastRankData, 10000, 10000)) //获取第10000名的分数
		{
			KXLOGDEBUG("Redis Error");
		}

		if (lastRankData.size() != 0)
		{
			m_CurMinScore[LEVEL_RANK_TYPE - 1] = lastRankData[0].source;
		}
		
	}
	else
	{
		m_CurMinScore[LEVEL_RANK_TYPE - 1] = 0;
	}

	return true;
}

bool CRankModel::BuildTowerRank()
{
	string strUserKey;//用户信息key
	string strTowerKey;//用户爬塔key
	string strDBKey;   //蓝钻信息Key
	string strCountKey; //计数Key
	string strUserUnionKey; //保存redis中用户的公会信息的key
	string UnionKey;        //保存redis中公会信息的key

	STowerRankData TowerRankData;//单个用户爬塔信息
	STowerRankKey TowerRankKey;//索引

	CRedisStorer *pRedisStorer = dynamic_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (NULL == pRedisStorer)
	{
		KXLOGDEBUG("Get RedisStorer Error by BuildTowerRank");
	}

	string towerRankKey = ModelKey::RankKey(TOWER_RANK_TYPE);//得到爬塔排行榜在redis中的key
	strCountKey = ModelKey::RankCountKey(TOWER_RANK_TYPE); //爬塔先后key
	vector<RankData> towerRankData;//存放爬塔的数据


	if (SUCCESS != pRedisStorer->ZCount(towerRankKey, m_CurRankCount[TOWER_RANK_TYPE-1]))//获取数量
	{
		KXLOGDEBUG("Redis Get TowerRankCount Error");
		return false;
	}

	if (SUCCESS != pRedisStorer->ZRange(towerRankKey, towerRankData, 0, MAX_RANK_SHOW_NUM - 1)) //获取前50排名的全部数据
	{
		KXLOGDEBUG("Redis Get TowerRankData Error");
		return false;
	}

	m_TowerShowRankMap.clear();//清空之前保存的数据
	int nUnionID = 0;//用户的公会ID
	for (vector<RankData>::iterator ator = towerRankData.begin(); ator != towerRankData.end(); ++ator)
	{
		CRedisStorer *pStorer = dynamic_cast<CRedisStorer*>(m_pStorage->GetStorer(ator->id));
		if (NULL == pStorer)
		{
			continue;
		}

		TowerRankKey.nUid = ator->id;
		TowerRankKey.nScore = ator->source / TOWER_LEVEL_POSITIONNUM - 1;		//存储的是当前爬塔准备挑战第几层，所以需要-1
		TowerRankData.nHeadID = 0;
		TowerRankData.nLevel = 0;
		TowerRankData.UserName = "";
		TowerRankData.UnionName = "";
		TowerRankData.cBDType = 0;
		TowerRankData.cBDLev = 0;
		nUnionID = 0;
		strUserKey = ModelKey::UsrKey(ator->id);
		strTowerKey = ModelKey::TowerTestKey(ator->id);
		strUserUnionKey = ModelKey::UserUnionKey(ator->id);

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

			TowerRankData.cBDType = VectValue[0];
			TowerRankData.cBDLev = VectValue[1];
		}

		//用户名
		pStorer->GetHashByField(strUserKey, USR_FD_USERNAME, TowerRankData.UserName);
		//头像
		pStorer->GetHashByField(strUserKey, USR_FD_HEADICON, TowerRankData.nHeadID);
		//等级
		pStorer->GetHashByField(strUserKey, USR_FD_USERLV, TowerRankData.nLevel);
		//爬楼最高层获取
		pStorer->GetHashByField(strTowerKey, TOWER_FD_FLOOR, TowerRankKey.nMaxTowerLevel);
		TowerRankKey.nMaxTowerLevel = TowerRankKey.nMaxTowerLevel - 1;
		//公会ID
		pStorer->GetHashByField(strUserUnionKey, USER_UNION_ID, nUnionID);
		if (nUnionID != 0)
		{
			UnionKey = ModelKey::UnionKey(nUnionID);
			pStorer->GetHashByField(UnionKey, UNION_FIELD_UNIONNAME, TowerRankData.UnionName);
		}

		m_TowerShowRankMap[TowerRankKey] = TowerRankData;
	}

	if (m_CurRankCount[TOWER_RANK_TYPE - 1] >= MAX_RANK_NUM)
	{
		vector<RankData> lastRankData;
		if (SUCCESS != pRedisStorer->ZRange(towerRankKey, lastRankData, 10000, 10000)) //获取第10000名的分数
		{
			KXLOGDEBUG("Redis Error");
		}

		if (lastRankData.size() != 0)
		{
			m_CurMinScore[TOWER_RANK_TYPE - 1] = lastRankData[0].source;
		}

	}
	else
	{
		m_CurMinScore[TOWER_RANK_TYPE - 1] = 0;
	}

	return true;
}

bool CRankModel::BuildUnionRank()
{
	string UnionKey;//公会key

	SUnionRankData UnionRankData;//单个用户公会信息
	SUnionRankKey  UnionRankKey;//索引

	CRedisStorer *pRedisStorer = dynamic_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (NULL == pRedisStorer)
	{
		KXLOGDEBUG("Get RedisStorer Error by BuildUnionRank");
	}

	string unionRankKey = ModelKey::RankKey(UNION_RANK_TYPE);//得到公会排行榜在redis中的key
	vector<RankData> unionRankData;//存放公会的数据


	if (SUCCESS != pRedisStorer->ZCount(unionRankKey, m_CurRankCount[UNION_RANK_TYPE - 1]))//获取数量
	{
		KXLOGDEBUG("Redis Get UnionRankCount Error");
		return false;
	}

	if (SUCCESS != pRedisStorer->ZRange(unionRankKey, unionRankData, 0, MAX_RANK_SHOW_NUM*2 - 1)) //获取前50排名的全部数据
	{
		KXLOGDEBUG("Redis Get UnionRankData Error");
		return false;
	}

	m_UnionShowRankMap.clear();
	int nIndex = 0;
	for (vector<RankData>::iterator ator = unionRankData.begin(); ator != unionRankData.end(); ++ator)
	{
		CRedisStorer *pStorer = dynamic_cast<CRedisStorer*>(m_pStorage->GetStorer(ator->id));
		if (NULL == pStorer)
		{
			continue;
		}
		nIndex++;
		UnionRankKey.nUnionID = ator->id;
		UnionRankKey.nUnionLevel = ator->source;
		UnionRankData.nUnionNum = 0;
		UnionRankData.UnionName = "";
		UnionRankData.nEmblemID = 0;

		UnionKey = ModelKey::UnionKey(ator->id);
		if (SUCCESS != pStorer->GetHashByField(UnionKey, UNION_FIELD_UNIONNAME, UnionRankData.UnionName))
		{
			pStorer->ZDel(UnionKey, nIndex, nIndex);
			continue;
		}

		if (SUCCESS != pStorer->GetHashByField(UnionKey, UNION_FIELD_EMBLEM, UnionRankData.nEmblemID))
		{
			continue;
		}

		map<int,int> MemberMap;
		if (SUCCESS != pStorer->GetHash(ModelKey::UnionMemberKey(ator->id), MemberMap))
		{
			continue;
		}

		UnionRankData.nUnionNum = MemberMap.size();
		m_UnionShowRankMap[UnionRankKey] = UnionRankData;
	}

	if (m_CurRankCount[UNION_RANK_TYPE - 1] >= MAX_RANK_NUM)
	{
		vector<RankData> lastRankData;
		if (SUCCESS != pRedisStorer->ZRange(unionRankKey, lastRankData, 10000, 10000)) //获取第10000名的分数
		{
			KXLOGDEBUG("Redis Error");
		}

		if (lastRankData.size() != 0)
		{
			m_CurMinScore[UNION_RANK_TYPE - 1] = lastRankData[0].source;
		}

	}
	else
	{
		m_CurMinScore[UNION_RANK_TYPE - 1] = 0;
	}

	return true;
}

bool CRankModel::setRankRewardFlag(int uid, int RType, int nFlag)
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

	std::string RewardKey = ModelKey::RankRweard(uid,RType);
	std::set<int> RewardFlag;
	if (SUCCESS != pRedisStorer->GetSetAll(RewardKey, RewardFlag))
	{
		return false;
	}

	if (RewardFlag.find(nFlag) != RewardFlag.end())
	{
		return false;
	}

	if (SUCCESS != pRedisStorer->SetAdd(RewardKey, nFlag))
	{
		return false;
	}

	return true;
}

bool CRankModel::getRankRewardFlag(int uid, int RType, int nFlag)
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

	std::string RewardKey = ModelKey::RankRweard(uid, RType);
	std::set<int> RewardFlag;
	if (SUCCESS != pRedisStorer->GetSetAll(RewardKey, RewardFlag))
	{
		return false;
	}

	if (RewardFlag.find(nFlag) != RewardFlag.end())
	{
		return false;
	}

	return true;
}


void CRankModel::clearData()
{
	memset(m_CurRankCount, 0, sizeof(m_CurRankCount));
	memset(m_CurMinScore, 0, sizeof(m_CurMinScore));
	memset(m_CurGetIndex, 0, sizeof(m_CurGetIndex));
	m_UnionShowRankMap.clear();
	m_TowerShowRankMap.clear();
	m_LevelShowRankMap.clear();
	return;
}
