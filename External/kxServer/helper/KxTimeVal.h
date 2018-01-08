/* 
*   TimeVal结构体
*   处理时间计算
*   
*   2015-04-24 Refactor By 宝爷
*/
#ifndef __KXTIMEVAL_H__
#define __KXTIMEVAL_H__

#define MILLION 1000000

#include "KxCore.h"

namespace KxServer {

// 时间结构体
class kxTimeVal
{
public:
    kxTimeVal()
    {
        tv_sec = 0;
        tv_usec = 0;
    }
    
    kxTimeVal(long sec, long usec)
    {
        tv_sec = sec;
        tv_usec = usec;
    }
    
    kxTimeVal(float t)
    {
        tv_sec = (long)t;
        tv_usec = (long)((t - tv_sec) * MILLION);
    }
    
    inline void add(const kxTimeVal& v)
    {
        tv_sec += v.tv_sec;
        tv_usec += v.tv_usec;
    }
    
    inline void sub(const kxTimeVal& v)
    {
        tv_sec -= v.tv_sec;
        tv_usec -= v.tv_usec;
    }
    
    // 以秒为单位设置时间
    inline void setFromfloat(float t)
    {
        tv_sec = (long)t;
        tv_usec = (long)((t - tv_sec) * MILLION);
    }
    
    // 以秒为单位返回当前设置的时间
    inline float getfloat() const
    {
        return tv_sec + 1.0f * tv_usec / MILLION;
    }
    
    // 设置为当前系统时间
    inline kxTimeVal& now()
    {
        gettimeofday((struct timeval *)this,  (struct timezone*)0);
        return *this;
    }
    
    inline const kxTimeVal operator+(const kxTimeVal& v) const
    {
        kxTimeVal ret = *this;
        ret.add(v);
        return ret;
    }
    
    inline kxTimeVal& operator+=(const kxTimeVal& v)
    {
        add(v);
        return *this;
    }
    
    inline kxTimeVal& operator+=(float v)
    {
        long t = (long)v;
        tv_sec += t;
        tv_usec += (long)((v - t) * MILLION);
        return *this;
    }
    
    inline const kxTimeVal operator-(const kxTimeVal& v) const
    {
        kxTimeVal ret = *this;
        ret.sub(v);
        return ret;
    }
    
    inline kxTimeVal& operator-=(const kxTimeVal& v)
    {
        sub(v);
        return *this;
    }
    
    inline kxTimeVal& operator-=(float v)
    {
        long t = (long)v;
        tv_sec -= t;
        tv_usec -= (long)((v - t) * MILLION);
        return *this;
    }
    
    inline bool operator==(const kxTimeVal& v) const
    {
        return tv_sec == v.tv_sec && tv_usec == v.tv_usec;
    }
    
    inline bool operator!=(const kxTimeVal& v) const
    {
        return tv_sec != v.tv_sec || tv_usec != v.tv_usec;
    }
    
    inline bool operator>(const kxTimeVal& v) const
    {
        if(tv_sec > v.tv_sec)
        {
            return true;
        }
        else if(tv_sec == v.tv_sec)
        {
            return tv_usec > v.tv_usec;
        }
        else
        {
            return false;
        }
    }
    
    inline bool operator<(const kxTimeVal& v) const
    {
        if(tv_sec < v.tv_sec)
        {
            return true;
        }
        else if(tv_sec == v.tv_sec)
        {
            return tv_usec < v.tv_usec;
        }
        else
        {
            return false;
        }
    }

    long tv_sec;		// 秒
    long tv_usec;		// 毫秒(百万分之一秒)
};

}

#endif