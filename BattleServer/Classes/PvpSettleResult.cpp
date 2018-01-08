#include "PvpSettleResult.h"
#include "CommonHelper.h"
#include "ModelHelper.h"
#include "PvpModel.h"
#include "UserModel.h"
#include "TeamModel.h"
#include "PvpRankModel.h"
#include "PvpTaskModel.h"
#include "Room.h"
#include "UserActionListener.h"
#include "GameDef.h"
#include "ConfGameSetting.h"
#include "BattleModels.h"
#include "Protocol.h"
#include "PvpProtocol.h"
#include "KxMemPool.h"
#include "CommOssHelper.h"
#include "KxCommManager.h"
#include "BattleDataCache.h"
#include "ItemDrop.h"
#include "PropUseHelper.h"
#include <stdlib.h>

#define _PSRMAX(a,b)            (((a) > (b)) ? (a) : (b))
#define _PSRMIN(a,b)            (((a) < (b)) ? (a) : (b))

CPvpSettleResult::CPvpSettleResult()
: m_nRoomType(0)
{
}

CPvpSettleResult::~CPvpSettleResult()
{
}

bool CPvpSettleResult::init(int roomType)
{
	m_nRoomType = roomType;
	return true;
}

void CPvpSettleResult::pvpSettleAccount(int winner, int loser, int result, int winerCryLv, int loserCryLv, int dt)
{
	if (m_nRoomType == PVPROOMTYPE_CHAMPIONSHIP)
	{
		overChampionPvp(winner, loser, result,winerCryLv,loserCryLv,dt);
	}
	else
	{
		overFairPvp(winner, loser, result, winerCryLv, loserCryLv, dt);
	}

    CModelHelper::addDayLiveness(winner, UNION_DAY_PVPLIVENESS);
    CModelHelper::addDayLiveness(loser, UNION_DAY_PVPLIVENESS);
}
	
int CPvpSettleResult::calcMMR(int myMMR, int otherMMR, int k, int result)
{
	float w = 0;
	float l = 0;
	int mmrChange = 0;
	//期望系数
	float fWinK = 1.0f, fDrawK = 0.5f, fLoseK = 0;
	//最大是4, 最小是-4
	float fPower = _PSRMAX(_PSRMIN(4.0f, (otherMMR - myMMR) / 3000.0f), -4.0f);
	//期望
	float e = 1.0f / (1 + pow(10.0f, fPower));
	int myDan = CCommonHelper::getUserMMRDan(myMMR);
	int otherDan = CCommonHelper::getUserMMRDan(otherMMR);
	float MMR_Kx = CCommonHelper::getDanMMR_Kx(myDan);
	float MMR_K = CCommonHelper::getDanMMR_K(myDan);

	w = myDan < otherDan ? 1 + ((otherDan - myDan) / 2.0f) : 1;
	l = myDan > otherDan ? 1 + ((myDan - otherDan) / 2.0f) : 1;

	if (k >= 0)
	{
		switch (result)
		{
		case -1:
			//平局
			mmrChange = (int)ceil(w *(fDrawK - e) * MMR_K * MMR_Kx);
			break;
		case 0:
			//失败
			mmrChange = (int)ceil(l *(fLoseK - e) * MMR_K * MMR_Kx);
			break;
		case 1:
			//胜利
			mmrChange = (int)ceil(w *(fWinK - e) * MMR_K * pow(MMR_Kx, abs(k)));
			break;
		}
	}
	else
	{
		switch (result)
		{
		case -1:
			//平局
			mmrChange = (int)ceil(w *(fDrawK - e) * MMR_K * MMR_Kx);
			break;
		case 0:
			//失败
			mmrChange = (int)ceil(l *(fLoseK - e) * MMR_K * pow(MMR_Kx, abs(k)));
			break;
		case 1:
			//胜利
			mmrChange = (int)ceil(w *(fWinK - e) * MMR_K * MMR_Kx);
			break;
		}
	}
	return _PSRMAX(_PSRMIN(mmrChange, 400), -400);
}

int CPvpSettleResult::calcIntegral(int myIntegral, int otherIntegral, int result)
{
	float w = 0;
	float l = 0;
	int integralChange = 0;
	//期望系数
	float fWinK = 1.0f, fDrawK = 0.5f, fLoseK = 0;
	//最大是4, 最小是-4
	float fPower = _PSRMAX(_PSRMIN(4.0f, (otherIntegral - myIntegral) / 3000.0f), -4.0f);
	//期望
	float e = 1.0f / (1 + pow(10.0f, fPower));
	int myDan = CCommonHelper::getUserIntegralDan(myIntegral);
	int otherDan = CCommonHelper::getUserIntegralDan(otherIntegral);
	float Arean_K = CCommonHelper::getDanArena_K(myDan);

	w = myDan < otherDan ? 1 + ((otherDan - myDan) / 2.0f) : 1;
	l = myDan > otherDan ? 1 + ((myDan - otherDan) / 2.0f) : 1;

	switch (result)
	{
	case -1:
		//平局
		integralChange = (int)ceil(w * (fDrawK - e)*Arean_K);
		break;
	case 0:
		//失败
		integralChange = (int)ceil(l * (fLoseK - e)*Arean_K);
		break;
	case 1:
		//胜利
		integralChange = (int)ceil(w * (fWinK - e)*Arean_K);
		break;
	}

	return _PSRMAX(_PSRMIN(integralChange, 100), -100);
}

