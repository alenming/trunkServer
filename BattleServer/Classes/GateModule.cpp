#include "GateModule.h"
#include "BattleServer.h"
#include "KxCommManager.h"

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

//when you recv a msg, Process will be call, the target is the msg sender
//监听模块，只允许一个Session连接进来
void CGateModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
	if (NULL != target)
	{
		CKxCommManager::getInstance()->setGate(target);
		CBattleServer::getInstance()->getMainPoller()->addCommObject(target, target->getPollType());
	}
}
