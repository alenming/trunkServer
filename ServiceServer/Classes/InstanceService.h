#ifndef __INSTANCE_SERVICE_H__
#define __INSTANCE_SERVICE_H__

#include "KxCommInterfaces.h"

class CInstanceService
{
public:
	//活动副本业务
	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//挑战活动副本
	static void ProcessChanllege(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//挑战结束
	static void ProcessFinish(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//购买次数
	static void ProcessBuyTimes(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__INSTANCE_SERVICE_H__
