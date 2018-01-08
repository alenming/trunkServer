/*
* 事件管理器
*
* 1.支持int和string为索引的两种事件容器
* 2.安全，高效地添加，删除事件监听者
* 3.触发指定事件
* 4.使用回调对象池和回调列表池优化了性能
* 5.设置监听者的优先级(暂不实现)
*
* 注意：这里的事件ID和事件名是没有联系的，只是方便操作
* 在事件遍历中，以及嵌套遍历中动态添加，删除都是安全的
*
* 2014-12-11 by 宝爷
*
* 1.改用模板为key，大大简化代码
* 2.优化触发，注册，移除事件的执行效率
* 3.优化了在事件回调中再次触发事件
*
* 2015-5-13 by 宝爷
*/
#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

#include <map>
#include <string>
#include <list>
#include <set>

#include "KxCSComm.h"
#include "CommTools.h"

class EventCallBack;
typedef std::set<EventCallBack*> EventList;

class EventCallBack
{
public:
    EventCallBack(Ref* ref, CallBackV fun, EventList* owner)
        :Target(ref),
        Func(fun),
        Owner(owner)
    {
        SAFE_RETAIN(Target);
    }

    ~EventCallBack()
    {
        SAFE_RELEASE(Target);
    }

    void init(Ref* ref, CallBackV fun, EventList* owner)
    {
        SAFE_RELEASE(Target);
        Target = ref;
        SAFE_RETAIN(Target);
        Func = fun;
        Owner = owner;
    }

    void clear()
    {
        SAFE_RELEASE(Target);
    }

    bool compare(Ref* ref, CallBackV fun)
    {
        return ref == Target && fun == Func;
    }

    inline void execute(void* data = NULL)
    {
        if (NULL != Target && NULL != Func)
        {
            (Target->*Func)(data);
        }
    }

    Ref* Target;        // 监听者对象
    CallBackV Func;		// void Ref::Func(void*)回调
    EventList* Owner;	// 所有者
};

template <typename TEventName>
class CEventManager
{
public:
    CEventManager()
    {
    }

    virtual ~CEventManager()
    {
        clearEventHandles();
    }

    // 安全注册一个事件
    // 如正在遍历中，会先缓存，待遍历完再进行添加
    void addEventHandle(TEventName eventId, Ref* target, CallBackV func)
    {
        EventList* listenerlist = initEventList(eventId, m_Listeners);
        EventCallBack* evObj = newEventCallBack(target, func, listenerlist);
        m_RigestEvents[evObj] = eventId;
        if (m_RaiseingMap[listenerlist] > 0)
        {
            m_AddCached.insert(evObj);
            return;
        }
        else
        {
            listenerlist->insert(evObj);
            EventList* regiestlist = initEventList(target, m_RigestObjects);
            regiestlist->insert(evObj);
        }
    }

    // 安全删除对象所注册的指定事件
    // 如正在遍历中会先缓存，待遍历完再进行删除
    void removeEventHandle(TEventName eventId, Ref* target, CallBackV func)
    {
        typename std::map<TEventName, EventList*>::iterator iter = m_Listeners.find(eventId);
        if (iter == m_Listeners.end()) return;

        EventList* eventList = iter->second;
        EventCallBack* evObj = findEventCallBack(eventList, target, func);
        if (NULL == evObj) return;

        if (m_RaiseingMap[eventList] > 0)
        {
            m_RemoveCached.insert(evObj);
        }
        else
        {
            removeEventCallBack(evObj);
        }
    }

    // 安全删除对象所注册的所有事件
    // 如正在遍历中会先缓存，待遍历完再进行删除
    void removeEventHandle(Ref* target)
    {
        std::map<Ref*, EventList*>::iterator iter = m_RigestObjects.find(target);
        if (iter == m_RigestObjects.end()) return;

        EventList* evList = iter->second;
        for (EventList::iterator eiter = evList->begin();
            eiter != evList->end(); ++eiter)
        {
            removeEventHandle(*eiter);
        }

        m_RigestObjects.erase(iter);
        delete evList;
    }

    // 安全触发事件
    // 可在事件回调中触发事件
    // 可在事件回调中安全地添加和删除事件
    void raiseEvent(TEventName eventId, void* data = NULL)
    {
        typename std::map<TEventName, EventList*>::iterator iter = m_Listeners.find(eventId);
        if (iter == m_Listeners.end()) return;

        EventList* listeners = iter->second;
        ++m_RaiseingMap[listeners];
        for (EventList::iterator liter = listeners->begin();
            liter != listeners->end(); ++liter)
        {
            (*liter)->execute(data);
        }
        --m_RaiseingMap[listeners];

        if (0 == m_RaiseingMap[listeners])
        {
            autoMerger();
        }
    }

