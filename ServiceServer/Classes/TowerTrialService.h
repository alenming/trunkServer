#ifndef __TOWERTRIAL_SERVICE_H__
#define __TOWERTRIAL_SERVICE_H__

#include "KxCommInterfaces.h"

class CTowerTrialService
{
public:
	// 业务处理
    static void processService(int maincmd, int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 迎战敌人
	static void processFighting(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 游戏结束
	static void processFinish(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 选择战斗外buff
	static void processChoseOuterBonus(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 打开宝箱
	static void processOpenTreasure(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 一键爬塔
	static void processOneKeyFighting(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__TOWERTRIAL_SERVICE_H__
