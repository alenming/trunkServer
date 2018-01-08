#ifndef _UNION_CLIENT_H__
#define _UNION_CLIENT_H__

#include "KXServer.h"


class CUnionClient : public KxServer::KxTCPClienter
{
public:
    CUnionClient();
    ~CUnionClient();
};


#endif //_UNION_CLIENT_H__
