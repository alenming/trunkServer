#include "SessionGMService.h"
#include "SessionClient.h"
#include "NetworkManager.h"
#include "Protocol.h"
#include "ServerData.h"
#include "SessionServer.h"
#include "GMProtocol.h"
#include <set>
#include "SessionGMService.h"
#include "KxCSComm.h"
#include "SessionConnect.h"
#include "KXServer.h"
#include "ChangeConnectModule.h"
#include "Protocol.h"
#include "NoticeProtocol.h"

using namespace KxServer;

void CSessionGMService::processGMLogic(char* buffer, unsigned int len, int nGroupID, int nKey, KxServer::IKxComm *target)
{
	CSessionClient* pClient = dynamic_cast<CSessionClient*>(target);
	CHECK_RETURN_DEBUG_VOID(NULL != pClient && sizeof(Head) <= len);
	// 直接转发给游戏服务器
	Head* head = reinterpret_cast<Head*>(buffer);
	char *pData = buffer + sizeof(Head);
	int nDataLen = len - sizeof(Head);
	CHECK_RETURN_DEBUG_VOID(head != NULL);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	int nUid = head->id;
	CServerConfig * pServerConfig = CSessionServer::getInstance()->getServerCoinfig();
	CHECK_RETURN_DEBUG_VOID(pServerConfig != NULL);
	ServerRouteConfigData *pRouteConifgData = pServerConfig->getRouteConfigData(nMainCmd, nSubCmd);
	CHECK_RETURN_DEBUG_VOID(pRouteConifgData != NULL);
	CHECK_RETURN_DEBUG_VOID(nMainCmd == CMD_GM);
	CHECK_RETURN_DEBUG_VOID(pClient->getPermission() >= pRouteConifgData->Permission);

	switch (nSubCmd)
	{
		case CMD_GM_CLOSE_CS:
		{
			processCloseServer(target);
		}
		break;
		case CMD_GM_SERVERNUM_CS:
		{
			processGetServerNum(target);
		}
		break;
		case CMD_GM_DYNUPDATE_CS:
		{
			processDynUpdateServer(pData,nDataLen,target);
		}
		break;
		case CMD_GM_ROUTE_CS:
		{
			processGMRouteChatServer(pData, nDataLen, nGroupID, nUid, target);
		}
		break;
	default:
		break;
	}
}

//处理GM 获取玩家人数请求
void CSessionGMService::processGetServerNum(KxServer::IKxComm *target)
{
	SGMServerNumSC NumSC;
	NumSC.nNum = CNetWorkManager::getInstance()->getCurClientNum();
	CNetWorkManager::getInstance()->sendDataToClient(dynamic_cast<CSessionClient*>(target), CMD_GM, CMD_GM_SERVERNUM_CS, (char*)&NumSC, sizeof(NumSC));
	return;
}

//处理GM 命令平滑关服请求
void CSessionGMService::processCloseServer(KxServer::IKxComm *target)
{
	CSessionServer::getInstance()->setSessionServerClosing();
	SGMFlagSC CloseSC;
	CloseSC.nFlag = 1;
	CNetWorkManager::getInstance()->sendDataToClient(dynamic_cast<CSessionClient*>(target), CMD_GM, CMD_GM_CLOSE_SC, (char*)&CloseSC, sizeof(CloseSC));
	return;
}

