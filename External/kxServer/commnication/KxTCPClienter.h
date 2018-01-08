/*
 * 服务端监听Socket Accept之后返回的Socket对象
 * 通过此对象可以与客户端通讯
 * 
 *  2013-04-20 By 宝爷
 *  
 */
#ifndef __TCPCLIENTER_H__
#define __TCPCLIENTER_H__

#include "KxTCPUnit.h"

namespace KxServer {

class KxTCPClienter : public KxTCPUnit
{
public:
	KxTCPClienter();
	virtual ~KxTCPClienter();

    virtual bool init(KXCOMMID fd);

	virtual void retain();

	virtual void release();

};

}

#endif
