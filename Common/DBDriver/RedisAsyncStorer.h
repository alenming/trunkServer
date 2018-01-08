#ifndef  __REDISASYNCSTORER__
#define  __REDISASYNCSTORER_
#include "hiredis/async.h"
#include "hiredis/hiredis.h"
#include "KxCommInterfaces.h"
#include <string>

using namespace  KxServer;

class CRedisAsyncStorer : public IKxComm
{
public:
	CRedisAsyncStorer();
	~CRedisAsyncStorer();


	// 初始化
	virtual bool init();

	//连接到Redis
	bool Connect(std::string ip, unsigned int port, std::string password);

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

	redisAsyncContext* getRedisAsyncContext();

private:
	redisAsyncContext*  m_Context;			//Redis连接上下文对象
	redisReply*			m_pReply;           //Redis命令返回的对象

	unsigned int		m_Port;				//连接的端口
	std::string			m_Ip;				//连接的IP
	std::string			m_pssward;			//密码
	KXCOMMID			m_sock;				//连接的fd
};



#endif