#include "KxCommManager.h"
#include "Protocol.h"
#include "ServerProtocol.h"

using namespace std;
using namespace KxServer;

CKxCommManager::CKxCommManager()
: m_Gate(NULL)
{
}

CKxCommManager::~CKxCommManager()
{
    KXSAFE_RELEASE(m_Gate);
}

CKxCommManager* CKxCommManager::m_pInstance = NULL;
CKxCommManager * CKxCommManager::getInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new CKxCommManager();
    }
    return m_pInstance;
}

void CKxCommManager::destroy()
{
    if (m_pInstance != NULL)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

IKxComm * CKxCommManager::getKxComm(unsigned int nId)
{
    map<unsigned int, IKxComm*>::iterator ator = m_MapClient.find(nId);
    if (ator == m_MapClient.end())
    {
        return NULL;
    }

    return ator->second;
}

bool CKxCommManager::addKxComm(unsigned int nId, KxServer::IKxComm* pClient)
{
    map<unsigned int, IKxComm*>::iterator ator = m_MapClient.find(nId);

    if (ator != m_MapClient.end())
    {
        return false;
    }

    pClient->retain();
    m_MapClient[nId] = pClient;
    return true;
}

bool CKxCommManager::delKxComm(unsigned int nId)
{
    map<unsigned int, IKxComm*>::iterator ator = m_MapClient.find(nId);
    if (ator == m_MapClient.end())
    {
        return false;
    }

    IKxComm* pClient = ator->second;
    if (pClient == NULL)
    {
        return false;
    }

    pClient->release();
	m_MapClient.erase(ator);
    return true;
}

bool CKxCommManager::sendData(int uid, int main, int sub, char *buffer, int len, KxServer::IKxComm *pKxComm/* = NULL*/)
{
    IKxComm *pClient = NULL;
    if (NULL != pKxComm)
    {
        pClient = pKxComm;
    }
    else
    {
        pClient = (m_Gate == NULL) ? getKxComm(uid) : m_Gate;
    }

	if (pClient == NULL)
	{
		return false;
	}

	unsigned int buffSize = sizeof(Head)+sizeof(Head);

	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

	//设置头部
	Head* head = reinterpret_cast<Head*>(buff);
	head->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubTransmit);
	head->length = buffSize + len;
	head->id = uid;

	Head* clientHead = reinterpret_cast<Head*>(
		buff + (buffSize - sizeof(Head)));
	clientHead->id = uid;
	clientHead->MakeCommand(main,sub);
	clientHead->length = sizeof(Head)+len;

	pClient->sendData(buff, buffSize);
	if (NULL != buffer && len > 0)
	{
		pClient->sendData(buffer, len);
	}

	kxMemMgrRecycle(buff, buffSize);
	return true;
}

bool CKxCommManager::forwardData(int uid, int main, int sub, char *buffer, int len, KxServer::IKxComm *pKxComm /*= NULL*/)
{
    IKxComm *pClient = NULL;
    if (NULL != pKxComm)
    {
        pClient = pKxComm;
    }
    else
    {
        pClient = (m_Gate == NULL) ? getKxComm(uid) : m_Gate;
    }

    if (pClient == NULL)
    {
        return false;
    }

    unsigned int buffSize = sizeof(Head)+sizeof(Head);

    char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

    //设置头部
    Head* head = reinterpret_cast<Head*>(buff);
    head->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubForward);
    head->length = buffSize + len;
    head->id = uid;

    Head* clientHead = reinterpret_cast<Head*>(
        buff + (buffSize - sizeof(Head)));
    clientHead->id = uid;
    clientHead->MakeCommand(main, sub);
    clientHead->length = sizeof(Head)+len;

    pClient->sendData(buff, buffSize);
    if (NULL != buffer && len > 0)
    {
        pClient->sendData(buffer, len);
    }

    kxMemMgrRecycle(buff, buffSize);
    return true;
}

int CKxCommManager::broadcast(std::vector<int>& uids, int cmd, char* data, unsigned int len)
{
	//需要分配的服务器协议长度
	unsigned int buffSize = sizeof(Head)* 2;   //服务器协议头
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));
	//设置头部
	Head* head = reinterpret_cast<Head*>(buff);
	head->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubTransmit);
	head->length = buffSize + len;

	//填充广播信息
	Head* clientHead = reinterpret_cast<Head*>(
		buff + (buffSize - sizeof(Head)));
	clientHead->cmd = cmd;
	clientHead->length = sizeof(Head)+len;

	//buffSize用来计算发送的长度
	int ret = 0;
	for (std::vector<int>::iterator iter = uids.begin();
		iter != uids.end(); ++iter)
	{
		IKxComm *pClient = (m_Gate == NULL) ? getKxComm(*iter) : m_Gate;
		if (pClient == NULL)
		{
			continue;
		}

        head->id = *iter;
        clientHead->id = *iter;

		ret = pClient->sendData(buff, buffSize);
		if (NULL != data)
		{
			ret += pClient->sendData(data, len);
		}
	}

	kxMemMgrRecycle(buff, buffSize);
	return ret;
}

int CKxCommManager::setSessionRoute(int uid, int key, int value)
{
	IKxComm *pClient = (m_Gate == NULL) ? getKxComm(uid) : m_Gate;
	if (pClient == NULL)
	{
		return false;
	}

	int length = sizeof(Head)+sizeof(ServerProtocol::SProSetRoute);
	char *data = reinterpret_cast<char *>(kxMemMgrAlocate(length));

	Head *pHead = reinterpret_cast<Head *>(data);
	pHead->id = uid;
	pHead->cmd = MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubSetRoute);
	pHead->length = length;

	ServerProtocol::SProSetRoute *pSetRoute = reinterpret_cast<ServerProtocol::SProSetRoute *>(pHead + 1);
	pSetRoute->nRouteKey = key;
	pSetRoute->nRoteValue = value;

	int ret = pClient->sendData(data, length);
	kxMemMgrRecycle(data, length);
	return ret;
}
