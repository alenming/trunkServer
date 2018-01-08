#include "KxTimerManager.h"
#include "KxLog.h"

#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)

int gettimeofday(struct timeval * val, struct timezone * zone)
{
    if (NULL != val)
    {
        LARGE_INTEGER liTime, liFreq;
        QueryPerformanceFrequency(&liFreq);
        QueryPerformanceCounter(&liTime);
        val->tv_sec = (long)(liTime.QuadPart / liFreq.QuadPart);
        val->tv_usec = (long)(liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val->tv_sec * 1000000.0);
    }
    return 0;
}

#endif

using namespace std;
namespace KxServer {

KxTimerObject::KxTimerObject()
:m_TimerList(NULL),
m_Prev(NULL),
m_Next(NULL),
m_Delay(0.0f),
m_Repeat(0),
m_TimeVal()
{
}

KxTimerObject::KxTimerObject(float delay, int repeat)
:m_TimerList(NULL),
m_Prev(NULL),
m_Next(NULL),
m_Delay(delay),
m_Repeat(repeat),
m_TimeVal()
{
}

KxTimerObject::~KxTimerObject()
{
}

void KxTimerObject::onTimer(const kxTimeVal& now)
{
}

void KxTimerObject::stop()
{
    if (NULL != m_TimerList)
    {
        m_Repeat = 0;
        m_TimerList->remove(this);
    }
}

void KxTimerObject::start(const kxTimeVal& now)
{
    m_TimeVal = now + m_Delay;
}

void KxTimerObject::startWithTime(const kxTimeVal& now)
{
    m_TimeVal = now;
}

KxTimerList::KxTimerList()
    :m_Length(0),
    m_Head(NULL),
    m_Tail(NULL),
    m_Timer(NULL)
{

}

KxTimerList::~KxTimerList()
{

}

bool KxTimerList::pushBack(KxTimerObject* obj)
{
    if (NULL == obj
        || NULL != obj->m_TimerList)
    {
        return false;
    }

    if (NULL == m_Tail)
    {
        m_Head = obj;
        obj->m_Prev = obj->m_Next = NULL;

    }
    else
    {
        m_Tail->m_Next = obj;
        obj->m_Prev = m_Tail;
        obj->m_Next = NULL;
    }
    
    m_Tail = obj;
    obj->m_TimerList = this;
    KXSAFE_RETAIN(obj);
    ++m_Length;
    return true;
}

bool KxTimerList::pushFront(KxTimerObject* obj)
{
    if (NULL == obj
        || NULL != obj->m_TimerList)
    {
        return false;
    }

    if (NULL == m_Head)
    {
        m_Tail = obj;
        obj->m_Prev = obj->m_Next = NULL;
    }
    else
    {
        m_Head->m_Prev = obj;
        obj->m_Next = m_Head;
        obj->m_Prev = NULL;
    }

    m_Head = obj;
    obj->m_TimerList = this;
    KXSAFE_RETAIN(obj);
    ++m_Length;
    return true;
}

bool KxTimerList::remove(KxTimerObject* obj)
{
    if (NULL == obj
        || obj->m_TimerList != this)
    {
        return false;
    }

    if (m_Head == obj)
    {
        m_Head = obj->m_Next;
        if (NULL != m_Head)
        {
            m_Head->m_Prev = NULL;
        }
        else
        {
            m_Tail = NULL;
        }
    }
    else if (m_Tail == obj)
    {
        m_Tail = obj->m_Prev;
        m_Tail->m_Next = NULL;
    }
    else
    {
        obj->m_Prev->m_Next = obj->m_Next;
        obj->m_Next->m_Prev = obj->m_Prev;
    }

    obj->m_Next = obj->m_Prev = NULL;
    obj->m_TimerList = NULL;
    KXSAFE_RELEASE(obj);
    --m_Length;
    return true;
}

bool KxTimerList::insert(KxTimerObject* obj)
{
    if (NULL == obj
        || NULL != obj->m_TimerList)
    {
        return false;
    }

    // 从头部开始搜索
    KxTimerObject* pNode = m_Head;
    const kxTimeVal& timeOut = obj->getTimeVal();
    if (NULL == pNode || !pNode->checkTime(timeOut))
    {
        pushFront(obj);
    }
    else
    {
        while(NULL != pNode)
        {
            // 如果该节点的超时时间大于要插入节点的超时时间
            if (!pNode->checkTime(timeOut))
            {
                obj->m_Next = pNode;
                obj->m_Prev = pNode->m_Prev;
                obj->m_Prev->m_Next = obj;
                pNode->m_Prev = obj;
                obj->m_TimerList = this;
                KXSAFE_RETAIN(obj);
                ++m_Length;
                break;
            }
            // 一直遍历到队尾 
            else if (m_Tail == pNode)
            {
                pushBack(obj);
                break;
            }
            pNode = pNode->m_Next;
        }
    }
    return true;
}

void KxTimerList::update(const kxTimeVal& now)
{
    while(NULL != m_Head)
    {
        if (m_Head->checkTime(now))
        {
            m_Timer = m_Head;
            m_Timer->retain();
            //KXLOGDEBUG("update time tick on %d %d", now.tv_sec, now.tv_usec);
            m_Timer->onTimer(now);
            
            // 如果在onTimer中，对m_Timer执行了stop操作，remove会失败（不stop的话加入其它列表的操作会失败）
            // 而对于AglieList中的Timer，Repeat必定为0
            // 只要是在本超时队列中的Timer，根据Repeat决定重复执行的次数
            if (remove(m_Timer) && m_Timer->getRepeat() != 0)
            {
                if (m_Timer->getRepeat() > 0)
                {
                    m_Timer->setRepeat(m_Timer->getRepeat() - 1);
                }
                m_Timer->start(now);
                pushBack(m_Timer);
            }
            KXSAFE_RELEASE(m_Timer);
        }
        else
        {
            break;
        }
    }
}

KxTimerManager::KxTimerManager()
{
    gettimeofday((struct timeval*)&(m_Now), NULL);
#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
    m_Timestamp = static_cast<int>(time(NULL));
#endif
    m_AglieTimerList = new KxTimerList();
}

KxTimerManager::~KxTimerManager()
{
    for (map<long, KxTimerList*>::iterator iter = m_FixTimerMap.begin();
        iter != m_FixTimerMap.end(); ++iter)
    {
        KXSAFE_RELEASE(iter->second);
    }
    KXSAFE_RELEASE(m_AglieTimerList);
}

void KxTimerManager::updateTimers()
{
	gettimeofday((struct timeval*)&(m_Now), 0);
#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
    m_Timestamp = static_cast<int>(time(NULL));
#endif
    //KXLOGDEBUG("update Timer %d", getMillisecond());

	for (map<long, KxTimerList*>::iterator iter = m_FixTimerMap.begin();
		iter != m_FixTimerMap.end(); )
	{
        iter->second->update(m_Now);
        if (0 == iter->second->length())
		{
            KXSAFE_RELEASE(iter->second);
			m_FixTimerMap.erase(iter++);
		}
		else
		{
			++iter;	
		}
	}

    m_AglieTimerList->update(m_Now);
}

bool KxTimerManager::addTimer(KxTimerObject* obj)
{
    KXASSERT(NULL != obj);

    /*long timeKey = timeToKey(obj->getDelay());
    KXLOGDEBUG("timer key %d", timeKey);
    if (obj->getRepeat() == 0 
        && m_FixTimerMap.find(timeKey) == m_FixTimerMap.end())
    {
        return attachToAglieList(obj);
    }*/

    return attachToFixList(obj);
}

bool KxTimerManager::addTimer(KxTimerObject* obj, float delay, int repeat)
{
    KXASSERT(NULL != obj);

    obj->setDelay(delay);
    obj->setRepeat(repeat);
    return addTimer(obj);
}

bool KxTimerManager::addTimer(KxTimerObject* obj, int delay, int repeat)
{
	KXASSERT(NULL != obj);
	kxTimeVal tv;
	tv.tv_sec = delay;
	tv.tv_usec = 0;
	obj->setDelay(tv);
	obj->setRepeat(repeat);
	return addTimer(obj);
}

bool KxTimerManager::addTimerOnTime(KxTimerObject* obj, long timestamp)
{
    KXASSERT(NULL != obj);
    obj->setRepeat(0);
#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
    kxTimeVal tv;
    tv.tv_sec = timestamp - m_Timestamp;
    tv.tv_usec = 0;
	kxTimeVal timeOut = m_Now + tv;
	obj->startWithTime(timeOut);
#else
    kxTimeVal tv;
    tv.tv_sec = timestamp;
    tv.tv_usec = 0;
    obj->startWithTime(tv);
#endif
    return m_AglieTimerList->insert(obj);
}

bool KxTimerManager::attachToFixList(KxTimerObject* obj)
{
    KXASSERT(NULL != obj);

    long timeKey = timeToKey(obj->getDelay());
    map<long, KxTimerList*>::iterator iter = m_FixTimerMap.find(timeKey);
    KxTimerList* targetList = NULL;
    if (iter == m_FixTimerMap.end())
    {
        targetList = new KxTimerList();
        m_FixTimerMap[timeKey] = targetList;
    }
    else
    {
        targetList = iter->second;
    }

    if (targetList->pushBack(obj))
    {
        obj->start(m_Now);
        //KXLOGDEBUG("attachToFixList time tick on %d %d", obj->getTimeVal().tv_sec, obj->getTimeVal().tv_usec);
        return true;
    }

    return false;
}

//if you just wan't update once, this function will be better
bool KxTimerManager::attachToAglieList(KxTimerObject* obj)
{
    KXASSERT(NULL != obj);
    KXASSERT(obj->getRepeat() == 0);

    kxTimeVal timeOut = m_Now + obj->getDelay();
    obj->startWithTime(timeOut);
    KXLOGDEBUG("attachToAglieList time tick on %d %d", obj->getTimeVal().tv_sec, obj->getTimeVal().tv_usec);
    if (m_AglieTimerList->insert(obj))
    {
        return true;
    }
    
    return false;
}

}
