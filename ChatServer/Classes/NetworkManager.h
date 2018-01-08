/*
 *  
 *  管理服务器网络结构, 通过NetworkManager可以对指定服务器发送消息
 *  
 */
#ifndef __CHAT_NETWORK_MANAGER_H__
#define __CHAT_NETWORK_MANAGER_H__

#include <map>
#include <vector>

#include "KXServer.h"
#include "ChatClient.h"

class CNetWorkManager
{
private:
    CNetWorkManager(void);
    virtual ~CNetWorkManager(void);

public:
    static CNetWorkManager* getInstance();
    static void destroy();

    //主动连后端服务器组ID
    bool addServer(int nGroupID, KxServer::IKxComm* obj);
	//通过类型获取服务器,Key对应的Value值来取模
	KxServer::IKxComm* getServer(int nGroupID, int nValue = 0);
    //获取某个服务器组
    std::vector<KxServer::IKxComm*>* getGroupServer(int nGroupID);
    //获取所有服务器
    std::map<int, std::vector<KxServer::IKxComm*> >& getAllServer() { return m_ServerMapByGroup; }

	//将连接添加到待验证连接列表中
	bool addGuest(unsigned int guestId, KxServer::IKxComm* obj);
	//获取待验证连接客户端
    KxServer::IKxComm* getGuest(unsigned int guestId);
    bool removeGuest(unsigned int guestId);

	//将连接转为连接OK的客户端
    bool changeGuestToUser(CChatClient* guest, unsigned int userId);
	//获取已经验证过的客户端
    KxServer::IKxComm* getUser(unsigned int userId);
    bool removeUser(unsigned int userId);

    //数据广播给所有玩家
    bool broadCastData(char *pszContext, int nLen);
    bool broadCastData(int nMainCmd, int nSubCmd, char *pszContext, int nLen);
    bool broadCastUnionData(int unionId, int nMainCmd, int nSubCmd, char *pszContext, int nLen);

    bool sendDataToClient(CChatClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen);
    bool sendDataToClient(int uid, int nMainCmd, int nSubCmd, char *pszContext, int nLen);
    //获取当前连接玩家数
    unsigned int getCurClientNum();
	//活动动态唯一ID
	unsigned int getGuestId();

	void closeAllServer();

    void addUnionChatUser(int unionId, int uid);
    void removeUnionChatUser(int unionId, int uid);

private:
    static CNetWorkManager* m_Instance;

    unsigned int m_GuestId;			                                    // 未验证客户端唯一ID
    std::map<int, std::vector<KxServer::IKxComm*> >	m_ServerMapByGroup; // 服务端连接管理
    std::map<unsigned int, KxServer::IKxComm*> m_GuestMap;		        // 未验证的客户端连接管理
    std::map<unsigned int, KxServer::IKxComm*> m_UserMap;       		// 已经验证过的客户端连接管理
    std::map<unsigned int, std::set<unsigned int> > m_UnionUserMap;     // 公会玩家<unionId, set<uid> >
};

#endif
