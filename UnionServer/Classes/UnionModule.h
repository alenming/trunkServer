#ifndef _UNION_MODULE_H__
#define _UNION_MODULE_H__

#include "BaseModule.h"

class CUnionModule : public CBaseModule
{
public:
    CUnionModule();
    ~CUnionModule();

    // IKxComm对象接收到完整数据后，调用的回调
    virtual void processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target);

    // IKxComm对象发生错误时，调用的回调
    virtual void processError(KxServer::IKxComm *target);
};


#endif //_UNION_MODULE_H__