void CPvpSettleResult::setWinnerPvpModel(int roomType, int uid, int changeMMR, int changeInte, int newRank)
{
	CPvpModel *model = dynamic_cast<CPvpModel*>(CModelHelper::getModel(uid, MODELTYPE_PVP));
	CPvpChestModel *pPvpChestModel = dynamic_cast<CPvpChestModel*>(CModelHelper::getModel(uid, MODELTYPE_PVPCHEST));
	if (NULL == model || NULL == pPvpChestModel)
	{
		return;
	}

	std::map<int, int> mapValues;
	int nActionID = ELA_PVP;
	model->GetPvpField(PVPCOMM_FD_DAYWINTIMES, mapValues[PVPCOMM_FD_DAYWINTIMES], true);
	model->GetPvpField(PVPCOMM_FD_DAYCONTINUSWINTIMES, mapValues[PVPCOMM_FD_DAYCONTINUSWINTIMES], true);
	model->GetPvpField(PVPCOMM_FD_DAYMAXCONITNUSWINTIMES, mapValues[PVPCOMM_FD_DAYMAXCONITNUSWINTIMES], true);
	model->GetPvpField(PVPCOMM_FD_DAYBATTLETIMES, mapValues[PVPCOMM_FD_DAYBATTLETIMES], true);

	if (roomType == PVPROOMTYPE_PVP)
	{
		//公平竞技
		model->GetPvpField(PVP_FD_MMR, mapValues[PVP_FD_MMR]);
		model->GetPvpField(PVP_FD_INTEGRAL, mapValues[PVP_FD_INTEGRAL]);
		model->GetPvpField(PVP_FD_CONTINUSWINTIMES, mapValues[PVP_FD_CONTINUSWINTIMES]);
		model->GetPvpField(PVP_FD_DANCONTINUSWINTIMES, mapValues[PVP_FD_DANCONTINUSWINTIMES]);
		model->GetPvpField(PVP_FD_HISTORYHIGHESTRANK, mapValues[PVP_FD_HISTORYHIGHESTRANK]);
		model->GetPvpField(PVP_FD_HISTORYHIGHESTINTEGRAL, mapValues[PVP_FD_HISTORYHIGHESTINTEGRAL]);
		model->GetPvpField(PVP_FD_TOTALWINTIMES, mapValues[PVP_FD_TOTALWINTIMES]);
		model->GetPvpField(PVP_FD_BATTLETIMES, mapValues[PVP_FD_BATTLETIMES]);
		model->GetPvpField(PVP_FD_HISTORYCONTINUSWINTIMES, mapValues[PVP_FD_HISTORYCONTINUSWINTIMES]);
		//MMR
		int beforeDan = CCommonHelper::getUserMMRDan(mapValues[PVP_FD_MMR]);
		mapValues[PVP_FD_MMR] += changeMMR;
		int afterDan = CCommonHelper::getUserMMRDan(mapValues[PVP_FD_MMR]);
		//段位连续胜场
		mapValues[PVP_FD_DANCONTINUSWINTIMES] = afterDan > beforeDan ?
			0 : mapValues[PVP_FD_DANCONTINUSWINTIMES] + 1;
		//正常连续胜场
		mapValues[PVP_FD_CONTINUSWINTIMES] = mapValues[PVP_FD_CONTINUSWINTIMES] < 0 ?
			0 : mapValues[PVP_FD_CONTINUSWINTIMES] + 1;
		//历史最高连胜场数
		if (mapValues[PVP_FD_CONTINUSWINTIMES] > mapValues[PVP_FD_HISTORYCONTINUSWINTIMES])
		{
			mapValues[PVP_FD_HISTORYCONTINUSWINTIMES] = mapValues[PVP_FD_CONTINUSWINTIMES];
		}
		//总战斗次数
		mapValues[PVP_FD_BATTLETIMES] += 1;
		//竞技积分
		mapValues[PVP_FD_INTEGRAL] += changeInte;
		//历史最高排名
		if (newRank > 0 && (mapValues[PVP_FD_HISTORYHIGHESTRANK] > newRank
			|| mapValues[PVP_FD_HISTORYHIGHESTRANK] == 0))
		{
			mapValues[PVP_FD_HISTORYHIGHESTRANK] = newRank;
		}
		//历史最高积分
		if (mapValues[PVP_FD_HISTORYHIGHESTINTEGRAL] < mapValues[PVP_FD_INTEGRAL])
		{
			mapValues[PVP_FD_HISTORYHIGHESTINTEGRAL] = mapValues[PVP_FD_INTEGRAL];
		}
		//总胜场次数
		mapValues[PVP_FD_TOTALWINTIMES] += 1;
	}
	else if (roomType == PVPROOMTYPE_CHAMPIONSHIP)
	{
		//锦标赛
		model->GetPvpField(CPN_FD_MMR, mapValues[CPN_FD_MMR]);
		model->GetPvpField(CPN_FD_INTEGRAL, mapValues[CPN_FD_INTEGRAL]);
		model->GetPvpField(CPN_FD_CONTINUSWINTIMES, mapValues[CPN_FD_CONTINUSWINTIMES]);
		model->GetPvpField(CPN_FD_DANCONTINUSWINTIMES, mapValues[CPN_FD_DANCONTINUSWINTIMES]);
		model->GetPvpField(CPN_FD_HISTORYHIGHESTRANK, mapValues[CPN_FD_HISTORYHIGHESTRANK]);
		model->GetPvpField(CPN_FD_HISTORYHIGHESTINTEGRAL, mapValues[CPN_FD_HISTORYHIGHESTINTEGRAL]);
		model->GetPvpField(CPN_FD_TOTALWINTIMES, mapValues[CPN_FD_TOTALWINTIMES]);
		model->GetPvpField(CPN_FD_BATTLETIMES, mapValues[CPN_FD_BATTLETIMES]);
		model->GetPvpField(CPN_FD_HISTORYCONTINUSWINTIMES, mapValues[CPN_FD_HISTORYCONTINUSWINTIMES]);
		//MMR
		int beforeDan = CCommonHelper::getUserMMRDan(mapValues[CPN_FD_MMR]);
		mapValues[CPN_FD_MMR] += changeMMR;
		int afterDan = CCommonHelper::getUserMMRDan(mapValues[CPN_FD_MMR]);
		//段位连续胜场
		mapValues[CPN_FD_DANCONTINUSWINTIMES] = afterDan > beforeDan ?
			0 : mapValues[CPN_FD_DANCONTINUSWINTIMES] + 1;
		//正常连续胜场
		mapValues[CPN_FD_CONTINUSWINTIMES] = mapValues[CPN_FD_CONTINUSWINTIMES] < 0 ?
			0 : mapValues[CPN_FD_CONTINUSWINTIMES] + 1;
		//历史最高连胜场数
		if (mapValues[CPN_FD_CONTINUSWINTIMES] > mapValues[CPN_FD_HISTORYCONTINUSWINTIMES])
		{
			mapValues[CPN_FD_HISTORYCONTINUSWINTIMES] = mapValues[CPN_FD_CONTINUSWINTIMES];
		}
		//总战斗次数
		mapValues[CPN_FD_BATTLETIMES] += 1;
		//竞技积分
		mapValues[CPN_FD_INTEGRAL] += changeInte;
		//历史最高排名
		if (newRank > 0 && (mapValues[CPN_FD_HISTORYHIGHESTRANK] > newRank
			|| mapValues[CPN_FD_HISTORYHIGHESTRANK] == 0))
		{
			mapValues[CPN_FD_HISTORYHIGHESTRANK] = newRank;
		}
		//历史最高积分
		if (mapValues[CPN_FD_HISTORYHIGHESTINTEGRAL] < mapValues[CPN_FD_INTEGRAL])
		{
			mapValues[CPN_FD_HISTORYHIGHESTINTEGRAL] = mapValues[CPN_FD_INTEGRAL];
		}
		//总胜场次数
		mapValues[CPN_FD_TOTALWINTIMES] += 1;
		nActionID = ELA_CHAMPION;
	}
	//最近一场游戏时间戳
	mapValues[PVPCOMM_FD_BATTLESTAMP] = (int)time(NULL);
	//日胜场
	mapValues[PVPCOMM_FD_DAYWINTIMES] += 1;
	//日连续胜场
	mapValues[PVPCOMM_FD_DAYCONTINUSWINTIMES] += 1;
	//日最高连胜
	if (mapValues[PVPCOMM_FD_DAYMAXCONITNUSWINTIMES] < mapValues[PVPCOMM_FD_DAYCONTINUSWINTIMES])
	{
		mapValues[PVPCOMM_FD_DAYMAXCONITNUSWINTIMES] = mapValues[PVPCOMM_FD_DAYCONTINUSWINTIMES];
	}
	//日战斗场数
	mapValues[PVPCOMM_FD_DAYBATTLETIMES] += 1;
	//不为空则设置可领取
	if (!CModelHelper::isPvpChestEmpty(uid))
	{
		mapValues[PVPCOMM_FD_CHESTSTATUS] = 1;
	}

	if (!model->SetPvpField(mapValues))
	{
		KXLOGERROR("set pvp field error! %s", __FUNCTION__);
	}

	//发送PVP结束事件
	ActionCommonInfo CommInfo;
	CommInfo.param1 = EPVP_WIN_BATTLE_FLAG;
	CommInfo.param2 = changeInte;
	CModelHelper::DispatchActionEvent(uid, nActionID, &CommInfo, sizeof(ActionCommonInfo));
}
	
