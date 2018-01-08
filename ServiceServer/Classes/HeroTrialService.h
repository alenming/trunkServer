#ifndef __HEROTRIAL_SERVICE_H__
#define __HEROTRIAL_SERVICE_H__

#include "KxCommInterfaces.h"

class CHeroTrialService
{
public:

	static void ProcessService(int maincmd, int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//金币副本开始
	static void ProcessChanllege(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//金币副本结束
	static void ProcessFinish(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif
