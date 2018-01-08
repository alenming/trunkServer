#ifndef _UNION_EXPIDITION_SERVICE_H__
#define _UNION_EXPIDITION_SERVICE_H__

#include "KXServer.h"
#include <vector>

class CUnionExpiditionModel;
struct SExpeditonWorldItem;
class CUnionExpiditionService
{
public:
	static void processService(int maincmd, int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//处理公会远征信息下发
	static void processExpiditionInfo(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//处理公会远征地图选择
	static void processExpiditionMapChoose(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//处理公会远征讨伐开始
	static void processExpiditionFightStart(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//处理公会远征讨伐结束
	static void processExpiditionFightFinish(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//处理公会伤害排行榜
	static void processExpiditionDamageRank(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//处理公会远征关卡信息
	static void processExpiditionStageInfo(int uid, char *buffer, int len, KxServer::IKxComm *commun);

private:
	static int				m_RankFreshTime;			//排行榜刷新时间

};


#endif //_UNION_EXPIDITION_SERVICE_H__
