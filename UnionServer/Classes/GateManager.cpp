#include "GateManager.h"
#include "Protocol.h"
#include "ServerProtocol.h"
#include "KXServer.h"
#include "KxCommManager.h"

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

int CGateManager::Send(unsigned int uid, char* buffer, unsigned int len)
{
    IKxComm *pKxComm = CKxCommManager::getInstance()->getKxComm(uid);

    if (pKxComm == NULL)
    {
        return -1;
    }

    return pKxComm->sendData(buffer, len);
}

int CGateManager::Transmit(int uid, int cmd, char* data, unsigned int len,KxServer::IKxComm *commu)
{
    unsigned int buffSize = sizeof(Head)+sizeof(Head);

    char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

    //设置头部
    Head* head = reinterpret_cast<Head*>(buff);
    head->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubTransmit);
    head->length = buffSize + len;
    head->id = uid;

    //设置要发送的Client头部
    Head* clientHead = reinterpret_cast<Head*>(
        buff + (buffSize - sizeof(Head)));
    clientHead->id = uid;
    clientHead->cmd = cmd;
    clientHead->length = sizeof(Head)+len;

    int ret = 0;
    if (commu == NULL)
    {
        ret = Send(uid, buff, buffSize);
        if (NULL != data)
        {
            ret += Send(uid, data, len);
        }
    }
    else
    {
        ret = commu->sendData(buff, buffSize);
        if (NULL != data)
        {
            ret += commu->sendData(data, len);
        }
    }

    kxMemMgrRecycle(buff, buffSize);
    return ret;
}

//设置路由
int CGateManager::RouteSetTransmit(int uid, int nKey, int nValue, KxServer::IKxComm* gate)
{
    unsigned int buffSize = sizeof(Head)+ sizeof(ServerProtocol::SProSetRoute);

    char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

    //设置头部
    Head* head = reinterpret_cast<Head*>(buff);
    head->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubSetRoute);
    head->length = buffSize;
    head->id = uid;

    ServerProtocol::SProSetRoute *pSetRoute = reinterpret_cast<ServerProtocol::SProSetRoute*>(buff + sizeof(Head));

    pSetRoute->nRouteKey = nKey;
    pSetRoute->nRoteValue = nValue;

    int ret = -1;
    if (gate != NULL)
    {
        ret = gate->sendData(buff, buffSize);
    }
    else
    {
        ret = Send(uid, buff, buffSize);
    }

    kxMemMgrRecycle(buff, buffSize);
    return ret;
}

int CGateManager::Broadcast(unsigned int uid, int cmd, char* data, unsigned int len)
{
    //需要分配的服务器协议长度
    unsigned int buffSize = sizeof(Head)* 2;   //服务器协议头
    char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));
    //设置头部
    Head* head = reinterpret_cast<Head*>(buff);
    head->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubBoardCast);
    head->length = buffSize + len;
    head->id = uid;

    Head* clientHead = reinterpret_cast<Head*>(
        buff + (buffSize - sizeof(Head)));
    clientHead->cmd = cmd;
    clientHead->length = sizeof(Head)+len;

    //buffSize用来计算发送的长度
    int ret = Send(uid,buff, buffSize);
    if (NULL != data)
    {
        ret += Send(uid,data, len);
    }

    kxMemMgrRecycle(buff, buffSize);
    return ret;
}