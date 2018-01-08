#ifndef _GAME_INTERFACE_H__
#define _GAME_INTERFACE_H__

#include "KXServer.h"
#include "ServerProtocol.h"

enum EEventId
{
    EventLogin,
    EventKick,
	EventClosed
};

struct IGameEvent
{
	virtual bool onUserEvent(int eventId, KxServer::IKxComm* target) = 0;
	virtual bool onUserProc(unsigned int id) = 0;
};

#endif //_GAME_INTERFACE_H__