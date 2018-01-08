#include "NewRedisStorer.h"
#include "KxLog.h"

CNewRedisStorer::CNewRedisStorer()
: m_bIsAppendMode(false)
, m_pContext(NULL)
, m_pReply(NULL)
, m_Port(0)
, m_AppendCount(0)
{

}

CNewRedisStorer::~CNewRedisStorer()
{

}

int CNewRedisStorer::connect(const char* ip, int port, const char* psw)
{
	m_Ip = ip;
	m_Port = port;
	m_Password = psw;

	m_pContext = redisConnect(ip, port);
	if (m_pContext == NULL)
	{
		KXLOGERROR("Connection error: can't allocate redis context\n");
		return SR_EXCEPTION;
	}

	if (m_pContext->err)
	{
		KXLOGERROR("Connection error: %s", m_pContext->errstr);
		redisFree(m_pContext);
		return SR_EXCEPTION;
	}

	if (NULL != psw)
	{
		return password(psw);
	}

	return SR_SUCCESS;
}

int CNewRedisStorer::reconnect()
{
	if (m_pContext != NULL)
	{
		if (m_pContext->err == 0)
		{
			return SR_SUCCESS;
		}
		redisFree(m_pContext);
		m_pContext = NULL;
	}

	m_pContext = redisConnect(m_Ip.c_str(), m_Port);
	if (m_pContext == NULL)
	{
		KXLOGERROR("Connection error: can't allocate redis context\n");
		return SR_EXCEPTION;
	}

	if (m_pContext->err)
	{
		KXLOGERROR("Connection error: %s", m_pContext->errstr);
		redisFree(m_pContext);
		return SR_EXCEPTION;
	}

	if (!m_Password.empty())
	{
		return password(m_Password.c_str());
	}

	return SR_SUCCESS;
}

int CNewRedisStorer::password(const char* psw)
{
	return executeCommand("auth %s", psw);
}

redisContext* CNewRedisStorer::getRedisContext()
{
	if (m_pContext == NULL || m_pContext->err)
	{
		if (SR_SUCCESS != reconnect())
		{
			return NULL;
		}
	}
	return m_pContext;
}

int CNewRedisStorer::beginAppend()
{
	if (m_bIsAppendMode)
	{
		return SR_SUCCESS;
	}

	m_bIsAppendMode = true;
	return SR_SUCCESS;
}

int CNewRedisStorer::endAppend(std::vector<redisReply*>& results)
{
	if (!m_bIsAppendMode)
	{
		KXLOGERROR("End append not in append mode!");
		return SR_FAIL;
	}

	redisContext* pContext = getRedisContext();
	if (NULL == pContext)
	{
		return SR_FAIL;
	}
	// 插入多少条, 则获得多少条
	for (int i = 0; i < m_AppendCount; ++i)
	{
		redisReply *pReply = NULL;
		if (REDIS_OK != redisGetReply(m_pContext, (void**)&pReply))
		{
			KXLOGERROR("Execute get append reply %s error!");
			continue;
		}
		results.push_back(pReply);
	}
	m_AppendCount = 0;
	m_bIsAppendMode = false;
	return SR_SUCCESS;
}

redisReply* CNewRedisStorer::getReply()
{
	return m_pReply;
}

int CNewRedisStorer::executeCommand(const char *format, ...)
{
	redisContext* pContext = getRedisContext();
	if (NULL == pContext)
	{
		return SR_FAIL;
	}

	if (m_bIsAppendMode)
	{
		va_list ap;
		va_start(ap, format);
		if (REDIS_OK != redisvAppendCommand(pContext, format, ap))
		{
			va_end(ap);
			KXLOGERROR("Execute append commond %s error!", format);
			return SR_FAIL;
		}
		va_end(ap);
		m_AppendCount++;
	}
	else
	{
		va_list ap;
		va_start(ap, format);
		m_pReply = (redisReply*)redisCommand(m_pContext, format, ap);
		va_end(ap);

		if (m_pReply->type == REDIS_REPLY_ERROR)
		{
			freeReplyObject(m_pReply);
			return SR_FAIL;
		}
	}
	// 注意, 外部要getReply并使用freeReplyObject是否对象
	return SR_SUCCESS;
}

int CNewRedisStorer::delKey(const char* key)
{
	return executeCommand("del %s", key);
}

int CNewRedisStorer::existKey(const char* key)
{
	return executeCommand("exsit %s", key);
}

int CNewRedisStorer::expireAt(const char* key, int timestamp)
{
	return executeCommand("expireat %s %d", key, timestamp);
}

int CNewRedisStorer::expire(const char* key, int seconds)
{
	return executeCommand("expire %s %d", key, seconds);
}

int CNewRedisStorer::ttl(const char* key)
{
	return executeCommand("ttl %s", key);
}

int CNewRedisStorer::persist(const char* key)
{
	return executeCommand("persist %s", key);
}

int CNewRedisStorer::getString(const char* key)
{
	return executeCommand("get %s", key);
}

int CNewRedisStorer::setString(const char* key, const char* value, unsigned int len)
{
	return executeCommand("set %s %b", key, value, len);
}

