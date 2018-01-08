#ifndef __SessionEvent_H__
#define __SessionEvent_H__
#include "GameInterface.h"
#include "KXServer.h"
#include "SessionServer.h"
#include "ServerProtocol.h"
#include "SessionClient.h"

using namespace KxServer;

class CSessionEvent :public IGameEvent
{
public:
	CSessionEvent();
	virtual ~CSessionEvent();

	virtual bool onUserEvent(int eventId, KxServer::IKxComm* target);
	virtual bool onUserProc(unsigned int id);
	bool sendDataToActor(CSessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen);
private:

};

#endif

