#include "CommOssHelper.h"
#include "StatModel.h"
#include "GameUser.h"
#include "GameUserManager.h"
#include "GameDef.h"
#include "KxCommManager.h"
#include "SummonerServer.h"

using namespace KxServer;
using namespace std;


CCommOssHelper::CCommOssHelper()
{
}

CCommOssHelper::~CCommOssHelper()
{
}

//注册日志
void CCommOssHelper::registerStatOss(int uid, std::string Mobile, int nChannelType, std::string ChannelName)
{
	CStatModel::getInstance()->MessageStart(uid, "URegI");

	char Temp[128] = { 0 };
	snprintf(Temp,sizeof(Temp),"%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);
	CStatModel::getInstance()->MessageDataAdd("mobile", Mobile);
	snprintf(Temp, sizeof(Temp), "%d", nChannelType);
	CStatModel::getInstance()->MessageDataAdd("channel", Temp);
	CStatModel::getInstance()->MessageDataAdd("channel_acc", ChannelName);
	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
	int nWorldID = pServer->getGroup();
	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);

    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageDataAdd("reg_time", Temp,1);
	CStatModel::getInstance()->MessageEnd(Temp);
}

//用户渠道绑定信息
void CCommOssHelper::userChannelOss(int uid, int nChannelType, string ChannelName)
{
	CStatModel::getInstance()->MessageStart(uid, "UChannelI");
	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);
	snprintf(Temp, sizeof(Temp), "%d", nChannelType);
	CStatModel::getInstance()->MessageDataAdd("channel", Temp);
	CStatModel::getInstance()->MessageDataAdd("channel_acc", ChannelName);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp, 1);
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//角色信息日志
void CCommOssHelper::crateActorStatOss(int uid)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CStatModel::getInstance()->MessageStart(uid, "UCreateI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server",Temp, 1);

	//日志发送时间戳
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//角色首次登陆信息日志
void CCommOssHelper::userLoginOss(int uid,int nFirstLoign)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CHECK_RETURN_VOID(pUserModel != NULL);

	CStatModel::getInstance()->MessageStart(uid, "ULoginI");

	char Temp[56] = { 0 };
	int Data = 0;
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);
	//是否首次登陆
	snprintf(Temp, sizeof(Temp), "%d", nFirstLoign);
	CStatModel::getInstance()->MessageDataAdd("first_login",Temp);

	//体力
	pUserModel->GetUserFieldVal(USR_FD_ENERGY, Data);
	snprintf(Temp, sizeof(Temp), "%d", Data);
	CStatModel::getInstance()->MessageDataAdd("energy", Temp);

	//金币
	pUserModel->GetUserFieldVal(USR_FD_GOLD, Data);
	snprintf(Temp, sizeof(Temp), "%d", Data);
	CStatModel::getInstance()->MessageDataAdd("gold", Temp);

	//塔币
	pUserModel->GetUserFieldVal(USR_FD_TOWERCOIN, Data);
	snprintf(Temp, sizeof(Temp), "%d", Data);
	CStatModel::getInstance()->MessageDataAdd("tower_token", Temp);

	//PVP币
	pUserModel->GetUserFieldVal(USR_FD_PVPCOIN, Data);
	snprintf(Temp, sizeof(Temp), "%d", Data);
	CStatModel::getInstance()->MessageDataAdd("pvp_token", Temp);

	//公会贡献
	pUserModel->GetUserFieldVal(USR_FD_UNIONCONTRIB, Data);
	snprintf(Temp, sizeof(Temp), "%d", Data);
	CStatModel::getInstance()->MessageDataAdd("union_token", Temp,1);

	//角色登陆时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

