/*
* ÁÄÌì·þÎñ²ã
*/

#ifndef __CHAT_SERVICE_H__
#define __CHAT_SERVICE_H__

#include "KxCommInterfaces.h"
#include <list>

struct ChatMessageInfo;
class CChatService
{
public:
	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processJoinRoomService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processQuitRoomService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processSendMessageService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

    static void processChatFunctionService(int uid, char *buffer, int len, KxServer::IKxComm *commun);

private:
    static void sendLastMessages(int uid, const std::list<ChatMessageInfo>& messageList);
};

#endif //__CHAT_SERVICE_H__
