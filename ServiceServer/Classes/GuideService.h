/*
* 成就服务层
* 1、判断检测由前端监听的成就是否完成
* 2、完成成就下发奖励
*/

#ifndef __STORY_SERVICE_H__
#define __STORY_SERVICE_H__

#include "KxCommInterfaces.h"

class CGuideService
{
public:

	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

	static void processRecord(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif