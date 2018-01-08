#ifndef _CCHECK_MODULE_H__
#define _CCHECK_MODULE_H__

#include "BaseModule.h"

class CCheckModule : public CBaseModule
{
public:
	CCheckModule();
	~CCheckModule();

	// IKxComm对象接收到完整数据后，调用的回调
	virtual void processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target);
	// IKxComm对象发生错误时，调用的回调
	virtual void processError(KxServer::IKxComm *target);
	// IKxComm对象内部发生特殊事件时触发的回调——如TCP非阻塞连接成功
	virtual void processEvent(int eventId, KxServer::IKxComm* target) {}

protected:

	//处理测试登陆
	void ProcessTestLogin(char* buffer, unsigned int len, KxServer::IKxComm *target);

	//处理平台登录
	void ProcessPFLogin(char* buffer, unsigned int len, KxServer::IKxComm *target);

	//处理游客登陆
	void ProcessGuestLoign(char* buffer, unsigned int len, KxServer::IKxComm *target);

	//游客绑定平台登录
	void ProcessBPFGuestLogin(char* buffer, unsigned int len, KxServer::IKxComm *target);

	//处理通用平台登录流程
	void processAnyPFLogin(char* buffer, unsigned int len, KxServer::IKxComm *target);

	//处理游客登陆
	void ProcessGuestAnyPFLogin(char* buffer, unsigned int len, KxServer::IKxComm *target);

	//游客绑定通用平台登录
	void processsBAnyPFGuestLogin(char* buffer, unsigned int len, KxServer::IKxComm *target);

    // 处理聊天服务器登录
    void processsChatLogin(char* buffer, unsigned int len, KxServer::IKxComm *target);

	//处理GM 登陆
	void processGMLogin(char* buffer, unsigned int len, KxServer::IKxComm *target);

	//处理使用已有账号密码登陆
	void processExistUserLogin(char* buffer, unsigned int len, KxServer::IKxComm *target);
};

#endif //_CCHECK_MODULE_H__
