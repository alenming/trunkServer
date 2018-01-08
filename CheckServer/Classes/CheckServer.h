#ifndef _CHECK_SERVER_H__
#define _CHECK_SERVER_H__

#include "SummonerServer.h"

class CCheckServer : public CSummonerServer
{
public:
	CCheckServer();
	~CCheckServer();

	static CCheckServer* getInstance();

	static void destroy();

    virtual void onServerUpdate();

	virtual bool onServerInit();

private:

	static CCheckServer *				m_pInstance;

};


#endif //_CHECK_SERVER_H__
