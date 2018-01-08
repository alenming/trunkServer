#ifndef __BASE_MODULE_H__
#define __BASE_MODULE_H__

#include "KXServer.h"
#include "ServerProtocol.h"

class CBaseModule : public KxServer::IKxModule
{
public:
    CBaseModule(void){}
    virtual ~CBaseModule(void){}
    
    virtual int processLength(char* buffer, unsigned int len)
    {
        if (len < sizeof(ServerProtocol::Head))
        {
            //len至少为4个字节, 上层调用如果不够4个字节则等待新数据.
            return sizeof(ServerProtocol::Head);
        }
        else
        {
            ServerProtocol::Head* head = (ServerProtocol::Head*)(buffer);
            //KXLOGDEBUG("CBaseModule Request Len %d main cmd %d sub cmd %d",
             //   head->length, head->MainCommand(), head->SubCommand());
            return head->length;
        }
    }
};

#endif
