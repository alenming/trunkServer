#ifndef _REBOT_MANAGER_H__
#define _REBOT_MANAGER_H__

#include "KXServer.h"
#include<map>

class CRebotPlayer;

class CRebotManager : public KxServer::KxBaseServer
{
public:
	static CRebotManager* getInstance();

	static void destroy();

	CRebotPlayer * GetRebotPlayer(int nUid);

	virtual bool onServerInit();

	// 服务器主循环回调
	virtual void onServerUpdate();

	virtual void onServerUninit();
protected:
	CRebotManager();
	~CRebotManager();

private:

	static CRebotManager *				m_pInstance;
	std::map<int, CRebotPlayer*>		m_MapRebot;

};


#endif //_REBOT_MANAGER_H__