void CPvpSettleResult::setLoserPvpModel(int roomType, int uid, int changeMMR, int changeInte, int newRank)
{
	CPvpModel *model = dynamic_cast<CPvpModel*>(CModelHelper::getModel(uid, MODELTYPE_PVP));
	if (NULL == model)
	{
		return;
	}

	std::map<int, int> mapValues;
	int nActionID = ELA_PVP;
	model->GetPvpField(PVPCOMM_FD_DAYWINTIMES, mapValues[PVPCOMM_FD_DAYWINTIMES], true);
	model->GetPvpField(PVPCOMM_FD_DAYCONTINUSWINTIMES, mapValues[PVPCOMM_FD_DAYCONTINUSWINTIMES], true);
	model->GetPvpField(PVPCOMM_FD_DAYMAXCONITNUSWINTIMES, mapValues[PVPCOMM_FD_DAYMAXCONITNUSWINTIMES], true);
	model->GetPvpField(PVPCOMM_FD_DAYBATTLETIMES, mapValues[PVPCOMM_FD_DAYBATTLETIMES], true);

	if (roomType == PVPROOMTYPE_PVP)
	{
		model->GetPvpField(PVP_FD_MMR, mapValues[PVP_FD_MMR]);
		model->GetPvpField(PVP_FD_INTEGRAL, mapValues[PVP_FD_INTEGRAL]);
		model->GetPvpField(PVP_FD_CONTINUSWINTIMES, mapValues[PVP_FD_CONTINUSWINTIMES]);
		model->GetPvpField(PVP_FD_DANCONTINUSWINTIMES, mapValues[PVP_FD_DANCONTINUSWINTIMES]);

		model->GetPvpField(PVP_FD_HISTORYHIGHESTRANK, mapValues[PVP_FD_HISTORYHIGHESTRANK]);
		model->GetPvpField(PVP_FD_HISTORYHIGHESTINTEGRAL, mapValues[PVP_FD_HISTORYHIGHESTINTEGRAL]);
		model->GetPvpField(PVP_FD_BATTLETIMES, mapValues[PVP_FD_BATTLETIMES]);

		//MMR, 此时公式算出的changeMMR为负数
		mapValues[PVP_FD_MMR] += changeMMR;
		if (mapValues[PVP_FD_MMR] < 0)
		{
			mapValues[PVP_FD_MMR] = 0;
		}
		//竞技积分, 此时公式算出的changeInte为负数
		mapValues[PVP_FD_INTEGRAL] += changeInte;
		if (mapValues[PVP_FD_INTEGRAL] < 0)
		{
			mapValues[PVP_FD_INTEGRAL] = 0;
		}
		//段位连续胜场
		mapValues[PVP_FD_DANCONTINUSWINTIMES] = mapValues[PVP_FD_DANCONTINUSWINTIMES] > 0 ?
			0 : mapValues[PVP_FD_DANCONTINUSWINTIMES] - 1;
		//正常连续胜场
		mapValues[PVP_FD_CONTINUSWINTIMES] = mapValues[PVP_FD_CONTINUSWINTIMES] > 0 ?
			0 : mapValues[PVP_FD_CONTINUSWINTIMES] - 1;
		//总战斗次数
		mapValues[PVP_FD_BATTLETIMES] += 1;
	}
	else if (roomType == PVPROOMTYPE_CHAMPIONSHIP)
	{
		model->GetPvpField(CPN_FD_MMR, mapValues[CPN_FD_MMR]);
		model->GetPvpField(CPN_FD_INTEGRAL, mapValues[CPN_FD_INTEGRAL]);
		model->GetPvpField(CPN_FD_CONTINUSWINTIMES, mapValues[CPN_FD_CONTINUSWINTIMES]);
		model->GetPvpField(CPN_FD_DANCONTINUSWINTIMES, mapValues[CPN_FD_DANCONTINUSWINTIMES]);

		model->GetPvpField(CPN_FD_HISTORYHIGHESTRANK, mapValues[CPN_FD_HISTORYHIGHESTRANK]);
		model->GetPvpField(CPN_FD_HISTORYHIGHESTINTEGRAL, mapValues[CPN_FD_HISTORYHIGHESTINTEGRAL]);
		model->GetPvpField(CPN_FD_BATTLETIMES, mapValues[CPN_FD_BATTLETIMES]);

		//MMR, 此时公式算出的changeMMR为负数
		mapValues[CPN_FD_MMR] += changeMMR;
		if (mapValues[CPN_FD_MMR] < 0)
		{
			mapValues[CPN_FD_MMR] = 0;
		}
		//竞技积分, 此时公式算出的changeInte为负数
		mapValues[CPN_FD_INTEGRAL] += changeInte;
		if (mapValues[CPN_FD_INTEGRAL] < 0)
		{
			mapValues[CPN_FD_INTEGRAL] = 0;
		}
		//段位连续胜场
		mapValues[CPN_FD_DANCONTINUSWINTIMES] = mapValues[CPN_FD_DANCONTINUSWINTIMES] > 0 ?
			0 : mapValues[CPN_FD_DANCONTINUSWINTIMES] - 1;
		//正常连续胜场
		mapValues[CPN_FD_CONTINUSWINTIMES] = mapValues[CPN_FD_CONTINUSWINTIMES] > 0 ?
			0 : mapValues[CPN_FD_CONTINUSWINTIMES] - 1;
		//总战斗次数
		mapValues[CPN_FD_BATTLETIMES] += 1;
		nActionID = ELA_CHAMPION;
	}

	//最近一场游戏时间戳
	mapValues[PVPCOMM_FD_BATTLESTAMP] = (int)time(NULL);
	//日连续胜场
	mapValues[PVPCOMM_FD_DAYCONTINUSWINTIMES] = 0;
	//日战斗场数
	mapValues[PVPCOMM_FD_DAYBATTLETIMES] += 1;

	if (!model->SetPvpField(mapValues))
	{
		KXLOGERROR("set pvp field error! %s", __FUNCTION__);
	}

	//发送PVP结束事件
	ActionCommonInfo CommInfo;
	CommInfo.param1 = EPVP_FAILED_BATTLE_FLAG;
	CommInfo.param2 = changeInte;
	CModelHelper::DispatchActionEvent(uid, nActionID, &CommInfo, sizeof(ActionCommonInfo));
}

