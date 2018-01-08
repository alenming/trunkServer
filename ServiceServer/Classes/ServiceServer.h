#ifndef __SERVICE_SERVER_H__
#define __SERVICE_SERVER_H__

#include "SummonerServer.h"

class CServiceServer :
	public CSummonerServer
{
private:
	CServiceServer();
	~CServiceServer();

	void AddModelType();

public:

	static CServiceServer* getInstance();
	static void destroy();

    virtual void onServerUpdate();
	virtual bool onServerInit();

private:

	static CServiceServer* m_Instance;
};

#endif //__SERVICE_SERVER_H__
