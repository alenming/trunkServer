#ifndef _MATCH_MODULE_H__
#define _MATCH_MODULE_H__

#include "KXServer.h"

class CMatchModule : public KxServer::IKxModule
{
public:
	CMatchModule();
	virtual ~CMatchModule();

	virtual int processLength(char* buffer, unsigned int len);

	// IKxComm对象接收到完整数据后，调用的回调
	virtual void processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target);
	// IKxComm对象发生错误时，调用的回调
	virtual void processError(KxServer::IKxComm *target);

};

#endif //_MATCH_MODULE_H__
