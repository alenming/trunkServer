#ifndef __HERO_SERVICE_H__
#define __HERO_SERVICE_H__

#include "KXServer.h"

class CHeroService
{
public:
    // 英雄服务
	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 英雄购买(抽奖)服务
    static void ProcessHeroBuy(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 英雄生成服务
	static void ProcessHeroGen(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 英雄装备服务
    static void ProcessHeroEquip(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 英雄升级服务
    static void ProcessHeroUpgrade(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 英雄升星服务
    static void ProcessHeroUpgradeStar(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 英雄天赋服务
    static void ProcessHeroTalent(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif