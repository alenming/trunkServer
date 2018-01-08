#ifndef _PLAYER_MODULE_H__
#define _PLAYER_MODULE_H__

//机器人通讯数据处理模块

#include "KXServer.h"

class CRebotPlayer;

class CPlayerModule : public KxServer::IKxModule
{
public:
	CPlayerModule();
	~CPlayerModule();

	void SetRebotPlayer(CRebotPlayer* pPlayer);
	virtual int processLength(char* buffer, unsigned int len);
	// IKxComm对象接收到完整数据后，调用的回调
	virtual void processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target);
	// IKxComm对象发生错误时，调用的回调
	virtual void processError(KxServer::IKxComm *target);
private:
	CRebotPlayer			*m_pRebot;
};


#endif //_PLAYER_MODULE_H__
