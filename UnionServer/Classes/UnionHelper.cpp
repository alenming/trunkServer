#include "UnionHelper.h"
#include "GameUserManager.h"
#include "ConfUnion.h"
#include "KxCommManager.h"
#include "RandGenerator.h"
#include "KxCSComm.h"
#include "UnionExpiditionModel.h"
#include "UnionModel.h"
#include "UnionManager.h"
#include "UnionProtocol.h"
#include "LoginProtocol.h"
#include "Protocol.h"
#include "ModelHelper.h"
#include "ModelDef.h"
#include <set>
#include <algorithm>
#include "ErrorCodeProtocol.h"
#include "MemoryOperation.h"
#include "RankModel.h"
#include "ChallengeHelper.h"
#include "ItemDrop.h"
#include "NoticeHelper.h"
#include "KxCommInterfaces.h"

using namespace std;

//对世界生成随机地图
int CUnionHelper::getRandomMapID(const SExpeditonWorldItem *pExpiditionWorldData)
{
	CHECK_RETURN_INT(pExpiditionWorldData != NULL);
	const vector<SMapWeight> &VectMap = pExpiditionWorldData->vecMap;
	//计算总权值
	int nWeight = 0;
	for (int i = 0; i < VectMap.size(); i++)
	{
		nWeight += VectMap[i].weight;
	}

	int nRandom = g_RandGenerator.MakeRandNum(0, nWeight);
	nWeight = 0;
	for (int i = 0; i < VectMap.size(); i++)
	{
		if (nRandom >= nWeight)
		{
			return VectMap[i].mapId;
		}
		nWeight += VectMap[i].weight;
	}

	return 0;
}

//校验地图序列是否合法
bool CUnionHelper::checkIndexData(CUnionExpiditionModel *pModel, int nIndex, int &nStageID, int &nStageLv)
{
	CHECK_RETURN(pModel != NULL);
	int nCurMapID = 0;
	CHECK_RETURN(pModel->getExpiditionFieldValue(UNION_EXPIDITION_MAPID, nCurMapID));
    CHECK_RETURN(nIndex > 0 && nIndex <= MAX_MAP_INDEX);
	SExpeditionMapData *pMapData = queryConfExpiditionMapData(nCurMapID);
	CHECK_RETURN(pMapData != NULL);
	//检测序列是否已经完成
	set<int>& SetIndex = pModel->getAllFinishIndex();
	set<int>::iterator iter = SetIndex.find(nIndex);
	CHECK_RETURN(iter == SetIndex.end());
	//检测关卡是不是初始序列ID
	VecInt::iterator ator;
	ator = find(pMapData->vecStartStages.begin(), pMapData->vecStartStages.end(), nIndex);
	if (ator != pMapData->vecStartStages.end())
	{
		nStageID = pMapData->stageList[nIndex - 1].nStageID;
		nStageLv = pMapData->stageList[nIndex - 1].nStageLv;
		return true;
	}
	else
	{
		for (int i = 0; i < MAX_MAP_INDEX; i++)
		{
			SExpeditonIndexData &IndexData = pMapData->stageList[i];
			CHECK_CONTINUE(IndexData.nStageID != 0);
			iter = SetIndex.find(i+1);
			CHECK_CONTINUE(iter != SetIndex.end());
			VecInt::iterator ator = find(IndexData.unlockIndexs.begin(), IndexData.unlockIndexs.end(),nIndex);
			if (ator != IndexData.unlockIndexs.end())
			{
				nStageID = pMapData->stageList[nIndex - 1].nStageID;
				nStageLv = pMapData->stageList[nIndex - 1].nStageLv;
				return true;
			}
		}
	}

	return false;
}

//获取角色的驻站BUFF
bool CUnionHelper::getExpiditionBuff(CUnionExpiditionModel *pModel, std::vector<BuffData> &Buff)
{
	CHECK_RETURN(pModel != NULL);
	int nCurMapID = 0;
	CHECK_RETURN(pModel->getExpiditionFieldValue(UNION_EXPIDITION_MAPID, nCurMapID));
	SExpeditionMapData *pMapData = queryConfExpiditionMapData(nCurMapID);
	CHECK_RETURN(pMapData != NULL);
	Buff.clear();
	//获取所有通关关卡
	set<int>& SetIndex = pModel->getAllFinishIndex();
	for (set<int>::iterator ator = SetIndex.begin(); ator != SetIndex.end(); ator++)
	{
		int nIndex = *ator;
		BuffData Data;
		Data.BuffId = pMapData->stageList[nIndex - 1].StageMapBuff[1];
		Data.Target = pMapData->stageList[nIndex - 1].StageMapBuff[0];
		Data.BuffLv = pMapData->stageList[nIndex - 1].StageMapBuff[2];
		Buff.push_back(Data);
	}

	return true;
}

