#ifndef __REDISSTORER_H__
#define __REDISSTORER_H__

#include <string>
#include <map>
#include <set>
#include <vector>
#include <list>
#include "hiredis.h"

#include "IStorer.h"

#define MAX_RANGESCORE 999999999		//ZRANGE 取反的最大值

enum ResultStatus
{
    DATAEXCEPTION   = -3,              // 处理异常
    RETRYFAILED     = -2,              // 重试失败
    FAILED          = -1,              // 失败返回
    SUCCESS         = 0                // 成功返回
};

struct RankData
{
    int id;
    int source;
};

struct RedisBinaryData
{
    unsigned int length;
	char* data;

	RedisBinaryData()
		:length(0)
		, data(NULL)
	{
	}

    RedisBinaryData(const char *d, int len)
    {
		data = (char*)malloc(len);
		memcpy(data, d, len);
		length = len;
    }

	~RedisBinaryData()
	{
		if (NULL != data)
		{
			free(data);
			length = 0;
		}
	}

	void Write(const char *d, int len)
	{
		if (NULL != data)
		{
			free(data);
			length = 0;
		}
		data = (char*)malloc(len);
		memcpy(data, d, len);
		length = len;
	}
};

class CRedisStorer : public IStorer
{
public:

    CRedisStorer();
    ~CRedisStorer();

public:
    
	// 连接指定redis服务器, 使用阻塞式连接
    int Connect(std::string ip, int port,std::string passward);

	// 按照之前的ip端口重连redis服务器
    int reconnect();

	// 获得redis内容
	redisContext *GetRedisContext();

    //删除KEY
    int DelKey(const std::string &key);

	//批量删除KEY
	int DelKey(std::vector<std::string> &keys);

	//批量获取KEY(以key为前缀)
	int GetKey(const std::string &key, std::vector<std::string> vec);

    //判断Key是否存在
    int ExistKey(const std::string &key);

    //指定Key在某个时间点过期
    int ExpireAt(const std::string &key, unsigned int timestamp);

    //指定Key多长时间之后过期
    int Expire(const std::string &key, int seconds);

    //获取Key过期的剩余时间
    int TTL(const std::string &key, int &seconds);

	//取消key的过期时间
	int Persist(const std::string &key);

	//*********************Set容器操作*********************
    //这里的Int是二进制
    int SetAdd(const std::string &key,const std::string &value);
    	
    int SetAdd(const std::string &key, const char* value, unsigned int len);

	int SetAdd(const std::string &key, int& value);

    int SetDel(const std::string &key, const std::string &value);
    	
    int SetDel(const std::string &key, const char* value, unsigned int len);
	
	int SetDel(const std::string &key, int &value);

    int GetSetAll(const std::string &key, std::set<std::string> &setret);
    
	//Set存储的是二进制指
    int GetSetAll(const std::string &key, std::set<int> &setret);

	//Set存储的是字符串
	int GetSetAllString(const std::string &key, std::set<int> &setret);
    
    //判断字符串value是否在Set中，是返回0
    int IsSet(const std::string &key, std::string value);
    
    //判断二进制数据value是否在Set中，是返回0
    int IsSet(const std::string &key, const char* value, unsigned int len);

    int SetMove(const std::string &src, const std::string &dest, const std::string &value);
    	
   	int SetMove(const std::string &src, const std::string &dest, const char* value, unsigned int len);

    int SetScard();
		
    //*********************字符串操作**********************
    
    //Get String by String Key
    //key: in
    //value: out
    //len: out
    //return 0 Success other faile
    int GetString(const std::string &key, char** value, unsigned int &len);
    	
    int GetString(const std::string &key, std::string &value);
    
    int GetString(const std::string &key, int &value);

    //Set String by String key
    //value: in
    //len: in, the length of value
    //return: 0 success
    int SetString(const std::string &key, const char* value, unsigned int len);
    	
    int SetString(const std::string &key, const std::string &value);

    int SetString(const std::string &key, int value);
    
    //获取String的一小段(可以是数据结构的某个字段)
    int GetStringRange(const std::string &key, int offset, int len, char** value);
    
    //设置String的一小段(可以是数据结构的某个字段)
    //offset String对应字段的偏移  valuelen要设置的数值长度 value 要设置的数值
    //return: 0 success  
    int SetStringRange(const std::string &key, int offset, int valuelen, char* value);

    int IncrString(const std::string &key, int &value);
		
    //*********************ZSet容器操作*********************
    //Add or Update User's Rank By source
    //int key, id, source
    //return: 0 success
	int Zadd(const std::string &key, int id, int source);
    
    //当minsource和maxsource为0时返回Sorted-Set的成员数量
    //否则返回这段分数中的玩家数量
    //return counts
    int ZCount(const std::string &key, int &count, int minsource = 0, int maxsource = 0);
    
    //获取某个成员的排行
    int ZRank(const std::string &key, int id, int &rank);

	//获取某个成员的积分
	int ZScore(const std::string &key, int id, int &Score);
    
    //获取某段排行的玩家以及他们的得分,默认获取top 10
    int ZRange(const std::string &key, std::vector<RankData> &ranks, int start = 0, int stop = 9);

	//删除排名数据
	int ZDel(const std::string &key, int start = 0, int stop = -1);

	//*********************Hash容器操作*********************
    //这里的Int是字符串
    //批量设置一个Hash "hmset %s %s "field1 value1 [field2 value2] .....
    // or "hmset %b %b "field1 field1len value1 value1len [field2 value2] .....
    int SetHash(const std::string &key, const char *format, ...);
        
