#include "RebotPlayer.h"
#include "LoginProtocol.h"
#include "ShopProtocol.h"
#include "Protocol.h"
#include "KXServer.h"
#include "RebotComm.h"
#include "RebotOpConfig.h"
#include "HeroProtocol.h"
#include "StageProtocol.h"
#include "BagProtocol.h"
#include "PvpProtocol.h"

using namespace KxServer;
using namespace std;


CRebotPlayer::CRebotPlayer()
{
}


CRebotPlayer::~CRebotPlayer()
{
}

bool CRebotPlayer::init(int nUid)							//初始化
{
	m_nUid = nUid;
	m_bFight = false;
	m_bLoginOK = false;
	m_bSendLogin = false;
	m_bSendMatching = false;
	m_pKxComm = NULL;
	m_bBattleID = 0;			//战斗ID
	//没有战斗，发送非战斗指令
	vector<SOpData> &VectOpData = CRebotOpConfig::getInstance()->GetRebotOp();
	m_VectOpData.assign(VectOpData.begin(), VectOpData.end());

	return true;
}

bool CRebotPlayer::SetKxComm(KxServer::IKxComm* pKxComm)		//设置通讯端口
{
	m_pKxComm = pKxComm;
	return true;
}

bool CRebotPlayer::PlayerOperator()								//玩家操作
{
	if (!m_bLoginOK)
	{
		PlayerCheckLogin();
		return true;
	}

	int nCurTime = (int)time(NULL);
	if (m_VectOpData.size() != 0)
	{
		vector<SOpData>::iterator ator = m_VectOpData.begin();
		SOpData *pData = &(*ator);
		if (!m_bSendMatching)
		{
			PlayerSendRequest(pData);
			if (pData->nMainCmd == 20 && pData->nSubCmd == 1)
			{
				if (m_bBattleID == 0)
				{
					m_bSendMatching = true;
				}
			}

			if (pData->nMainCmd == 8 && pData->nSubCmd == 1)
			{
				return true;
			}

			m_VectOpData.erase(ator);
		}
	}

	return true;
}

void CRebotPlayer::encryptBuffer(int mainCmd, int subCmd, char* buff, int len)
{
	if (CMD_STAGE == mainCmd && CMD_STAGE_FINISH_CS == subCmd)
	{
		int start = 0;
		while (start < len / 2)
		{
			char *num1 = buff + start;
			char *num2 = buff + (len - start - 1);

			int t = *num1 ^ MakeCommand(mainCmd, subCmd);
			*num1 = *num2 ^ MakeCommand(mainCmd, subCmd);
			*num2 = t;

			start += 1;
		}
	}
}

//处理接收数据
bool CRebotPlayer::ProcessRecvData(int mainCmd, int nSubCmd, char *pszContext, int nLen)
{
	switch (mainCmd)
	{
	case CMD_LOGIN:
		ProcessLoginData(nSubCmd, pszContext, nLen);
		break;

	case CMD_USER:
		break;

	case CMD_SUMMONER:
		break;

	case CMD_HERO:
		break;

	case CMD_BAG:
		break;
	case CMD_STAGE:		    // pve场景
	case CMD_PVP:		    // pvp匹配
	{
		processPvpResultData(nSubCmd);
	}
	case CMD_BATTLE:		// pvp战斗
		break;
	case CMD_TEAM:
		break;

	case CMD_TASK:
		break;

	case CMD_ACHIEVEMENT:
		break;

	case CMD_GUIDE:
		break;

	case CMD_MAIL:
		break;
	case CMD_MATCH:
		ProcessPvpMatchData(nSubCmd, pszContext, nLen);
		break;
	case CMD_PVPCHEST:
		break;

	case CMD_INSTANCE:
		break;

	case CMD_GOLDTEST:
		break;

	case CMD_HEROTEST:
		break;

	case CMD_TOWERTEST:
		break;

	case CMD_SHOP:
		break;
	case CMD_RANK:
		break;
	case CMD_ACTIVE:
		break;
	case CMD_PAY:
		break;
	default:
		break;
	}

	return true;
}

void CRebotPlayer::PlayerCheckLogin()								//玩家登陆
{
	if (m_bSendLogin)
	{
		return;
	}

	int nLen = sizeof(Head)+sizeof(LoginCS);
	char *pSendData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nLen));

	Head *phead = (Head*)(pSendData);

	phead->MakeCommand(CMD_LOGIN, CMD_LOGIN_CHECK_TEST_CS);
	phead->id = m_nUid;
	phead->length = nLen;

	LoginCS *pLoginData = (LoginCS*)(phead + 1);
	pLoginData->userid = m_nUid;
	m_pKxComm->sendData(pSendData, nLen);
	KxServer::kxMemMgrRecycle(pSendData, nLen);
	m_bSendLogin = true;
	int nCurTime = (int)time(NULL);
	KXLOGDEBUG("LoginStart!! uid:%d Time: %d", m_nUid,nCurTime);
}

void CRebotPlayer::PlayerLogin()									//玩家登陆
{
	int nLen = sizeof(Head)+sizeof(LoginCS);
	char *pSendData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nLen));

	Head *phead = (Head*)(pSendData);

	phead->MakeCommand(CMD_LOGIN, CMD_LOGIN_CS);
	phead->id = m_nUid;
	phead->length = nLen;

	LoginCS *pLoginData = (LoginCS*)(phead + 1);
	pLoginData->userid = m_nUid;
	m_pKxComm->sendData(pSendData, nLen);
	KxServer::kxMemMgrRecycle(pSendData, nLen);
}

