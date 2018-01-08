#ifndef __BASE_MODULE_H__
#define __BASE_MODULE_H__

#include "KXServer.h"

class CBaseModule : public KxServer::IKxModule
{
public:
    CBaseModule(void){}
    virtual ~CBaseModule(void){}
    
    virtual int processLength(char* buffer, unsigned int len)
    {
        if (len < sizeof(int))
        {
            //len至少为4个字节, 上层调用如果不够4个字节则等待新数据.
            return sizeof(int);
        }
        else
        {
            //KXLOGDEBUG("CBaseModule Request Len %d", *(int*)(buffer));
            return *(int*)(buffer);
        }
    }
};

#endif
