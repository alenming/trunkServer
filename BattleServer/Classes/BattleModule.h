#ifndef __TESTMODULE_H__
#define __TESTMODULE_H__

#include "KxCommInterfaces.h"

class CBattleModule 
    : public KxServer::IKxModule
{
public:
    CBattleModule(void);
    ~CBattleModule(void);

public:
    virtual int processLength(char* buffer, unsigned int len);
    virtual void processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target);
    virtual void processError(KxServer::IKxComm *target);
};

#endif
