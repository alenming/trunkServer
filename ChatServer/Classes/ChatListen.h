#ifndef _CHAT_LISTEN_H__
#define _CHAT_LISTEN_H__

#include "KXServer.h"

class CChatListen: public KxServer::KxTCPListener
{
public:
    CChatListen();
    ~CChatListen();

    // Accept时回调，根据KXCOMMID创建KxTCPClienter
    virtual KxServer::KxTCPClienter* onAccept(KXCOMMID client);
};


#endif //_CHAT_LISTEN_H__
