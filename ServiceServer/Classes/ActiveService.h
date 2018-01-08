#ifndef _ACTIVE_SERVICE_H__
#define _ACTIVE_SERVICE_H__

#include "KxCommInterfaces.h"

class CActiveService
{
public:
	CActiveService();
	~CActiveService();

	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

	//处理活动领取业务
	static void ProcessActiveGetService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	//处理7天活动领取
	static void processSevenActiveGetService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	//处理QQ蓝钻活动领取
	static void processQQActiveGetService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

	//处理兑换活动领取
	static void processExChangeGetService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

protected:
	//处理活动商店礼包购买
	static void ProcessActiveShopBuy(int uid, int nActiveID, int nGiftID,int nActiveType,KxServer::IKxComm *commun);

	//处理任务活动的任务领取
	static void ProcessActiveTaskGet(int uid, int nActiveID, int nActiveTaskID,int nActiveType, KxServer::IKxComm *commun);

	//处理月卡活动中的月卡数据领取
	static void processActiveMonthGet(int uid, int nActiveID, int nMonthCardID, KxServer::IKxComm *commun);

	//处理任务活动中时间任务领取
	static void processTimesActiveTaskGet(int uid, int nActiveID, int nActiveTaskID, int nActiveType, KxServer::IKxComm *commun);

	//处理在线时长的活动任务
	static void processOLTimeActiveTaskGet(int uid, int nActiveID, int nActiveTaskID, int nActiveType, KxServer::IKxComm *commun);

};


#endif //_ACTIVE_SERVICE_H__

