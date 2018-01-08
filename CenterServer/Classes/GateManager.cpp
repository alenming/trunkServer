#include "GateManager.h"
#include "Protocol.h"
#include "ServerProtocol.h"
#include "KXServer.h"

using namespace KxServer;
using namespace std;

CGateManager::CGateManager(void)
: m_Gate(NULL)
{
}

CGateManager::~CGateManager(void)
{
}

CGateManager* CGateManager::m_Instance = NULL;
CGateManager* CGateManager::getInstance()
{
    if(NULL == m_Instance)
    {
        m_Instance = new CGateManager();
    }

    return m_Instance;
}

void CGateManager::destroy()
{
    if (NULL != m_Instance)
    {
        delete m_Instance;
        m_Instance = NULL;
    }
}

int CGateManager::Send(char* buffer, unsigned int len)
{
    if (NULL == m_Gate || buffer == NULL)
    {
        return -1;
    }

    return m_Gate->sendData(buffer, len);
}

int CGateManager::Transmit(int uid, int cmd, char* data, unsigned int len)
{
    unsigned int buffSize = sizeof(Head) + sizeof(Head);

    char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

    //设置头部
    Head* head = reinterpret_cast<Head*>(buff);
    head->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubTransmit);
    //head->cmd = CommandTransmit;
    head->length = buffSize + len;
    head->id = uid;
    //KXLOGDEBUG("send server package length %d cmd %d", head->length, head->cmd);

   /* TransmitSS* transmit = reinterpret_cast<TransmitSS*>(buff + sizeof(HeadSS));
    transmit->userId = uid;*/

    //设置要发送的Client头部
    Head* clientHead = reinterpret_cast<Head*>(
        buff + (buffSize - sizeof(Head)));
	clientHead->id = uid;
    clientHead->cmd = cmd;
    clientHead->length = sizeof(Head) + len;

    int ret = Send(buff, buffSize);
    if (NULL != data)
    {
        ret += Send(data, len);
    }

    kxMemMgrRecycle(buff, buffSize);
    return ret;
}

int CGateManager::Broadcast(std::vector<int>& uids, int cmd, char* data, unsigned int len)
{
    //需要分配的服务器协议长度
    unsigned int buffSize = sizeof(Head)* 2;   //服务器协议头
    char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));
    //设置头部
    Head* head = reinterpret_cast<Head*>(buff);
    head->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubBoardCast);
    head->length = buffSize + len;

    //填充广播信息
    //DispatchSS* dispatch = reinterpret_cast<DispatchSS*>(buff + sizeof(HeadSS));
    //dispatch->userCount = uids.size();
    //int *puids = reinterpret_cast<int*>(buff + sizeof(HeadSS) + sizeof(int));

    //int i = 0;
    //for (vector<int>::iterator iter = uids.begin();
    //    iter != uids.end(); ++iter)
    //{
    //    puids[i] = *iter;
    //    //指针偏移到下一个
    //    ++i;
    //}

    Head* clientHead = reinterpret_cast<Head*>(
        buff + (buffSize - sizeof(Head)));
    clientHead->cmd = cmd;
    clientHead->length = sizeof(Head) + len;

    //buffSize用来计算发送的长度
    int ret = Send(buff, buffSize);
    if (NULL != data)
    {
        ret += Send(data, len);
    }

    kxMemMgrRecycle(buff, buffSize);
    return ret;
}