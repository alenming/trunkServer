/*
    聊天信息管理类
*/

#ifndef _CHAT_MESSAGE_MANAGER_H__
#define _CHAT_MESSAGE_MANAGER_H__

#include <map>
#include <list>
#include <set>

struct ChatMessageInfo
{
    char headId;                // 头像id
    char chatType;              // 聊天类型EChatType
    char userLv;                // 玩家等级
    char chatMessageType;       // 聊天信息类型EChatMessageType
    int userId;                 // 玩家id
    int sendTime;               // 发送时间戳
    int targetId;               // 目标id(公会聊天为公会Id, 私聊为对方id)
    int extend;                 // 扩展字段
    char name[20];              // 玩家名字
    char content[128];          // 内容
};

enum EChatMessageType
{
    CHAT_MESSAGE_TYPE_TEXT = 1, // 文本
    CHAT_MESSAGE_TYPE_AUDIO,    // 语音(暂时无用)
};

enum EChatType
{
    CHAT_TYPE_PRIVATE,          // 私聊(暂时无用)
    CHAT_TYPE_WORLD,            // 世界聊天
    CHAT_TYPE_UNION,            // 公会聊天(需要加入和退出)
};

enum EChatRoom
{
    CHAT_ROOM_NON,
    CHAT_ROOM_WORLD,            // 世界聊天室
    CHAT_ROOM_UNION,            // 公会聊天室
};

class CChatMessageManager
{
private:
    CChatMessageManager();
    ~CChatMessageManager();

public:
    static CChatMessageManager* getInstance();
    static void destroy();

    // 添加世界聊天信息
    void addWorldChatMessage(ChatMessageInfo &info);
    // 获取世界聊天信息
    const std::list<ChatMessageInfo>& getWorldChatMessage();
    // 添加公会聊天信息
    void addUnionChatMessage(int unionId, ChatMessageInfo &info);
    // 获取公会聊天信息
    void getUnionChatMessage(int unionId, std::list<ChatMessageInfo>& chatList);
    // 添加到禁言列表
    void addIllegal(int uid);
    // 从禁言列表移除
    void removeIllegal(int uid);
    // 是否是违规者
    bool isIllegal(int uid);

private:
    static CChatMessageManager *               m_pInstance;
    std::set<int>                              m_setIllegalUsers;   // 被禁言的玩家
    std::list<ChatMessageInfo>                 m_listWorldMessage;  //
    std::map<int, std::list<ChatMessageInfo> > m_mapUnionMessage;   // <unionid, list<> >
};


#endif //_CHAT_MESSAGE_MANAGER_H__
