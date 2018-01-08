#ifndef _CHANGE_CONNECT_MODULE_H__
#define _CHANGE_CONNECT_MODULE_H__

#include "ConnectModule.h"

class CChangeConnectModule : public CBaseModule
{
public:
	CChangeConnectModule();
	~CChangeConnectModule();

	void init();

	//游戏服务器返回服务器协议，转发给指定的前端
	virtual void processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target);

	//与游戏服务器断开连接，重新连接
	//（必须重新new一个，通讯ID变化，由框架清理相关资源）
	virtual void processError(KxServer::IKxComm *target);
	virtual void processEvent(int eventId, KxServer::IKxComm* target);

	bool addCallBackClient(KxServer::IKxComm *target);

protected:
	KxServer::IKxComm*								m_GMClient;					// 处理连接回调应答连接
	int												m_nConnectNum;				//重新切换某组服务器个数
	CConnectModule*									m_pConnectModule;			//游戏连接处理对象

};

#endif //_CHANGE_CONNECT_MODULE_H__

