#include "KxSelectPoller.h"

using namespace std;

#define KXSELECT_ERROR(obj, iter, pollset) {\
    obj->onError();\
    removeCommObject(obj); \
    pollset.erase(iter++);\
}

namespace KxServer {

KxSelectPoller::KxSelectPoller()
: m_MaxCount(0)
, m_IsBlock(false)
{
	FD_ZERO(&m_InSet);
	FD_ZERO(&m_OutSet);
    FD_ZERO(&m_ExceptSet);
    m_TimeOut.tv_sec = 0;
    m_TimeOut.tv_usec = 10;
    m_MaxCount = 0;
}

KxSelectPoller::~KxSelectPoller()
{
    for (map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.begin();
        iter != m_PollMap.end(); ++iter)
    {
        iter->second->release();
    }
}

int KxSelectPoller::poll()
{
    fd_set inset = m_InSet;
    fd_set outset = m_OutSet;
    fd_set expset = m_ExceptSet;
    int ret = select(m_MaxCount, &inset, &outset, &expset,
        m_IsBlock ? NULL : reinterpret_cast<timeval*>(&m_TimeOut));
    m_CurrentPollObject = NULL;
    if (ret > 0)
    {
        int eventCounts = ret;
        map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.begin();
        while (eventCounts > 0 && iter != m_PollMap.end())
        {
            IKxComm* obj = iter->second;
            m_CurrentPollObject = obj;
            KXCOMMID cid = obj->getCommId();
            int pollType = obj->getPollType();
            do
            {
                bool isError = false;
                if (FD_ISSET(cid, &inset))
                {
                    --eventCounts;
                    if (!isError && 0 > obj->onRecv())
                    {
                        isError = true;
                        KXSELECT_ERROR(obj, iter, m_PollMap);
                    }
                }

                if (FD_ISSET(cid, &outset))
                {
                    --eventCounts;
                    if (!isError && 0 > obj->onSend())
                    {
                        isError = true;
                        KXSELECT_ERROR(obj, iter, m_PollMap);
                    }
                }

                if (FD_ISSET(cid, &expset))
                {
                    --eventCounts;
                    if (!isError)
                    {
                        isError = true;
                        KXSELECT_ERROR(obj, iter, m_PollMap);
                    }
                }

                if (!isError && pollType != obj->getPollType())
                {
                    modifyCommObject(obj, obj->getPollType());
                }
                
                isError ? iter : ++iter;
            } while (false);
        }
    }
    else if(0 < ret)
    {
        // error
    }
    m_CurrentPollObject = NULL;
    clear();
    return ret;
}

int KxSelectPoller::addCommObject(IKxComm* obj, int type)
{
    if (NULL == obj || m_PollMap.find(obj->getCommId()) != m_PollMap.end())
    {
        return -1;
    }

    KXCOMMID cid = obj->getCommId();
    obj->retain();
    m_PollMap[cid] = obj;
    obj->setPollType(type);
    obj->setPoller(this);
#if(KX_TARGET_PLATFORM != KX_PLATFORM_WIN32)
    if(m_MaxCount <= cid)
    {
        m_MaxCount = cid + 1;
    }
#endif

    if (type & KXPOLLTYPE_IN)
    {
        FD_SET(cid, &m_InSet);
    }
    if (type &  KXPOLLTYPE_OUT)
    {
        FD_SET(cid, &m_OutSet);
    }
    if (type & KXPOLLTYPE_ERR)
    {
        FD_SET(cid, &m_ExceptSet);
    }

    return 0;
}

int KxSelectPoller::modifyCommObject(IKxComm* obj, int type)
{
    if (obj != NULL
        && m_PollMap.find(obj->getCommId()) == m_PollMap.end())
    {
        return -1;
    }

    KXCOMMID cid = obj->getCommId();
    if (type & KXPOLLTYPE_IN)
    {
        FD_SET(cid, &m_InSet);
    }
    else
    {
        FD_CLR(cid, &m_InSet);
    }

    if (type & KXPOLLTYPE_OUT)
    {
        FD_SET(cid, &m_OutSet);
    }
    else
    {
        FD_CLR(cid, &m_OutSet);
    }

    if (type & KXPOLLTYPE_ERR)
    {
        FD_SET(cid, &m_ExceptSet);
    }
    else
    {
        FD_CLR(cid, &m_ExceptSet);
    }

    obj->setPollType(type);
    return 0;
}

int KxSelectPoller::removeCommObject(IKxComm* obj)
{
    if (NULL != obj
        && m_PollMap.find(obj->getCommId()) != m_PollMap.end())
    {
        obj->setPoller(NULL);
        m_RemoveSet.insert(obj);
        return 0;
    }
    return -1;
}

IKxComm* KxSelectPoller::getComm(KXCOMMID cid)
{
    map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.find(cid);
    if (iter == m_PollMap.end())
    {
        return NULL;
    }
    else
    {
        return iter->second;
    }
}

void KxSelectPoller::clear()
{
    for (set<IKxComm*>::iterator iter = m_RemoveSet.begin();
        iter != m_RemoveSet.end(); ++iter)
    {
        IKxComm* obj = *iter;
        KXCOMMID cid = obj->getCommId();
        FD_CLR(cid, &m_InSet);
        FD_CLR(cid, &m_OutSet);
        FD_CLR(cid, &m_ExceptSet);
        obj->setPoller(NULL);
        obj->release();
        m_PollMap.erase(cid);
    }

    m_RemoveSet.clear();
}

}
