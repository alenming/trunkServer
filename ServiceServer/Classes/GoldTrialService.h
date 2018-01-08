#ifndef __GOLDTRIAL_SERVICE_H__
#define __GOLDTRIAL_SERVICE_H__

#include "KxCommInterfaces.h"

class CGoldTrialService
{
public:
    static void ProcessService(int maincmd, int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//金币副本开始
	static void ProcessChallenge(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//金币副本结束
	static void ProcessFinish(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//领取宝箱
	static void ProcessRewardChest(int uid, char * buffer, int len, KxServer::IKxComm * commun);
};

#endif