void CCommOssHelper::userLoginOutOss(int uid, int LoginTime)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CStatModel::getInstance()->MessageStart(uid, "ULogoutI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);

	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CHECK_RETURN_VOID(pUserModel != NULL);

	//本次登录时间
	snprintf(Temp, sizeof(Temp), "%d", LoginTime);
	CStatModel::getInstance()->MessageDataAdd("login_time",Temp,1);

	//角色登出时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//用户新手引导日志
void CCommOssHelper::newGuidOss(int uid, int nStep)
{
	CStatModel::getInstance()->MessageStart(uid, "UGuildI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);
	//步骤
	snprintf(Temp, sizeof(Temp), "%d", nStep);
	CStatModel::getInstance()->MessageDataAdd("guild", Temp, 1);

    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//用户升级信息
void CCommOssHelper::userLevelUpOss(int uid)
{

	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CStatModel::getInstance()->MessageStart(uid, "ULvupI");

	char Temp[56] = { 0 };
	int Data = 0;
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);
	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);

	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CHECK_RETURN_VOID(pUserModel != NULL);
	//角色等级
	pUserModel->GetUserFieldVal(USR_FD_USERLV, Data);
	snprintf(Temp, sizeof(Temp), "%d", Data);
	CStatModel::getInstance()->MessageDataAdd("lv",Temp,1);
	//日志记录时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//用户充值信息
void CCommOssHelper::userCashOss(int uid,int nPayValue)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CStatModel::getInstance()->MessageStart(uid, "UCashI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);
	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);
	snprintf(Temp, sizeof(Temp), "%d", nPayValue);
	CStatModel::getInstance()->MessageDataAdd("cash", Temp,1);

	//日志记录时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//用户钻石消费信息
void CCommOssHelper::userDiamondPayOss(int uid, int Diamond, int nPayType,int nNum, int Info)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CStatModel::getInstance()->MessageStart(uid, "UDPayI");

	char Temp[56] = { 0 };
	int Data = 0;
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);
	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);
	snprintf(Temp, sizeof(Temp), "%d", Diamond);
	CStatModel::getInstance()->MessageDataAdd("diamond",Temp);
	snprintf(Temp, sizeof(Temp), "%d", nPayType);
	CStatModel::getInstance()->MessageDataAdd("pay_for", Temp);
	
	if (Info != 0)
	{
		snprintf(Temp, sizeof(Temp), "%d", Info);
		CStatModel::getInstance()->MessageDataAdd("get", Temp);
	}

	snprintf(Temp, sizeof(Temp), "%d", nNum);
	CStatModel::getInstance()->MessageDataAdd("num", Temp,1);

	//日志记录时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//用户钻石购买信息
void CCommOssHelper::userDiamondGetOss(int uid, int Diamond, int nReason)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CStatModel::getInstance()->MessageStart(uid, "UDGainI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);
	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);
	snprintf(Temp, sizeof(Temp), "%d", Diamond);
	CStatModel::getInstance()->MessageDataAdd("diamond", Temp);
	snprintf(Temp, sizeof(Temp), "%d", nReason);
	CStatModel::getInstance()->MessageDataAdd("reason", Temp,1);

	//日志记录时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//用户非钻石购买信息
