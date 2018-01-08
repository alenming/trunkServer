#ifndef __REDISSTORER_H__
#define __REDISSTORER_H__

#include <string>
#include <map>
#include <set>
#include <vector>
#include "hiredis.h"

#include "IStorer.h"

enum NEWSTORERESULT
{
	SR_EXCEPTION = -2,
	SR_FAIL		 = -1,
	SR_SUCCESS	 = 0,
};

class CNewRedisStorer : public IStorer
{
public:
    CNewRedisStorer();
    ~CNewRedisStorer();

public:
	// 连接指定redis服务器, 使用阻塞式连接
	virtual int connect(const char* ip, int port, const char* psw);
	// 按照之前的ip端口重连redis服务器
    virtual int reconnect();
	// AUTH psw, 输入密码
	virtual int password(const char* psw);
	// 获得redis内容
    virtual redisContext* getRedisContext();

    // 开启append方式获取数据，开启之后执行的redis指令会被缓存在本地
    virtual int beginAppend();
    // append方式必须调用endAppend来结束append模式并获取数据
    virtual int endAppend(std::vector<redisReply*>& results);
    // 在非append方式下获取数据
    virtual redisReply* getReply();

    //*********************Key操作*********************
    // DEL key, 删除KEY
    int delKey(const char* key);
    // EXSIT key, 判断Key是否存在
    int existKey(const char* key);
    // EXPIREAT key timestamp, 指定Key在某个时间戳(time())过期
    int expireAt(const char* key, int timestamp);
    // EXPIRE key seconds, 指定Key多长时间之后过期
    int expire(const char* key, int seconds);
    // TTL key, 获取Key过期的剩余时间，reply返回integer
    int ttl(const char* key);
	// PERSIST key, 取消key的过期时间，reply返回integer
    int persist(const char* key);

    //*********************字符串操作**********************
	// GET key, reply返回字符串
    int getString(const char* key);
	// SET key value, reply返回ok
    int setString(const char* key, const char* value, unsigned int len);
    int setString(const char* key, const char* value);
    int setString(const char* key, int value);
	// SETRANGE key offset value valuelen
	int setStringRange(const char* key, int offset, char* value, int valuelen);
	// GETRANGE key offset offset+len
	int getStringRange(const char* key, int offset, int len);
	// INCRBY key value 
    int incrString(const char* key, int value = 1);

	//*********************Hash容器操作*********************
    // 批量设置字段，HMSET key field value [field value ...]，reply返回字符串
    int setHash(const char* key, std::map<int, int> &mapset);
    int setHash(const char* key, std::map<std::string, std::string> &mapset);
	int setHash(const char* key, std::map<int, std::string> &mapset);
    int setHash(const char* key, std::map<std::string, int> &mapset);
    // 批量获取字段，HMGET key field [field ...]
    // reply返回REDIS_REPLY_ARRAY，按field1 value1 filed2 value2格式排列
    int getHash(const char* key, std::map<int, int> &mapget);
    int getHash(const char* key, std::map<std::string, std::string> &mapget);
    int getHash(const char* key, std::map<int, std::string> &mapget);
    int getHash(const char* key, std::map<std::string, int> &mapget);
    // 获取所有字段，HGETALL key
    // reply返回REDIS_REPLY_ARRAY，按field1 value1 filed2 value2格式排列
    int getHash(const char* key);
    // 获取指定字段，HGET key field
    int getHashByField(const char* key, const char* field);
    int getHashByField(const char* key, int field);
    // 设置指定字段，HSET key filed value
    int setHashByField(const char* key, int field, int value);
    int setHashByField(const char* key, int field, char* value);
    int setHashByField(const char* key, int field, char* value, int len);
    int setHashByField(const char* key, const char* field, int value);
    int setHashByField(const char* key, const char* field, const char* value);
    int setHashByField(const char* key, const char* field, char* value, int len);
    // 删除指定字段，HDEL key filed
    int delHashByField(const char* key, const char* field);
    int delHashByField(const char* key, int field);
    // HEXISTS key field, 判断是否有field
    int hashFieldExist(const char* key, const char* field);
    int hashFieldExist(const char* key, int field);
    // HINCRBY key field value 递增对应hash field
    int increHashByField(const char* key, const char* field, int value);
    int increHashByField(const char* key, int field, int value);
    // HLEN key, 返回hash元素个数
    int hashLen(const char* key);

