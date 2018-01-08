#ifndef _HTTP_CHECK_MODEL_H__
#define _HTTP_CHECK_MODEL_H__

#include "HttpHelper.h"
#include <string>
#include "KXServer.h"
#include "StorageManager.h"

class CUidModelHelper
{
public:
	bool	init();
	static	CUidModelHelper* getInstance();
	static	void destroy();

	bool	GetUidFromData(const char * openid,int nChannelID,int &uid,int &IsNew);

	bool	getExistUidFromData(const char * openid, int nChannelID, int &uid);

	bool	BandUidToData(const char * openid,int nChannelID,const char * szMobile,int uid);

	bool	GetGetstUid(const char * moblie, char *password, int nLen, int &uid, int &IsNew);

	bool	checkLoginUseFull(int nChannelID,const char *openid, const char *token);
	//对对应openid的附加信息进行指定转换
	bool	exchangecheckExtraData(const char * openid, int nUid);

	bool	getGMLoginUid(const char *admin, char *password,int &uid,int& nPerssion);

protected:
	bool	GetNewGuestUid(const char * moblie, char *password, int nLen, int &uid);

	bool	GetOldGuestUid(const char * moblie, char *password, int nLen, int &uid);

private:
	static CUidModelHelper *			m_pInstance;
	Storage *							m_pStorage;
};

class CUidCheckModel : public CHttpObject
{
public:
	CUidCheckModel(int nClientid);
	~CUidCheckModel();

	void SetCheckComm(KxServer::IKxComm * pComm);

	void SetOpenId(std::string openid);

	void SetGuestUid(int uid);

	virtual void processCallback(int code);

private:

	int									m_nClientid;									//用户动态ID
	int									m_nUid;											//用户UID
	bool								m_IsGuest;										//用户是否为游客
	KxServer::IKxComm *					m_pKxComm;										//用户连接
	std::string							m_openid;
};


#endif //_HTTP_CHECK_MODEL_H__