//处理GM 命令热更新服务器请求
void CSessionGMService::processDynUpdateServer(char *pData, int nLen, KxServer::IKxComm *target)
{
	//热更新
	CHECK_RETURN_DEBUG_VOID(pData != NULL && nLen == sizeof(SGMDynUpdateCS));
	SGMDynUpdateCS *pUpdateCS = (SGMDynUpdateCS *)pData;
	//正在更新中
	if (CNetWorkManager::getInstance()->getChangeFlag())
	{
		return;
	}

	CChangeConnectModule *pConnectModule = new CChangeConnectModule();
	pConnectModule->init();
	pConnectModule->addCallBackClient(target);
	CNetWorkManager::getInstance()->clearBakServer(pUpdateCS->nGroupID);

	for (int i = 0; i < pUpdateCS->nNum; i++)
	{
		KXLOGDEBUG("===> SessionServer Connect to Server: IP=127.0.0.1, Port=%d",pUpdateCS->nStartPort + i);
		CSessionConnector *pConnector = new CSessionConnector();
		if (!pConnector->init() || !pConnector->connect("127.0.0.1", pUpdateCS->nStartPort+i,pUpdateCS->nGroupID, true))
		{
			KXLOGERROR("===> SessionServer Connect to Server Failed!");
			SGMFlagSC UpdateSC;
			UpdateSC.nFlag = 0;
			CNetWorkManager::getInstance()->sendDataToClient(dynamic_cast<CSessionClient*>(target), CMD_GM, CMD_GM_DYNUPDATE_SC, (char*)&UpdateSC, sizeof(UpdateSC));
			CNetWorkManager::getInstance()->clearBakServer(pUpdateCS->nGroupID);
			KXSAFE_RELEASE(pConnector);
			KXSAFE_RELEASE(pConnectModule);
			return;
		}

		pConnector->setModule(pConnectModule);
		CSessionServer::getInstance()->getMainPoller()->addCommObject(pConnector, pConnector->getPollType());
		CNetWorkManager::getInstance()->addBakServer(pUpdateCS->nGroupID, pConnector);
		KXSAFE_RELEASE(pConnector);
	}

	return;
}

//处理GM 命令数据转发聊天服
void CSessionGMService::processGMRouteChatServer(char *pData, int nLen, int nGroupID, int nKey, KxServer::IKxComm *target)
{
	CHECK_RETURN_DEBUG_VOID(pData != NULL && nLen > sizeof(SGMRouteChatCS));
	SGMRouteChatCS *pUpdateCS = (SGMRouteChatCS *)pData;

	CSessionClient* pClient = dynamic_cast<CSessionClient*>(target);
	unsigned int buffSize = sizeof(Head) + sizeof(NoticeSC)+sizeof(NoticeInfo)+sizeof(NoticeParamContent)+sizeof(short) +pUpdateCS->nLen;
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));
	int nOffset = 0;

	//构建聊天消息
	Head* clientHead = reinterpret_cast<Head*>(buff);
	clientHead->id = nKey;
	clientHead->MakeCommand(CMD_NOTICE, CMD_NOTICE_SS);
	clientHead->length = buffSize;
	nOffset += sizeof(Head);

	NoticeSC *pNoticeSC = reinterpret_cast<NoticeSC*>(buff + nOffset);
	pNoticeSC->noticeCount = 1;
	nOffset += sizeof(NoticeSC);

	NoticeInfo *pNoticeInfo = reinterpret_cast<NoticeInfo*>(buff + nOffset);
	pNoticeInfo->noticeId = 16;
	pNoticeInfo->paramCount = 1;
	nOffset += sizeof(NoticeInfo);

	NoticeParamContent *pParamContent = reinterpret_cast<NoticeParamContent*>(buff + nOffset);
	pParamContent->paramType = PARAM_TYPE_STRING;
	nOffset += sizeof(NoticeParamContent);
	
	short *pLen = reinterpret_cast<short*>(buff + nOffset);
	(*pLen) = pUpdateCS->nLen;
	nOffset += sizeof(short);

	char *pReadContext = reinterpret_cast<char*>(buff + nOffset);
	char *pContext = reinterpret_cast<char*>(pData + sizeof(SGMRouteChatCS));
	memcpy(pReadContext, pContext, pUpdateCS->nLen);
	pClient->sendDataToGroupServer(nGroupID, buff,buffSize);
	kxMemMgrRecycle(buff, buffSize);
}