void CPvpSettleResult::setDrawPvpModel(int roomType, int uid, int changeMMR, int changeInte, int newRank)
{
	CPvpModel *model = dynamic_cast<CPvpModel*>(CModelHelper::getModel(uid, MODELTYPE_PVP));
	if (NULL == model)
	{
		return;
	}

	std::map<int, int> mapValues;
	int nActionID = ELA_PVP;
	model->GetPvpField(PVPCOMM_FD_DAYWINTIMES, mapValues[PVPCOMM_FD_DAYWINTIMES]);
	model->GetPvpField(PVPCOMM_FD_DAYBATTLETIMES, mapValues[PVPCOMM_FD_DAYBATTLETIMES]);

	if (roomType == PVPROOMTYPE_PVP)
	{
		//公平竞技
		model->GetPvpField(PVP_FD_MMR, mapValues[PVP_FD_MMR]);
		model->GetPvpField(PVP_FD_INTEGRAL, mapValues[PVP_FD_INTEGRAL]);
		model->GetPvpField(PVP_FD_HISTORYHIGHESTRANK, mapValues[PVP_FD_HISTORYHIGHESTRANK]);
		model->GetPvpField(PVP_FD_HISTORYHIGHESTINTEGRAL, mapValues[PVP_FD_HISTORYHIGHESTINTEGRAL]);
		model->GetPvpField(PVP_FD_TOTALWINTIMES, mapValues[PVP_FD_TOTALWINTIMES]);
		model->GetPvpField(PVP_FD_BATTLETIMES, mapValues[PVP_FD_BATTLETIMES]);
		//积分计算
		mapValues[PVP_FD_MMR] += changeMMR;
		mapValues[PVP_FD_INTEGRAL] += changeInte;
		//历史最高积分
		if (mapValues[PVP_FD_INTEGRAL] > mapValues[PVP_FD_HISTORYHIGHESTINTEGRAL])
		{
			mapValues[PVP_FD_HISTORYHIGHESTINTEGRAL] = mapValues[PVP_FD_INTEGRAL];
		}
		//正常连续胜场
		mapValues[PVP_FD_DANCONTINUSWINTIMES] = 0;
		mapValues[PVP_FD_CONTINUSWINTIMES] = 0;
		//总战斗次数
		mapValues[PVP_FD_BATTLETIMES] += 1;
		//历史最高排名
		if (newRank > 0 && (mapValues[PVP_FD_HISTORYHIGHESTRANK] > newRank
			|| mapValues[PVP_FD_HISTORYHIGHESTRANK] == 0))
		{
			mapValues[PVP_FD_HISTORYHIGHESTRANK] = newRank;
		}
	}
	else if (roomType == PVPROOMTYPE_CHAMPIONSHIP)
	{
		//锦标赛
		model->GetPvpField(CPN_FD_MMR, mapValues[CPN_FD_MMR]);
		model->GetPvpField(CPN_FD_INTEGRAL, mapValues[CPN_FD_INTEGRAL]);
		model->GetPvpField(CPN_FD_HISTORYHIGHESTRANK, mapValues[CPN_FD_HISTORYHIGHESTRANK]);
		model->GetPvpField(CPN_FD_HISTORYHIGHESTINTEGRAL, mapValues[CPN_FD_HISTORYHIGHESTINTEGRAL]);
		model->GetPvpField(CPN_FD_TOTALWINTIMES, mapValues[CPN_FD_TOTALWINTIMES]);
		model->GetPvpField(CPN_FD_BATTLETIMES, mapValues[CPN_FD_BATTLETIMES]);
		//积分计算
		mapValues[CPN_FD_MMR] += changeMMR;
		mapValues[CPN_FD_INTEGRAL] += changeInte;
		//历史最高积分
		if (mapValues[CPN_FD_INTEGRAL] > mapValues[CPN_FD_HISTORYHIGHESTINTEGRAL])
		{
			mapValues[CPN_FD_HISTORYHIGHESTINTEGRAL] = mapValues[CPN_FD_INTEGRAL];
		}
		//正常连续胜场
		mapValues[CPN_FD_DANCONTINUSWINTIMES] = 0;
		mapValues[CPN_FD_CONTINUSWINTIMES] = 0;
		//总战斗次数
		mapValues[CPN_FD_BATTLETIMES] += 1;
		//历史最高排名
		if (newRank > 0 && (mapValues[CPN_FD_HISTORYHIGHESTRANK] > newRank
			|| mapValues[CPN_FD_HISTORYHIGHESTRANK] == 0))
		{
			mapValues[CPN_FD_HISTORYHIGHESTRANK] = newRank;
		}
		nActionID = ELA_CHAMPION;
	}
	//最近一场游戏时间戳
	mapValues[PVPCOMM_FD_BATTLESTAMP] = (int)time(NULL);
	//日战斗场数
	mapValues[PVPCOMM_FD_DAYBATTLETIMES] += 1;
	//日连续胜场
	mapValues[PVPCOMM_FD_DAYCONTINUSWINTIMES] = 0;

	if (!model->SetPvpField(mapValues))
	{
		KXLOGERROR("set pvp field error! %s", __FUNCTION__);
	}
	//发送PVP结束事件
	ActionCommonInfo CommInfo;
	CommInfo.param1 = EPVP_DRAW_BATTLE_FLAG;
	CommInfo.param2 = changeInte;
	CModelHelper::DispatchActionEvent(model->GetUid(), nActionID, &CommInfo, sizeof(ActionCommonInfo));
}

