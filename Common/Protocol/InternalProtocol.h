#ifndef __INTERNAL_PROTOCOL_H__
#define __INTERNAL_PROTOCOL_H__

#pragma pack(1)

enum InternalProtocol
{
	INTER_CMD_DISCONNECT = 0,		//玩家断线
};

struct InternalDisconnectSS
{
	int uid;						//玩家id
	int tag;						//扩展字段
};

#pragma pack()

#endif //__INTERNAL_PROTOCOL_H__
