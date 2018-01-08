#ifndef __KX_TIMER_CALLBACK_H__
#define __KX_TIMER_CALLBACK_H__

#include "KxTimerManager.h"

namespace KxServer {

template<class T>
class KxTimerCallback : public KxTimerObject
{
public:
	typedef void(T::*pTimerCallback)();
	KxTimerCallback()
	{
		m_Object = NULL;
		m_Callback = NULL;
	}

	KxTimerCallback(float delay, int repeat = 0)
	{
		KxTimerObject::KxTimerObject(delay, repeat);
		m_Object = NULL;
		m_Callback = NULL;
	}

	virtual ~KxTimerCallback()
	{
		KXSAFE_RELEASE(m_Object);
	}

	void init(float delay, int repeat = 0)
	{
		setDelay(delay);
		setRepeat(repeat);
	}

    void setCallback(T* obj, pTimerCallback callback)
    {
        KXSAFE_RELEASE(m_Object);
        KXSAFE_RETAIN(obj);

        m_Object = obj;
        m_Callback = callback;
    }

	void clean()
	{
		KXSAFE_RELEASE(m_Object);
	}

    virtual void onTimer(const kxTimeVal& now)
    {
        if (NULL != m_Object)
        {
            (m_Object->*m_Callback)();
        }
    }

private:
    T* m_Object;
    pTimerCallback m_Callback;
};

}
#endif