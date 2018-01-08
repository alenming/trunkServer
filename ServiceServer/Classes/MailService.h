/*
* 邮件服务
* 1.读取邮件(包含普通、web邮件)
*/

#ifndef __MAIL_SERVICE_H__
#define __MAIL_SERVICE_H__

#include "KxCommInterfaces.h"
#include "BagModel.h"
#include "ServiceDef.h"

#include <list>

class CMailService
{
public:
	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 获取邮件附件
	static void processGetMailGoodsService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	//读取邮件内容
	static void processReadMailContextService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 添加web邮件
    static void processWebMailService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    //邮件模型刷新
    static void processUpdateMailService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__MAIL_SERVICE_H__
