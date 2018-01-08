/*
* 转发通知(全服)服务层
*/

#ifndef __NOTICE_SERVICE_H__
#define __NOTICE_SERVICE_H__

#include "KxCommInterfaces.h"

class CNoticeService
{
public:
	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processNoticeService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__NOTICE_SERVICE_H__
