#ifndef __RedisHelper_
#define __RedisHelper_

#include "RedisStorer.h"

class CRedisHelper
{
public:
	CRedisHelper();
	~CRedisHelper();

	//CRedisHelper从HiRedis返回的结果中将内容提取出来放入指定容器，并释放之前的资源

	//返回的数据是数组
	static bool replyArray(redisReply* m_pReply, std::set<std::string> &setret);
	static bool replyArray(redisReply* m_pReply, std::set<int> &setret);
	static bool replyArray(redisReply* m_pReply, std::vector<int> &str);
	static int replyHashArray(redisReply* m_pReply, std::map<std::string, std::string> &mapret);
	static int replyHashArray(redisReply* m_pReply, std::map<int, int> &mapret);
	static int replyHashArray(redisReply* m_pReply, std::map<std::string, int> &mapret);
	static int replyHashArray(redisReply* m_pReply, std::map<int, std::string> &mapret);
	static int replyHashArray(redisReply* m_pReply, std::map<int, struct RedisBinaryData> &mapret);
	static int replyHashArray(redisReply* m_pReply, std::map<std::string, struct RedisBinaryData> &mapret);
	

	//返回的数据是字符串
	static int replyString(redisReply* m_pReply, char** value, unsigned int &len);
	static int replyString(redisReply* m_pReply, char** value, int &len);
	static int replyString(redisReply* m_pReply, std::string &value);
	static int replyString(redisReply* m_pReply, int &value);

	//返回的数据是实数
	static bool replyInteger(redisReply* m_pReply, int &value);
	static bool replyInteger(redisReply* m_pReply, long long &value);
	
private:

};


#endif