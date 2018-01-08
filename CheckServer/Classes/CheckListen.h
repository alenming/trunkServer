#ifndef _CCHECK_LISTEN_H__
#define _CCHECK_LISTEN_H__

#include "KXServer.h"

class CCheckListen : public KxServer::KxTCPListener
{
public:
	CCheckListen();
	~CCheckListen();

	virtual KxServer::KxTCPClienter* onAccept(KXCOMMID client);
};

#endif //_CCHECK_LISTEN_H__
