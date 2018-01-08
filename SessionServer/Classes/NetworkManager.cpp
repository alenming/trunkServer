#include "NetworkManager.h"
#include "SessionClient.h"
#include "SessionServer.h"
#include "SessionConnect.h"
#include "Protocol.h"

using namespace KxServer;
using namespace std;

CNetWorkManager::CNetWorkManager(void)
: m_GuestId(1)
{
	m_bChanging = false;
}

CNetWorkManager::~CNetWorkManager(void)
{
	m_bChanging = false;
    map<unsigned int, KxServer::IKxComm*>::iterator iterGuest = m_GuestMap.begin();
    for (; iterGuest != m_GuestMap.end(); ++iterGuest)
    {
        iterGuest->second->release();
    }
    map<unsigned int, KxServer::IKxComm*>::iterator iterUser = m_UserMap.begin();
    for (; iterUser != m_UserMap.end(); ++iterUser)
    {
        iterUser->second->release();
    }
    map<int, vector<KxServer::IKxComm*> >::iterator iterServer = m_ServerMapByGroup.begin();
    for (; iterServer != m_ServerMapByGroup.end(); ++iterServer)
    {
        vector<KxServer::IKxComm*>& vecServer = iterServer->second;
        for (vector<KxServer::IKxComm*>::iterator iterSock = vecServer.begin();
            iterSock != vecServer.end(); ++iterSock)
        {
            (*iterSock)->release();
        }
    }
}

CNetWorkManager* CNetWorkManager::m_Instance = NULL;
CNetWorkManager* CNetWorkManager::getInstance()
{
    if (NULL == m_Instance)
    {
        m_Instance = new CNetWorkManager();
    }

    return m_Instance;
}

void CNetWorkManager::destroy()
{
    if (NULL != m_Instance)
    {
        delete m_Instance;
        m_Instance = NULL;
    }
}

// 通过类型获取服务器,Key对应的Value值来取模
KxServer::IKxComm* CNetWorkManager::getServer(int nGroupID, int nValue)
{
	map<int, std::vector<KxServer::IKxComm*> >::iterator ator = m_ServerMapByGroup.find(nGroupID);
	if (ator == m_ServerMapByGroup.end())
	{
		return NULL;
	}

	vector<KxServer::IKxComm*> &vectServer = ator->second;
    if (vectServer.size() == 0)
    {
        return NULL;
    }

	// 有Value值，按Value取模
	if (nValue != 0)
	{
        return vectServer[nValue % vectServer.size()];
	}
	else
	{
        return vectServer[rand() % vectServer.size()];
	}
}

// 获取某个服务器组
std::vector<KxServer::IKxComm*>* CNetWorkManager::getGroupServer(int nGroupID)
{
    map<int, std::vector<KxServer::IKxComm*> >::iterator ator = m_ServerMapByGroup.find(nGroupID);
    if (ator == m_ServerMapByGroup.end())
    {
        return NULL;
    }
    vector<KxServer::IKxComm*> *pVectServer = &(ator->second);
    if (pVectServer->size() == 0)
    {
        return NULL;
    }
    return pVectServer;
}

//获取某个服务器组备份
std::vector<KxServer::IKxComm*>* CNetWorkManager::getBakGroupServer(int nGroupID)
{
	map<int, std::vector<KxServer::IKxComm*> >::iterator ator = m_BakServerMapByGroup.find(nGroupID);
	if (ator == m_BakServerMapByGroup.end())
	{
		return NULL;
	}

	vector<KxServer::IKxComm*> *pVectServer = &(ator->second);
	if (pVectServer->size() == 0)
	{
		return NULL;
	}
	return pVectServer;
}

bool CNetWorkManager::addServer(int nGroupID, KxServer::IKxComm* obj)
{
    if (obj == NULL)
    {
        return false;
    }

	std::map<int, vector<KxServer::IKxComm*> >::iterator ator = m_ServerMapByGroup.find(nGroupID);
	if (NULL != obj && ator == m_ServerMapByGroup.end())
    {
		vector<KxServer::IKxComm*> ServerList;
		ServerList.push_back(obj);
		m_ServerMapByGroup[nGroupID] = ServerList;
    }
	else
	{
		vector<KxServer::IKxComm*> &serverList = ator->second;
		serverList.push_back(obj);
	}
    obj->retain();
    return true;
}

//添加备份后端服务器组ID
bool CNetWorkManager::addBakServer(int nGroupID, KxServer::IKxComm* obj)
{
	if (obj == NULL)
	{
		return false;
	}

	std::map<int, vector<KxServer::IKxComm*> >::iterator ator = m_BakServerMapByGroup.find(nGroupID);
	if (NULL != obj && ator == m_BakServerMapByGroup.end())
	{
		vector<KxServer::IKxComm*> ServerList;
		ServerList.push_back(obj);
		m_BakServerMapByGroup[nGroupID] = ServerList;
	}
	else
	{
		vector<KxServer::IKxComm*> &serverList = ator->second;
		serverList.push_back(obj);
	}
	obj->retain();
	return true;
}

//清除某个备份服务器组ID
bool CNetWorkManager::clearBakServer(int nGroupID)
{
	std::map<int, vector<KxServer::IKxComm*> >::iterator ator = m_BakServerMapByGroup.find(nGroupID);
	if (ator != m_BakServerMapByGroup.end())
	{
		vector<KxServer::IKxComm*> ServerList = ator->second;
		for (int i = 0; i < ServerList.size(); i++)
		{
			ServerList[i]->release();
		}

		m_BakServerMapByGroup.erase(ator);
	}

	return true;
}

