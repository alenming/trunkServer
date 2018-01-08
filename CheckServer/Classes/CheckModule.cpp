#include "CheckModule.h"
#include "Protocol.h"
#include "LoginProtocol.h"
#include "ServerProtocol.h"
#include "HttpCheckModel.h"
#include "GameDef.h"
#include "MD5.h"
#include "GMProtocol.h"
//#include "SDKUrlLoader.h"

using namespace KxServer;

CCheckModule::CCheckModule()
{
}


CCheckModule::~CCheckModule()
{
}

// IKxComm对象接收到完整数据后，调用的回调
void CCheckModule::processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target)
{
	//发给指定的前端
    Head* head = reinterpret_cast<Head*>(buffer);
    int nMainCmd = head->MainCommand();
    int nSubCmd = head->SubCommand();
	unsigned int nClientID = head->id;

    //KXLOGDEBUG("CheckServer maincmd %d, subcmd %d, len %d, nClientID %d", nMainCmd, nSubCmd, len, nClientID);
    switch (nMainCmd)
	{
    case CMD_LOGIN:
		{
			switch (nSubCmd)
			{
				case CMD_LOGIN_CHECK_TEST_CS:
				{
					ProcessTestLogin(buffer, len, target);
				}
				break;
				case CMD_LOGIN_CHECK_PF_CS:
				{
					//ProcessPFLogin(buffer, len, target);
					processAnyPFLogin(buffer, len, target);
				}
				break;
				case CMD_LOGIN_CHECK_GUEST_CS:
				{
					ProcessGuestLoign(buffer, len, target);
				}
				break;
				case CMD_LOGIN_CHECK_BINDGUEST_CS:
				{
					//ProcessBPFGuestLogin(buffer, len, target);
					processsBAnyPFGuestLogin(buffer, len, target);
				}
				break;
				case CMD_LOGIN_EXISTUSER_CS:
				{
					processExistUserLogin(buffer, len, target);
				}
				break;
                case CMD_LOGIN_CHECK_CHAT_CS:
				{
					processsChatLogin(buffer, len, target);
				}
				break;
			default:
				break;
			}
		}
		break;
	case CMD_GM:
		{
			switch (nSubCmd)
			{
				case CMD_GM_LOGIN_CS:
				{
					processGMLogin(buffer, len, target);
				}
				break;
			}
		}
		break;
	default:
		break;
	}
}

// IKxComm对象发生错误时，调用的回调
void CCheckModule::processError(KxServer::IKxComm *target)
{
    KXLOGDEBUG("CCheckModule::processError");
}

//处理测试登陆
void CCheckModule::ProcessTestLogin(char* buffer, unsigned int len, KxServer::IKxComm *target)
{
	if (buffer == NULL || len != sizeof(Head)+sizeof(LoginCheckTestCS))
	{
		return;
	}

	Head* head = reinterpret_cast<Head*>(buffer);
	unsigned int *pUid = (unsigned int *)(buffer + sizeof(Head));
	unsigned int nClientID = head->id;
	// + 唯一ID+RouteKey+RouteValue+权限+返回给用户的数据
	unsigned int buffSize = sizeof(ServerProtocol::SProInitSCC) + sizeof(Head)* 2 + sizeof(LoginCheckSC);
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));
	//设置头部
	Head* pHead = reinterpret_cast<Head*>(buff);
	pHead->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubInit);
	pHead->length = buffSize;

	ServerProtocol::SProInitSCC *pInitSCC = reinterpret_cast<ServerProtocol::SProInitSCC*>(buff + sizeof(Head));
	pInitSCC->nGuestId = nClientID;
	pInitSCC->nPermssion = 1;
	pInitSCC->nRouteKey = emRouteLoginKey;
	pInitSCC->nRouteValue = *pUid;

	Head *pClientHead = reinterpret_cast<Head*>(buff + sizeof(Head) + sizeof(ServerProtocol::SProInitSCC));
	pClientHead->id = (*pUid);
	pClientHead->MakeCommand(CMD_LOGIN, CMD_LOGIN_CHECK_SC);
	pClientHead->length = sizeof(Head)+sizeof(LoginCheckSC);

	LoginCheckSC *pCheckSC = reinterpret_cast<LoginCheckSC*>(buff + sizeof(Head)*2 + sizeof(ServerProtocol::SProInitSCC));
	pCheckSC->uid = (*pUid);
	pCheckSC->nIsNew = 0;
	pCheckSC->nIsGuest = 0;

	target->sendData(buff, buffSize);
	kxMemMgrRecycle(buff, buffSize);
}

//处理平台登录
void CCheckModule::ProcessPFLogin(char* buffer, unsigned int len, KxServer::IKxComm *target)
{
	//if (buffer== NULL || len != sizeof(Head)+sizeof(LoginCheckPlatformCS))
	//{
	//	return;
	//}

	//Head* head = reinterpret_cast<Head*>(buffer);
	//unsigned int nClientID = head->id;
	//LoginCheckPlatformCS * pLoginCheckCS = (LoginCheckPlatformCS*)(buffer + sizeof(Head));

	//CUidCheckModel *pHttpObject = new CUidCheckModel(nClientID);
	//pHttpObject->SetCheckComm(target);
	//pHttpObject->SetOpenId(pLoginCheckCS->openid);


	//time_t CurTime;
	//time(&CurTime);
	//char szTime[28];
	//char szSign[128];
	//int nLen = 0;
	//snprintf(szTime, sizeof(szTime), "%d", CurTime);
	//CMD5 iMD5;

	//if (pLoginCheckCS->nChannelID == NORMAL_LOGIN_TYPE)
	//{
	//	string szLoginUrl = CSDKUrlLoader::getInstance()->GetUrlData(URL_LOGIN_TYPE);
	//	pHttpObject->setUrl(szLoginUrl);

	//	pHttpObject->setGetParam("appid", "1105417303");

	//	nLen = snprintf(szSign, sizeof(szSign), "P9LieBJxMy9kwgqX");
	//	snprintf(szSign + nLen, sizeof(szSign)-nLen, "%d", CurTime);
	//	iMD5.GenerateMD5((unsigned char*)szSign, strlen(szSign));
	//	pHttpObject->setGetParam("sig", iMD5.ToString());

	//}
	//else
	//{
	//	string szLoginUrl = CSDKUrlLoader::getInstance()->GetUrlData(URL_WXLOGIN_TYPE);
	//	pHttpObject->setUrl(szLoginUrl);

	//	pHttpObject->setGetParam("appid", "wx759c332a8b1cf39e");
	//	nLen = snprintf(szSign, sizeof(szSign), "b47e6042939e8ce935d4fd28fea6aa5f");
	//	snprintf(szSign + nLen, sizeof(szSign)-nLen, "%d", CurTime);
	//	iMD5.GenerateMD5((unsigned char*)szSign, strlen(szSign));
	//	pHttpObject->setGetParam("sig", iMD5.ToString());
	//}

	//pHttpObject->setGetParam("openid", pLoginCheckCS->openid);
	//pHttpObject->setGetParam("openkey", pLoginCheckCS->openkey);
	//pHttpObject->setGetParam("timestamp", szTime);
	//CHttpHelper::getInstance()->addHttpRequest(pHttpObject);
}

//处理游客登陆
void CCheckModule::ProcessGuestLoign(char* buffer, unsigned int len, KxServer::IKxComm *target)
{
	if (buffer == NULL || len != sizeof(Head)+sizeof(LoginCheckGuestCS))
	{
		return;
	}

	Head* head = reinterpret_cast<Head*>(buffer);
	unsigned int nClientID = head->id;
	LoginCheckGuestCS *pGuestCS = (LoginCheckGuestCS *)(buffer + sizeof(Head));
	char password[33] = {0};
	int nUid = -1;
	int nIsNew = 0;
	memcpy(password, pGuestCS->password, sizeof(pGuestCS->password));
	CUidModelHelper::getInstance()->GetGetstUid(pGuestCS->szMobile, password, sizeof(password), nUid, nIsNew);

	unsigned int buffSize = sizeof(ServerProtocol::SProInitSCC) + sizeof(Head)* 2 + sizeof(LoginCheckSC);		//+ 唯一ID+RouteKey+RouteValue+权限+返回给用户的数据
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

	//设置头部
	Head* pHead = reinterpret_cast<Head*>(buff);
	pHead->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubInit);
	pHead->length = buffSize;

	ServerProtocol::SProInitSCC *pInitSCC = reinterpret_cast<ServerProtocol::SProInitSCC*>(buff + sizeof(Head));
    pInitSCC->nGuestId = nClientID;
	pInitSCC->nPermssion = 1;
	pInitSCC->nRouteKey = emRouteLoginKey;
	pInitSCC->nRouteValue = nUid;

	Head *pClientHead = (Head *)(buff + sizeof(Head)+sizeof(ServerProtocol::SProInitSCC));
	pClientHead->id = nUid;
	pClientHead->MakeCommand(CMD_LOGIN, CMD_LOGIN_CHECK_SC);
	pClientHead->length = sizeof(Head)+sizeof(LoginCheckSC);

	LoginCheckSC *pNewGuestSC = (LoginCheckSC *)(buff + sizeof(Head)* 2 + sizeof(ServerProtocol::SProInitSCC));
	pNewGuestSC->uid = nUid;

	pNewGuestSC->nIsNew = nIsNew;
	pNewGuestSC->nIsGuest = 1;
	target->sendData(buff, buffSize);
	kxMemMgrRecycle(buff, buffSize);
}

//处理通用平台登录流程
void CCheckModule::processAnyPFLogin(char* buffer, unsigned int len, KxServer::IKxComm *target)
{
	if (buffer == NULL || len != sizeof(Head)+sizeof(LoginCheckPlatformCS))
	{
		return;
	}

	Head* head = reinterpret_cast<Head*>(buffer);
	unsigned int nClientID = head->id;
	LoginCheckPlatformCS * pLoginCheckCS = (LoginCheckPlatformCS*)(buffer + sizeof(Head));
	int nUid = 0;
	int nIsNew = 0;
	if (!CUidModelHelper::getInstance()->checkLoginUseFull(pLoginCheckCS->nChannelID, pLoginCheckCS->openid, pLoginCheckCS->openkey)
        || !CUidModelHelper::getInstance()->GetUidFromData(pLoginCheckCS->openid, pLoginCheckCS->nChannelID, nUid, nIsNew))
	{
		nUid = -1;
	}

	if (nUid != -1)
	{
		CUidModelHelper::getInstance()->exchangecheckExtraData(pLoginCheckCS->openid, nUid);
	}
	
    // 唯一ID+RouteKey+RouteValue+权限+返回给用户的数据
	unsigned int buffSize = sizeof(ServerProtocol::SProInitSCC) + sizeof(Head)* 2 + sizeof(LoginCheckSC);
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

	// 设置头部
	Head* pHead = reinterpret_cast<Head*>(buff);
	pHead->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubInit);
	pHead->length = buffSize;

	ServerProtocol::SProInitSCC *pInitSCC = reinterpret_cast<ServerProtocol::SProInitSCC*>(buff + sizeof(Head));
	pInitSCC->nGuestId = nClientID;
	pInitSCC->nRouteKey = emRouteLoginKey;
	pInitSCC->nPermssion = 1;
	pInitSCC->nRouteValue = nUid;
	if (nUid == -1)
	{
		pInitSCC->nPermssion = 0;
	}
	
	Head *pClientHead = (Head *)(buff + sizeof(Head)+sizeof(ServerProtocol::SProInitSCC));
	pClientHead->id = nUid;
	pClientHead->MakeCommand(CMD_LOGIN, CMD_LOGIN_CHECK_SC);
	pClientHead->length = sizeof(Head)+sizeof(LoginCheckSC);

	LoginCheckSC *pNewGuestSC = (LoginCheckSC *)(buff + sizeof(Head)* 2 + sizeof(ServerProtocol::SProInitSCC));
	pNewGuestSC->uid = nUid;
	pNewGuestSC->nIsNew = nIsNew;
	pNewGuestSC->nIsGuest = 0;

	target->sendData(buff, buffSize);
	kxMemMgrRecycle(buff, buffSize);
}

//游客绑定通用平台登录
void CCheckModule::processsBAnyPFGuestLogin(char* buffer, unsigned int len, KxServer::IKxComm *target)
{
	if (buffer == NULL || len != sizeof(Head)+sizeof(LoginCheckBindGuestCS))
	{
		return;
	}

	Head* head = reinterpret_cast<Head*>(buffer);
	unsigned int nClientID = head->id;
	LoginCheckBindGuestCS *pGuestCS = (LoginCheckBindGuestCS *)(buffer + sizeof(Head));
	char password[33] = { 0 };
	int nUid = pGuestCS->uid;
	int nIsNew = 0;
	memcpy(password, pGuestCS->password, sizeof(pGuestCS->password));
	//校验账号/密码
	if (!CUidModelHelper::getInstance()->GetGetstUid(pGuestCS->szMobile,password, sizeof(password), nUid, nIsNew))
	{
		nUid = -1;
	}

	//校验openid+token的合法性
	if (!CUidModelHelper::getInstance()->checkLoginUseFull(pGuestCS->nChannelID, pGuestCS->openid, pGuestCS->openkey)
        || !CUidModelHelper::getInstance()->BandUidToData(pGuestCS->openid, pGuestCS->nChannelID, pGuestCS->szMobile, nUid))
	{
		nUid = -1;
	}

	unsigned int buffSize = sizeof(ServerProtocol::SProInitSCC) + sizeof(Head)* 2 + sizeof(LoginCheckSC);		//+ 唯一ID+RouteKey+RouteValue+权限+返回给用户的数据
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

	//设置头部
	Head* pHead = reinterpret_cast<Head*>(buff);
	pHead->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubInit);
	pHead->length = buffSize;

	ServerProtocol::SProInitSCC *pInitSCC = reinterpret_cast<ServerProtocol::SProInitSCC*>(buff + sizeof(Head));
	pInitSCC->nGuestId = nClientID;
	pInitSCC->nPermssion = 1;
	pInitSCC->nRouteKey = emRouteLoginKey;
	pInitSCC->nRouteValue = nUid;
	if (nUid == -1)
	{
		pInitSCC->nPermssion = 0;
	}

	Head *pClientHead = (Head *)(buff + sizeof(Head)+sizeof(ServerProtocol::SProInitSCC));
	pClientHead->id = nUid;
	pClientHead->MakeCommand(CMD_LOGIN, CMD_LOGIN_CHECK_SC);
	pClientHead->length = sizeof(Head)+sizeof(LoginCheckSC);

	LoginCheckSC *pNewGuestSC = (LoginCheckSC *)(buff + sizeof(Head)* 2 + sizeof(ServerProtocol::SProInitSCC));
	pNewGuestSC->uid = nUid;
	pNewGuestSC->uid = nIsNew;
	pNewGuestSC->nIsGuest = 1;

	target->sendData(buff, buffSize);
	kxMemMgrRecycle(buff, buffSize);
}

void CCheckModule::processsChatLogin(char* buffer, unsigned int len, KxServer::IKxComm *target)
{
    if (buffer == NULL || len != sizeof(Head)+sizeof(LoginCheckChatCS))
    {
        return;
    }

    Head* head = reinterpret_cast<Head*>(buffer);
    unsigned int *pUid = (unsigned int *)(buffer + sizeof(Head));
    unsigned int nClientID = head->id;
    // + 唯一ID+RouteKey+RouteValue+权限+返回给用户的数据
    unsigned int buffSize = sizeof(ServerProtocol::SProInitSCC) + sizeof(Head)* 2 + sizeof(int);
    char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));
    //设置头部
    Head* pHead = reinterpret_cast<Head*>(buff);
    pHead->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubInit);
    pHead->length = buffSize;

    ServerProtocol::SProInitSCC *pInitSCC = reinterpret_cast<ServerProtocol::SProInitSCC*>(buff + sizeof(Head));
    pInitSCC->nGuestId = nClientID;
    pInitSCC->nPermssion = 1;
    pInitSCC->nRouteKey = emRouteChatKey;
    pInitSCC->nRouteValue = *pUid;

    Head *pClientHead = reinterpret_cast<Head*>(buff + sizeof(Head)+sizeof(ServerProtocol::SProInitSCC));
    pClientHead->id = (*pUid);
    pClientHead->MakeCommand(CMD_LOGIN, CMD_LOGIN_CHECK_CHAT_SC);
    pClientHead->length = sizeof(Head)+sizeof(int);

    int *pCheckUid = reinterpret_cast<int*>(buff + sizeof(Head)* 2 + sizeof(ServerProtocol::SProInitSCC));
    *pCheckUid = (*pUid);

    target->sendData(buff, buffSize);
    kxMemMgrRecycle(buff, buffSize);
}

//处理GM 登陆
void CCheckModule::processGMLogin(char* buffer, unsigned int len, KxServer::IKxComm *target)
{
	if (buffer == NULL || len != sizeof(Head)+sizeof(SGMLoginCS))
	{
		return;
	}

	Head* head = reinterpret_cast<Head*>(buffer);
	SGMLoginCS *pGMClientCS = (SGMLoginCS *)(buffer + sizeof(Head));
	unsigned int nClientID = head->id;

	int nUid = 0;
	int nPerssion = 0;
	//校验openid+token的合法性
	CUidModelHelper::getInstance()->getGMLoginUid(pGMClientCS->szAdimin, pGMClientCS->szPassword, nUid, nPerssion);
	
	unsigned int buffSize = sizeof(ServerProtocol::SProInitSCC) + sizeof(Head)* 2 + sizeof(LoginCheckSC);		//+ 唯一ID+RouteKey+RouteValue+权限+返回给用户的数据
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

	//设置头部
	Head* pHead = reinterpret_cast<Head*>(buff);
	pHead->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubInit);
	pHead->length = buffSize;

	ServerProtocol::SProInitSCC *pInitSCC = reinterpret_cast<ServerProtocol::SProInitSCC*>(buff + sizeof(Head));
	pInitSCC->nGuestId = nClientID;
	pInitSCC->nPermssion = nPerssion;
	pInitSCC->nRouteKey = emRouteLoginKey;
	pInitSCC->nRouteValue = nUid;
	if (nUid == -1)
	{
		pInitSCC->nPermssion = 0;
	}

	Head *pClientHead = (Head *)(buff + sizeof(Head)+sizeof(ServerProtocol::SProInitSCC));
	pClientHead->id = nUid;
	pClientHead->MakeCommand(CMD_GM,CMD_GM_LOGIN_SC);
	pClientHead->length = sizeof(Head)+sizeof(SGMLoginSC);

	SGMLoginSC *pNewGuestSC = (SGMLoginSC *)(buff + sizeof(Head)* 2 + sizeof(ServerProtocol::SProInitSCC));
	pNewGuestSC->nUid = nUid;
	target->sendData(buff, buffSize);
	kxMemMgrRecycle(buff, buffSize);
}

//处理使用已有账号密码登陆
void CCheckModule::processExistUserLogin(char* buffer, unsigned int len, KxServer::IKxComm *target)
{
	if (buffer == NULL || len != sizeof(Head)+sizeof(LoginCheckPlatformCS))
	{
		return;
	}

	Head* head = reinterpret_cast<Head*>(buffer);
	unsigned int nClientID = head->id;
	LoginCheckPlatformCS * pLoginCheckCS = (LoginCheckPlatformCS*)(buffer + sizeof(Head));
	int nUid = 0;
	int nIsNew = 0;
	if (!CUidModelHelper::getInstance()->checkLoginUseFull(pLoginCheckCS->nChannelID, pLoginCheckCS->openid, pLoginCheckCS->openkey)
		|| !CUidModelHelper::getInstance()->getExistUidFromData(pLoginCheckCS->openid, pLoginCheckCS->nChannelID, nUid))
	{
		nUid = -1;
	}

	// 唯一ID+RouteKey+RouteValue+权限+返回给用户的数据
	unsigned int buffSize = sizeof(ServerProtocol::SProInitSCC) + sizeof(Head)* 2 + sizeof(LoginCheckSC);
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

	// 设置头部
	Head* pHead = reinterpret_cast<Head*>(buff);
	pHead->MakeCommand(ServerProtocol::ServerMain, ServerProtocol::ServerSubInit);
	pHead->length = buffSize;

	ServerProtocol::SProInitSCC *pInitSCC = reinterpret_cast<ServerProtocol::SProInitSCC*>(buff + sizeof(Head));
	pInitSCC->nGuestId = nClientID;
	pInitSCC->nPermssion = 1;
	pInitSCC->nRouteKey = emRouteLoginKey;
	pInitSCC->nRouteValue = nUid;
	if (nUid == -1)
	{
		pInitSCC->nPermssion = 0;
	}

	Head *pClientHead = (Head *)(buff + sizeof(Head)+sizeof(ServerProtocol::SProInitSCC));
	pClientHead->id = nUid;
	pClientHead->MakeCommand(CMD_LOGIN, CMD_LOGIN_CHECK_SC);
	pClientHead->length = sizeof(Head)+sizeof(LoginCheckSC);

	LoginCheckSC *pNewGuestSC = (LoginCheckSC *)(buff + sizeof(Head)* 2 + sizeof(ServerProtocol::SProInitSCC));
	pNewGuestSC->uid = nUid;
	pNewGuestSC->nIsNew = nIsNew;
	pNewGuestSC->nIsGuest = 0;

	target->sendData(buff, buffSize);
	kxMemMgrRecycle(buff, buffSize);
}