//征战结束设定
bool CUnionHelper::finishExpiditionWar(CUnionExpiditionModel *pModel, bool bWin)
{
	CHECK_RETURN(pModel != NULL);
	//远征是否结束
	int nCurTime = time(NULL);
	int nTmpTime = 0;
	int nWorldID = 0;
	CHECK_RETURN(pModel->getExpiditionFieldValue(UNION_EXPIDITION_FINISHTIME, nTmpTime));
	CHECK_RETURN(pModel->getExpiditionFieldValue(UNION_EXPIDITION_WORLDID, nWorldID));
	const SExpeditonWorldItem *pExpiditionWorldData = queryConfExpiditionWordData(nWorldID);
	CHECK_RETURN(pExpiditionWorldData != NULL);
	bool bFinish = false;
	//如果不是打败最终BOSS
	if (!bWin)
	{
		//如果结束，计算冷却时间,不能进入挑战
		if (nTmpTime < nCurTime)
		{
			nTmpTime = nTmpTime + pExpiditionWorldData->nColdTime;
			CHECK_RETURN(pModel->setExpiditinoFieldVale(UNION_EXPIDITION_COLDFINISHTIME, nTmpTime));
			bFinish = true;
		}
	}
	else
	{
		nTmpTime = nCurTime + pExpiditionWorldData->nColdTime;
		CHECK_RETURN(pModel->setExpiditinoFieldVale(UNION_EXPIDITION_COLDFINISHTIME, nTmpTime));
		CHECK_RETURN(pModel->setExpiditinoFieldVale(UNION_EXPIDITION_FINISHFLAG, 1));
		CHECK_RETURN(pModel->setExpiditinoFieldVale(UNION_EXPIDITION_FINISHTIME, nCurTime));
		bFinish = true;
	}

	if (bFinish)
	{
		CHECK_RETURN(pModel->setExpiditinoFieldVale(UNION_EXPIDITION_WORLDID, 0));
		CHECK_RETURN(pModel->setExpiditinoFieldVale(UNION_EXPIDITION_MAPID, 0));
		pModel->clearCurExpiditionBossHp();
		pModel->clearExpiditionFinishIndex();
		pModel->clearStageIndexShowData();
		return true;
	}

	return false;
}

//刷新公会商店商品
bool CUnionHelper::freshUnionShopData(CUnionShopModel *pModel, int nUnionDropID)
{
	CHECK_RETURN(pModel != NULL);
	std::map<int,int> MapDropShop;
	CItemDrop::UnionShopDrop(nUnionDropID, MapDropShop);

	if (MapDropShop.size() != 0)
	{
		pModel->setUnionShop(MapDropShop);
	}

	return true;
}

bool CUnionHelper::addUnionLiveness(int unionId, int liveness)
{
    CUnionModel *pUnionModel = CUnionManager::getInstane()->getUnionModel(unionId);
    if (NULL == pUnionModel)
    {
        KXLOGERROR("can't find union, unionId %d", unionId);
        return false;
    }

    int nTodayLiveness = 0, nUnionLv = 0, nReputation = 0;
    pUnionModel->GetUnionFieldVal(UNION_FIELD_LIVENESS, nTodayLiveness);
    pUnionModel->GetUnionFieldVal(UNION_FIELD_LV, nUnionLv);
    pUnionModel->GetUnionFieldVal(UNION_FIELD_REPUTATION, nReputation);

    const UnionLevelItem* pUnionLevelItem = queryConfUnionLevel(nUnionLv);
    CHECK_RETURN(NULL != pUnionLevelItem);
    CHECK_RETURN(liveness > 0);

    int nNewTodayLiveness = nTodayLiveness + liveness;
    std::map<int, int> mapInfo;
    mapInfo[UNION_FIELD_LIVENESS] = nNewTodayLiveness;

    // 活跃度直接转换为声望
    nReputation += liveness;
    mapInfo[UNION_FIELD_REPUTATION] = nReputation;

    // 活跃福利
    if (nNewTodayLiveness >= pUnionLevelItem->ActiveReward)
    {
        bool bBoradLiveness = false;
        bool bBoradSLiveness = false;
        if (nTodayLiveness < pUnionLevelItem->ActiveReward)
        {
            // 广播活跃宝箱福利
            bBoradLiveness = true;
        }

        if (nNewTodayLiveness >= pUnionLevelItem->ActiveSReward)
        {
            if (nTodayLiveness < pUnionLevelItem->ActiveSReward)
            {
                // 广播超级活跃宝箱福利
                bBoradSLiveness = true;
            }
        }

        if (bBoradSLiveness || bBoradLiveness)
        {
            CUnionHelper::sendMessageTips(pUnionModel, UNION_MESSAGE_LIVENESS);
        }
    }

    // 处理连续升级的问题
    mapInfo[UNION_FIELD_LV] = calcUnionLv(nUnionLv, nReputation);

    if (!pUnionModel->SetUnionFieldVals(mapInfo))
    {
        return false;
    }

    if (mapInfo[UNION_FIELD_LV] != nUnionLv)
    {
        CRankModel::getInstance()->AddRankData(UNION_RANK_TYPE, unionId, mapInfo[UNION_FIELD_LV]);
    }

    return true;
}

