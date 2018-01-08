#ifndef _CREBOT_PLAYER_H__
#define _CREBOT_PLAYER_H__

#include "KxServer.h"
#include <vector>

struct SOpData;

class CRebotPlayer
{
public:
	CRebotPlayer();
	~CRebotPlayer();

	bool init(int nUid);							//初始化

	bool SetKxComm(KxServer::IKxComm* pKxComm);		//设置通讯端口

	bool SetLoginState(bool bState);				//设置登陆状态

	bool PlayerOperator();							//玩家操作

	void encryptBuffer(int mainCmd, int subCmd, char* buff, int len);
public:
	//处理接收数据
	bool ProcessRecvData(int mainCmd, int nSubCmd, char *pszContext, int nLen);

	//处理登陆数据接收
	bool ProcessLoginData(int nSubCmd, char *pszContext, int nLen);

	//处理PVP战斗匹配时的战斗ID
	bool ProcessPvpMatchData(int nSubCmd, char *pszContext, int nLen);

	//处理接收PVP战斗结算
	bool processPvpResultData(int nSubCmd);


protected:
	void PlayerCheckLogin();							//玩家验证

	void PlayerLogin();									//玩家登陆

	void PlayerSendRequest(SOpData *pData);				//玩家发送请求

private:

	int						m_nUid;					//用户UID
	KxServer::IKxComm*		m_pKxComm;				//数据通讯端口
	bool					m_bLoginOK;				//是否登录完成
	bool					m_bSendLogin;			//是否发送登陆消息
	bool					m_bFight;				//是否在战斗
	int						m_bBattleID;			//战斗ID
	bool					m_bSendMatching;		//是否在匹配中
	std::vector<SOpData>	m_VectOpData;			//玩家的操作列表

};


#endif //_CREBOT_PLAYER_H__
