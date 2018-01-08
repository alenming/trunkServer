#include "GateModule.h"
#include "MatchServer.h"

using namespace KxServer;

CGateModule::CGateModule(void)
{
}

CGateModule::~CGateModule(void)
{
}

int CGateModule::processLength(char* buffer, unsigned int len)
{
    return len;
}

void CGateModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
	if (NULL != target)
	{
		CMatchServer::getInstance()->getMainPoller()->addCommObject(target, target->getPollType());
	}

	KXLOGDEBUG("session connected!");
}
