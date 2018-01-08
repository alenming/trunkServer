#include "PlayerModule.h"
#include "RebotPlayer.h"
#include "Protocol.h"
#include "RebotManager.h"
#include "ServerProtocol.h"


CPlayerModule::CPlayerModule()
{
}


CPlayerModule::~CPlayerModule()
{
}

void CPlayerModule::SetRebotPlayer(CRebotPlayer* pPlayer)
{

}

int CPlayerModule::processLength(char* buffer, unsigned int len)
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

// IKxComm对象接收到完整数据后，调用的回调
void CPlayerModule::processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target)
{
	Head *pHead = reinterpret_cast<Head*>(buffer);
	int cmd = pHead->MainCommand();
	int subCmd = pHead->SubCommand();

	char *pData = reinterpret_cast<char*>(pHead->data());
	int uid = pHead->id;
	int length = len - sizeof(Head);

	CRebotPlayer *pPlayer = CRebotManager::getInstance()->GetRebotPlayer(uid);

	if (pPlayer == NULL)
	{
		return;
	}

	pPlayer->ProcessRecvData(cmd, subCmd, pData, length);
}

// IKxComm对象发生错误时，调用的回调
void CPlayerModule::processError(KxServer::IKxComm *target)
{

}