    int SetHash(const std::string &key, std::map<int, int> &mapset);
    
    //批量设置一个Hash
    int SetHash(const std::string &key, std::map<std::string, std::string> &mapset);

	//批量设置一个Hash
	int SetHash(const std::string &key, std::map<int, std::string> &mapset);
    
	//批量设置hash
	int SetHash(const std::string &key, std::map<std::string, int> &mapset);

    //获取整个Hash
    int GetHash(const std::string &key, std::map<std::string, std::string> &mapret);
    
	//获取整个hash
    int GetHash(const std::string &key, std::map<int, int> &mapret);

	//获取整个hash
	int GetHash(const std::string &key, std::map<std::string, int> &mapret);
    
	//获取整个hash
	int GetHash(const std::string &key, std::map<int, std::string> &mapret);

    //获得所有的hash二进制内容
    int GetHash(const std::string &key, std::map<int, RedisBinaryData> &mapret);

	//获取所有的hash二进制内容
	int GetHash(const std::string &key, std::map<std::string, RedisBinaryData> &mapret);

    //获取某个Field的函数, mapret的key已填充
    int GetHashByField(const std::string &key, std::map<std::string, std::string> &mapret);
    
	//获取某个Field的函数, mapret的key已填充
	int GetHashByField(const std::string &key, std::map<int, int> &mapret);

    //获取某个Field的函数
    int GetHashByField(const std::string &key, const std::string &field, std::string &value);

    //获得某个field函数
    int GetHashByField(const std::string &key, const std::string &field, int &value);

    //获取某个Field的函数 %d %d
    int GetHashByField(const std::string &key, int field, int &value);

    //获得某个Field值
    int GetHashByField(const std::string &key, int field, std::string &value);

	//获得两个filed值
	int GetHashBy2Field(const std::string &key, int field1, int field2, int &value1, int &value2);

    //获得field的二进制值
    int GetHashByField(const std::string &key, std::string field, char *value, int &len);
	
    //获得field的二进制值
    int GetHashByField(const std::string &key, int field, char *value, int &len);

    //设置某个Field的函数
    int SetHashByField(const std::string &key, const std::string &field, const std::string &value);

    //设置某个field的函数
    int SetHashByField(const std::string &key, const std::string &field, int &value);
	
    //获取某个Field的函数 %d %d
    int SetHashByField(const std::string &key, int field, int value);

    //设置某个field的函数
    int SetHashByField(const std::string &key, int field, std::string value);

    //设置某个field的二进制值
    int SetHashByField(const std::string &key, const std::string &field, char *value, int len);

    //设置某个field的二进制值
    int SetHashByField(const std::string &key, int field, char *value, int len);

    //删除某个field
    int DelHashByField(const std::string &key, const std::string &field);

    //删除某个field
    int DelHashByField(const std::string &key, int field);

    //判断Field是否存在
    int HashFieldExist(const std::string &key, const std::string &field);

    //判断Field是否存在
    int HashFieldExist(const std::string &key, int field);

    //自增或自减key filed value
    //value为正数增加
    //value为负数减少
    //value将存放增加或减少后的值
    int IncreHashByField(const std::string &key, const std::string &field, int &value);

    //自增或自减key filed value
    //value为正数增加
    //value为负数减少
    //value将存放增加或减少后的值
    int IncreHashByField(const std::string &key, int field, int &value);

    // 获得一个hash key的域个数
    int HashLen(const std::string &key, long long &value);

    //*********************List容器操作*********************

    // 将一个二进制值插入在表头
    int ListPush(const std::string &key, char *value, int len);
    
    // 插入int列表
    int ListPush(const std::string &key, int value);

	// 插入string到列表
	int ListPush(const std::string &key, std::string &str);

    // block 阻塞操作， pop首个元素， 如果列表中没有元素，阻塞至有元素才会返回
    //int ListBlockPop(const std::string &key, char *value, int len, int seconds);

    // non-block 非阻塞操作， pop首个元素， 移除掉并返回首个值
    int ListPop(const std::string &key, char *value, int &len);

	// 移除并返回首个元素
	int ListPop(const std::string &key, int &value);

    // 移除并返回尾部元素
    int ListRPop(const std::string &key, int &value);

    // 获得所有的元素, 非二进制获取接口
    int ListRange(const std::string &key, std::set<std::string> &str);

    // 获取int列表
    int ListRange(const std::string &key, std::vector<int> &str);

	//获取string列表
	int ListRange(const std::string &key, std::vector<std::string> &str);

	//获得元素到列表中
	int ListRange(const std::string &key, std::list<int> &lst);
    // 获得指定index的二进制元素， 
    // 0代表首个元素，1代表第二个元素，
    // -1代表最后一个元素， -2代表倒数第二个元素
    // 获得指定位置的二进制
    int ListIndex(const std::string &key, int index, char *value, int &len);

    // 获得字符串
    int ListIndex(const std::string &key, int index, std::string &value);

    //count > 0 : 从表头开始向表尾搜索，移除与 value 相等的元素，数量为 count
    //count < 0 : 从表尾开始向表头搜索，移除与 value 相等的元素，数量为 count 的绝对值。
    //count = 0 : 移除表中所有与 value 相等的值。
    int ListRemove(const std::string &key, int count, std::string &value);
    int ListRemove(const std::string &key, int count, int value);

private:

    redisContext*   m_pContext;         //Redis连接上下文对象
    redisReply*     m_pReply;           //Redis命令对象
	
	int             m_Port;
    std::string     m_Ip;
    char            m_Buffer[2048];
	std::string     m_pssward;
};

#endif
