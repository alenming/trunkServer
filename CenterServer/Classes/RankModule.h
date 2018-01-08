#ifndef __SERVICE_MODULE_H__
#define __SERVICE_MODULE_H__

#include "KxCommInterfaces.h"

class CRankModule
	: public KxServer::IKxModule
{
public:
	CRankModule(void);
	~CRankModule(void);

public:

	virtual int processLength(char* buffer, unsigned int len);

	virtual void processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target);

	virtual void processError(KxServer::IKxComm *target);
};

#endif //__SERVICE_MODULE_H__