bool CUnionHelper::addUnionReputation(int unionId, int reputation)
{
    CUnionModel *pUnionModel = CUnionManager::getInstane()->getUnionModel(unionId);
    if (NULL == pUnionModel)
    {
        KXLOGERROR("can't find union, unionId %d", unionId);
        return false;
    }

    int nUnionLv = 0, nReputation = 0;
    pUnionModel->GetUnionFieldVal(UNION_FIELD_LV, nUnionLv);
    pUnionModel->GetUnionFieldVal(UNION_FIELD_REPUTATION, nReputation);

    nReputation += reputation;

    std::map<int, int> mapInfo;
    mapInfo[UNION_FIELD_REPUTATION] = nReputation;
    mapInfo[UNION_FIELD_LV] = calcUnionLv(nUnionLv, nReputation);

    if (!pUnionModel->SetUnionFieldVals(mapInfo))
    {
        KXLOGERROR("SetUnionFieldVals fail, unionId %d", unionId);
        return false;
    }

    if (mapInfo[UNION_FIELD_LV] != nUnionLv)
    {
        CRankModel::getInstance()->AddRankData(UNION_RANK_TYPE, unionId, mapInfo[UNION_FIELD_LV]);
    }

    return true;
}

int CUnionHelper::calcUnionLv(int curLv, int curReputation)
{
    int newCurLv = curLv;
    const UnionLevelItem* pUnionLevelItem = queryConfUnionLevel(curLv);
    // 连续升级
    while (pUnionLevelItem && curReputation >= pUnionLevelItem->UpLevelCost)
    {
        curLv += 1;
        pUnionLevelItem = queryConfUnionLevel(curLv);
        if (NULL != pUnionLevelItem)
        {
            newCurLv = curLv;
        }
    }

    return newCurLv;
}

int CUnionHelper::checkCreate(int uid, std::string unionName)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    if (NULL == pGameUser)
    {
        return UNION_EC_FAILD;
    }

    CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
    CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
	CHECK_RETURN_INT(pUserModel != NULL);
	CHECK_RETURN_INT(pUserUnionModel != NULL);

    // 没有加入或创建公会&等级达到
    int nUserLv = 0, nUnionID = 0, nApplyStamp = 0, nGold = 0;
    if (!pUserModel->GetUserFieldVal(USR_FD_GOLD, nGold, true)
        || !pUserModel->GetUserFieldVal(USR_FD_USERLV, nUserLv, true)
        || !pUserUnionModel->getUserUnionValue(USER_UNION_ID, nUnionID)
        || !pUserUnionModel->getUserUnionValue(USER_UNION_APPLYUNIONSTAMP, nApplyStamp))
    {
        return UNION_EC_FAILD;
    }

    if (nUnionID > 0)
    {
        return UNION_EC_OWN_UNION;
    }

    if (nApplyStamp > 0 && nApplyStamp > int(time(NULL)))
    {
        return UNION_EC_APPLY_TIMESTAMP;
    }

    const UnionItem &unionSetting = queryConfUnion();
    if (nUserLv < unionSetting.UnLockLv)
    {
        return UNION_EC_NOENOUGH_LV;
    }

    if (nGold < unionSetting.CostCoin)
    {
        return UNION_EC_NOENOUGH_MONEY;
    }

    return checkUnionName(unionName);
}

int CUnionHelper::checkUnionName(std::string unionName)
{
    if ("" == unionName)
    {
        return UNION_EC_NAME_LEGAL;
    }

    const map<int, CUnion*> &mapAllUnion = CUnionManager::getInstane()->getAllUnion();
    map<int, CUnion*>::const_iterator iter = mapAllUnion.begin();
    for (; iter != mapAllUnion.end(); iter++)
    {
        CUnion* pUnion = iter->second;
        if (NULL != pUnion)
        {
            if (pUnion->getUnionName() == unionName)
            {
                return UNION_EC_NAME_REPEAT;
            }
        }
    }

    return UNION_EC_SUCCESS;
}

