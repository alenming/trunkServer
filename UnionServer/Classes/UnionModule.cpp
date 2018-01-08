#include "UnionModule.h"
#include "Protocol.h"
#include "UnionService.h"
#include "ServerProtocol.h"
#include "SProtocolService.h"
#include "LoginUnionService.h"
#include "UnionExpiditionService.h"

using namespace KxServer;

CUnionModule::CUnionModule()
{
}


CUnionModule::~CUnionModule()
{
}

// IKxComm对象接收到完整数据后，调用的回调
void CUnionModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
    Head *head = reinterpret_cast<Head *>(buffer);
    int cmd = head->MainCommand();
    int subcmd = head->SubCommand();
    int uid = head->id;
    char *data = reinterpret_cast<char *>(head->data());
    int length = head->length - sizeof(Head);

    switch (cmd)
    {
    //处理服务器内部协议
    case ServerProtocol::ServerMain:
        {
            CSProtocolService::ProcessSProtocolService(subcmd, uid, data, length, target);
        }
        break;
    case CMD_LOGIN:
        {
            CLoginUnionService::processService(subcmd, uid, data, length, target);
        }
        break;

    case CMD_UNION:
        CUnionService::processService(subcmd, uid, data, length, target);
        break;
	case CMD_UNIONEXPIDITION:
		{
			CUnionExpiditionService::processService(cmd, subcmd, uid, data, length, target);
		}
		break;

    default:
        break;
    }

     
}

// IKxComm对象发生错误时，调用的回调
void CUnionModule::processError(IKxComm *target)
{

}
