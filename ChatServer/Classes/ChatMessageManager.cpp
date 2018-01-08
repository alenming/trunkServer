#include "ChatMessageManager.h"

using namespace std;

const static unsigned int MAX_CHAT = 50;

CChatMessageManager* CChatMessageManager::m_pInstance = NULL;
CChatMessageManager::CChatMessageManager()
{
}

CChatMessageManager::~CChatMessageManager()
{

}

CChatMessageManager* CChatMessageManager::getInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CChatMessageManager();
	}
	return m_pInstance;
}

void CChatMessageManager::destroy()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void CChatMessageManager::addWorldChatMessage(ChatMessageInfo &info)
{
    if (m_listWorldMessage.size() >= MAX_CHAT)
    {
        m_listWorldMessage.pop_front();
    }

    m_listWorldMessage.push_back(info);
}

const list<ChatMessageInfo>& CChatMessageManager::getWorldChatMessage()
{
    return m_listWorldMessage;
}

void CChatMessageManager::addUnionChatMessage(int unionId, ChatMessageInfo &info)
{
    map<int, list<ChatMessageInfo> >::iterator iter = m_mapUnionMessage.find(unionId);
    if (iter != m_mapUnionMessage.end())
    {
        if (iter->second.size() > MAX_CHAT)
        {
            iter->second.pop_front();
        }
    }

    m_mapUnionMessage[unionId].push_back(info);
}

void CChatMessageManager::getUnionChatMessage(int unionId, std::list<ChatMessageInfo>& chatList)
{
    map<int, list<ChatMessageInfo> >::iterator iter = m_mapUnionMessage.find(unionId);
    if (iter != m_mapUnionMessage.end())
    {
        chatList.assign(iter->second.begin(), iter->second.end());
    }
}

void CChatMessageManager::addIllegal(int uid)
{
    m_setIllegalUsers.insert(uid);
}

void CChatMessageManager::removeIllegal(int uid)
{
    set<int>::iterator iter = m_setIllegalUsers.find(uid);
    if (iter != m_setIllegalUsers.end())
    {
        m_setIllegalUsers.erase(iter);
    }
}

bool CChatMessageManager::isIllegal(int uid)
{
    return m_setIllegalUsers.find(uid) != m_setIllegalUsers.end();
}