void CPvpSettleResult::overFairPvp(int winner, int loser, int result, int winerCryLv, int loserCryLv, int dt)
{
	CPvpRankModel *pPvpRankModel = CPvpRankModel::getInstance();
	// 平局也有winner loser, 只是结算用, 不代表具体含义
	CPvpModel *pWinnerPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(winner, MODELTYPE_PVP));
	CPvpModel *pLoserPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(loser, MODELTYPE_PVP));
	// pvp setting
	CConfPvpSetting *pPvpSetting = dynamic_cast<CConfPvpSetting*>(
		CConfManager::getInstance()->getConf(CONF_PVP_SETTING));

	//定级赛数值与分差
	int nWinnerGNum = 0, nWinnerGDval = 0;
	//各自的MMR
	int nWinnerMMR = 0, nLoserMMR = 0;
	//积分
	int nWinnerIntegral = 0, nLoserIntegral = 0;
	//变化系数(连胜次数)
	int nKW = 0, nKL = 0;

	pWinnerPvpModel->GetPvpField(PVP_FD_MMR, nWinnerMMR);
	pWinnerPvpModel->GetPvpField(PVP_FD_INTEGRAL, nWinnerIntegral);
	pWinnerPvpModel->GetPvpField(PVP_FD_DANCONTINUSWINTIMES, nKW);

	pLoserPvpModel->GetPvpField(PVP_FD_MMR, nLoserMMR);
	pLoserPvpModel->GetPvpField(PVP_FD_INTEGRAL, nLoserIntegral);
	pLoserPvpModel->GetPvpField(PVP_FD_DANCONTINUSWINTIMES, nKL);

	//计算MMR
	int nWChangeMMR = 0;
	int nLChangeMMR = 0;
	int nWChangeIntegral = 0;
	int nLChangeIntegral = 0;
	if (result == CHALLENGE_DRAW)
	{
		nWChangeMMR = calcMMR(nWinnerMMR, nLoserMMR, nKW, -1);
		nLChangeMMR = calcMMR(nLoserMMR, nWinnerMMR, nKL, -1);
	}
	else
	{
		//计算MMR
		nWChangeMMR = calcMMR(nWinnerMMR, nLoserMMR, nKW, 1);
		nLChangeMMR = calcMMR(nLoserMMR, nWinnerMMR, nKL, 0);
		//计算积分
		nWChangeIntegral = calcIntegral(nWinnerIntegral, nLoserIntegral, 1);
		//积分大于一定积分才扣除
		if (nLoserIntegral > pPvpSetting->m_PvpSetting.CriticalPoint)
		{
			nLChangeIntegral = calcIntegral(nLoserIntegral, nWinnerIntegral, 0);
		}
	}

	//获得各自排名, 计算各自排名差值
	int nRankNow = 0;
	std::vector<DropItemInfo> vecDropItems;
	//胜利者战斗结束现有多少积分
	nWinnerIntegral += nWChangeIntegral;
	if (nWinnerIntegral < 0)
	{
		nWinnerIntegral = 0;
	}

	pPvpRankModel->AddPvpRank(winner, nWinnerIntegral);
	pPvpRankModel->GetPvpRank(winner, nRankNow);

	if (result == CHALLENGE_DRAW)
	{
		setDrawPvpModel(m_nRoomType, winner, nWChangeMMR, nWChangeIntegral, nRankNow);
	}
	else
	{
		//设置pvp胜利者玩家数据
		setWinnerPvpModel(m_nRoomType, winner, nWChangeMMR, nWChangeIntegral, nRankNow);

		int winnerDropId = CModelHelper::getWinDropId(winner);
		if (winnerDropId > 0)
		{
			CItemDrop::Drop(winnerDropId, vecDropItems);
			CPropUseHelper::getInstance()->AddItems(winner, vecDropItems);
		}
	}

	int nLSendLen = sizeof(PvpResultSC);
	char *pLoserData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nLSendLen));
	int nWSendLen = sizeof(PvpResultSC)+sizeof(DropItemInfo)*vecDropItems.size();
	char *pWinnerData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nWSendLen));
	//成功者的数据
	PvpResultSC * pResultSC = reinterpret_cast<PvpResultSC*>(pWinnerData);
	pResultSC->roomType = m_nRoomType;
	pResultSC->result = result == CHALLENGE_DRAW ? CHALLENGE_DRAW : CHALLENGE_WON;
	pResultSC->rankNow = nRankNow;
    pResultSC->integral = nWinnerIntegral;
	pResultSC->itemCount = vecDropItems.size();
	
	DropItemInfo *item = reinterpret_cast<DropItemInfo*>(pResultSC + 1);
	for (std::vector<DropItemInfo>::iterator iter = vecDropItems.begin();
		iter != vecDropItems.end(); ++iter)
	{
		memcpy(item, &(*iter), sizeof(DropItemInfo));
		iter += 1;
	}
    // 可以先判断玩家是否离线
    //CBattleDataCache::getInstance()->pushBackBattleData(winner, *pResultSC);
	//失败者排名的数据
	nRankNow = 0;
	// 失败者结算后有多少积分
	nLoserIntegral += nLChangeIntegral;
	if (nLoserIntegral < 0)
	{
		nLoserIntegral = 0;
	}

	pPvpRankModel->AddPvpRank(loser, nLoserIntegral);
	pPvpRankModel->GetPvpRank(loser, nRankNow);

	if (result == CHALLENGE_DRAW)
	{
		setDrawPvpModel(m_nRoomType, loser, nLChangeMMR, nLChangeIntegral, nRankNow);
	}
	else
	{
		//设置pvp失败者玩家数据
		setLoserPvpModel(m_nRoomType, loser, nLChangeMMR, nLChangeIntegral, nRankNow);
	}

	pResultSC = reinterpret_cast<PvpResultSC*>(pLoserData);
	pResultSC->roomType = m_nRoomType;
	pResultSC->result = result == CHALLENGE_DRAW ? CHALLENGE_DRAW : CHALLENGE_FAILED;
	pResultSC->rankNow = nRankNow;
    pResultSC->integral = nLoserIntegral;
	pResultSC->itemCount = 0;
    // 可以先判断玩家是否离线
    //CBattleDataCache::getInstance()->pushBackBattleData(loser, *pResultSC);

	//发送给玩家
	CKxCommManager::getInstance()->sendData(winner, CMD_PVP, CMD_PVP_RESULT_SC, pWinnerData, nWSendLen);
	CKxCommManager::getInstance()->sendData(loser, CMD_PVP, CMD_PVP_RESULT_SC, pLoserData, nLSendLen);
	//回收内存
	KxServer::kxMemMgrRecycle(pWinnerData, nWSendLen);
	KxServer::kxMemMgrRecycle(pLoserData, nLSendLen);

	//统计信息
	CTeamModel *pWinTeamModel = dynamic_cast<CTeamModel*>(CModelHelper::getModel(winner, MODELTYPE_TEAM));
	CHECK_RETURN_VOID(pWinTeamModel != NULL);

	TeamInfo WinTeam;
	if (!pWinTeamModel->GetTeamInfo(ETT_SPORTE, WinTeam))
	{
		return;
	}

	CTeamModel *pLostTeamModel = dynamic_cast<CTeamModel*>(CModelHelper::getModel(loser, MODELTYPE_TEAM));
	CHECK_RETURN_VOID(pLostTeamModel != NULL);

	TeamInfo LostTeam;
	if (!pLostTeamModel->GetTeamInfo(ETT_SPORTE, LostTeam))
	{
		return;
	}

	//CCommOssHelper::PvpLogicOss(winner, true, nWinnerIntegral, LostTeam.summonerId, m_nRoomType,winerCryLv,dt);
	//CCommOssHelper::PvpLogicOss(loser, false, nLoserIntegral, WinTeam.summonerId,m_nRoomType,loserCryLv,dt);
}

