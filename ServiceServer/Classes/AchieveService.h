/*
* 成就服务层
* 1、判断检测由前端监听的成就是否完成
* 2、完成成就下发奖励
*/

#ifndef __ACHIEVE_SERVICE_H__
#define __ACHIEVE_SERVICE_H__

#include "KxCommInterfaces.h"

class CAchieveService
{
public:
    static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 完成成就
    static void processFinishService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 领取成就奖励
    static void processAwardService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif