#include "HttpCheckModel.h"
#include "Protocol.h"
#include "LoginProtocol.h"
#include "ServerProtocol.h"
#include "KXServer.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "GameDef.h"
#include <string>
#include "json/json.h"
#include "MD5.h"
#include "KxCSComm.h"

using namespace KxServer;
using namespace std;

CUidModelHelper * CUidModelHelper::m_pInstance = NULL;

bool CUidModelHelper::init()
{
	m_pStorage = StorageManager::getInstance()->GetStorage(STORAGE_CHECK);
	if (NULL == m_pStorage)
	{
		return false;
	}

	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));

	if (redisStorer == NULL)
	{
		return false;
	}

	string CountKey = ModelKey::UsrCountKey();

	if (SUCCESS != redisStorer->ExistKey(CountKey))
	{
		if (SUCCESS != redisStorer->SetHashByField(CountKey, 0, DEFAULT_USER_ID))
		{
			return false;
		}
	}

	return true;
}

CUidModelHelper* CUidModelHelper::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CUidModelHelper;
	}

	return m_pInstance;
}

void CUidModelHelper::destroy()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CUidModelHelper::GetUidFromData(const char * openid, int nChannelID, int &uid, int &IsNew)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));

	if (redisStorer == NULL)
	{
		return false;
	}

	string OpenKey = ModelKey::UserOpenKey(openid,nChannelID);
	string CountKey = ModelKey::UsrCountKey();
	uid = 1;

	if (SUCCESS != redisStorer->ExistKey(OpenKey))
	{
		if (SUCCESS != redisStorer->IncreHashByField(CountKey, 0, uid))
		{
			uid = -1;
			KXLOGDEBUG("CUidModelHelper::GetUidFromData,redisStorer->IncreHashByField Failed");
			return false;
		}

		if (SUCCESS != redisStorer->SetHashByField(OpenKey, 0, uid))
		{
			uid = -1;
			KXLOGDEBUG("CUidModelHelper::GetUidFromData,redisStorer->SetHashByField Failed");
			return false;
		}

		IsNew = 1;
	}
	else
	{
		if (SUCCESS != redisStorer->GetHashByField(OpenKey, 0, uid))
		{
			uid = -1;
			KXLOGDEBUG("CUidModelHelper::GetUidFromData,redisStorer->GetHashByField Failed");
			return false;
		}

		IsNew = 0;
	}

	return true;
}

bool CUidModelHelper::getExistUidFromData(const char * openid, int nChannelID, int &uid)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));

	if (redisStorer == NULL)
	{
		return false;
	}

	string OpenKey = ModelKey::UserOpenKey(openid, nChannelID);
	string CountKey = ModelKey::UsrCountKey();

	if (SUCCESS != redisStorer->ExistKey(OpenKey))
	{
		uid = -1;
		KXLOGDEBUG("CUidModelHelper::getExistUidFromData,redisStorer->ExistKey Failed");
		return false;
	}

	if (SUCCESS != redisStorer->GetHashByField(OpenKey, 0, uid))
	{
		uid = -1;
		KXLOGDEBUG("CUidModelHelper::getExistUidFromData,redisStorer->GetHashByField Failed");
		return false;
	}

	return true;
}

bool CUidModelHelper::BandUidToData(const char * openid, int nChannelID, const char * szMobile, int uid)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));

	if (redisStorer == NULL)
	{
		KXLOGDEBUG("CUidModelHelper::BandUidToData,redisStorer == NULL");
		return false;
	}

	string OpenKey = ModelKey::UserOpenKey(openid, nChannelID);
	string MobileKey = ModelKey::UserOpenKey(szMobile,0);

	if (SUCCESS != redisStorer->ExistKey(MobileKey))
	{
		KXLOGDEBUG("CUidModelHelper::BandUidToData,redisStorer->ExistKey");
		return false;
	}

	if (SUCCESS != redisStorer->ExistKey(OpenKey))
	{
		if (SUCCESS != redisStorer->SetHashByField(OpenKey, 0, uid))
		{
			uid = -1;
			KXLOGDEBUG("CUidModelHelper::BandUidToData,redisStorer->SetHashByField");
			return false;
		}

		if (SUCCESS != redisStorer->DelKey(MobileKey))
		{
			KXLOGDEBUG("CUidModelHelper::BandUidToData,redisStorer->DelKey");
			return false;
		}
	}

	return true;
}

bool CUidModelHelper::GetGetstUid(const char * moblie, char *password, int nLen, int &uid, int &IsNew)
{
	IsNew = 0;
	//新游客
	if (GetOldGuestUid(moblie, password, nLen, uid))
	{
		if (uid == -1)
		{
			IsNew = 1;
			return GetNewGuestUid(moblie, password, nLen, uid);
		}

		return true;
	}

	uid = -1;
	return false;
}

bool CUidModelHelper::checkLoginUseFull(int nChannelID, const char *openid, const char *token)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (redisStorer == NULL)
	{
		return false;
	}

	string AnySDKKey = ModelKey::AnySDKCheckKey(nChannelID, openid);
	string nCheckToken;
	if (SUCCESS != redisStorer->GetString(AnySDKKey, nCheckToken))
	{
		KXLOGDEBUG("CUidModelHelper::BandUidToData,redisStorer->GetString");
		return false;
	}

	if (strcmp(token, nCheckToken.c_str()) != 0)
	{
		KXLOGDEBUG("CUidModelHelper::BandUidToData,strcmp(token, nCheckToken.c_str())");
		return false;
	}

	return true;
}

//对对应openid的附加信息进行指定转换
bool CUidModelHelper::exchangecheckExtraData(const char * openid, int nUid)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (redisStorer == NULL)
	{
		return false;
	}

	string strExtraKey = ModelKey::ckExtraDataKey(openid);
	string nExtraData;
	if (SUCCESS != redisStorer->GetString(strExtraKey, nExtraData))
	{
		KXLOGDEBUG("CUidModelHelper::exchangecheckExtraData,redisStorer->GetString");
		return true;
	}

	Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_USER);
	if (NULL == pStorage)
	{
		KXLOGDEBUG("CUidModelHelper::exchangecheckExtraData,StorageManager::getInstance()->GetStorage(STORAGEID_USER)");
		return false;
	}

	CRedisStorer *pUidStorer = static_cast<CRedisStorer*>(pStorage->GetStorer(nUid));
	if (pUidStorer == NULL)
	{
		KXLOGDEBUG("CUidModelHelper::exchangecheckExtraData,pUidStorer == NULL");
		return false;
	}

	strExtraKey = ModelKey::ckExtraDataKey(nUid);
	pUidStorer->SetString(strExtraKey, nExtraData);
	return true;
}

bool CUidModelHelper::getGMLoginUid(const char *admin, char *password, int &uid, int& nPerssion)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	if (redisStorer == NULL)
	{
		return false;
	}

	string GMUserKey = ModelKey::GMUserKey(admin);
	string GMPerKey = ModelKey::GMPerssionKey(admin);
	string CountKey = ModelKey::UsrCountKey();
	if (SUCCESS != redisStorer->GetHashByField(GMPerKey, password, nPerssion))
	{
		//如果没有权限，登陆失败
		uid = -1;
		nPerssion = 0;
		KXLOGDEBUG("CUidModelHelper::getGMLoginUid has no right %s", GMPerKey.c_str());
		return false;
	}

	if (SUCCESS != redisStorer->GetHashByField(GMUserKey, 0, uid))
	{
		if (SUCCESS != redisStorer->IncreHashByField(CountKey, 0, uid))
		{
			uid = -1;
			nPerssion = 0;
			KXLOGDEBUG("CUidModelHelper::getGMLoginUid SUCCESS != redisStorer->IncreHashByField(CountKey, 0, uid) %s", CountKey.c_str());
			return false;
		}
	}

	return true;
}

bool CUidModelHelper::GetNewGuestUid(const char * moblie, char *password, int nLen, int &uid)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));
	CHECK_RETURN(redisStorer != NULL);
	CHECK_RETURN(moblie != NULL);
	string CountKey = ModelKey::UsrCountKey();
	string MobileKey = ModelKey::UserOpenKey(moblie,0);
	uid = 1;

	if (SUCCESS != redisStorer->IncreHashByField(CountKey, 0, uid))
	{
		uid = -1;
		KXLOGDEBUG("SUCCESS != redisStorer->IncreHashByField(CountKey, 0, uid) %s", CountKey.c_str());
		return false;
	}

	if (SUCCESS != redisStorer->SetHashByField(MobileKey, 0, uid))
	{
		uid = -1;
		KXLOGDEBUG("SUCCESS != redisStorer->SetHashByField(MobileKey, 0, uid) Line %s", MobileKey.c_str());
		return false;
	}

	string GuestPs = ModelKey::GuestpsKey(moblie);
	CMD5 Md5;
	Md5.GenerateMD5((unsigned char*)GuestPs.c_str(), strlen(GuestPs.c_str()));
	string Ps = Md5.ToString();

	CHECK_RETURN(Ps.length() <= nLen);
	memcpy(password, Ps.c_str(), Ps.length());
	return true;
}

