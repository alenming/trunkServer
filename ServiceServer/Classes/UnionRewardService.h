#ifndef _UNION_REWARD_SERVICE_H__
#define _UNION_REWARD_SERVICE_H__

#include "KXServer.h"
#include <vector>

class CUnionExpiditionRewardService
{
public:
	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//处理公会远征伤害排行奖励领取
	static void processExpiditionRewardGet(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

class CUnionRewardService
{
public:
    static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
    //处理公会福利奖励领取
    static void processWelfareReward(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //_UNION_REWARD_SERVICE_H__