EUnionErrorCode CUnionHelper::checkApply(int uid, int unionId)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    if (NULL == pGameUser)
    {
        return UNION_EC_FAILD;
    }

    CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
    CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
    CUnionModel *pUnionModel = CUnionManager::getInstane()->getUnionModel(unionId);
    if (NULL == pUserModel || NULL == pUserModel || NULL == pUserModel)
    {
        return UNION_EC_FAILD;
    }

    const UnionItem &unionSetting = queryConfUnion();
    int nLimitLv = 0, nUserLv = 0, nUnionID = 0, nApplyStamp = 0, nUnionLv = 0, nApplyCount = 0;
    if (!pUserModel->GetUserFieldVal(USR_FD_USERLV, nUserLv)
        || !pUserUnionModel->getUserUnionValue(USER_UNION_ID, nUnionID)
        || !pUserUnionModel->getUserUnionValue(USER_UNION_APPLYUNIONSTAMP, nApplyStamp)
        || !pUserUnionModel->getUserUnionValue(USER_UNION_DAYAPPLAYCOUNT, nApplyCount)
        || !pUnionModel->GetUnionFieldVal(UNION_FIELD_LIMITLV, nLimitLv)
        || !pUnionModel->GetUnionFieldVal(UNION_FIELD_LV, nUnionLv))
    {
        return UNION_EC_FAILD;
    }

    const UnionLevelItem *pUnionItem = queryConfUnionLevel(nUnionLv);
    if (NULL == pUnionItem)
    {
        KXLOGERROR("unionlevel config can't find unionlv %d", nUnionLv);
        return UNION_EC_FAILD;
    }

    // 满员
    if ((int)(pUnionModel->GetMembers().size()) >= pUnionItem->MemberLimit)
    {
        return UNION_EC_MEMBER_FULL;
    }

    if (nUserLv < nLimitLv)
    {
        return UNION_EC_NOENOUGH_LV;
    }

    if (nUnionID > 0)
    {
        return UNION_EC_OWN_UNION;
    }

    int nCurTime = int(time(NULL));
    if (nApplyStamp > 0 && nCurTime < nApplyStamp)
    {
        return UNION_EC_APPLY_TIMESTAMP;
    }

    // 是否重复申请
    map<int, int> mapApplyInfo = pUserUnionModel->getApplyInfo();
    map<int, int>::iterator iter = mapApplyInfo.begin();
    for (; iter != mapApplyInfo.end(); iter++)
    {
        if (iter->second < nCurTime)
        {
            pUserUnionModel->removeApplyInfo(iter->first);
            pUnionModel->RemoveAudit(uid);
            continue;
        }

        if (iter->first == unionId)
        {
            return UNION_EC_APPLY_SAME;
        }
    }

    // 申请次数
    if (nApplyCount >= unionSetting.ApplyCount)
    {
        return UNION_EC_NOENOUGH_APPLYCOUNT;
    }

    return UNION_EC_SUCCESS;
}

CUnion *CUnionHelper::getUserUnion(int uid)
{
	CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(CModelHelper::getModel(uid, MODELTYPE_USERUNION));
	if (NULL == pUserUnionModel)
	{
		KXLOGERROR("%d get union model error when getUserUnion!", uid);
		return NULL;
	}
	int unionId = 0;
	if (!pUserUnionModel->getUserUnionValue(USER_UNION_ID, unionId))
	{
		KXLOGERROR("%d get union value error when getUserUnion!", uid);
		return NULL;
	}

	return CUnionManager::getInstane()->getUnion(unionId);
}

bool CUnionHelper::canKick(int initiativePos, int passivePos)
{
    // 会长可以踢所有成员,副会长只能踢副会长以下的会员
    if (initiativePos >= UNION_POS_CHAIRMAN)
    {
        return true;
    }
    else if (initiativePos >= UNION_POS_VICE_CHAIRMAN)
    {
        if (passivePos < UNION_POS_VICE_CHAIRMAN)
        {
            return true;
        }
    }

    return false;
}

bool CUnionHelper::canRelieve(int initiativePos, int passivePos)
{
    // 会长撤小于会长大于普通成员的任
    if (initiativePos >= UNION_POS_CHAIRMAN)
    {
        if (passivePos > UNION_POS_NORMAL
            && passivePos < UNION_POS_CHAIRMAN)
        {
            return true;
        }
    }

    return false;
}

bool CUnionHelper::canTransfer(int initiativePos, int passivePos)
{
    // 会长可以转权利给任何人,副会长只能转给普通成员
    if (initiativePos >= UNION_POS_CHAIRMAN)
    {
        return true;
    }
    else if (initiativePos >= UNION_POS_VICE_CHAIRMAN)
    {
        if (passivePos < UNION_POS_VICE_CHAIRMAN)
        {
            return true;
        }
    }

    return false;
}

bool CUnionHelper::canAppoint(int initiativePos, int passivePos)
{
    // 会长把普通会员任命为副会长
    if (initiativePos >= UNION_POS_CHAIRMAN)
    {
        if (passivePos < UNION_POS_VICE_CHAIRMAN)
        {
            return true;
        }
    }

    return false;
}

void CUnionHelper::searchUnion(std::string searchName, list<UnionListInfo> &searchList)
{
    // 获取公会信息
    UnionListInfo info;
    // 1、能转换为Int先查找公会ID
    int nUnionID = atoi(searchName.c_str());
    if (nUnionID > 0)
    {
        CUnion *pUnion = CUnionManager::getInstane()->getUnion(nUnionID);
        if (NULL != pUnion)
        {
            CUnionModel *pUnionModel = pUnion->getUnionModel();
            if (pUnionModel)
            {
                fillUnionInfo(pUnionModel, info);

                searchList.push_back(info);
            }
        }
    }

    // 2、搜索公会名称
    map<int, CUnion*> mapAllUnion = CUnionManager::getInstane()->getAllUnion();
    map<int, CUnion*>::iterator iter = mapAllUnion.begin();
    for (; iter != mapAllUnion.end(); iter++)
    {
        if (iter->first != nUnionID)
        {
            CUnion *pUnion = iter->second;
            if (pUnion)
            {
                CUnionModel *pUnionModel = pUnion->getUnionModel();
                std::string unionName = pUnionModel->GetUnionName();
                if (string::npos != unionName.find(searchName))
                {
                    fillUnionInfo(pUnionModel, info);

                    searchList.push_back(info);
                }
            }
        }
    }
}

