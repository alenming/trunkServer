#ifndef _SHOP_SERVICE_H__
#define _SHOP_SERVICE_H__

#include "KXServer.h"


class CShopService
{
public:
	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

	//处理商店购买
	static void ProcessShopGoodsBuy(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	//处理公会商店购买
	static void processUnionShopBuy(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	//处理商店刷新
	static void ProcessShopFresh(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};


#endif //_SHOP_SERVICE_H__
