#include "ChatService.h"
#include "Protocol.h"
#include "ChatProtocol.h"
#include "NetworkManager.h"
#include "ChatMessageManager.h"
#include "KxMemPool.h"
#include "ChatHelper.h"

using namespace std;
using namespace KxServer;

void CChatService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    switch (subcmd)
    {
    case CMD_CHAT_JOINROOM_CS:
        processJoinRoomService(uid, buffer, len, commun);
        break;
    case CMD_CHAT_QUITROOM_CS:
        processQuitRoomService(uid, buffer, len, commun);
        break;
    case CMD_CHAT_SENDMESSAGE_CS:
        processSendMessageService(uid, buffer, len, commun);
        break;
    case CMD_CHAT_FUNCTION_SS:
        processSendMessageService(uid, buffer, len, commun);
        break;
    default:
        break;
    }
}

void CChatService::processJoinRoomService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    if (len != sizeof(ChatJoinRoomCS))
    {
        return;
    }

    if (!CChatHelper::isPlayerLegal(uid))
    {
        KXLOGINFO("uid %d socketId %d is a bad guy join room",
            uid, commun->getCommId());
        return;
    }

    ChatJoinRoomCS *pChatJoinRoomCS = reinterpret_cast<ChatJoinRoomCS*>(buffer);
    if (CHAT_ROOM_WORLD == pChatJoinRoomCS->roomType)
    {
        // 下发世界聊天的信息
        sendLastMessages(uid, CChatMessageManager::getInstance()->getWorldChatMessage());
    }
    else
    {
        CNetWorkManager::getInstance()->addUnionChatUser(pChatJoinRoomCS->roomId, uid);

        list<ChatMessageInfo> messageList;
        CChatMessageManager::getInstance()->getUnionChatMessage(pChatJoinRoomCS->roomId, messageList);

        // 下发公会聊天的信息
        sendLastMessages(uid, messageList);
    }
}

void CChatService::processQuitRoomService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    if (len != sizeof(ChatQuitRoomCS))
    {
        return;
    }

    ChatQuitRoomCS *pChatQuitRoomCS = reinterpret_cast<ChatQuitRoomCS*>(buffer);
    CNetWorkManager::getInstance()->removeUnionChatUser(pChatQuitRoomCS->roomId, uid);
}

void CChatService::processSendMessageService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    if (len != sizeof(ChatMessageInfo))
    {
        return;
    }

    CNetWorkManager *pChatUserManager = CNetWorkManager::getInstance();

    ChatMessageInfo *pChatMessageInfo = reinterpret_cast<ChatMessageInfo*>(buffer);
    pChatMessageInfo->sendTime = int(time(NULL));
    // 如果内容非法，则直接返回给该玩家（让他以为发送成功，像个SB一样在那里狂发）
    if (!CChatHelper::isContentLegal(pChatMessageInfo->chatType, pChatMessageInfo->content))
    {
        commun->sendData(buffer, len);
        KXLOGINFO("uid %d socketId %d is a bad guy, say %s",
            uid, commun->getCommId(), pChatMessageInfo->content);
        return;
    }
    
    if (CHAT_TYPE_WORLD == pChatMessageInfo->chatType)
    {
        //

        // 广播给所有在聊天服的玩家
        pChatUserManager->broadCastData(CMD_CHAT, 
            CMD_CHAT_RECEIVE_SINGLEMESSAGE_SC,
            buffer, 
            len);

        // 添加到世界聊天列表
        CChatMessageManager::getInstance()->addWorldChatMessage(*pChatMessageInfo);
    }
    else if (CHAT_TYPE_UNION == pChatMessageInfo->chatType)
    {
        // 广播给登录聊天服并且加入该公会聊天室的玩家
        pChatUserManager->broadCastUnionData(pChatMessageInfo->targetId,
            CMD_CHAT,
            CMD_CHAT_RECEIVE_SINGLEMESSAGE_SC,
            buffer,
            len);

        // 添加到公会聊天列表
        CChatMessageManager::getInstance()->addUnionChatMessage(pChatMessageInfo->targetId, *pChatMessageInfo);
    }
}

void CChatService::processChatFunctionService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    if (len != sizeof(ChatFunctionSS))
    {
        return;
    }

    ChatFunctionSS *pChatFunctionSS = reinterpret_cast<ChatFunctionSS*>(buffer);
    if (ECF_SHUTUP == pChatFunctionSS->type)
    {
        CChatMessageManager::getInstance()->addIllegal(pChatFunctionSS->uid);
    }
    else if (ECF_REMOVE_SHUTUP == pChatFunctionSS->type)
    {
        CChatMessageManager::getInstance()->removeIllegal(pChatFunctionSS->uid);
    }
}

void CChatService::sendLastMessages(int uid, const list<ChatMessageInfo>& messageList)
{
    if (messageList.empty())
    {
        return;
    }

    int len = sizeof(ChatReceiveMessageSC)+sizeof(ChatMessageInfo)* messageList.size();
    char *buff = reinterpret_cast<char*>(kxMemMgrAlocate(len));

    ChatReceiveMessageSC *pChatReceiveMessageSC = reinterpret_cast<ChatReceiveMessageSC*>(buff);
    pChatReceiveMessageSC->chatCount = messageList.size();

    ChatMessageInfo *pChatMessageInfo = reinterpret_cast<ChatMessageInfo*>(pChatReceiveMessageSC + 1);
    list<ChatMessageInfo>::const_reverse_iterator iter = messageList.rbegin();
    for (; iter != messageList.rend(); ++iter)
    {
        memcpy(pChatMessageInfo, &(*iter), sizeof(ChatMessageInfo));
        pChatMessageInfo += 1;
    }

    CNetWorkManager::getInstance()->sendDataToClient(uid
        , CMD_CHAT, CMD_CHAT_RECEIVE_MOREMESSAGE_SC
        , buff, len);

    kxMemMgrRecycle(buff, len);
}
