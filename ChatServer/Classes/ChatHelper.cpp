#include "ChatHelper.h"
#include "ChatMessageManager.h"

using namespace KxServer;
using namespace ServerProtocol;

CChatHelper::CChatHelper()
{
}

CChatHelper::~CChatHelper()
{
}

void CChatHelper::ServerSubSetPermission(char* buffer, unsigned int len, IKxComm *target, Head* head)
{
	unsigned int dataLen = len - sizeof(Head);
	CNetWorkManager *pNetWorkManager = CNetWorkManager::getInstance();
	CChatClient *pChatClient = dynamic_cast<CChatClient *>(pNetWorkManager->getUser(head->id));
	if (dataLen == sizeof(SProSetPermission) && pChatClient != NULL)
	{
		SProSetPermission *pPermissionSet = reinterpret_cast<SProSetPermission*>(buffer + sizeof(Head));
		pChatClient->setPermission(pPermissionSet->Permission);
	}
}

void CChatHelper::ServerSubDisconnect(char* buffer, unsigned int len, IKxComm *target, Head* head, CConnectModule* connectModule)
{
// 	CNetWorkManager *pNetWorkManager = CNetWorkManager::getInstance();
// 	CChatClient *pChatClient = dynamic_cast<CChatClient *>(pNetWorkManager->getUser(head->id));
// 	// 由业务逻辑决定是否踢人成功，成功才将用户踢下线，业务逻辑也可以在踢人之前
// 	if (pChatClient != NULL && connectModule->processUserEvent(EventKick, pChatClient))
// 	{
// 	// 设置头部
// 	head->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubPlayerOffLine);
// 	// 发送下线消息给哪些数据处理过的服务器
// 	pChatClient->sendDataToAllServer(buffer, sizeof(Head));
// 	// 执行玩家被T下线逻辑
// 	pChatClient->clean();
//	}
}

void CChatHelper::ServerSubSetRoute(char* buffer, unsigned int len, Head* head)
{
	unsigned int dataLen = len - sizeof(Head);
	CNetWorkManager *pNetWorkManager = CNetWorkManager::getInstance();
	CChatClient *pChatClient = dynamic_cast<CChatClient *>(pNetWorkManager->getUser(head->id));
	if (dataLen == sizeof(SProSetRoute) && pChatClient != NULL)
	{
        SProSetRoute *pRouteSet = reinterpret_cast<SProSetRoute*>(buffer + sizeof(Head));
        pChatClient->setConServer(pRouteSet->nRouteKey, pRouteSet->nRoteValue);
	}
}

void CChatHelper::ServerSubTransmit(char* buffer, unsigned int len, Head* head)
{
	CNetWorkManager *pNetWorkManager = CNetWorkManager::getInstance();
	CChatClient *pChatClient = dynamic_cast<CChatClient *>(pNetWorkManager->getUser(head->id));
	if (pChatClient != NULL)
	{
	    pChatClient->sendData(buffer + sizeof(Head), len - sizeof(Head));
	}
}

void CChatHelper::ServerSubBoardCast(char* buffer, unsigned int len)
{
	CNetWorkManager *pNetWorkManager = CNetWorkManager::getInstance();
	pNetWorkManager->broadCastData(buffer + sizeof(Head), len - sizeof(Head));
}

void CChatHelper::ServerSubInit(char* buffer, unsigned int len, CConnectModule* connectModule)
{
	unsigned int dataLen = len - sizeof(Head);
	CNetWorkManager *pNetWorkManager = CNetWorkManager::getInstance();
	if (dataLen < sizeof(SProInitSCC))
	{
		return;
	}

	unsigned int offset = sizeof(Head);
	SProInitSCC *pInitSS = reinterpret_cast<SProInitSCC*>(buffer + offset);
	CChatClient *pChatClient = dynamic_cast<CChatClient *>(pNetWorkManager->getGuest(pInitSS->nGuestId));
	// 连接断开/或者连接不存在
	if (pChatClient == NULL)
	{
		return;
	}

	offset += sizeof(SProInitSCC);
	if (pInitSS->nPermssion == 0)
	{
		// 验证失败
		pChatClient->sendData(buffer + offset, len - offset);
		pChatClient->clean();
	}
	else
	{
		pChatClient->sendData(buffer + offset, len - offset);
		pChatClient->setUserId(pInitSS->nRouteValue);
		pChatClient->setPermission(pInitSS->nPermssion);

        //// 触发登录成功事件，由具体接口处理顶号的实现
        //if (connectModule->processUserEvent(EventLogin, pChatClient))
        //{
        //    // 验证成功，guest转user
        //    pSessionClient->setConServer(pInitSS->nRouteKey, pInitSS->nRouteValue);
        //    pNetWorkManager->changeGuestToUser(pChatClient, pInitSS->nRouteValue);
        //}
        //else
        //{
        //    pSessionClient->clean();
        //}
        // 验证成功，guest转user
        pChatClient->setConServer(pInitSS->nRouteKey, pInitSS->nRouteValue);
        pNetWorkManager->changeGuestToUser(pChatClient, pInitSS->nRouteValue);
	}
}

void CChatHelper::ServerSubQueryLoad(char* buffer, unsigned int len, Head* head)
{
	CNetWorkManager *pNetWorkManager = CNetWorkManager::getInstance();
	CChatClient *pChatClient = dynamic_cast<CChatClient *>(pNetWorkManager->getUser(head->id));
	if (pChatClient != NULL)
	{
		unsigned int buffSize = sizeof(Head) + sizeof(SPServerLoad);
		char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));
		// 设置头部
		Head* head = reinterpret_cast<Head*>(buff);
		head->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubResponseLoad);
		head->length = buffSize;
		head->id = head->id;
		// 设置连接数
		SPServerLoad *pServerLoad = reinterpret_cast<SPServerLoad *>(buff + sizeof(Head));
		pServerLoad->ClientNum = pNetWorkManager->getCurClientNum();
		pChatClient->sendData(buff, buffSize);
		kxMemMgrRecycle(buff, buffSize);
	}
}

void CChatHelper::ServerSubForward(char* buffer, unsigned int len, Head* head)
{
    CNetWorkManager *pNetWorkManager = CNetWorkManager::getInstance();
    CChatClient *pChatClient = dynamic_cast<CChatClient *>(pNetWorkManager->getUser(head->id));
    if (pChatClient != NULL)
    {
        Head* head = reinterpret_cast<Head*>(buffer + sizeof(Head));
        CServerConfig * pServerConfig = CChatServer::getInstance()->getServerCoinfig();
        ServerRouteConfigData *pRouteConifgData = pServerConfig->getRouteConfigData(head->MainCommand(), head->SubCommand());
        if (NULL == pRouteConifgData)
        {
            return;
        }

        pChatClient->sendDataToServer(pRouteConifgData->GroupID, pRouteConifgData->RouteKey, 
            buffer + sizeof(Head), len - sizeof(Head));
    }
}

// 内容是否合法
bool CChatHelper::isContentLegal(int chatType, const std::string& str)
{
    if (str.find("604662") != string::npos)
    {
        return false;
    }
    return true;
}

// 玩家是否合法
bool CChatHelper::isPlayerLegal(int uid)
{
    if (uid == 137780)
    {
        return false;
    }
    return !CChatMessageManager::getInstance()->isIllegal(uid);
}
