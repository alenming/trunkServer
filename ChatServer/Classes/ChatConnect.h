#ifndef _CHAT_CONNECT_H__
#define _CHAT_CONNECT_H__

#include "KXServer.h"
#include <map>

#define RECONNECT_TIME_INTERVAL		5			// 重连间隔5秒
#define MAX_RECONECT_TIME			0xFFFFFFFF	// 重连最大次数

class CChatConnector : public KxServer::KxTCPConnector
{
public:
	CChatConnector();
	virtual ~CChatConnector();

	// 连接指定的IP和端口
	virtual bool connect(const char* addr, int port, bool nonblock = true);
	// 连接成功失败回调
	virtual void onConnected(bool success);
	// 发生错误时回调，由IKxCommPoller调用
	virtual int onError();

	// 设置定时重连
	virtual void setTimer(int nSec);
	// 定时触发
	virtual void onTimer();

private:
	// 重连
	bool reconnect();

private:
	int m_nServerID;	    // 服务器ID
	int m_Port;			    // 服务器端口
	char m_strIP[16];	    // 服务器IP
	KxServer::KxTimerCallback<CChatConnector>* m_TimerCallBack;
};


#endif //SESSION_CONNECT_H__
