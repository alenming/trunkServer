/*
 * 服务器基类，定义服务器的初始化，运行等行为
 *
 *  2013-04-20 By 宝爷
 *  
 */
#ifndef __BASESERVER_H__
#define __BASESERVER_H__

#include "KxCommInterfaces.h"
#include "KxTimerManager.h"

namespace KxServer {

class KxBaseServer : public KxObject
{
protected:
	KxBaseServer();
	virtual ~KxBaseServer();

public:
    static KxBaseServer* getInstance() 
    {
        return m_Server; 
    }
    static void destory()
    {
        if (m_Server != NULL)
        {
            delete m_Server;
            m_Server = NULL;
        }
    }

	// 启动服务器
	virtual void startServer();

	// 服务器主循环
	virtual void runServer();
    
    // 服务器主循环回调
    virtual void onServerUpdate();

	// 服务器初始化
	virtual bool onServerInit();

	// 服务器结束
	virtual void onServerUninit();

    virtual void stopServer() { m_IsRunning = false; }

    KxTimerManager* getTimerManager()
    {
        return m_TimerMgr;
    }

    IKxCommPoller* getMainPoller()
    {
        return m_Poller;
    }

	void SetServerID(int nServerID) { m_ServerID = nServerID; }

	int  GetServerID() { return m_ServerID; }

protected:
	IKxCommPoller*	m_Poller;
    KxTimerManager* m_TimerMgr;
	bool            m_IsRunning;
	int				m_ServerID;
	int				m_Tick;
    static KxBaseServer* m_Server;
};

}

#endif
