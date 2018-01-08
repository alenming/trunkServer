#include "RedisAsyncStorer.h"

void connectCallback(const redisAsyncContext *c, int status) {
	if (status != REDIS_OK) {
		printf("Error: %s\n", c->errstr);
		return;
	}
	printf("Connected...\n");
}

void getCallback(redisAsyncContext *c, void *r, void *privdata) 
{
	time_t t = time(NULL);
	tm *tt = localtime(&t);
	printf("结束时间%d:%d", tt->tm_min, tt->tm_sec);
}

CRedisAsyncStorer::CRedisAsyncStorer()
{
	m_Context = NULL;
	m_pReply  = NULL;
	m_pssward = "";
	m_sock	  = KXINVALID_COMMID;
}

CRedisAsyncStorer::~CRedisAsyncStorer()
{
}

// 初始化
bool CRedisAsyncStorer::init()
{
	return true;
}

//连接到Redis服务器
bool CRedisAsyncStorer::Connect(std::string ip, unsigned int port, std::string password)
{
	if ((ip.empty()) && (port > 65535))//检验参数合法性
	{
		return false;
	}
	else
	{
		if (NULL != m_Context)//之前是否有连接
		{
			redisAsyncDisconnect(m_Context);//有则先释放
		}

		m_Context = redisAsyncConnect(ip.c_str(), port);//异步连接，此时建立了socket并且连接到了服务器
		if (m_Context->err)//出错则返回
		{
			return false;
		}
		else
		{
			m_sock = m_Context->c.fd; //设置fd
			m_PollType = KXPOLLTYPE_IN | KXPOLLTYPE_OUT;			//设置关心的事件类型,转换到KXPOLL_TYPE，用于添加进poller
		}


		
		redisAsyncSetConnectCallback(m_Context, connectCallback);

		
		for (int i = 0; i < 1000000; i++)
		{
			int status = redisAsyncCommand(m_Context, NULL, NULL, "zadd za %d 686", i, i);
			if (REDIS_OK != status)
			{
				return false;
			}
		}

		redisAsyncCommand(m_Context, getCallback, NULL, "set 10000 188");

		if ("" != password)
		{
			int status;
			status = redisAsyncCommand(m_Context, NULL, NULL, "auth %s", password.c_str());
			if (REDIS_OK != status)
			{
				return false;
			}
			return true;
		}


		return true;
	}
}

// 发送数据
int CRedisAsyncStorer::sendData(const char* buffer, unsigned int len)
{
	return 1;
}

// 接收数据
int CRedisAsyncStorer::recvData(char* buffer, unsigned int len)
{
	return 1;
}

// 关闭该通讯对象
void CRedisAsyncStorer::close()
{

}

// 获取通讯ID（通常是socket fd）
KXCOMMID CRedisAsyncStorer::getCommId()
{
	return m_sock;
}

// 接收到数据时触发的回调，由IKxCommPoller调用
// -1 表示错误
// 0 表示正确
// 大于0 表示数据可能未接收完
int CRedisAsyncStorer::onRecv()
{
	redisAsyncHandleRead(m_Context);
	return 1;
}

// 数据可被发送时触发的回调，由IKxCommPoller调用
// -1 表示错误
// 0 表示正确
// 大于0 表示数据可能未发送完
int CRedisAsyncStorer::onSend()
{
	redisAsyncHandleWrite(m_Context);
	return 1;
}

redisAsyncContext* CRedisAsyncStorer::getRedisAsyncContext()
{
	return m_Context;
}