    // 清除所有的事件，不可在事件回调中调用
    void clearEventHandles()
    {
        m_RaiseingMap.clear();
        m_AddCached.clear();
        m_RemoveCached.clear();

        deleteAndClearMap(this->m_Listeners);
		deleteAndClearMap(this->m_RigestObjects);
		deleteAndClearList(this->m_CallbackCached);
		deleteAndClearList(this->m_EventListCached);

        for (typename std::map<EventCallBack*, TEventName>::iterator iter = m_RigestEvents.begin();
            iter != m_RigestEvents.end(); ++iter)
        {
            delete iter->first;
        }
        m_RigestEvents.clear();
    }

private:
    template <typename K>
    EventList* initEventList(K key, std::map<K, EventList*>& eventMap)
    {
        if (NULL == eventMap[key])
        {
            return eventMap[key] = newEventList();
        }
        else
        {
            return eventMap[key];
        }
    }

    EventCallBack* findEventCallBack(EventList* eventList, Ref* target, CallBackV func)
    {
        for (EventList::iterator iter = eventList->begin();
            iter != eventList->end(); ++iter)
        {
            if ((*iter)->compare(target, func))
            {
                return *iter;
            }
        }

        return NULL;
    }

    void removeEventHandle(EventCallBack* eventCallBack)
    {
        typename std::map<TEventName, EventList*>::iterator iter = m_Listeners.find(m_RigestEvents[eventCallBack]);
        if (iter == m_Listeners.end()) return;

        EventList* eventList = iter->second;
        if (m_RaiseingMap[eventList] > 0)
        {
            m_RemoveCached.insert(eventCallBack);
        }
        else
        {
            eventList->erase(eventCallBack);
            m_RigestEvents.erase(eventCallBack);
            freeEventCallBack(eventCallBack);
            if (eventList->size() == 0)
            {
                m_Listeners.erase(iter);
                freeEventList(eventList);
            }
        }
    }

    void removeEventCallBack(EventCallBack* eventCallBack)
    {
        EventList* evList = eventCallBack->Owner;
        evList->erase(eventCallBack);
        if (evList->size() == 0)
        {
            m_Listeners.erase(m_RigestEvents[eventCallBack]);
            freeEventList(evList);
        }

        m_RigestObjects[eventCallBack->Target]->erase(eventCallBack);
        m_RigestEvents.erase(eventCallBack);
        freeEventCallBack(eventCallBack);
    }

    void autoMerger()
    {
        for (EventList::iterator iter = m_AddCached.begin();
            iter != m_AddCached.end(); ++iter)
        {
            EventCallBack* evObj = *iter;
            evObj->Owner->insert(evObj);
            EventList* regiestlist = initEventList(evObj->Target, m_RigestObjects);
            regiestlist->insert(evObj);
        }

        for (EventList::iterator iter = m_RemoveCached.begin();
            iter != m_RemoveCached.end(); ++iter)
        {
            removeEventCallBack(*iter);
        }
    }

    EventCallBack* newEventCallBack(Ref* ref, CallBackV fun, EventList* owner)
    {
        if (m_CallbackCached.size() == 0)
        {
            return new EventCallBack(ref, fun, owner);
        }
        else
        {
            EventCallBack* eventCallBack = *m_CallbackCached.begin();
            m_CallbackCached.erase(m_CallbackCached.begin());
            eventCallBack->init(ref, fun, owner);
            return eventCallBack;
        }
    }

    void freeEventCallBack(EventCallBack* eventCallBack)
    {
        eventCallBack->clear();
        m_CallbackCached.push_back(eventCallBack);
    }

    EventList* newEventList()
    {
        if (m_EventListCached.size() == 0)
        {
            return new EventList();
        }
        else
        {
            EventList* eventList = *m_EventListCached.begin();
            m_EventListCached.erase(m_EventListCached.begin());
            return eventList;
        }
    }

    void freeEventList(EventList* eventList)
    {
        m_EventListCached.push_back(eventList);
    }

private:
    std::map<EventList*, int> m_RaiseingMap;
    std::map<TEventName, EventList*> m_Listeners;
    std::map<EventCallBack*, TEventName> m_RigestEvents;
    std::map<Ref*, EventList*> m_RigestObjects;
    std::list<EventCallBack*> m_CallbackCached;
    std::list<EventList*> m_EventListCached;
    EventList m_AddCached;
    EventList m_RemoveCached;
};

#endif