void CPvpSettleResult::overChampionPvp(int winner, int loser, int result, int winerCryLv, int loserCryLv, int dt)
{	//先刷新两个玩家数据
	CModelHelper::updatePvpInfo(winner);
	CModelHelper::updatePvpInfo(loser);

	CPvpRankModel *pPvpRankModel = CPvpRankModel::getInstance();
	// 平局也有winner loser, 只是结算用, 不代表具体含义
	CPvpModel *pWinnerPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(winner, MODELTYPE_PVP));
	CPvpModel *pLoserPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(loser, MODELTYPE_PVP));

	CConfPvpSetting *pPvpSetting = dynamic_cast<CConfPvpSetting*>(
		CConfManager::getInstance()->getConf(CONF_PVP_SETTING));

	CConfPvpGradingChange *pPvpGChangeConf = dynamic_cast<CConfPvpGradingChange*>(
		CConfManager::getInstance()->getConf(CONF_PVP_GRADING_CHANGE));

	//定级赛数值与分差
	int nWinnerGNum = 0, nWinnerGDval = 0;
	//定级赛数值与分差
	int nLoserGNum = 0, nLoserGDval = 0;
	//各自的MMR
	int nWinnerMMR = 0, nLoserMMR = 0;
	//积分
	int nWinnerIntegral = 0, nLoserIntegral = 0;
	//变化系数(连胜次数)
	int nKW = 0, nKL = 0;

	pWinnerPvpModel->GetPvpField(CPN_FD_MMR, nWinnerMMR);
	pWinnerPvpModel->GetPvpField(CPN_FD_INTEGRAL, nWinnerIntegral);
	pWinnerPvpModel->GetPvpField(CPN_FD_DANCONTINUSWINTIMES, nKW);

	pLoserPvpModel->GetPvpField(CPN_FD_MMR, nLoserMMR);
	pLoserPvpModel->GetPvpField(CPN_FD_INTEGRAL, nLoserIntegral);
	pLoserPvpModel->GetPvpField(CPN_FD_DANCONTINUSWINTIMES, nKL);

	//计算MMR
	int nWChangeMMR = 0;
	int nLChangeMMR = 0;
	int nWChangeIntegral = 0;
	int nLChangeIntegral = 0;
	if (result == CHALLENGE_DRAW)
	{
		nWChangeMMR = calcMMR(nWinnerMMR, nLoserMMR, nKW, -1);
		nLChangeMMR = calcMMR(nLoserMMR, nWinnerMMR, nKL, -1);
	}
	else
	{
		//计算MMR
		nWChangeMMR = calcMMR(nWinnerMMR, nLoserMMR, nKW, 1);
		nLChangeMMR = calcMMR(nLoserMMR, nWinnerMMR, nKL, 0);
		//计算积分
		nWChangeIntegral = calcIntegral(nWinnerIntegral, nLoserIntegral, 1);
		nLChangeIntegral = calcIntegral(nLoserIntegral, nWinnerIntegral, 0);
	}

	//获得各自排名, 计算各自排名差值
	int nRankNow = 0;
	// 如果定级赛没结束
	pWinnerPvpModel->GetPvpField(CPN_FD_GRADINGNUM, nWinnerGNum);
	pWinnerPvpModel->GetPvpField(CPN_FD_GRADINGDVAL, nWinnerGDval);
	if (nWinnerGNum < pPvpSetting->m_PvpSetting.GradingNum)
	{
		nWinnerGNum += 1;
		nWinnerGDval += nWChangeMMR;
		pWinnerPvpModel->SetPvpField(CPN_FD_GRADINGNUM, nWinnerGNum);
		pWinnerPvpModel->SetPvpField(CPN_FD_GRADINGDVAL, nWinnerGDval);
		// 定级赛结束
		if (nWinnerGNum == pPvpSetting->m_PvpSetting.GradingNum)
		{
			std::vector<PvpGradingChange> &changeConf = pPvpGChangeConf->GetGradingChange();
			for (std::vector<PvpGradingChange>::iterator iter = changeConf.begin();
				iter != changeConf.end(); ++iter)
			{
				if (nWinnerGDval > iter->ChangeMMRLower
					&& nWinnerGDval < iter->ChangeMMRUpper)
				{
					nWChangeIntegral += iter->ChangePoint;
				}
			}
		}
	}
	//胜利者战斗结束现有多少积分
	nWinnerIntegral += nWChangeIntegral;
	if (nWinnerIntegral < 0)
	{
		nWinnerIntegral = 0;
	}
	// 5场以上的为定级赛之后
	if (nWinnerGNum >= pPvpSetting->m_PvpSetting.GradingNum)
	{
		//加入排行
		pPvpRankModel->addChampionRank(winner, nWinnerIntegral);
		pPvpRankModel->getChampionRank(winner, nRankNow);
	}

	if (result == CHALLENGE_DRAW)
	{
		setDrawPvpModel(m_nRoomType, winner, nWChangeMMR, nWChangeIntegral, nRankNow);
	}
	else
	{
		//设置pvp胜利者玩家数据
		setWinnerPvpModel(m_nRoomType, winner, nWChangeMMR, nWChangeIntegral, nRankNow);
	}

	int nSendLen = sizeof(PvpResultSC);
	char *pWinnerData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nSendLen));
	char *pLoserData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nSendLen));

	//成功者的数据
	PvpResultSC * pResultSC = reinterpret_cast<PvpResultSC*>(pWinnerData);
	pResultSC->roomType = m_nRoomType;
	pResultSC->result = result == CHALLENGE_DRAW ? CHALLENGE_DRAW : CHALLENGE_WON;
	pResultSC->rankNow = nRankNow;
	pResultSC->integral = nWinnerIntegral;
	pResultSC->itemCount = 0;
    // 可以先判断玩家是否离线
    //CBattleDataCache::getInstance()->pushBackBattleData(winner, *pResultSC);

	//失败者排名的数据
	nRankNow = 0;
	// 如果定级赛没结束
	pLoserPvpModel->GetPvpField(CPN_FD_GRADINGNUM, nLoserGNum);
	pLoserPvpModel->GetPvpField(CPN_FD_GRADINGDVAL, nLoserGDval);
	if (nLoserGNum < pPvpSetting->m_PvpSetting.GradingNum)
	{
		nLoserGNum += 1;
		nLoserGDval += nLChangeMMR;
		pLoserPvpModel->SetPvpField(CPN_FD_GRADINGNUM, nLoserGNum);
		pLoserPvpModel->SetPvpField(CPN_FD_GRADINGDVAL, nLoserGDval);
		// 定级赛结束
		if (nLoserGNum == pPvpSetting->m_PvpSetting.GradingNum)
		{
			std::vector<PvpGradingChange> &changeConf = pPvpGChangeConf->GetGradingChange();
			for (std::vector<PvpGradingChange>::iterator iter = changeConf.begin();
				iter != changeConf.end(); ++iter)
			{
				if (nLoserGDval > iter->ChangeMMRLower
					&& nLoserGDval < iter->ChangeMMRUpper)
				{
					nLChangeIntegral += iter->ChangePoint;
				}
			}
		}
	}
	// 失败者结算后有多少积分
	nLoserIntegral += nLChangeIntegral;
	if (nLoserIntegral < 0)
	{
		nLoserIntegral = 0;
	}
	// 5场以上的为定级赛之后
	if (nLoserGNum >= pPvpSetting->m_PvpSetting.GradingNum)
	{
		//加入排行
		pPvpRankModel->addChampionRank(loser, nLoserIntegral);
		pPvpRankModel->getChampionRank(loser, nRankNow);
	}

	if (result == CHALLENGE_DRAW)
	{
		setDrawPvpModel(m_nRoomType, loser, nLChangeMMR, nLChangeIntegral, nRankNow);
	}
	else
	{
		//设置pvp失败者玩家数据
		setLoserPvpModel(m_nRoomType, loser, nLChangeMMR, nLChangeIntegral, nRankNow);
	}

	pResultSC = reinterpret_cast<PvpResultSC*>(pLoserData);
	pResultSC->roomType = m_nRoomType;
	pResultSC->result = result == CHALLENGE_DRAW ? CHALLENGE_DRAW : CHALLENGE_FAILED;
	pResultSC->rankNow = nRankNow;
    pResultSC->integral = nLoserIntegral;
	pResultSC->itemCount = 0;
    // 可以先判断玩家是否离线
    //CBattleDataCache::getInstance()->pushBackBattleData(loser, *pResultSC);
	//发送给玩家
	CKxCommManager::getInstance()->sendData(winner, CMD_PVP, CMD_PVP_RESULT_SC, pWinnerData, nSendLen);
	CKxCommManager::getInstance()->sendData(loser, CMD_PVP, CMD_PVP_RESULT_SC, pLoserData, nSendLen);
	//回收内存
	KxServer::kxMemMgrRecycle(pWinnerData, nSendLen);
	KxServer::kxMemMgrRecycle(pLoserData, nSendLen);

	//统计信息
	CTeamModel *pWinTeamModel = dynamic_cast<CTeamModel*>(CModelHelper::getModel(winner, MODELTYPE_TEAM));
	CHECK_RETURN_VOID(pWinTeamModel != NULL);

	TeamInfo WinTeam;
	if (!pWinTeamModel->GetTeamInfo(ETT_SPORTE, WinTeam))
	{
		return;
	}

	CTeamModel *pLostTeamModel = dynamic_cast<CTeamModel*>(CModelHelper::getModel(loser, MODELTYPE_TEAM));
	CHECK_RETURN_VOID(pLostTeamModel != NULL);

	TeamInfo LostTeam;
	if (!pLostTeamModel->GetTeamInfo(ETT_SPORTE, LostTeam))
	{
		return;
	}

	//CCommOssHelper::PvpLogicOss(winner, true, nWinnerIntegral, LostTeam.summonerId, m_nRoomType, winerCryLv, dt);
	//CCommOssHelper::PvpLogicOss(loser, false, nLoserIntegral, WinTeam.summonerId, m_nRoomType, loserCryLv, dt);
}
