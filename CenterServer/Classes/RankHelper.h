#ifndef _RANKHELPER_H__
#define _RANKHELPER_H__

#include "KXServer.h"

class CRankHelper : public KxServer::KxTimerObject
{
public:
	static CRankHelper* getInstance();

	static void destroy();

	bool init(KxServer::KxBaseServer *pBaseServer = NULL);

	virtual void onTimer(const KxServer::kxTimeVal& now);

	virtual void setTimer(int sec, int nNum = 0);

private:
	CRankHelper();
	~CRankHelper();

	void SetTowerRankTimer(int nCurTime);
private:
	static CRankHelper *							m_pInstance;
	KxServer::KxBaseServer *						m_pServer;				//服务器对象指针
};

#endif //_RANKHELPER_H__

