/*
* 服务层辅助类
* 1、提供发送数据接口
* 2、提供发送任务/成就事件接口
* 3、提供创建新英雄模型接口(包括默认解锁技能/天赋)
* 4、提供创建新任务模型接口(包括重置任务时间戳计算)
* 5、提供玩家获取经验计算等级接口(包括满级判断)
*/

#ifndef __SERVICEHELPER_H__
#define __SERVICEHELPER_H__

#include "KxCommInterfaces.h"
#include "Protocol.h"
#include "GateManager.h"
#include "ServiceDef.h"
#include "HeroModel.h"
#include "Task.h"
#include "GameUserManager.h"
#include "UnionProtocol.h"
#include "StageProtocol.h"
#include "ServerProtocol.h"

#define DAY_TO_SECOND(D) D * 24 * 3600

class CServiceHelper
{
public:
	// 发送消息
    static int SentMsg(int uid, int maincmd, int subcmd, char *buffer, int len, KxServer::IKxComm *commu)
    {
		//KXLOGDEBUG("sentMsg main = %d sub = %d len = %d\n", maincmd, subcmd, len);
        return CGateManager::getInstance()->Transmit(uid, MakeCommand(maincmd, subcmd), buffer, len);
    }

    // 发送奖励信息
    static int SentAwardMsg(int uid, int maincmd, int subcmd, KxServer::IKxComm *commu)
    {
        char *buffer = NULL; 
        int len = 0;

        return SentMsg(uid, maincmd, subcmd, buffer, len, commu);
    }
};

#endif 
