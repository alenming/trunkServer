#ifndef __SERVICE_SERVER_H__
#define __SERVICE_SERVER_H__

#include "SummonerServer.h"

class CRankServer :
	public CSummonerServer
{
private:
	CRankServer();
	~CRankServer();

public:

	static CRankServer* getInstance();
	static void destroy();

    virtual void onServerUpdate();
	virtual bool onServerInit();

private:

	static CRankServer* m_Instance;
};

#endif //__SERVICE_SERVER_H__
