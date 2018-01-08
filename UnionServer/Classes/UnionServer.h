#ifndef _UNION_SERVER_H__
#define _UNION_SERVER_H__

#include "SummonerServer.h"

#define UNION_SERVER_ID 1

class CUnionServer : public CSummonerServer
{
public:
    // 服务器初始化
    virtual bool onServerInit();

	static CUnionServer * getInstance();

    static void destroy();

private:
    CUnionServer();
    ~CUnionServer();

	void AddModelType();

private:
    static CUnionServer * m_pInstance;
};


#endif //_UNION_SERVER_H__