void CUnionHelper::fillUnionInfo(CUnionModel *pUnionModel, UnionListInfo &info)
{
	CHECK_RETURN_VOID(pUnionModel != NULL);
    memset(&info, 0, sizeof(UnionListInfo));
    info.unionID = pUnionModel->GetUnionID();
    info.memberCount = pUnionModel->GetMembers().size();

    pUnionModel->GetUnionFieldVal(UNION_FIELD_LV, info.unionLv);
    pUnionModel->GetUnionFieldVal(UNION_FIELD_LIMITLV, info.limitLv);
    pUnionModel->GetUnionFieldVal(UNION_FIELD_EMBLEM, info.emblem);

    int nChairmanId = 0;
    pUnionModel->GetUnionFieldVal(UNION_FIELD_CHAIRMAN, nChairmanId);

    UserBasicInfo userInfo;
    if (!CModelHelper::getUserInfo(nChairmanId, userInfo))
    {
        KXLOGERROR("getUserInfo is fail, userid %d", nChairmanId);
    }

    std::string unionName = pUnionModel->GetUnionName();
    std::string notice = pUnionModel->GetUnionNotice();
    strncpys(info.unionName, unionName.c_str(), strlen(unionName.c_str()));
    strncpys(info.chairmanName, userInfo.name.c_str(), strlen(userInfo.name.c_str()));
    strncpys(info.notice, notice.c_str(), strlen(notice.c_str()));
    info.identity = userInfo.identity;
}

int CUnionHelper::quitUnion(int uid)
{
    int nStamp = -1;
    KxServer::IKxComm * pKxComm = CKxCommManager::getInstance()->getKxComm(uid);
    if (NULL != pKxComm)
    {
        CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
        CHECK_RETURN_INT(NULL != pGameUser);
        CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
        CPersonMercenaryModel* pPersonMercenaryModel = dynamic_cast<CPersonMercenaryModel *>(pGameUser->getModel(MODELTYPE_MERCENARY));
        CHECK_RETURN_INT(NULL != pUserUnionModel && NULL != pPersonMercenaryModel);

        int nUnionID = 0;
        CHECK_RETURN(pUserUnionModel->getUserUnionValue(USER_UNION_ID, nUnionID));
        CUnionModel *pUnionModel = CUnionManager::getInstane()->getUnionModel(nUnionID);
        CHECK_RETURN_INT(NULL != pUnionModel);
        CHECK_RETURN_INT(pUnionModel->RemoveMember(uid));
		
        CHECK_RETURN_INT(pUserUnionModel->setUserUnionValue(USER_UNION_ID, 0));
        nStamp = int(time(NULL)) + queryConfUnion().ApplyCD * 60;
        CHECK_RETURN_INT(pUserUnionModel->setUserUnionValue(USER_UNION_APPLYUNIONSTAMP, nStamp));

        //清除公会佣兵信息
		CUnion* pUnion = CUnionManager::getInstane()->getUnion(nUnionID);
		if (NULL != pUnion)
		{
			map<int, int> mapPersonMercenaryInfo = pPersonMercenaryModel->GetPersonDispatchMercenaryInfo();
			for (map<int, int>::iterator mapit = mapPersonMercenaryInfo.begin(); mapit != mapPersonMercenaryInfo.end(); ++mapit)
			{
				//发送给前端通知召回
				CMemoryOperation cMemOp(sizeof(int)* 5);
				cMemOp.WriteData(UNION_MESSAGE_MERCENARYRecall);
				cMemOp.WriteData(mapit->second);
				cMemOp.WriteData(0);
				cMemOp.WriteData(uid);
				cMemOp.WriteData(mapit->first);
				if (pUnion->getMercenaryModel()->RecallMercenary(mapit->second))
				{
					vector<int> vUnionMembers;
					pUnion->getUnionModel()->GetMembers(vUnionMembers);
					CKxCommManager::getInstance()->broadcast(vUnionMembers,
						MakeCommand(CMD_UNION, CMD_UNION_MESSAGE_SC),
						cMemOp.GetBuffer(), cMemOp.GetDataLenth());
				}
			}
		}

        //清除个人佣兵信息
		pPersonMercenaryModel->ClearAllDispatchedMercenary();
		pPersonMercenaryModel->ClearAllEmployedMercenaryInfo();
        // 公会竞技积分扣除

    }
    else
    {
        int nUnionID = 0;
        CHECK_RETURN(CUserUnionModel::getOffLineUserUnionValue(uid, USER_UNION_ID, nUnionID));
        CUnionModel *pUnionModel = CUnionManager::getInstane()->getUnionModel(nUnionID);
        CHECK_RETURN_INT(NULL != pUnionModel);
        CHECK_RETURN_INT(pUnionModel->RemoveMember(uid));

		//清除公会佣兵信息
		CUnion* pUnion = CUnionManager::getInstane()->getUnion(nUnionID);
		if (NULL != pUnion)
		{
			map<int, int> mapPersonMercenaryInfo;
			CPersonMercenaryModel::SetOffRemoveMercenaryInfo(uid, mapPersonMercenaryInfo);
			for (map<int, int>::iterator mapit = mapPersonMercenaryInfo.begin(); mapit != mapPersonMercenaryInfo.end(); ++mapit)
			{
				//发送给前端通知召回
				CMemoryOperation cMemOp(sizeof(int)* 5);
				cMemOp.WriteData(UNION_MESSAGE_MERCENARYRecall);
				cMemOp.WriteData(mapit->second);
				cMemOp.WriteData(0);
				cMemOp.WriteData(uid);
				cMemOp.WriteData(mapit->first);
				if (pUnion->getMercenaryModel()->RecallMercenary(mapit->second))
				{
					vector<int> vUnionMembers;
					pUnion->getUnionModel()->GetMembers(vUnionMembers);
					CKxCommManager::getInstance()->broadcast(vUnionMembers,
						MakeCommand(CMD_UNION, CMD_UNION_MESSAGE_SC),
						cMemOp.GetBuffer(), cMemOp.GetDataLenth());
				}
			}
		}

        CHECK_RETURN_INT(CUserUnionModel::setOffLineUserUnionValue(uid, USER_UNION_ID, 0));
        nStamp = int(time(NULL)) + queryConfUnion().ApplyCD * 60;
        CHECK_RETURN_INT(CUserUnionModel::setOffLineUserUnionValue(uid, USER_UNION_APPLYUNIONSTAMP, nStamp));
    }

    return nStamp;
}