void CCommOssHelper::userTokenShopOss(int uid,int TokenType,int Cost,int PayforType,int Num,int Info)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CStatModel::getInstance()->MessageStart(uid, "UTShopI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);
	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server",Temp);
	snprintf(Temp, sizeof(Temp), "%d", TokenType);
	CStatModel::getInstance()->MessageDataAdd("token_type",Temp);
	snprintf(Temp, sizeof(Temp), "%d", Cost);
	CStatModel::getInstance()->MessageDataAdd("token", Temp);
	snprintf(Temp, sizeof(Temp), "%d", PayforType);
	CStatModel::getInstance()->MessageDataAdd("pay_for", Temp);
	snprintf(Temp, sizeof(Temp), "%d", Info);
	CStatModel::getInstance()->MessageDataAdd("get", Temp);
	snprintf(Temp, sizeof(Temp), "%d", Num);
	CStatModel::getInstance()->MessageDataAdd("num", Temp,1);

	//日志记录时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//用户VIP升级信息
void CCommOssHelper::UserVipUpOss(int uid)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CStatModel::getInstance()->MessageStart(uid, "UVipI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);
	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);
	//VIP等级
	CStatModel::getInstance()->MessageDataAdd("vip", "0", 1);
	//日志记录时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//关卡日志统计
void CCommOssHelper::StageLogicOss(int uid, SOssPveLogData &LogData)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(pGameUser->getModel(MODELTYPE_HERO));
	CHECK_RETURN_VOID(pHeroModel != NULL);

	CStatModel::getInstance()->MessageStart(uid, "UPveI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);
	//关卡ID
	snprintf(Temp, sizeof(Temp), "%d", LogData.nStageID);
	CStatModel::getInstance()->MessageDataAdd("stage", Temp);
	
	//召唤师ID
	snprintf(Temp, sizeof(Temp), "%d", LogData.nSummonerID);
	CStatModel::getInstance()->MessageDataAdd("summoner", Temp);
	//英雄等级
	DBHeroAttr heroAttr;
	for (int i = 0; i < MAX_HERO_COUNT; ++i)
	{
		int heroId = LogData.nHeroID[i];
		if (heroId != 0)
		{
			if (!pHeroModel->GetHero(heroId, heroAttr))
			{
				continue;
			}

			snprintf(Temp, sizeof(Temp), "%d", i+1);
			std::string szHeroIndex = "hero";
			std::string szHeroLv = "hero";
			std::string szHeroTalent = "hero";
			std::string szHeroStart = "hero";
			szHeroIndex += Temp;
			szHeroLv += Temp;
			szHeroLv += "_lv";
			szHeroTalent += Temp;
			szHeroTalent += "_t";
			szHeroStart += Temp;
			szHeroStart += "_s";
			snprintf(Temp, sizeof(Temp), "%d", heroId);
			CStatModel::getInstance()->MessageDataAdd(szHeroIndex, Temp);
			snprintf(Temp, sizeof(Temp), "%d", heroAttr.level);
			CStatModel::getInstance()->MessageDataAdd(szHeroLv, Temp);
            for (int j = 0; j < TALENT_COUNT; ++j)
            {
                snprintf(Temp, sizeof(Temp), "%d", heroAttr.talent[j]);
            }
			CStatModel::getInstance()->MessageDataAdd(szHeroTalent, Temp);
			snprintf(Temp, sizeof(Temp), "%d", heroAttr.star);
			CStatModel::getInstance()->MessageDataAdd(szHeroStart, Temp);
		}
	}

	snprintf(Temp, sizeof(Temp), "%d", LogData.nCryStalLevel);
	CStatModel::getInstance()->MessageDataAdd("crystal_lv", Temp);

	//是否通关
	int Win = 0;
	if (LogData.bWin)
	{
		Win = 1;
	}

	snprintf(Temp, sizeof(Temp), "%d", LogData.nStartNum);
	CStatModel::getInstance()->MessageDataAdd("get_stars", Temp);

	snprintf(Temp, sizeof(Temp), "%d", LogData.nTime);
	CStatModel::getInstance()->MessageDataAdd("fight_time", Temp);

	snprintf(Temp, sizeof(Temp), "%d", Win);
	CStatModel::getInstance()->MessageDataAdd("win", Temp);

	//解锁关卡ID
	snprintf(Temp, sizeof(Temp), "%d", LogData.UnLockStageID);
	CStatModel::getInstance()->MessageDataAdd("UnLockStageID", Temp,1);

	//时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//PVP日志统计
void CCommOssHelper::PvpLogicOss(int uid, SOssPvpLogData &LogData)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	if (pGameUser == NULL)
	{
		return;
	}

	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(pGameUser->getModel(MODELTYPE_HERO));
	CHECK_RETURN_VOID(pHeroModel != NULL);
	
	CStatModel::getInstance()->MessageStart(uid, "UPvpI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);

	snprintf(Temp, sizeof(Temp), "%d", LogData.nPvpType);
	CStatModel::getInstance()->MessageDataAdd("pvp_type", Temp);

	//召唤师ID
	snprintf(Temp, sizeof(Temp), "%d", LogData.nSummonerID);
	CStatModel::getInstance()->MessageDataAdd("summoner", Temp);
	
	//英雄等级
	DBHeroAttr heroAttr;
	for (int i = 0; i < MAX_HERO_COUNT; ++i)
	{
		int heroId = LogData.nHeroID[i];
		if (heroId != 0)
		{
			if (!pHeroModel->GetHero(heroId, heroAttr))
			{
				continue;
			}

			snprintf(Temp, sizeof(Temp), "%d", i + 1);
			std::string szHeroIndex = "hero";
			std::string szHeroLv = "hero";
			std::string szHeroTalent = "hero";
			std::string szHeroStart = "hero";
			szHeroIndex += Temp;
			szHeroLv += Temp;
			szHeroLv += "_lv";
			szHeroTalent += Temp;
			szHeroTalent += "_t";
			szHeroStart += Temp;
			szHeroStart += "_s";

			snprintf(Temp, sizeof(Temp), "%d", heroId);
			CStatModel::getInstance()->MessageDataAdd(szHeroIndex, Temp);
			snprintf(Temp, sizeof(Temp), "%d", heroAttr.level);
			CStatModel::getInstance()->MessageDataAdd(szHeroLv, Temp);
            for (int j = 0; j < TALENT_COUNT; ++j)
            {
                snprintf(Temp, sizeof(Temp), "%d", heroAttr.talent[j]);
            }
			CStatModel::getInstance()->MessageDataAdd(szHeroTalent, Temp);
			snprintf(Temp, sizeof(Temp), "%d", heroAttr.star);
			CStatModel::getInstance()->MessageDataAdd(szHeroStart, Temp);
		}
	}

	//PVP敌方召唤师
	snprintf(Temp, sizeof(Temp), "%d", LogData.nOpponentSummer);
	CStatModel::getInstance()->MessageDataAdd("opponent_summoner", Temp);

	snprintf(Temp, sizeof(Temp), "%d", LogData.nCryStalLevel);
	CStatModel::getInstance()->MessageDataAdd("crystal_lv", Temp);

	//是否通关
	int Win = 0;
	if (LogData.bWin)
	{
		Win = 1;
	}

	snprintf(Temp, sizeof(Temp), "%d", LogData.nTime);
	CStatModel::getInstance()->MessageDataAdd("fight_time", Temp);

	snprintf(Temp, sizeof(Temp), "%d", Win);
	CStatModel::getInstance()->MessageDataAdd("win", Temp);

	snprintf(Temp, sizeof(Temp), "%d", LogData.nPvpNum);
	CStatModel::getInstance()->MessageDataAdd("pvp_point", Temp,1);

	//时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//其他副本战斗信息
void CCommOssHelper::UserSpStageOss(int uid,int StageID,int StageType,bool bWin)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CStatModel::getInstance()->MessageStart(uid, "UStageI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);
	//关卡ID
	snprintf(Temp, sizeof(Temp), "%d", StageID);
	CStatModel::getInstance()->MessageDataAdd("stage", Temp);
	//关卡类型
	snprintf(Temp, sizeof(Temp), "%d", StageType);
	CStatModel::getInstance()->MessageDataAdd("stage_type", Temp);

	CTeamModel *pTeamModel = dynamic_cast<CTeamModel*>(pGameUser->getModel(MODELTYPE_TEAM));
	CHECK_RETURN_VOID(pTeamModel != NULL);

	TeamInfo passTeam;
	if (!pTeamModel->GetTeamInfo(ETT_PASE, passTeam))
	{
		return;
	}

	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(pGameUser->getModel(MODELTYPE_HERO));
	CHECK_RETURN_VOID(pHeroModel != NULL);

	//召唤师ID
	snprintf(Temp, sizeof(Temp), "%d", passTeam.summonerId);
	CStatModel::getInstance()->MessageDataAdd("summoner", Temp);
	//英雄等级
	DBHeroAttr heroAttr;
	for (int i = 0; i < MAX_HERO_COUNT; ++i)
	{
		int heroId = passTeam.heroId[i];
		if (heroId != 0)
		{
			if (!pHeroModel->GetHero(heroId, heroAttr))
			{
				continue;
			}

			snprintf(Temp, sizeof(Temp), "%d", i + 1);
			std::string szHeroIndex = "hero";
			std::string szHeroLv = "hero";
			std::string szHeroTalent = "hero";
			std::string szHeroStart = "hero";
			szHeroIndex += Temp;
			szHeroLv += Temp;
			szHeroLv += "_lv";
			szHeroTalent += Temp;
			szHeroTalent += "_t";
			szHeroStart += Temp;
			szHeroStart += "_s";

			snprintf(Temp, sizeof(Temp), "%d", heroId);
			CStatModel::getInstance()->MessageDataAdd(szHeroIndex, Temp);
			snprintf(Temp, sizeof(Temp), "%d", heroAttr.level);
			CStatModel::getInstance()->MessageDataAdd(szHeroLv, Temp);
            for (int j = 0; j < TALENT_COUNT; ++j)
            {
                snprintf(Temp, sizeof(Temp), "%d", heroAttr.talent[j]);
            }
			CStatModel::getInstance()->MessageDataAdd(szHeroTalent, Temp);
			snprintf(Temp, sizeof(Temp), "%d", heroAttr.star);
			CStatModel::getInstance()->MessageDataAdd(szHeroStart, Temp);
		}
	}

	//是否通关
	int Win = 0;
	if (bWin)
	{
		Win = 1;
	}

	snprintf(Temp, sizeof(Temp), "%d", Win);
	CStatModel::getInstance()->MessageDataAdd("win", Temp,1);

	//时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//关卡扫荡统计
void CCommOssHelper::StageSweepOss(int uid, int nStageID,int nTimes)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CStatModel::getInstance()->MessageStart(uid, "USweepI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);
	//关卡ID
	snprintf(Temp, sizeof(Temp), "%d", nStageID);
	CStatModel::getInstance()->MessageDataAdd("stage", Temp);
	//关卡扫荡次数
	snprintf(Temp, sizeof(Temp), "%d", nTimes);
	CStatModel::getInstance()->MessageDataAdd("times", Temp,1);

	//时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//卡牌升星信息
void CCommOssHelper::UserCardUpStartOss(int uid, int heroId)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(pGameUser->getModel(MODELTYPE_HERO));
	CHECK_RETURN_VOID(pHeroModel != NULL);

	CStatModel::getInstance()->MessageStart(uid, "UCStarI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);


	DBHeroAttr heroAttr;
	CHECK_RETURN_VOID(pHeroModel->GetHero(heroId, heroAttr));
	snprintf(Temp, sizeof(Temp), "%d", heroId);
	CStatModel::getInstance()->MessageDataAdd("card", Temp);
	snprintf(Temp, sizeof(Temp), "%d", heroAttr.star);
	CStatModel::getInstance()->MessageDataAdd("star", Temp,1);

	//时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//卡牌升级信息
void CCommOssHelper::UserCardLvUpOss(int uid, int heroId)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN_VOID(NULL != pGameUser);
	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(pGameUser->getModel(MODELTYPE_HERO));
	CHECK_RETURN_VOID(NULL != pHeroModel);

	CStatModel::getInstance()->MessageStart(uid, "UCLvupI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);

	
	DBHeroAttr heroAttr;
	CHECK_RETURN_VOID(pHeroModel->GetHero(heroId, heroAttr));
	snprintf(Temp, sizeof(Temp), "%d", heroId);
	CStatModel::getInstance()->MessageDataAdd("card", Temp,1);
	//snprintf(Temp, "%d", Info.heroStarLv);
	//CStatModel::getInstance()->MessageDataAdd("star", Temp);

	//时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//当前在线用户信息
void CCommOssHelper::ServerStatusOss()
{
	char Temp[56] = { 0 };
	int Data = 0;

	CStatModel::getInstance()->MessageStart(0, "ServerStatusInfo");
	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
	int nServerID = 0;
	if (pServer != NULL)
	{
		nServerID = pServer->getServerId();
	}

	snprintf(Temp, sizeof(Temp), "%d", nServerID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);

    int nWorldID = pServer->getGroup();
	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("world", Temp);

	Data = CKxCommManager::getInstance()->getOnLineNum();
	snprintf(Temp, sizeof(Temp), "%d", Data);
	CStatModel::getInstance()->MessageDataAdd("user_num", Temp,1);
	//时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//用户礼包领取消息
void CCommOssHelper::UserGiftGetOss(int uid, int nChannelID, int nGiftID, std::string CDKey)
{
	CStatModel::getInstance()->MessageStart(uid, "UGiftI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
    int nWorldID = pServer->getGroup();

	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);
	snprintf(Temp, sizeof(Temp), "%d", nChannelID);
	CStatModel::getInstance()->MessageDataAdd("channel", Temp);
	snprintf(Temp, sizeof(Temp), "%d", nGiftID);
	CStatModel::getInstance()->MessageDataAdd("gift", Temp);
	CStatModel::getInstance()->MessageDataAdd("cd-key", CDKey,1);
	//时间
    snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//用户匹配消息
void CCommOssHelper::UserMatchOss(int uid, int PvpType, int nMathTime, int nMatchFlag)
{
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN_VOID(NULL != pGameUser);
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel*>(pGameUser->getModel(MODELTYPE_PVP));
	CHECK_RETURN_VOID(NULL != pPvpModel);

	int nPvpNum = 0;
	if (PvpType == enNormalMathType)
	{
		pPvpModel->GetPvpField(PVP_FD_INTEGRAL, nPvpNum, true);
	}
	else
	{
		pPvpModel->GetPvpField(CPN_FD_INTEGRAL, nPvpNum, true);
	}
	
	CStatModel::getInstance()->MessageStart(uid, "UPvpMatchI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
	int nWorldID = pServer->getGroup();
	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);
	snprintf(Temp, sizeof(Temp), "%d", PvpType);
	CStatModel::getInstance()->MessageDataAdd("pvp_type", Temp);
	snprintf(Temp, sizeof(Temp), "%d", nMathTime);
	CStatModel::getInstance()->MessageDataAdd("match_time", Temp);
	snprintf(Temp, sizeof(Temp), "%d", nMatchFlag);
	CStatModel::getInstance()->MessageDataAdd("match_success", Temp);
	snprintf(Temp, sizeof(Temp), "%d", nPvpNum);
	CStatModel::getInstance()->MessageDataAdd("pvp_point", Temp,1);
	//时间
	snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//活动数据消息
void CCommOssHelper::UserActiveOss(int uid, int nActiveID, int nParamID)
{
	CStatModel::getInstance()->MessageStart(uid, "UserOpInfoI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
	int nWorldID = pServer->getGroup();
	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);
	snprintf(Temp, sizeof(Temp), "%d", nActiveID);
	CStatModel::getInstance()->MessageDataAdd("op_id", Temp);
	snprintf(Temp, sizeof(Temp), "%d", nParamID);
	CStatModel::getInstance()->MessageDataAdd("task_id", Temp,1);
	//时间
	snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}

//用户道具信息
void CCommOssHelper::userGoodsGainOss(int uid, int nActionID, int nGoodsID, int nGoodsNum)
{
	CStatModel::getInstance()->MessageStart(uid, "UserGainInfoI");

	char Temp[56] = { 0 };
	snprintf(Temp, sizeof(Temp), "%d", uid);
	CStatModel::getInstance()->MessageDataAdd("uid", Temp);

	//服务器
	CSummonerServer *pServer = CSummonerServer::getNewInstance();
	int nWorldID = pServer->getGroup();
	snprintf(Temp, sizeof(Temp), "%d", nWorldID);
	CStatModel::getInstance()->MessageDataAdd("server", Temp);
	snprintf(Temp, sizeof(Temp), "%d", nActionID);
	CStatModel::getInstance()->MessageDataAdd("action", Temp);
	snprintf(Temp, sizeof(Temp), "%d", nGoodsID);
	CStatModel::getInstance()->MessageDataAdd("goodsid", Temp);
	snprintf(Temp, sizeof(Temp), "%d", nGoodsNum);
	CStatModel::getInstance()->MessageDataAdd("goodsnum", Temp, 1);
	//时间
	snprintf(Temp, sizeof(Temp), "%d", pServer->getTimerManager()->getTimestamp());
	CStatModel::getInstance()->MessageEnd(Temp);
}
