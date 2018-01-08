/*
 * 实现跨平台的Select
 * 在每次Poll轮询的时候，会将异常的对象添加到删除列表中
 * 在轮询完之后，一次性删除
 *
 *  2013-04-20 By 宝爷
 *  
 */
#ifndef __SELECTPOLLER_H__
#define __SELECTPOLLER_H__

#include <set>
#include <map>

#include "KxCore.h"
#include "KxCommInterfaces.h"
#include "KxTimeVal.h"

namespace KxServer {

class KxSelectPoller : public IKxCommPoller
{
public:
	KxSelectPoller();
	virtual ~KxSelectPoller();

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

    inline void setTimeOut(float timeOut){ m_TimeOut.setFromfloat(timeOut); m_IsBlock = false; }
    inline void setTimeOut(long sec, long usec) { m_TimeOut = kxTimeVal(sec, usec); m_IsBlock = false; }
    inline void setPollBlock(bool isBlock) { m_IsBlock = isBlock; }

private:
	void clear();

private:
    int m_MaxCount;
    bool m_IsBlock;
    fd_set m_InSet;
    fd_set m_OutSet;
    fd_set m_ExceptSet;
	kxTimeVal m_TimeOut;

    std::map<KXCOMMID, IKxComm*> m_PollMap;
    std::set<IKxComm*> m_RemoveSet;
};

}

#endif