int CNewRedisStorer::setString(const char* key, const char* value)
{
	return executeCommand("set %s %s", key, value);
}

int CNewRedisStorer::setString(const char* key, int value)
{
	return executeCommand("set %s %d", key, value);
}

int CNewRedisStorer::setStringRange(const char* key, int offset, char* value, int valuelen)
{
	return executeCommand("getrange %s %d %b", key, offset, value, valuelen);
}

int CNewRedisStorer::getStringRange(const char* key, int offset, int len)
{
	return executeCommand("getrange %s %d %d", key, offset, offset + len);
}

int CNewRedisStorer::incrString(const char* key, int value)
{
	return executeCommand("incrby %s %d", key, value);
}

int CNewRedisStorer::setHash(const char* key, std::map<int, int> &mapset)
{
	std::string commond = "hset %s ";
	commond += key;
	for (std::map<int, int>::iterator iter = mapset.begin();
		iter != mapset.end(); ++iter)
	{
		char field[32] = {};
		char value[32] = {};
		snprintf(field, sizeof(field), " %d", iter->first);
		snprintf(value, sizeof(value), " %d", iter->second);
		commond += field;
		commond += value;
	}
	return executeCommand(commond.c_str());
}

int CNewRedisStorer::setHash(const char* key, std::map<std::string, std::string> &mapset)
{
	std::string commond = "hset %s ";
	commond += key;
	for (std::map<std::string, std::string>::iterator iter = mapset.begin();
		iter != mapset.end(); ++iter)
	{
		commond += " ";
		commond += iter->first;
		commond += " ";
		commond += iter->second;
	}
	return executeCommand(commond.c_str());
}

int CNewRedisStorer::setHash(const char* key, std::map<int, std::string> &mapset)
{
	std::string commond = "hset %s ";
	commond += key;
	for (std::map<int, std::string>::iterator iter = mapset.begin();
		iter != mapset.end(); ++iter)
	{
		char field[32] = {};
		snprintf(field, sizeof(field), " %d", iter->first);
		commond += field;
		commond += " ";
		commond += iter->second;
	}
	return executeCommand(commond.c_str());
}

int CNewRedisStorer::setHash(const char* key, std::map<std::string, int> &mapset)
{
	std::string commond = "hset %s ";
	commond += key;
	for (std::map<std::string, int>::iterator iter = mapset.begin();
		iter != mapset.end(); ++iter)
	{
		commond += " ";
		commond += iter->first;
		char value[32] = {};
		snprintf(value, sizeof(value), " %d", iter->second);
		commond += value;
	}
	return executeCommand(commond.c_str());
}

int CNewRedisStorer::getHash(const char* key, std::map<int, int> &mapget)
{
	std::string commond = "hset %s ";
	commond += key;
	for (std::map<int, int>::iterator iter = mapget.begin();
		iter != mapget.end(); ++iter)
	{
		char field[32] = {};
		snprintf(field, sizeof(field), " %d", iter->first);
		commond += field;
	}
	return executeCommand(commond.c_str());
}

int CNewRedisStorer::getHash(const char* key, std::map<std::string, std::string> &mapget)
{
	std::string commond = "hset %s ";
	commond += key;
	for (std::map<std::string, std::string>::iterator iter = mapget.begin();
		iter != mapget.end(); ++iter)
	{
		commond += " ";
		commond += iter->first;
	}
	return executeCommand(commond.c_str());
}

int CNewRedisStorer::getHash(const char* key, std::map<int, std::string> &mapget)
{
	std::string commond = "hset %s ";
	commond += key;
	for (std::map<int, std::string>::iterator iter = mapget.begin();
		iter != mapget.end(); ++iter)
	{
		char field[32] = {};
		snprintf(field, sizeof(field), " %d", iter->first);
		commond += field;
	}
	return executeCommand(commond.c_str());
}

int CNewRedisStorer::getHash(const char* key, std::map<std::string, int> &mapget)
{
	std::string commond = "hset %s ";
	commond += key;
	for (std::map<std::string, int>::iterator iter = mapget.begin();
		iter != mapget.end(); ++iter)
	{
		commond += " ";
		commond += iter->first;
	}
	return executeCommand(commond.c_str());
}

int CNewRedisStorer::getHash(const char* key)
{
	return executeCommand("hget %s", key);
}

int CNewRedisStorer::getHashByField(const char* key, const char* field)
{
	return executeCommand("hget %s %s", key, field);
}

int CNewRedisStorer::getHashByField(const char* key, int field)
{
	return executeCommand("hget %s %d", key, field);
}

int CNewRedisStorer::setHashByField(const char* key, int field, int value)
{
	return executeCommand("hset %s %d %d", key, field, value);
}

int CNewRedisStorer::setHashByField(const char* key, int field, char* value)
{
	return executeCommand("hset %s %d %s", key, field, value);
}

int CNewRedisStorer::setHashByField(const char* key, int field, char* value, int len)
{
	return executeCommand("hset %s %d %b", key, field, value, len);
}