void CRebotPlayer::PlayerSendRequest(SOpData *pData)				//玩家发送请求
{
	int nTotalLen = 0;
	for (int i = 0; i < pData->VectParm.size(); i++)
	{
		SOpParamData &Param = pData->VectParm[i];

		if (Param.nParamType == emRebotParamInt)
		{
			nTotalLen += sizeof(int);
		}
		else
		{
			nTotalLen += Param.nLen;
		}
	}

	nTotalLen += sizeof(Head);

	char *pSendData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nTotalLen));
	Head *phead = (Head*)(pSendData);

	phead->MakeCommand(pData->nMainCmd,pData->nSubCmd);
	phead->id = m_nUid;
	phead->length = nTotalLen;

	char *pContext = (char*)(phead + 1);
	int nOffset = 0;
	for (int i = 0; i < pData->VectParm.size(); i++)
	{
		SOpParamData &Param = pData->VectParm[i];

		if (Param.nParamType == emRebotParamInt)
		{
			int nValue = 0;
			if (Param.nLen == emRebotUidSParam)
			{
				nValue = m_nUid;
			}
			else if (Param.nLen == emRebotBattleSParam)
			{
				nValue = m_bBattleID;
			}
			else
			{
				nValue = atoi(Param.szParam.c_str());
			}

			memcpy(pContext + nOffset, &nValue, sizeof(int));
			nOffset += sizeof(int);
		}
		else
		{
			memcpy(pContext + nOffset, Param.szParam.c_str(), Param.nLen);
			nOffset += Param.nLen;
		}
	}

	encryptBuffer(pData->nMainCmd, pData->nSubCmd, pSendData + sizeof(Head), nTotalLen - sizeof(Head));
	m_pKxComm->sendData(pSendData,nTotalLen);
	KxServer::kxMemMgrRecycle(pSendData,nTotalLen);

}

//处理登陆数据接收
bool CRebotPlayer::ProcessLoginData(int nSubCmd, char *pszContext, int nLen)
{
	switch (nSubCmd)
	{
		case CMD_LOGIN_FINISH_SC:		//登录完成
		{
			m_bLoginOK = true;
			int nCurTime = (int)time(NULL);
			KXLOGDEBUG("LoginFinish!! uid:%d,nCurTime:%d",m_nUid,nCurTime);
		}
		break;
		case CMD_LOGIN_SC:				//玩家数据下发
		case CMD_LOGIN_USERMODEL_SC:	//玩家模型数据
		case CMD_LOGIN_BAGMODEL_SC:		//背包模型数据
		case CMD_LOGIN_EQUIPMODEL_SC:	//装备模型数据
		case CMD_LOGIN_SUMMONMODEL_SC:	//召唤师模型数据
		case CMD_LOGIN_HEROMODEL_SC:		//英雄模型数据
		case CMD_LOGIN_STAGEMODEL_SC:	//关卡模型数据
		case CMD_LOGIN_TEAMMODEL_SC:     //队伍模型数据
		case CMD_LOGIN_TASKMODEL_SC:     //任务模型数据
		case CMD_LOGIN_ACHIEVEMODEL_SC:  //成就模型数据
		case CMD_LOGIN_GUIDEMODEL_SC:	//引导模型数据
		case CMD_LOGIN_UNIONMODEL_SC:	//公会模型数据
		case CMD_LOGIN_MAILMODEL_SC:	//邮件模型数据
		case CMD_LOGIN_INSTANCEMODEL_SC:	//活动副本模型
		case CMD_LOGIN_GOLDTESTMODEL_SC: //金币试炼数据模型
		case CMD_LOGIN_HEROTESTMODEL_SC:	//英雄试炼数据模型
		case CMD_LOGIN_TOWERTESTMODEL_SC: //爬塔试炼数据
		case CMD_LOGIN_PVPMODEL_SC:		//pvp模型数据
		case CMD_LOGIN_SHOPMODEL_SC:		//商店模型数据
		case CMD_LOGIN_ACTIVEMODEL_SC:	//活动模型数据
		case CMD_LOGIN_BAN_SC:			//服务器封停应答
		case CMD_LOGIN_RECHANGE_SC:		//服务器角色被顶号应答
		case CMD_LOGIN_TICK_SC:			//服务器角色被T应答
		{

		}
		break;
		case CMD_LOGIN_CHECK_SC:		//验证应答
		{
			PlayerLogin();
		}
		break;
		default:
			break;
	}

	return true;
}

//处理PVP战斗匹配时的战斗ID
bool CRebotPlayer::ProcessPvpMatchData(int nSubCmd, char *pszContext, int nLen)
{
	switch (nSubCmd)
	{
	case CMD_MATCH_MATCHING_SC:		//正在匹配
	case CMD_MATCH_REMATCH_SC:		//重新匹配
	break;
	case CMD_MATCH_MATCHSUCCESS_SC:	//成功匹配到信息
		{
			if (nLen != sizeof(MatchMatchSuccessSC) || pszContext == NULL)
			{
				return false;
			}

			MatchMatchSuccessSC *pMsg = (MatchMatchSuccessSC*)pszContext;

			m_bBattleID = pMsg->battleId;
			m_bSendMatching = false;
		}
	break;
	case CMD_MATCH_CANCEL_SC:		//取消匹配
		{
			m_bBattleID = 0;
		}
	break;
	break;
	default:
	break;
	}

	return true;
}


//处理接收PVP战斗结算
bool CRebotPlayer::processPvpResultData(int nSubCmd)
{
	int nCurTime = (int)time(NULL);
	switch (nSubCmd)
	{
	case CMD_PVP_RESULT_SC:
	{
		KXLOGDEBUG("PvpFinish!! uid:%d,nTime:%d", m_nUid,nCurTime);
		m_VectOpData.clear();			//最终PVP打完，则清空所有请求
	}
	default:
		break;
	}

	return true;
}