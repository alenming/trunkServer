#ifndef _RANK_SERVICE_H__
#define _RANK_SERVICE_H__

#include "KxCommInterfaces.h"

class CRankService
{
public:
	CRankService();
	~CRankService();

	static void ProcessService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

protected:
	static void ProcessRankService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//处理PVP排行榜
	static void ProcessPvpRankService(int uid, KxServer::IKxComm *commun);
	//处理等级排行榜
	static void ProcessLevelRankService(int uid, KxServer::IKxComm *commun);
	//处理公会排行榜
	static void ProcessUnionRankService(int uid, KxServer::IKxComm *commun);
	//处理爬塔排行榜
	static void ProcessTowerRankService(int uid, KxServer::IKxComm *commun);
	//处理锦标赛排行榜
	static void processChampionRankService(int uid, KxServer::IKxComm *commun);

	static int m_LevelExpireTime;					//等级排行榜数据过期时间
	static int m_PvpExpireTime;						//PVP排行榜数据过期时间
	static int m_UnionExpireTime;					//公会排行榜数据过期时间
	static int m_TowerExpireTime;					//爬塔试炼排行榜数据过期时间
	static int m_ChampionExpireTime;				//锦标赛排行榜数据过期时间

};


#endif //_RANK_SERVICE_H__
