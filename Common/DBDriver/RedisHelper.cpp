#include "RedisHelper.h"

CRedisHelper::CRedisHelper()
{
}

CRedisHelper::~CRedisHelper()
{
}

bool CRedisHelper::replyArray(redisReply* m_pReply, std::set<std::string> &setret)
{
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			setret.insert(std::string(m_pReply->element[j]->str));
		}
		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}

bool CRedisHelper::replyArray(redisReply* m_pReply, std::set<int> &setret)
{
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			setret.insert(static_cast<int>(*(m_pReply->element[j]->str)));
		}

		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}

bool CRedisHelper::replyArray(redisReply* m_pReply, std::vector<int> &str)
{
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			str.push_back(atoi(m_pReply->element[j]->str));
		}

		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}

int CRedisHelper::replyHashArray(redisReply* m_pReply, std::map<std::string, std::string> &mapret)
{
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		//成对出现的id和分数，这里不为0的话，下面的代码会崩溃
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			std::string mykey = std::string(m_pReply->element[j]->str);
			mapret[mykey] = std::string(m_pReply->element[++j]->str);
		}

		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false; 
}

int CRedisHelper::replyHashArray(redisReply* m_pReply, std::map<int, int> &mapret)
{

	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		//成对出现的id和分数，这里不为0的话，下面的代码会崩溃
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			int mykey = atoi(m_pReply->element[j]->str);
			int myval = atoi(m_pReply->element[++j]->str);
			mapret[mykey] = myval;
		}

		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}

int CRedisHelper::replyHashArray(redisReply* m_pReply, std::map<std::string, int> &mapret)
{
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		//成对出现的id和分数，这里不为0的话，下面的代码会崩溃
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			std::string mykey = m_pReply->element[j]->str;
			int myval = atoi(m_pReply->element[++j]->str);
			mapret[mykey] = myval;
		}
		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}


int CRedisHelper::replyHashArray(redisReply* m_pReply, std::map<int, std::string> &mapret)
{
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		//成对出现的id和分数，这里不为0的话，下面的代码会崩溃
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			int mykey = atoi(m_pReply->element[j]->str);
			mapret[mykey] = m_pReply->element[++j]->str;;
		}
		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}


int CRedisHelper::replyHashArray(redisReply* m_pReply, std::map<int, struct RedisBinaryData> &mapret)
{
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		//成对出现的id和分数，这里不为0的话，下面的代码会崩溃
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			int mykey = atoi(m_pReply->element[j]->str);

			int dataIdx = ++j;
			RedisBinaryData sbinary;
			mapret[mykey] = sbinary;
			mapret[mykey].Write(m_pReply->element[dataIdx]->str, m_pReply->element[dataIdx]->len);
		}
		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}

int CRedisHelper::replyHashArray(redisReply* m_pReply, std::map<std::string, struct RedisBinaryData> &mapret)
{
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		//成对出现的id和分数，这里不为0的话，下面的代码会崩溃
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			std::string mykey = m_pReply->element[j]->str;
			int dataIdx = ++j;
			RedisBinaryData sbinary;
			mapret[mykey] = sbinary;
			mapret[mykey].Write(m_pReply->element[dataIdx]->str, m_pReply->element[dataIdx]->len);
		}
		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}

int CRedisHelper::replyString(redisReply* m_pReply, char** value, unsigned int &len)
{
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		//没有数据或缓冲区不足
		if (m_pReply->len <= 0 || m_pReply->len > static_cast<int>(len))
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		//拷贝数据到输出缓冲区
		memcpy(*value, m_pReply->str, m_pReply->len);
		len = m_pReply->len;

		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}

int CRedisHelper::replyString(redisReply* m_pReply, char** value, int &len)
{
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		//没有数据或缓冲区不足
		if (m_pReply->len <= 0 || m_pReply->len > len)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		//拷贝数据到输出缓冲区
		memcpy(*value, m_pReply->str, m_pReply->len);
		len = m_pReply->len;

		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}


int CRedisHelper::replyString(redisReply* m_pReply, std::string &value)
{
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		//拷贝字符串
		value = std::string(m_pReply->str);
		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}

int CRedisHelper::replyString(redisReply* m_pReply, int &value)
{
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		value = atoi(m_pReply->str);
		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}

bool CRedisHelper::replyInteger(redisReply* m_pReply, int &value)
{
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		value = static_cast<int>(m_pReply->integer);
		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}

bool CRedisHelper::replyInteger(redisReply* m_pReply, long long &value)
{
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		value = static_cast<int>(m_pReply->integer);
		//处理成功
		freeReplyObject(m_pReply);
		return true;
	}
	return false;
}