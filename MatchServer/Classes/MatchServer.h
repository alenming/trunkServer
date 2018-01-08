#ifndef _MATCH_SERVER_H__
#define _MATCH_SERVER_H__

#include "SummonerServer.h"

class CMatchServer : public CSummonerServer
{
private:

	CMatchServer();
	~CMatchServer();

public:

	static CMatchServer * getInstance();
	static void destroy();
	// 服务器初始化
	virtual bool onServerInit();
	// 本服务器拥有的模型
	void AddModelType();

private:

	static CMatchServer * m_pInstance;
};

#endif //_MATCH_SERVER_H__