EUnionErrorCode CUnionHelper::auditUser(int nUnionID, int uid, bool agree)
{
    CUnionModel *pUnionModel = CUnionManager::getInstane()->getUnionModel(nUnionID);
    if (NULL == pUnionModel)
    {
        return UNION_EC_FAILD;
    }

    // 是否过了审核时间
    int nAuditTime = 0;
    if (!pUnionModel->GetAuditInfo(uid, nAuditTime))
    {
        return UNION_EC_ALREADY_DO;
    }

    pUnionModel->RemoveAudit(uid);
    if (int(time(NULL)) > nAuditTime)
    {
        return UNION_EC_AUDIT_OVERTIME;
    }

    string name = "";
    CGameUser *pJoinUser = CGameUserManager::getInstance()->getGameUser(uid);
    // 该玩家在线
    if (NULL != pJoinUser)
    {
        CUserModel *pJoinUserModel = dynamic_cast<CUserModel*>(pJoinUser->getModel(MODELTYPE_USER));
        CUserUnionModel *pJoinUserUnionModel = dynamic_cast<CUserUnionModel*>(pJoinUser->getModel(MODELTYPE_USERUNION));
        if (UNION_APPLY_AGREE == agree)
        {
            // 该玩家是否已经加入公会
            int nJoinUnionID = 0;
            if (pJoinUserUnionModel->getUserUnionValue(USER_UNION_ID, nJoinUnionID))
            {
                if (nJoinUnionID > 0)
                {
                    return UNION_EC_OWN_UNION;
                }

                // 加入公会列表
                if (!pJoinUserUnionModel->setUserUnionValue(USER_UNION_ID, nUnionID)
                    || !pUnionModel->AddMember(uid, UNION_POS_NORMAL))
                {
                    return UNION_EC_FAILD;
                }

                // 清掉公会审核信息
                pJoinUserUnionModel->removeAllApplyInfo();
            }
        }
        else
        {
            pJoinUserUnionModel->removeApplyInfo(nUnionID);
            return UNION_EC_REFUSE_AUDIT;
        }

        name = pJoinUserModel->GetName();
    }
    else
    {
        if (UNION_APPLY_AGREE == agree)
        {
            int nJoinUnionID = 0;
            if (!CUserUnionModel::getOffLineUserUnionValue(uid, USER_UNION_ID, nJoinUnionID))
            {
                return UNION_EC_FAILD;
            }

            if (nJoinUnionID > 0)
            {
                return UNION_EC_OWN_UNION;
            }

            if (!CUserUnionModel::setOffLineUserUnionValue(uid, USER_UNION_ID, nUnionID)
                || !pUnionModel->AddMember(uid, UNION_POS_NORMAL))
            {
                return UNION_EC_FAILD;
            }

            // 清掉公会审核信息
            if (!CUserUnionModel::removeOffLineAllApplyInfo(uid))
            {
                return UNION_EC_FAILD;
            }
        }
        else
        {
            if (!CUserUnionModel::removeOffLineApplyInfo(uid, nUnionID))
            {
                return UNION_EC_FAILD;
            }  

            return UNION_EC_REFUSE_AUDIT;
        }

        name = CModelHelper::getUserName(uid);
    }

    addUnionLog(nUnionID, UNION_FUNC_JOIN, name);
    return UNION_EC_PASS_AUDIT;
}