    //*********************Set容器操作*********************
	// SADD key value, 添加一个元素
    int setAdd(const char* key, const char* value);
    int setAdd(const char* key, const char* value, unsigned int len);
	// SREM key value, 移除对应元素
    int removeSetMember(const char* key, const char* value);
    int removeSetMember(const char* key, const char* value, unsigned int len);
	// SMEMBERS key, 获得所有元素
    int getSetMembers(const char* key);
	// SISMEMBER key value, 查看是否有value
    int isSetMember(const char* key, const char* value);
    int isSetMember(const char* key, const char* value, unsigned int len);
	// SMOVE srckey destkey value, 将value从srckey中移到destkey中
    int setMove(const char* srckey, const char* destkey, const char* value);
	int setMove(const char* srckey, const char* destkey, const char* value, unsigned int len);

    //*********************ZSet容器操作*********************
	// ZADD key score member, 往key中添加一个记录
    int zadd(const char* key, int member, int score);
	// ZCOUNT key, 如果minscore且maxscore为0, 返回zset成员个数, 否则返回范围内的成员个数
	int zcount(const char* key, int minscore = 0, int maxscore = 0);
	// ZRANK key member, 根据score排名, 从小到大排序, score最小排名0
	int zrank(const char* key, int member);
	// ZREVRANK key member, 根据score排名, 从大到小排序, score最大排名0
	int zrevrank(const char* key, int member);
	// ZSCORE key member, 返回member的score值
	int zscore(const char* key, int member);
	// ZRANGE key start stop, 根据score从小到大排名, 取得排名区间内成员
    int zrange(const char* key, int start, int stop);
	// ZREVRANGE key start stop, 根据score从大到小排名, 取得排名区间内成员 -1代表最后一个
	int zrevrange(const char* key, int start, int stop);
	// ZREMRANGEBYRANK key start stop, 通过排名移除成员, -1代码最后一个
    int zremovebyrank(const char* key, int start = 0, int stop = -1);
	// ZREMRANGEBYSCORE key minscore maxscore, 移除score区间内的成员
	int zremrangebyscore(const char* key, int minscore, int maxscore);

    //*********************List容器操作*********************
    // LPUSH key value, 将一个二进制值插入在表头
    int listPush(const char* key, char *value, int len);
    // LPUSH key value, 插入int元素
    int listPush(const char* key, int value);
	// LPOP key, non-block 非阻塞操作，pop首个元素，reply返回第一个元素的值
    int listPop(const char* key);
    // LRANGE key start stop, 返回区间内的成员
    int listRange(const char* key, int start = 0, int stop = -1);
    // LINDEX key index, 获得对应成员, 0代表首个元素，1代表第二个元素，-1代表最后一个元素， -2代表倒数第二个元素
    int listIndex(const char* key, int index);
	// count > 0 : 从表头开始向表尾搜索，移除与 value 相等的元素，数量为 count
	// count < 0 : 从表尾开始向表头搜索，移除与 value 相等的元素，数量为 count 的绝对值。
	// count = 0 : 移除表中所有与 value 相等的值。
    int listRemove(const char* key, int count, const char* value);
    int listRemove(const char* key, int count, int value);

private:
    // 自动根据操作模式执行指令
    int executeCommand(const char *format, ...);

private:

	bool					m_bIsAppendMode;	//是否是append模式
    redisContext*			m_pContext;         //Redis连接上下文对象
    redisReply*				m_pReply;           //Redis命令对象
	int						m_Port;				//端口
	int						m_AppendCount;		//append命令个数
    std::string				m_Ip;				//ip
	std::string				m_Password;			//密码
};

#include "KXServer.h"

class CRedisAsyncStorer : public CNewRedisStorer, KxServer::IKxComm
{
public:
    CRedisAsyncStorer();
    ~CRedisAsyncStorer();

public:
    // 连接指定redis服务器, 使用阻塞式连接
    virtual int connect(const char* ip, int port, const char* passward);
    // 按照之前的ip端口重连redis服务器
    virtual int reconnect();
    // 获得redis内容
    virtual redisContext* getRedisContext();
    
    //*********************IKxComm函数重载*********************
    // 发送数据
    virtual int sendData(const char* buffer, unsigned int len);
    // 接收数据
    virtual int recvData(char* buffer, unsigned int len);
    // 关闭该通讯对象
    virtual void close();
    // 获取socket fd）
    virtual KXCOMMID getCommId();
    // 接收到数据时触发的回调，由IKxCommPoller调用0
    virtual int onRecv();
    // 数据可被发送时触发的回调，由IKxCommPoller调用
    virtual int onSend();

private:
    // 禁用以下方法
    // 开启append方式获取数据，开启之后执行的redis指令会被缓存在本地
    virtual int beginAppend();
    // append方式必须调用endAppend来结束append模式并获取数据
    virtual int endAppend(std::vector<redisReply*>& results);
    // 在非append方式下获取数据
    virtual redisReply* getReply();
};

#endif
