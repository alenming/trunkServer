#ifndef __SERVICE_MODULE_H__
#define __SERVICE_MODULE_H__

#include "KxCommInterfaces.h"

class CServiceModule
	: public KxServer::IKxModule
{
public:
	CServiceModule(void);
	~CServiceModule(void);

public:

	virtual int processLength(char* buffer, unsigned int len);

	virtual void processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target);

	virtual void processError(KxServer::IKxComm *target);
};

#endif //__SERVICE_MODULE_H__
