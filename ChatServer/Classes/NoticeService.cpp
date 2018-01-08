#include "NoticeService.h"
#include "Protocol.h"
#include "NoticeProtocol.h"
#include "NetworkManager.h"
#include "KxMemPool.h"

using namespace std;
using namespace KxServer;

void CNoticeService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    switch (subcmd)
    {
    case CMD_NOTICE_SS:
        processNoticeService(uid, buffer, len, commun);
        break;
    default:
        break;
    }
}

void CNoticeService::processNoticeService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CNetWorkManager::getInstance()->broadCastData(CMD_NOTICE, CMD_NOTICE_SC, buffer, len);
}
