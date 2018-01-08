#ifndef __BATTLESERVER_H__
#define __BATTLESERVER_H__

#include "SummonerServer.h"

class CBattleServer : 
    public CSummonerServer
{
public:
    CBattleServer(void);
    ~CBattleServer(void);

    static CBattleServer* getInstance();
    static void destroy();

    virtual bool onServerInit();

private:
    static CBattleServer* m_Instance;
};

#endif 
