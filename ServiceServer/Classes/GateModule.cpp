#include "GateModule.h"
#include "ServiceServer.h"
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

void CGateModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
	//IKxComm* gate = CGateManager::getInstance()->GetGate();
// 	if (NULL == gate)
// 	{
	//CGateManager::getInstance()->SetGate(target);
    CKxCommManager::getInstance()->setGate(target);
    CServiceServer::getInstance()->getMainPoller()->addCommObject(target, target->getPollType());
// 	}
// 	else
// 	{
// 		//关闭之前也许可以发送一个消息过去，告诉它关闭的原因
// 		target->close();
// 	}
}