bool CUnionHelper::addUnionLog(int nUnionId, int eventType, std::string nameOne, std::string nameTwo /*= ""*/, int extend /*= 0*/)
{
    CUnionModel *pUnionModel = CUnionManager::getInstane()->getUnionModel(nUnionId);
    if (NULL == pUnionModel)
    {
        return false;
    }

    // 添加日志
    UnionLogInfo info;
    memset(&info, 0, sizeof(UnionLogInfo));
    info.timeStamp = time(NULL);
    info.eventType = eventType;
    info.extend = extend;
    strncpys(info.userName1, nameOne.c_str(), strlen(nameOne.c_str()));
    strncpys(info.userName2, nameTwo.c_str(), strlen(nameTwo.c_str()));

    return pUnionModel->AddLog(info);
}

int CUnionHelper::mercenaryPrize(UnionMercenaryInfo& info)
{
	CConfUnionMercenaryPrize *conf = dynamic_cast<CConfUnionMercenaryPrize *>(CConfManager::getInstance()->getConf(CONF_UNION_MERCENARY));
	CHECK_RETURN_INT(conf != NULL);
	SMercenaryPrize *pPrize = reinterpret_cast<SMercenaryPrize *>(conf->getData(info.stSoldierCardData.SoldierLv));
	CHECK_RETURN_INT(pPrize != NULL);
	
	int nEmployedTimes = info.nEmployedTimes;
	int nUnitTime = (time(NULL) - info.nDispatchTime)/60;
	if (info.nEmployedTimes > pPrize->MaxEmployed)
	{
		nEmployedTimes = pPrize->MaxEmployed;
	}
	if (nUnitTime>pPrize->MaxTime)
	{
		nUnitTime = pPrize->MaxTime;
	}

	int gold = nUnitTime / 15 * pPrize->TimeProfit + nEmployedTimes*pPrize->EmployedProfit;
	return gold;
}

void CUnionHelper::sendOwnUnionInfo(int uid, int unionId)
{
    OwnUnionInfo ownUnionInfo;
    if (!fillOwnUnionInfo(uid, unionId, ownUnionInfo))
    {
        KXLOGERROR("CUnionHelper::sendOwnUnionInfo fill own union info fail, uid %d, unionId %d", uid, unionId);
        return;
    }

    CKxCommManager::getInstance()->sendData(uid, CMD_UNION, CMD_UNION_BEAUDIT_SC,
        reinterpret_cast<char*>(&ownUnionInfo), sizeof(OwnUnionInfo));
	
	//下发公会商店数据
	CUnion *pUnion = CUnionManager::getInstane()->getUnion(unionId);
	CHECK_RETURN_VOID(pUnion != NULL);
	pUnion->sendUnionShopData(uid);
}

void CUnionHelper::sendBeFunctionInfo(int uid, int unionId, int funcType)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN_VOID(NULL != pGameUser);

    CUnionModel *pUnionModel = CUnionManager::getInstane()->getUnionModel(unionId);
    CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
    CHECK_RETURN_VOID(NULL != pUnionModel && NULL != pUserUnionModel);

    std::string unionName = pUnionModel->GetUnionName();

    UnionBeFunctionSC unionBeFunctionSC;
    unionBeFunctionSC.funcType = funcType;
    memset(unionBeFunctionSC.unionName, 0, sizeof(unionBeFunctionSC.unionName));
    strncpys(unionBeFunctionSC.unionName, unionName.c_str(), strlen(unionName.c_str()));
    CKxCommManager::getInstance()->sendData(uid, CMD_UNION, CMD_UNION_BEFUNCTION_SC,
        reinterpret_cast<char*>(&unionBeFunctionSC), sizeof(UnionBeFunctionSC));
}

void CUnionHelper::sendMessageTips(CUnionModel *pUnionModel, int tipsType)
{
    vector<int> members;
    pUnionModel->GetMembers(members);

    if (members.empty())
    {
        return;
    }

    UnionMessageSC unionMessageSC;
    unionMessageSC.type = tipsType;
    CKxCommManager::getInstance()->broadcast(members,
        MakeCommand(CMD_UNION, CMD_UNION_MESSAGE_SC),
        reinterpret_cast<char*>(&unionMessageSC), sizeof(UnionMessageSC));
}

CUnionModel *CUnionHelper::getUnionModel(int uid)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN_NULL(NULL != pGameUser);

    CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
    CHECK_RETURN_NULL(NULL != pUserUnionModel);

    int nUnionID = 0;
    CHECK_RETURN_NULL(pUserUnionModel->getUserUnionValue(USER_UNION_ID, nUnionID));

    return CUnionManager::getInstane()->getUnionModel(nUnionID);
}

