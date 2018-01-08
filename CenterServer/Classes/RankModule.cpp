#include "RankModule.h"
#include "RankService.h"
#include "PayProtocol.h"
#include "ModelHelper.h"
#include "Protocol.h"
#include "ServerProtocol.h"
#include "KxLog.h"

CRankModule::CRankModule()
{
}

CRankModule::~CRankModule(void)
{
}

int CRankModule::processLength(char* buffer, unsigned int len)
{
	// 返回一个包的长度, 由包头解析, 解析的长度包括包头
	int ret = reinterpret_cast<Head*>(buffer)->length;
	return ret;
}

void CRankModule::processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target)
{
	Head *head = reinterpret_cast<Head *>(buffer);
	int cmd = head->MainCommand();
	int subcmd = head->SubCommand();
	int uid = head->id;
	char *data = reinterpret_cast<char *>(head->data());
	int length = head->length - sizeof(Head);

	//KXLOGDEBUG("RankServer maincmd %d, subcmd %d, len %d, uid %d", cmd, subcmd, len, uid);
	switch (cmd)
	{
	case CMD_RANK:
		CRankService::ProcessService(subcmd, uid, data, length, target);
		break;
	
	default:
		break;
	}
}

void CRankModule::processError(KxServer::IKxComm *target)
{

}