bool CNetWorkManager::addGuest(unsigned int guestId, KxServer::IKxComm* obj)
{
	map<unsigned int, KxServer::IKxComm*>::iterator iter = m_GuestMap.find(guestId);
	if (iter != m_GuestMap.end())
	{
		//客户端ID重复
		return false;
	}

	m_GuestMap[guestId] = obj;
	KXSAFE_RETAIN(obj);
	return true;
}

// 获取待验证连接客户端
KxServer::IKxComm* CNetWorkManager::getGuest(unsigned int guestId)
{
	map<unsigned int, KxServer::IKxComm*>::iterator iter = m_GuestMap.find(guestId);
	if (iter == m_GuestMap.end())
	{
		//客户端ID找不到
		return NULL;
	}
	return iter->second;
}

bool CNetWorkManager::removeGuest(unsigned int guestId)
{
	map<unsigned int, KxServer::IKxComm*>::iterator iter = m_GuestMap.find(guestId);
    if (iter == m_GuestMap.end())
    {
        return false;
    }

    KXSAFE_RELEASE(iter->second);
    m_GuestMap.erase(iter);
    return true;
}

// 将连接转为连接OK的客户端
bool CNetWorkManager::changeGuestToUser(CSessionClient* guest, unsigned int userId)
{
    map<unsigned int, KxServer::IKxComm*>::iterator iter = m_GuestMap.find(guest->getGuestId());
	if (iter == m_GuestMap.end())
	{
		// 客户端ID找不到
		return false;
	}
    // 此处本该有一次release和retain，相互抵消
    m_UserMap[userId] = guest;
    m_GuestMap.erase(iter);
	return true;
}

// 获取已经验证过的客户端
KxServer::IKxComm* CNetWorkManager::getUser(unsigned int userId)
{
	map<unsigned int, KxServer::IKxComm*>::iterator iter = m_UserMap.find(userId);
	if (iter == m_UserMap.end())
	{
		//客户端ID找不到
		return NULL;
	}
	return iter->second;
}

bool CNetWorkManager::removeUser(unsigned int userId)
{
    map<unsigned int, KxServer::IKxComm*>::iterator iter = m_UserMap.find(userId);
    if (iter == m_UserMap.end())
    {
        return false;
    }

    KXSAFE_RELEASE(iter->second);
    m_UserMap.erase(iter);
    return true;
}

// 数据广播给所有玩家
bool CNetWorkManager::broadCastData(char *pszContext, int nLen)
{
    for (map<unsigned int, KxServer::IKxComm*>::iterator ator = m_UserMap.begin();
        ator != m_UserMap.end(); ++ator)
    {
        ator->second->sendData(pszContext, nLen);
    }
    return true;
}

bool CNetWorkManager::sendDataToClient(CSessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen)
{
	unsigned int buffSize = sizeof(Head) + nLen;
	bool ret = false;
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

	//设置头部
	Head* head = reinterpret_cast<Head*>(buff);
	head->MakeCommand(nMainCmd, nSubCmd);
	head->length = buffSize + nLen;
	head->id = pClient->getUserId();
	memcpy(buff + sizeof(Head), pszContext, nLen);
	pClient->sendData(buff, buffSize);
	kxMemMgrRecycle(buff, buffSize);
	return ret;
}

//获取当前连接玩家数
unsigned int CNetWorkManager::getCurClientNum()
{
    return m_GuestMap.size() + m_UserMap.size();
}

unsigned int CNetWorkManager::genGuestId()
{
	if (m_GuestId >= 0xFFFFFFFF)
	{
		m_GuestId = 1;
	}
	return m_GuestId++;
}

void CNetWorkManager::closeAllServer()
{
	/*std::map<int, std::vector<KxServer::IKxComm*> >::iterator mapit = m_ServerMapByGroup.begin();
	std::vector<KxServer::IKxComm*>::iterator vecit;
	for (; mapit!=m_ServerMapByGroup.end(); ++mapit)
	{
		vecit = mapit->second.begin();
		for (; vecit!=mapit->second.end(); ++vecit)
		{
			CSessionServer::getInstance()->getMainPoller()->removeCommObject((*vecit));
			(*vecit)->close();
			(*vecit)->release();
		}
	}*/
	m_ServerMapByGroup.clear();
}

void CNetWorkManager::onTimer(const KxServer::kxTimeVal& now)
{
	for (std::map<int, std::vector<KxServer::IKxComm*> >::iterator ator = m_BakServerMapByGroup.begin(); ator != m_BakServerMapByGroup.end(); ++ator)
	{
		int nGroupID = ator->first;
		//删除旧连接
		std::map<int, std::vector<KxServer::IKxComm*> >::iterator mapit = m_ServerMapByGroup.find(nGroupID);
		if (mapit != m_ServerMapByGroup.end())
		{
			std::vector<KxServer::IKxComm*> &VectComm = mapit->second;
			for (int i = 0; i < VectComm.size(); i++)
			{
				VectComm[i]->release();
				CSessionServer::getInstance()->getMainPoller()->removeCommObject(VectComm[i]);
			}
			m_ServerMapByGroup.erase(mapit);
		}

		//添加新的连接
		m_ServerMapByGroup[nGroupID] = ator->second;
	}

	m_BakServerMapByGroup.clear();
	m_bChanging = false;
}