bool CUnionHelper::fillOwnUnionInfo(int uid, int unionId, OwnUnionInfo &ownUnionInfo)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN(NULL != pGameUser);

    CUnionModel *pUnionModel = CUnionManager::getInstane()->getUnionModel(unionId);
	CUnion *pUnion = CUnionManager::getInstane()->getUnion(unionId);
	CHECK_RETURN(pUnion != NULL);
	CUnionExpiditionModel *pUnionExpiditionModel = pUnion->getExpiditionModel();
	CHECK_RETURN(pUnionExpiditionModel != NULL);
    CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
    CHECK_RETURN(NULL != pUnionModel && NULL != pUserUnionModel);

    CModelHelper::updateUserUnionInfo(uid);

    memset(&ownUnionInfo, 0, sizeof(OwnUnionInfo));
    ownUnionInfo.unionID = unionId;
    pUserUnionModel->getUserUnionValue(USER_UNION_DAYPVPLIVENESS, ownUnionInfo.todayPvpLiveness);
    pUserUnionModel->getUserUnionValue(USER_UNION_DAYSTAGELIVENESS, ownUnionInfo.todayStageLiveness);
    pUserUnionModel->getUserUnionValue(USER_UNION_CONTRIBUTION, ownUnionInfo.totalContribution);
    pUserUnionModel->getUserUnionValue(USER_UNION_WELFARE, ownUnionInfo.welfareTag);
    pUnionModel->GetUnionFieldVal(UNION_FIELD_LIVENESS, ownUnionInfo.unionLiveness);
    pUnionModel->GetUnionFieldVal(UNION_FIELD_LV, ownUnionInfo.unionLv);
    pUnionModel->GetUnionFieldVal(UNION_FIELD_ORIGINLV, ownUnionInfo.originUnionLv);
    pUnionModel->GetUnionFieldVal(UNION_FIELD_EMBLEM, ownUnionInfo.emblem);
    ownUnionInfo.pos = pUnionModel->GetMemberPos(uid);
    ownUnionInfo.hasAudit = pUnionModel->GetAudits().size() > 0 ? 1 : 0;
    ownUnionInfo.hasExpiditionReward = pUserUnionModel->hasExpiditionReward() ? 1 : 0;
	int nValue = 0;
	pUnionExpiditionModel->getExpiditionFieldValue(UNION_EXPIDITION_FINISHTIME, nValue);
	ownUnionInfo.nExFinishTime = nValue;
	pUnionExpiditionModel->getExpiditionFieldValue(UNION_EXPIDITION_COLDFINISHTIME, nValue);
	ownUnionInfo.nExRestFinishTime = nValue;
    std::string unionName = pUnionModel->GetUnionName();
    std::string unionNotice = pUnionModel->GetUnionNotice();
    strncpys(ownUnionInfo.unionName, unionName.c_str(), strlen(unionName.c_str()));
    strncpys(ownUnionInfo.notice, unionNotice.c_str(), strlen(unionNotice.c_str()));

    return true;
}

void CUnionHelper::noticeUnionMembers(int unionId, char *buff, int len)
{
    CUnionModel *pUnionModel = CUnionManager::getInstane()->getUnionModel(unionId);
    if (NULL == pUnionModel)
    {
        KXLOGERROR("can't find unionid %d", unionId);
        return;
    }

    std::vector<int> members;
    pUnionModel->GetMembers(members);

    if (!members.empty())
    {
        CKxCommManager::getInstance()->broadcast(members,
            MakeCommand(CMD_NOTICE, CMD_NOTICE_SC),
            buff,
            len);
    }
}

void CUnionHelper::noticeUnion(int unionId, int noticeId)
{
    int buffSize = sizeof(NoticeSC)+sizeof(NoticeInfo);
    char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(buffSize));

    NoticeSC *pNoticeSC = reinterpret_cast<NoticeSC*>(buff);
    pNoticeSC->noticeCount = 1;

    NoticeInfo *pNoticeInfo = reinterpret_cast<NoticeInfo*>(pNoticeSC + 1);
    pNoticeInfo->noticeId = noticeId;
    pNoticeInfo->paramCount = 0;

    noticeUnionMembers(unionId, buff, buffSize);
}

void CUnionHelper::noticeUnion(int unionId, int noticeId, std::string szVal)
{
    int buffSize = sizeof(NoticeSC)+
        CNoticeHelper::getStringInfoLen(szVal.length() + 1);
    char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(buffSize));

    NoticeSC *pNoticeSC = reinterpret_cast<NoticeSC*>(buff);
    pNoticeSC->noticeCount = 1;

    CNoticeHelper::getStringNoticeInfo(buff + sizeof(NoticeSC), noticeId, szVal);
    
    noticeUnionMembers(unionId, buff, buffSize);
    
    KxServer::kxMemMgrRecycle(buff, buffSize);
}

void CUnionHelper::noticeUnion(int unionId, int noticeId, std::string szValOne, std::string szValTwo)
{
    int buffSize = sizeof(NoticeSC)+
        CNoticeHelper::getStringStringInfoLen(szValOne.length() + 1, szValTwo.length() + 1);
    char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(buffSize));

    NoticeSC *pNoticeSC = reinterpret_cast<NoticeSC*>(buff);
    pNoticeSC->noticeCount = 1;

    do
    {
        CNoticeHelper::getStringStringNoticeInfo(buff + sizeof(NoticeSC), noticeId, szValOne, szValTwo);

        noticeUnionMembers(unionId, buff, buffSize);

    } while (false);

    KxServer::kxMemMgrRecycle(buff, buffSize);
}
