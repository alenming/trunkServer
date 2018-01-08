#ifndef __SESSION_LISTENER_H__
#define __SESSION_LISTENER_H__

#include "KxTCPListener.h"

class CSessionListener :
    public KxServer::KxTCPListener
{
public:
    CSessionListener();
    virtual ~CSessionListener(void);

    virtual KxServer::KxTCPClienter* onAccept(KXCOMMID client);
};

#endif //__SESSION_LISTENER_H__
