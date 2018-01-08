#ifndef _CACHESTORER_H_
#define _CACHESTORER_H_
/*
//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include "libmemcached/memcached.h"

/*
Memcached操作常见返回状态   by王永宝 2011-11-23 19:52:06
*MEMCACHED_SUCCESS                  成功
*MEMCACHED_FAILURE                  失败
*MEMCACHED_WRITE_FAILURE            发送失败
*MEMCACHED_READ_FAILURE             读取失败1
*MEMCACHED_UNKNOWN_READ_FAILURE     读取失败2
*MEMCACHED_TIMEOUT                  超时
*MEMCACHED_NOTFOUND                 找不到key
*MEMCACHED_STORED                   已经存储
*MEMCACHED_NOTSTORED                未存储
*MEMCACHED_DATA_EXISTS              数据已经存在
*/
/*
class CCacheStorer
{
public:
    CCacheStorer();
    ~CCacheStorer();
    
    //添加一台缓存服务器
    bool addServer(const std::string addr, int port);
    //清除缓存服务器
    void FreeServer();
    
    //设置超时时间
    bool SetExpiration(time_t time);
    //获取超时时间
    time_t GetExpiration();
    //设置标志
    bool SetFlags(uint32_t flg);
    //获取操作后的状态
    inline memcached_return GetState(){return m_ReturnValue;};
    
    //设置key-value
    bool Set(const char* key, const char* value, size_t len);
    //获取key-value
    bool Get(const char* key, char* value, size_t valuelen, size_t &len);
    //增加key-value
    bool Add(const char* key, const char* value, size_t len);
    //删除key-value
    bool Delete(const char* key);
    //替换key-vaule
    bool Replace(const char* key, const char* value, size_t len);
    //追加数据
    bool Append(const char* key, const char* value, size_t len);
    //自增
    bool Increment(const char* key, unsigned int offset, uint64_t* value);
    //自减
    bool Decrement(const char* key, unsigned int offset, uint64_t* value);

private:
    inline memcached_st * SelectCache();

private:
    std::vector<memcached_st *>     m_MemPtrList;      //MEMCACHE句柄链表
    memcached_server_st*            m_ServerPtr;       //服务器列句柄 
    memcached_st*                   m_MemPtr;          //Memcached对象指针
    memcached_return                m_ReturnValue;     //操作返回值
    time_t                          m_Expiration;      //超时时间
    uint32_t                        m_Flags;           //标识
    int                             m_RetryTimes;      //重试次数
};
*/
#endif //_CACHESTORER_H_
