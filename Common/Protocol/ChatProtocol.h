#ifndef __CHAT_PROTOCOL_H__
#define __CHAT_PROTOCOL_H__

#pragma pack(1)

enum CHATPROTOCOL
{
	CMD_CHAT_CSBEGIN,
    CMD_CHAT_JOINROOM_CS,                   // 加入聊天室
    CMD_CHAT_QUITROOM_CS,                   // 退出聊天室(目前公会使用),无返回信息
    CMD_CHAT_SENDMESSAGE_CS,                // 发送信息,无返回信息
	CMD_CHAT_CSEND,

    CMD_CHAT_SCBEGIN = 100,
    CMD_CHAT_RECEIVE_MOREMESSAGE_SC,        // 接受多条信息
    CMD_CHAT_RECEIVE_SINGLEMESSAGE_SC,      // 接受单条信息
	CMD_CHAT_SCEND,

    CMD_CHAT_SSBEGIN = 200,
    CMD_CHAT_FUNCTION_SS,                   // 相关功能,禁言等等
    CMD_CHAT_SSEND,
};

// CMD_CHAT_JOINROOM_CS
struct ChatJoinRoomCS
{
    char roomType;          // 房间类型
    int roomId;             // 房间Id<公会Id>
};

// CMD_CHAT_QUITROOM_CS
struct ChatQuitRoomCS
{
    int roomId;             // 房间Id<公会Id>
};

// CMD_CHAT_SENDMESSAGE_CS
//ChatMessageInfo

// CMD_CHAT_RECEIVE_MOREMESSAGE_SC
struct ChatReceiveMessageSC
{
    char chatCount;         // 聊天条数
};

// CMD_CHAT_RECEIVE_SINGLEMESSAGE_SC
//ChatMessageInfo

enum EChatFunction
{
    ECF_SHUTUP,
    ECF_REMOVE_SHUTUP,
};

// CMD_CHAT_FUNCTION_SS
struct ChatFunctionSS
{
    char type;              // 类型 0禁言1移除禁言
    int uid;                // 玩家id
};

#pragma pack()

#endif //__CHAT_PROTOCOL_H__