int CNewRedisStorer::setHashByField(const char* key, const char* field, int value)
{
	return executeCommand("hset %s %s %d", key, field, value);
}

int CNewRedisStorer::setHashByField(const char* key, const char* field, const char* value)
{
	return executeCommand("hset %s %s %s", key, field, value);
}

int CNewRedisStorer::setHashByField(const char* key, const char* field, char* value, int len)
{
	return executeCommand("hset %s %s %b", key, field, value, len);
}

int CNewRedisStorer::delHashByField(const char* key, const char* field)
{
	return executeCommand("hdel %s %s", key, field);
}

int CNewRedisStorer::delHashByField(const char* key, int field)
{
	return executeCommand("hdel %s %d", key, field);
}

int CNewRedisStorer::hashFieldExist(const char* key, const char* field)
{
	return executeCommand("hexists %s %s", key, field);
}

int CNewRedisStorer::hashFieldExist(const char* key, int field)
{
	return executeCommand("hexists %s %d", key, field);
}

int CNewRedisStorer::increHashByField(const char* key, const char* field, int value)
{
	return executeCommand("hincrby %s %s %d", key, field, value);
}

int CNewRedisStorer::increHashByField(const char* key, int field, int value)
{
	return executeCommand("hincrby %s %d %d", key, field, value);
}

int CNewRedisStorer::hashLen(const char* key)
{
	return executeCommand("hlen %s", key);
}

int CNewRedisStorer::setAdd(const char* key, const char* value)
{
	return executeCommand("sadd %s %s", key, value);
}

int CNewRedisStorer::setAdd(const char* key, const char* value, unsigned int len)
{
	return executeCommand("sadd %s %d", key, value, len);
}

int CNewRedisStorer::removeSetMember(const char* key, const char* value)
{
	return executeCommand("srem %s %s", key, value);
}

int CNewRedisStorer::removeSetMember(const char* key, const char* value, unsigned int len)
{
	return executeCommand("srem %s %d", key, value);
}

int CNewRedisStorer::getSetMembers(const char* key)
{
	return executeCommand("smembers %s", key);
}

int CNewRedisStorer::isSetMember(const char* key, const char* value)
{
	return executeCommand("sismember %s %s", key, value);
}

int CNewRedisStorer::isSetMember(const char* key, const char* value, unsigned int len)
{
	return executeCommand("sismember %s %d", key, value, len);
}

int CNewRedisStorer::setMove(const char* srckey, const char* destkey, const char* value)
{
	return executeCommand("smove %s %s %s", srckey, destkey, value);
}

int CNewRedisStorer::setMove(const char* srckey, const char* destkey, const char* value, unsigned int len)
{
	return executeCommand("smove %s %s %b", srckey, destkey, value, len);
}

int CNewRedisStorer::zadd(const char* key, int member, int score)
{
	return executeCommand("zadd %s %d %d", key, score, member);
}

int CNewRedisStorer::zcount(const char* key, int minscore, int maxscore)
{
	if (minscore == 0 && maxscore == 0)
	{
		return executeCommand("zcount %s -inf +inf", key);
	}
	return executeCommand("zcount %s %d %d", key, minscore, maxscore);
}

int CNewRedisStorer::zrank(const char* key, int member)
{
	return executeCommand("zrank %s %d", key, member);
}

int CNewRedisStorer::zrevrank(const char* key, int member)
{
	return executeCommand("zrevrank %s %d", key, member);
}

int CNewRedisStorer::zscore(const char* key, int member)
{
	return executeCommand("zscore %s %d", key, member);
}

int CNewRedisStorer::zrange(const char* key, int start, int stop)
{
	return executeCommand("zrange %s %d %d withscores", key, start, stop);
}

int CNewRedisStorer::zrevrange(const char* key, int start, int stop)
{
	return executeCommand("zrevrange %s %d %d withscores", key, start, stop);
}

int CNewRedisStorer::zremovebyrank(const char* key, int start, int stop)
{
	return executeCommand("zremrangebyrank %s %d %d", key, start, stop);
}

int CNewRedisStorer::zremrangebyscore(const char* key, int minscore, int maxscore)
{
	return executeCommand("zremrangebyscore %s %d %d", key, minscore, maxscore);
}

int CNewRedisStorer::listPush(const char* key, char *value, int len)
{
	return executeCommand("lpush %s %b", key, value, len);
}

int CNewRedisStorer::listPush(const char* key, int value)
{
	return executeCommand("lpush %s %d", key, value);
}

int CNewRedisStorer::listPop(const char* key)
{
	return executeCommand("lpop %s", key);
}

int CNewRedisStorer::listRange(const char* key, int start, int stop)
{
	return executeCommand("lrange %s %d %d", key, start, stop);
}

int CNewRedisStorer::listIndex(const char* key, int index)
{
	return executeCommand("lindex %s %d", key, index);
}

int CNewRedisStorer::listRemove(const char* key, int count, const char* value)
{
	return executeCommand("lrem %s %d %s", key, count, value);
}

int CNewRedisStorer::listRemove(const char* key, int count, int value)
{
	return executeCommand("lrem %s %d %d", key, count, value);
}
