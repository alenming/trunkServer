/*
 * 实现Linux下的Epoll
 *
 *  2013-04-20 By 宝爷
 *  
 */
#ifndef __EPOLLER_H__
#define __EPOLLER_H__

#include <map>
#include "KxCore.h"
#include "KxCommInterfaces.h"

namespace KxServer {
    
#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)

#include <sys/epoll.h>
#define MAX_EPOLL_EVENTS_PER_RUN 4096

class KxEpoller : public IKxCommPoller
{
public:
	KxEpoller(int maxEventQueue = 32000);
	virtual ~KxEpoller();

    // 进行轮询，并返回触发的事件数
    // 轮询失败返回-1
    virtual int poll();

    // 注册一个IKxComm通讯对象，并retain
    // 允许重复注册，重复注册相当于调用modifyCommObject
    // 添加成功(包括重复组成)返回0，失败返回-1
    virtual int addCommObject(IKxComm* obj, int type);

    // 修改IKxComm通讯对象所监听的事件
    // 未注册的IKxComm通讯对象调用该方法会失败
    // 修改成功返回0，失败返回-1
    virtual int modifyCommObject(IKxComm* obj, int type);

    // 移除已经注册的IKxComm通讯对象
    // 移除成功返回0，失败返回-1
    virtual int removeCommObject(IKxComm* obj);

    IKxComm* getComm(KXCOMMID cid);

    inline void setTimeout(int timeout) { m_TimeOut = timeout; }

private:
    int applyChange(KXCOMMID fd, int opt, int events);

private:
	int m_EpollFd;
	int m_MaxEventQueue;
	int m_TimeOut;
	epoll_event m_Events[MAX_EPOLL_EVENTS_PER_RUN];
    std::map<KXCOMMID, IKxComm*> m_PollMap;
};

#endif

}

#endif