bool CUidModelHelper::GetOldGuestUid(const char * moblie, char *password, int nLen, int &uid)
{
	CRedisStorer *redisStorer = static_cast<CRedisStorer*>(m_pStorage->GetStorer(1));

	CHECK_RETURN(redisStorer != NULL);
	CHECK_RETURN(moblie != NULL);
	string MobileKey = ModelKey::UserOpenKey(moblie, 0);
	if (SUCCESS != redisStorer->GetHashByField(MobileKey, 0, uid))
	{
		// 没有该游客
		uid = -1;
		KXLOGDEBUG("get MobileKey faile %s", MobileKey.c_str());
		return true;
	}

	string GuestPs = ModelKey::GuestpsKey(moblie);
	CMD5 Md5;
	Md5.GenerateMD5((unsigned char*)GuestPs.c_str(), strlen(GuestPs.c_str()));
	string Ps = Md5.ToString();

	CHECK_RETURN(Ps.length() <= nLen);
	if (0 != strncmp(password, Ps.c_str(), nLen))
	{
		KXLOGERROR("password check faile %s to %s", password, Ps.c_str());
		return false;
	}
	return true;
}


CUidCheckModel::CUidCheckModel(int nClientid)
{
	m_nClientid = nClientid;
	m_pKxComm = NULL;
	m_IsGuest = false;
	m_openid.clear();
	m_nUid = -1;
}

CUidCheckModel::~CUidCheckModel()
{
}

void CUidCheckModel::SetCheckComm(KxServer::IKxComm * pComm)
{
	m_pKxComm = pComm;
}

void CUidCheckModel::SetOpenId(std::string openid)
{
	m_openid = openid;
}

void CUidCheckModel::SetGuestUid(int uid)
{
	m_nUid = uid;
	m_IsGuest = true;
}

void CUidCheckModel::processCallback(int code)
{
	Json::Reader reader;
	Json::Value root;
	bool bUse = true;
	int nIsNew = 0;
	if (reader.parse(m_Buffer, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素  
	{
		int code = root["ret"].asInt();    // 访问节点
		string msg = root["msg"].asString();
		if (code != 0)
		{
			KXLOGDEBUG("Error, Error %s", msg.c_str());
			bUse = false;
		}
	}

	if (m_pKxComm == NULL)
	{
		return;
	}

	if (bUse)
	{
		//if (!CUidModelHelper::getInstance()->GetUidFromData(m_openid.c_str(), m_nUid, nIsNew))
		//{
		//	return;
		//}
	}
	else
	{
		m_nUid = -1;
	}

	int len = sizeof(LoginCheckSC);
	unsigned int buffSize = sizeof(ServerProtocol::SProInitSCC) + len + sizeof(Head) + sizeof(Head);		//+ 唯一ID+RouteKey+RouteValue+权限+返回给用户的数据

	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));
	//设置头部
	Head* head = reinterpret_cast<Head*>(buff);
	head->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubInit);
	head->length = buffSize;

	ServerProtocol::SProInitSCC *pInitSCC = reinterpret_cast<ServerProtocol::SProInitSCC*>(buff + sizeof(Head));

    pInitSCC->nGuestId = m_nClientid;
	pInitSCC->nPermssion = 1;
	pInitSCC->nRouteKey = emRouteLoginKey;
	pInitSCC->nRouteValue = m_nUid;

	Head *pLoginHead = reinterpret_cast<Head *>(buff + sizeof(Head)+sizeof(ServerProtocol::SProInitSCC));

	pLoginHead->id = m_nUid;
	pLoginHead->MakeCommand(CMD_LOGIN, CMD_LOGIN_CHECK_SC);
	pLoginHead->length = sizeof(Head)+sizeof(LoginCheckSC);

	LoginCheckSC *pLoginCheck = reinterpret_cast<LoginCheckSC *>(buff + sizeof(Head)*2+sizeof(ServerProtocol::SProInitSCC));
	pLoginCheck->uid = m_nUid;
	m_pKxComm->sendData(buff, buffSize);
	kxMemMgrRecycle(buff, buffSize);
}

