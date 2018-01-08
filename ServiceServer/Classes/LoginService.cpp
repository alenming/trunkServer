#include "LoginService.h"
#include "KxMemPool.h"
#include <string.h>

#include "Protocol.h"
#include "LoginProtocol.h"
#include "ModelHelper.h"
#include "GameUserManager.h"
#include "GameDef.h"
#include "TimeCalcTool.h"

#include "GlobalMailModel.h"

#include "ConfGameSetting.h"
#include "ConfStage.h"
#include "KxCommManager.h"
#include "ItemDrop.h"
#include "RankModel.h"
#include "PvpRankModel.h"
#include "CommOssHelper.h"
#include "UserActiveModel.h"
#include "ConfActive.h"
#include "ActiveHelper.h"
#include "ChallengeRoomManager.h"
#include "CommonHelper.h"
#include "TimeCalcTool.h"
#include "TaskAchieveHelper.h"
#include "ServerProtocol.h"
#include "UnionShopModel.h"
#include "UnionExpiditionModel.h"

using namespace std;
using namespace KxServer;

void sendUserModel(int uid, CUserModel *model)
{
	int len = sizeof(LoginUserModelInfo);
	LoginUserModelInfo *userInfo = reinterpret_cast<LoginUserModelInfo*>(
		KxServer::kxMemMgrAlocate(len));
	int nValue = 0;
	memset(userInfo->name, 0, sizeof(userInfo->name));
    strcpy(userInfo->name, model->GetName().c_str());
	model->GetUserFieldVal(USR_FD_USERID, userInfo->userId);
	model->GetUserFieldVal(USR_FD_USERLV, nValue);
	userInfo->cuserLv = nValue;
	model->GetUserFieldVal(USR_FD_EXP, userInfo->userExp);
	model->GetUserFieldVal(USR_FD_GOLD, userInfo->gold);
	model->GetUserFieldVal(USR_FD_DIAMOND, userInfo->diamond);
	model->GetUserFieldVal(USR_FD_TOWERCOIN, userInfo->towerCoin);
	model->GetUserFieldVal(USR_FD_PVPCOIN, userInfo->pvpCoin);
	model->GetUserFieldVal(USR_FD_UNIONCONTRIB, userInfo->unionContrib);
	model->GetUserFieldVal(USR_FD_FLASHCARD10, userInfo->flashcard10);
	model->GetUserFieldVal(USR_FD_FLASHCARD, userInfo->flashcard);
	model->GetUserFieldVal(USR_FD_HEADICON, userInfo->headId);
	nValue = 0;
	model->GetUserFieldVal(USR_FD_BAGCAPACITY,nValue);
	userInfo->sbagCapacity = nValue;
	nValue = 0;
	model->GetUserFieldVal(USR_FD_FREEHEROTIMES,nValue);
	userInfo->cfreeHeroTimes = nValue;
	nValue = 0;
	model->GetUserFieldVal(USR_FD_FREERENAME,nValue);
	userInfo->changeNameFree = nValue;
	nValue = 0;
	model->GetUserFieldVal(USR_FD_BUYGOLDTIMES,nValue);
	userInfo->sbuyGoldTimes = nValue;
	model->GetUserFieldVal(USR_FD_PAYMENT, userInfo->payment);
	model->GetUserFieldVal(USR_FD_MONTHCARDSTAMP, userInfo->monthCardStamp);

	model->GetUserFieldVal(USR_FD_SIGNDAY, userInfo->nTotalSignDay);
	model->GetUserFieldVal(USR_FD_MONTHDAY, userInfo->nMonthSignDay);
	nValue = 0;
	model->GetUserFieldVal(USR_FD_DAYFLAG,nValue);
	userInfo->cDaySignFlag = nValue;
	model->GetUserFieldVal(USR_FD_INCSIGNFLAG, userInfo->nTotalSignFlag);
	nValue = 0;
	model->GetUserFieldVal(USR_FD_FIRSTPAYFLAG,nValue);
	userInfo->cFirstPayFlag = nValue;
	CUserActiveModel::getMonthCardValue(uid, FOND_PID, userInfo->nFondFlag);
	nValue = 0;
	model->GetUserFieldVal(USR_FD_QQVIPLV,nValue);
	userInfo->cBDLv = nValue;
	model->GetUserFieldVal(USR_FD_QQVIPTIMESTAMP, userInfo->nBDTimeStamp);
	nValue = 0;
	model->GetUserFieldVal(USR_FD_QQVIPTYPE,nValue);
	userInfo->cBDType = nValue;

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_USERMODEL_SC,
		reinterpret_cast<char *>(userInfo), len);

	KxServer::kxMemMgrRecycle(userInfo, len);
}

void sendBagModel(int uid, int capacity, CBagModel *model)
{
	std::map<int, int> &bagItems = model->GetItems();
	int len = sizeof(LoginBagModelInfo)+bagItems.size() * sizeof(BagItemInfo);
	LoginBagModelInfo *bagInfo = reinterpret_cast<LoginBagModelInfo*>(
		KxServer::kxMemMgrAlocate(len));
	bagInfo->scurCapacity = capacity;
	bagInfo->count = bagItems.size();
	// 位移
	BagItemInfo *item = reinterpret_cast<BagItemInfo*>(bagInfo + 1);
	for (std::map<int, int>::iterator iter = bagItems.begin();
		iter != bagItems.end(); ++iter)
	{
		item->id = iter->first;
		item->val = iter->second;
		++item;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_BAGMODEL_SC,
		reinterpret_cast<char *>(bagInfo), len);

	KxServer::kxMemMgrRecycle(bagInfo, len);
}

void sendEquipModel(int uid, CEquipModel *model)
{
	std::map<int, SEquipInfo> &equipsItems = model->GetEquipsInfo();
	int len = sizeof(LoginEquipModelInfo)+sizeof(EquipItemInfo)* equipsItems.size();

	LoginEquipModelInfo *equipInfo = reinterpret_cast<LoginEquipModelInfo*>(
		KxServer::kxMemMgrAlocate(len));

	equipInfo->count = equipsItems.size();
	EquipItemInfo *item = reinterpret_cast<EquipItemInfo*>(equipInfo + 1);
	for (std::map<int, SEquipInfo>::iterator iter = equipsItems.begin();
		iter != equipsItems.end(); ++iter)
	{
		memset(item, 0, sizeof(EquipItemInfo));
		item->equipId = iter->first;
		item->confId = iter->second.nItemID;
		item->cMainPropNum = iter->second.cMainPropNum;
		for (int i = 0; i < MAX_EQUIP_EFFECT_NUM; i++)
		{
			item->cEffectID[i] = iter->second.cEffectID[i];
			item->sEffectValue[i] = iter->second.sEffectValue[i];
		}
		++item;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_EQUIPMODEL_SC,
		reinterpret_cast<char *>(equipInfo), len);

	KxServer::kxMemMgrRecycle(equipInfo, len);
}

void sendSummonModel(int uid, CSummonModel *model)
{
	std::vector<int> summonlist = model->GetAllSummon();
	int len = sizeof(LoginSummonModelInfo)+summonlist.size()*sizeof(int);
	LoginSummonModelInfo *summonInfo = reinterpret_cast<LoginSummonModelInfo*>(
		KxServer::kxMemMgrAlocate(len));
	summonInfo->count = summonlist.size();

	int *summon = reinterpret_cast<int*>(summonInfo + 1);
	for (std::vector<int>::iterator iter = summonlist.begin();
		iter != summonlist.end(); ++iter)
	{
		*summon = *iter;
		++summon;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_SUMMONMODEL_SC,
		reinterpret_cast<char *>(summonInfo), len);

	KxServer::kxMemMgrRecycle(summonInfo, len);
}

void sendHeroModel(int uid, int heroCapacity, CHeroModel *model)
{
	std::map<int, DBHeroAttr>& heros = model->GetHeros();
	// 所有英雄信息
	int nSendLen = sizeof(LoginHeroModelInfo) + heros.size() * sizeof(LoginHeroInfo);
	char *pSendData = static_cast<char*>(KxServer::kxMemMgrAlocate(nSendLen));

	LoginHeroModelInfo *pHeros = reinterpret_cast<LoginHeroModelInfo*>(pSendData);
	pHeros->count = heros.size();

	LoginHeroInfo *pHeroAttr = reinterpret_cast<LoginHeroInfo *>(pHeros+1);
	for (std::map<int, DBHeroAttr>::iterator iter = heros.begin(); 
		iter != heros.end(); ++iter)
	{
		pHeroAttr->heroId = iter->first;
		pHeroAttr->heroFrag = iter->second.fragment;
		pHeroAttr->heroLv = iter->second.level;
		pHeroAttr->heroStar = iter->second.star;
		pHeroAttr->heroExp = iter->second.exp;
        memcpy(pHeroAttr->heroTalent, iter->second.talent, sizeof(pHeroAttr->heroTalent));
		// 下发时, 0代表Weapon, 前端需要注意一下
		for (int i = 0; i < 6; ++i)
		{
			pHeroAttr->equip[i] = iter->second.equipId[i+1];
		}
		// 下一个英雄
		pHeroAttr += 1;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_HEROMODEL_SC, pSendData, nSendLen);
	KxServer::kxMemMgrRecycle(pSendData, nSendLen);
}

void sendStageModel(int uid, CStageModel *model)
{
	// 数据库信息
	std::map<int, int>& stageStatus = model->GetStageStatusMap();
	std::map<int, int>& eliteStatus = model->GetEliteStatusMap();
	std::map<int, int>& chapterStatus = model->GetChapterStatusMap();
	std::map<int, int>& eliteChapterStatus = model->GetEliteChapterStatusMap();
	std::map<int, DBEliteStageInfo>& eliteRecordInfo = model->GetEliteStageInfo();
	std::map<int, std::map<int, int> >& chapterboxstatus = model->getAllChapterBoxMap();

	int nBoxLen = 0;
	for (std::map<int, std::map<int, int> >::iterator ator = chapterboxstatus.begin(); ator != chapterboxstatus.end(); ++ator)
	{
		nBoxLen += ator->second.size();
	}

	int sendLen = sizeof(LoginStageModelInfo)
		+chapterStatus.size() * sizeof(ChapterStatusInfo)
		+eliteChapterStatus.size() * sizeof(EliteChapterStatusInfo)
		+stageStatus.size() * sizeof(StageStatusInfo)
		+eliteStatus.size() * sizeof(EliteStatusInfo)
		+eliteRecordInfo.size() * sizeof(EliteRecordInfo) + nBoxLen;

	char *pSendBuffer = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(sendLen));
	LoginStageModelInfo *pStageModel = reinterpret_cast<LoginStageModelInfo*>(pSendBuffer);
	pStageModel->curStage = model->GetCurStage();
	pStageModel->curElite = model->GetCurElite();
	pStageModel->chapterCount = chapterStatus.size();
	pStageModel->eliteChapterCount = eliteChapterStatus.size();
	pStageModel->stageCount = stageStatus.size();
	pStageModel->eliteCount = eliteStatus.size();
	pStageModel->eliteRecordCount = eliteRecordInfo.size();

	ChapterStatusInfo *pChapterStatus = reinterpret_cast<ChapterStatusInfo *>(pStageModel + 1);
	for (std::map<int, int>::iterator iter = chapterStatus.begin();
		iter != chapterStatus.end(); ++iter)
	{
		pChapterStatus->chapterID = iter->first;
		pChapterStatus->chapterStatus = iter->second;
		pChapterStatus->chapterboxCount = 0;
		std::map<int, int>* pBosStatus = model->getChapterBoxStatusMap(iter->first);
		if (pBosStatus != NULL)
		{
			pChapterStatus->chapterboxCount = pBosStatus->size();
			pChapterStatus += 1;
			unsigned char *pChapterBox = reinterpret_cast<unsigned char *>(pChapterStatus);
			for (std::map<int, int>::iterator ator = pBosStatus->begin(); ator != pBosStatus->end(); ++ator)
			{
				(*pChapterBox) = static_cast<unsigned char>(ator->first);
				pChapterBox += 1;
			}

			pChapterStatus = reinterpret_cast<ChapterStatusInfo *>(pChapterBox);
		}
		else
		{
			pChapterStatus += 1;
		}
	}

	EliteChapterStatusInfo *pEliteChapterStatus = reinterpret_cast<EliteChapterStatusInfo *>(pChapterStatus);
	for (std::map<int, int>::iterator iter = eliteChapterStatus.begin();
		iter != eliteChapterStatus.end(); ++iter)
	{
		pEliteChapterStatus->chapterID = iter->first;
		pEliteChapterStatus->chapterStatus = iter->second;
		pEliteChapterStatus += 1;
	}

	StageStatusInfo *pStageStatus = reinterpret_cast<StageStatusInfo *>(pEliteChapterStatus);
	for (std::map<int, int>::iterator iter = stageStatus.begin();
		iter != stageStatus.end(); ++iter)
	{
		pStageStatus->stageId = iter->first;
		pStageStatus->stageStatus = iter->second;
		pStageStatus += 1;
	}

	EliteStatusInfo *pEliteStatus = reinterpret_cast<EliteStatusInfo *>(pStageStatus);
	for (std::map<int, int>::iterator iter = eliteStatus.begin();
		iter != eliteStatus.end(); ++iter)
	{
		pEliteStatus->stageId = iter->first;
		pEliteStatus->stageStatus = iter->second;
		pEliteStatus += 1;
	}

	EliteRecordInfo *pEliteRecord = reinterpret_cast<EliteRecordInfo *>(pEliteStatus);
	for (std::map<int, DBEliteStageInfo>::iterator iter = eliteRecordInfo.begin();
		iter != eliteRecordInfo.end(); ++iter)
	{
		pEliteRecord->stageId = iter->first;
		pEliteRecord->canUseTimes = iter->second.challengeTimes;
		pEliteRecord->useStamp = iter->second.useRecoverStamp;
		pEliteRecord->buyTimes = iter->second.buyTimes;
		pEliteRecord->buyStamp = iter->second.buyRecoverStamp;
		pEliteRecord += 1;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_STAGEMODEL_SC,
		pSendBuffer, sendLen);

	KxServer::kxMemMgrRecycle(pSendBuffer, sendLen);
}

void sendTeamModel(int uid, CTeamModel *model)
{
	int len = sizeof(LoginTeamModelInfo)+model->getAllTeamInfo().size() * sizeof(TeamInfo);
	char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(len));

	LoginTeamModelInfo *pLoginTeamModelInfo = reinterpret_cast<LoginTeamModelInfo*>(buff);
	pLoginTeamModelInfo->teamCount = model->getAllTeamInfo().size();

	// 偏移一个单位(LoginTeamModelInfo)
	TeamInfo *pTeamInfo = reinterpret_cast<TeamInfo *>(pLoginTeamModelInfo + 1);
	std::map<int, TeamInfo>::iterator iter = model->getAllTeamInfo().begin();
	for (; iter != model->getAllTeamInfo().end(); ++iter)
	{
		memcpy(pTeamInfo, &(iter->second), sizeof(TeamInfo));
		// 偏移一个单位(TeamInfo)
		pTeamInfo += 1;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_TEAMMODEL_SC, buff, len);
	KxServer::kxMemMgrRecycle(buff, len);
}

void sendTaskModel(int uid, CTaskModel *model)
{
	int len = sizeof(LoginTaskModelInfo)+model->GetTasks().size() * sizeof(TaskInfo);
	char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(len));

	LoginTaskModelInfo *pLoginTaskModelInfo = reinterpret_cast<LoginTaskModelInfo*>(buff);
	pLoginTaskModelInfo->taskCount = model->GetTasks().size();

	// 偏移一个单位(LoginTaskModelInfo)
	pLoginTaskModelInfo += 1;

	TaskInfo *pTaskInfo = reinterpret_cast<TaskInfo *>(pLoginTaskModelInfo);
	std::map<int, TaskDBInfo>::iterator iter = model->GetTasks().begin();
	for (; iter != model->GetTasks().end(); ++iter)
	{
		pTaskInfo->taskID = iter->first;
		pTaskInfo->taskVal = iter->second.taskVal;
		pTaskInfo->taskStatus = iter->second.taskStatus;
		pTaskInfo->resetTime = iter->second.resetTime;

		// 偏移一个单位(TaskInfo)
		pTaskInfo += 1;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_TASKMODEL_SC, buff, len);
	KxServer::kxMemMgrRecycle(buff, len);
}

void sendAchieveModel(int uid, CAchievementModel *model)
{
	int len = sizeof(LoginAchieveModelInfo)+model->GetAllAchievement().size() * sizeof(AchieveInfo);
	char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(len));

	LoginAchieveModelInfo *pLoginAchieveModelInfo = reinterpret_cast<LoginAchieveModelInfo*>(buff);
	pLoginAchieveModelInfo->achieveCount = model->GetAllAchievement().size();

	// 偏移一个单位(LoginAchieveModelInfo)
	pLoginAchieveModelInfo += 1;

	AchieveInfo *pAchieveInfo = reinterpret_cast<AchieveInfo *>(pLoginAchieveModelInfo);
	std::map<int, AchieveDBInfo>::iterator iter = model->GetAllAchievement().begin();
	for (; iter != model->GetAllAchievement().end(); ++iter)
	{
		pAchieveInfo->achieveID = iter->first;
		pAchieveInfo->achieveVal = iter->second.achieveVal;
		pAchieveInfo->achieveStatus = iter->second.achieveStatus;

		// 偏移一个单位(AchieveInfo)
		pAchieveInfo += 1;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_ACHIEVEMODEL_SC, buff, len);
	KxServer::kxMemMgrRecycle(buff, len);
}

void sendGuideModel(int uid, CGuideModel *model)
{
	std::set<int> GuideSet = model->GetNewGuideID();

	int len = sizeof(LoginGuideInfo)+GuideSet.size() * sizeof(int);
	char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(len));

	LoginGuideInfo *pGuideInfo = reinterpret_cast<LoginGuideInfo*>(buff);

	pGuideInfo->nNum = GuideSet.size();

	int *pGuideID = reinterpret_cast<int*>(pGuideInfo + 1);

	for (std::set<int>::iterator ator = GuideSet.begin(); ator != GuideSet.end(); ++ator)
	{
		(*pGuideID) = (*ator);
		pGuideID++;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_GUIDEMODEL_SC, buff, len);
	KxServer::kxMemMgrRecycle(buff, len);
}

void sendUnionModel(int uid, CUserUnionModel *model)
{
    int nUnionID = 0;
    CHECK_RETURN_VOID(model->getUserUnionValue(USER_UNION_ID, nUnionID));

    if (nUnionID <= 0)
    {
        map<int, int> MapApplyUnion = model->getApplyInfo();
        int len = sizeof(LoginUnionModelInfo)+sizeof(NoUnionInfo)+sizeof(ApplyInfo)*MapApplyUnion.size();
        char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(len));
        int offset = sizeof(LoginUnionModelInfo);
        LoginUnionModelInfo *pLoginUnionModelInfo = reinterpret_cast<LoginUnionModelInfo *>(buff);
        pLoginUnionModelInfo->hasUnion = 0;

        offset += sizeof(NoUnionInfo);
        NoUnionInfo *pNoUnionInfo = reinterpret_cast<NoUnionInfo *>(pLoginUnionModelInfo + 1);
        pNoUnionInfo->applyCount = 0;
        model->getUserUnionValue(USER_UNION_DAYAPPLAYCOUNT, pNoUnionInfo->todayApplyCount);
        model->getUserUnionValue(USER_UNION_APPLYUNIONSTAMP, pNoUnionInfo->applyStamp);

        ApplyInfo *pApplyInfo = reinterpret_cast<ApplyInfo *>(pNoUnionInfo + 1);
        //检查其是否申请了公会
        int nCurStamp = time(NULL);
        map<int, int>::iterator ator = MapApplyUnion.begin();
        for (; ator != MapApplyUnion.end(); ++ator)
        {
            if (nCurStamp > ator->second)
            {
                continue;
            }

            ++pNoUnionInfo->applyCount;
            pApplyInfo->unionID = ator->first;
            pApplyInfo->applyTime = ator->second;

            ++pApplyInfo;
            offset += sizeof(ApplyInfo);
        }

        CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_UNIONMODEL_SC, buff, offset);
        KxServer::kxMemMgrRecycle(buff, len);
    }
}

void sendActivityInstanceModel(int uid, CInstanceModel *model)
{
	std::map<int, DBActivityInstanceInfo> mapInstance = model->GetAllInstanceInfo();
	int sendLen = sizeof(LoginInstanceModelInfo)+sizeof(InstanceInfo)*mapInstance.size();
	char *pSendBuffer = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(sendLen));

	LoginInstanceModelInfo *pLoginInstance = reinterpret_cast<LoginInstanceModelInfo *>(pSendBuffer);
	pLoginInstance->instanceCount = mapInstance.size();

	InstanceInfo *pInstanceInfo = reinterpret_cast<InstanceInfo *>(pLoginInstance + 1);
	for (std::map<int, DBActivityInstanceInfo>::iterator iter = mapInstance.begin();
		iter != mapInstance.end(); ++iter)
	{
		pInstanceInfo->activityId = iter->first;
		pInstanceInfo->useTimes = iter->second.useTimes;
		pInstanceInfo->useStamp = iter->second.useStamp;
		pInstanceInfo->buyTimes = iter->second.buyTimes;
		pInstanceInfo->buyStamp = iter->second.buyStamp;
		pInstanceInfo->easy = iter->second.easy;
		pInstanceInfo->normal = iter->second.normal;
		pInstanceInfo->difficult = iter->second.difficult;
		pInstanceInfo->hell = iter->second.hell;
		pInstanceInfo->legend = iter->second.legend;

		pInstanceInfo += 1;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_INSTANCEMODEL_SC,
		pSendBuffer, sendLen);
	KxServer::kxMemMgrRecycle(pSendBuffer, sendLen);
}

void sendMailModel(int uid,CUserModel *pUserModel,CMailModel *pMailModel)
{
	// 普通个人邮件
	std::map<int, BaseMailInfo> &mapBasicInfo = pMailModel->GetMailBasicInfo();
	std::map<int, std::vector<DropItemInfo> > &mapItemInfo = pMailModel->GetMailItemInfo();
	std::map<int, std::string> &mapContextInfo = pMailModel->GetMailContextInfo();
	// 系统全局邮件
	CGlobalMailModel *pGlobalMailModel = CGlobalMailModel::getInstance();
	std::map<int, BaseMailInfo>& mapGMailBase = pGlobalMailModel->GetGlobalMailBase();
	std::map<int, std::string>& mapGMailCtx = pGlobalMailModel->GetGlobalMailContexts();
	std::map<int, std::vector<DropItemInfo> >& mapGMailItems = pGlobalMailModel->GetGlobalMailItems();

	// 注意, 无论basic中的有没有标题, 都会发送一个标题大小
	int len = sizeof(LoginMailModelInfo)+mapBasicInfo.size() * sizeof(LoginMailInfo);
	len += mapGMailBase.size() * sizeof(LoginMailInfo);

	char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(len));

	LoginMailModelInfo *pLoginMailModelInfo = reinterpret_cast<LoginMailModelInfo*>(buff);
	memset(pLoginMailModelInfo, 0, sizeof(LoginMailModelInfo));
	
	int normailSize = mapBasicInfo.size();
	pLoginMailModelInfo->normalMailCount = normailSize;
	pLoginMailModelInfo->webMailCount = mapGMailBase.size();
	// 1.普通邮件部分

	char * pMalBuff = reinterpret_cast<char *>(pLoginMailModelInfo + 1);
	LoginMailInfo *pNoramlMailInfo = reinterpret_cast<LoginMailInfo *>(pMalBuff);
	for (std::map<int, BaseMailInfo>::iterator iter = mapBasicInfo.begin();
		iter != mapBasicInfo.end(); ++iter)
	{
		// 邮件id
		pNoramlMailInfo->nMailID = iter->first;
		pNoramlMailInfo->mailConfID = iter->second.mailConfID;
		pNoramlMailInfo->sendTimeStamp = iter->second.sendTimeStamp;
		strncpy(pNoramlMailInfo->szTitle, iter->second.szTitle, strlen(iter->second.szTitle)+1);
		pNoramlMailInfo++;
	}

	LoginMailInfo *pGlobalMail = reinterpret_cast<LoginMailInfo *>(pNoramlMailInfo);
	//2.全局邮件id
	for (std::map<int, BaseMailInfo>::iterator iter = mapGMailBase.begin();
		iter != mapGMailBase.end(); ++iter)
	{
		// 全局邮件id
		int gMailId = iter->first;
		int nUserCreatTime = 0;
		//全服邮件为处理过的
		if (pMailModel->IsReadWebMail(gMailId))
		{
			pLoginMailModelInfo->webMailCount--;
			continue;
		}

		pUserModel->GetUserFieldVal(USR_FD_CREATETIME, nUserCreatTime);
		if (nUserCreatTime > iter->second.sendTimeStamp)
		{
			pLoginMailModelInfo->webMailCount--;
			continue;
		}

		pGlobalMail->nMailID = iter->first;
		pGlobalMail->mailConfID = iter->second.mailConfID;
		pGlobalMail->sendTimeStamp = iter->second.sendTimeStamp;
		strncpy(pGlobalMail->szTitle, iter->second.szTitle, strlen(iter->second.szTitle)+1);
		pGlobalMail++;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_MAILMODEL_SC, buff, len);
	KxServer::kxMemMgrRecycle(buff, len);
}

void sendGoldTestModel(int uid, CGoldTestModel *pGoldTestModel)
{
	LoginGoldTestModelInfo goldTestInfo;
	memset(&goldTestInfo, 0, sizeof(LoginGoldTestModelInfo));
	pGoldTestModel->GetFieldNum(GT_FD_USETIMES, goldTestInfo.nUseTimes);
	pGoldTestModel->GetFieldNum(GT_FD_RESETSTAMP, goldTestInfo.nUseStamp);
	pGoldTestModel->GetFieldNum(GT_FD_DAMAGE, goldTestInfo.nDamage);
	pGoldTestModel->GetFieldNum(GT_FD_CHESTFLAG, goldTestInfo.nChestData);

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_GOLDTESTMODEL_SC
		, reinterpret_cast<char *>(&goldTestInfo), sizeof(LoginGoldTestModelInfo));
}

void sendHeroTestModel(int uid, CHeroTestModel * pHeroTestModel)
{
	int nUseStamp = pHeroTestModel->GetResetStamp();
	std::map<int, int> &mapChallengeTimes = pHeroTestModel->GetAllChallengeTimes();

	int nTotalLen = sizeof(LoginHeroTestModelInfo)+mapChallengeTimes.size() * sizeof(HeroTestTimesInfo);
	char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(nTotalLen));

	int nLen = 0;
	LoginHeroTestModelInfo * pHeroTestInfo = (LoginHeroTestModelInfo *)buff;
	pHeroTestInfo->nUseStamp = nUseStamp;
	pHeroTestInfo->nTimesNum = (int)mapChallengeTimes.size();
	nLen += sizeof(LoginHeroTestModelInfo);

	std::map<int, int>::iterator it = mapChallengeTimes.begin();
	for (; it != mapChallengeTimes.end(); ++it)
	{
		HeroTestTimesInfo * pInfo = (HeroTestTimesInfo *)(buff + nLen);
		pInfo->nInstanceId = it->first;
		pInfo->nTimes = it->second;
		nLen += sizeof(HeroTestTimesInfo);
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_HEROTESTMODEL_SC, buff, nLen);
	KxServer::kxMemMgrRecycle(buff, nTotalLen);
}

void sendTowerTestModel(int uid, CTowerTestModel *model)
{
	std::vector<int> outerBonusList = model->GetOuterBonusList();
	int nSendLen = sizeof(LoginTowerTestModelInfo);
	char *pSendData = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(nSendLen));
	LoginTowerTestModelInfo *pModelInfo = reinterpret_cast<LoginTowerTestModelInfo*>(pSendData);

	//是否处于能挑战爬塔的时间
	const TowerTestSettingItem * pSettingConf = queryConfTowerSetting();
	CHECK_RETURN_VOID(NULL != pSettingConf);
	int nCurTime = static_cast<time_t>(time(NULL));
	int nStartTime = CTimeCalcTool::curDayTimeZero(pSettingConf->nStartWeekDay);
	int nEndTime = nStartTime + pSettingConf->nInterval * 3600;

	//爬塔未开启给定1
	if (nCurTime < nStartTime || nCurTime > nEndTime)
	{
		pModelInfo->floor = 1;
	}
	else
	{
		pModelInfo->floor = model->GetTowerTestField(TOWER_FD_FLOOR);
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_TOWERTESTMODEL_SC, pSendData, nSendLen);
	KxServer::kxMemMgrRecycle(pSendData, nSendLen);
}

void sendPvpModel(int uid, CPvpModel *model)
{
	CModelHelper::updatePvpInfo(uid);
	int nSendLen = sizeof(LoginPvpModelInfo);
	char* pSendData = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(nSendLen));

	LoginPvpModelInfo * pModelInfo = reinterpret_cast<LoginPvpModelInfo *>(pSendData);
	model->GetPvpBattleId(pModelInfo->battleId);
	model->GetPvpField(PVP_FD_INTEGRAL, pModelInfo->integral);
    model->GetPvpField(PVP_FD_CONTINUSWINTIMES, pModelInfo->continusWinTimes);
	model->GetPvpField(PVP_FD_HISTORYHIGHESTRANK, pModelInfo->historyHighestRank);
	model->GetPvpField(PVP_FD_HISTORYHIGHESTINTEGRAL, pModelInfo->historyHighestIntegral);
    model->GetPvpField(PVP_FD_HISTORYCONTINUSWINTIMES, pModelInfo->historyContinusWinTimes);

	model->GetPvpField(PVPCOMM_FD_DAYRESETSTAMP, pModelInfo->dayResetStamp);
	model->GetPvpField(PVPCOMM_FD_DAYWINTIMES, pModelInfo->dayWinTimes);
	model->GetPvpField(PVPCOMM_FD_DAYCONTINUSWINTIMES, pModelInfo->dayContinusWinTimes);
	model->GetPvpField(PVPCOMM_FD_DAYMAXCONITNUSWINTIMES, pModelInfo->dayMaxContinusWinTimes);
	model->GetPvpField(PVPCOMM_FD_DAYBATTLETIMES, pModelInfo->dayBattleTimes);
	model->GetPvpField(PVPCOMM_FD_DAYBUYCHESTTIMES, pModelInfo->dayBuyChestTimes);
	model->GetPvpField(PVPCOMM_FD_LASTCHESTGENTIME, pModelInfo->lastChestGenStamp);
	model->GetPvpField(PVPCOMM_FD_CHESTSTATUS, pModelInfo->chestStatus);
	model->GetPvpField(PVPCOMM_FD_CHESTORDER, pModelInfo->chestInsurance);
	model->GetPvpField(PVPCOMM_FD_REWARDFLAG, pModelInfo->rewardFlag);

	CPvpRankModel::getInstance()->GetPvpRank(uid, pModelInfo->rank);
	//锦标赛排名
    /*CPvpRankModel::getInstance()->getChampionRank(uid, pModelInfo->cpnRank);*/
	//设置routeId
	if (pModelInfo->battleId > 0)
	{
		CKxCommManager::getInstance()->setSessionRoute(uid, emRouteBattleKey, pModelInfo->battleId);
	}
	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_PVPMODEL_SC,
		pSendData, nSendLen);

	KxServer::kxMemMgrRecycle(pSendData, nSendLen);
}

void sendPvpChestModel(int uid, CPvpChestModel *model)
{
	const std::list<int> &chestList = model->getChestIds();
	int nSendLen = sizeof(LoginPvpChestModelInfo) + sizeof(int)* chestList.size();
	char* pSendData = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(nSendLen));

	LoginPvpChestModelInfo *pChestInfo = reinterpret_cast<LoginPvpChestModelInfo*>(pSendData);
	pChestInfo->chestCount = chestList.size();

	int *chestId = reinterpret_cast<int*>(pChestInfo+1);
    // 注:存储新的宝箱在头部,删除尾部
	for (std::list<int>::const_reverse_iterator iter = chestList.rbegin();
		iter != chestList.rend(); ++iter)
	{
		*chestId = *iter;
		chestId += 1;
	}	
	
	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_PVPCHESTMODEL_SC,
		pSendData, nSendLen);
	KxServer::kxMemMgrRecycle(pSendData, nSendLen);
}

void SendShopModel(int uid,CShopModel *pShopModel)
{
	CHECK_RETURN_DEBUG_VOID(NULL != pShopModel);
	std::map<int, ShopGoodsMap> ShopMap = pShopModel->GetAllShop();
	std::map<int, int> MapDiamond = pShopModel->GetDiamondDouble();

	//计算总的buffer缓冲区长度
	int nLen = 0;
	for (std::map<int, ShopGoodsMap>::iterator ator = ShopMap.begin(); ator != ShopMap.end(); ++ator)
	{
		ShopGoodsMap* pGoodsMap = pShopModel->GetShopData(ator->first);
		CHECK_CONTINUE(NULL != pGoodsMap);
		nLen += sizeof(SLoginShopData)+pGoodsMap->size() * sizeof(SLoginShopGoodsData);
	}

	nLen += MapDiamond.size() *sizeof(SLoginDiamondData);
	nLen += sizeof(SLoginDiamondShopData);

	nLen += sizeof(LoginShopModelInfo);
	char *pSendData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nLen));
	char *pCurData = pSendData;

	int nOffset = 0;
	LoginShopModelInfo*pLoginShopModel = reinterpret_cast<LoginShopModelInfo*>(pSendData);
	pLoginShopModel->cCount = ShopMap.size();
	pLoginShopModel++;
	nOffset += sizeof(LoginShopModelInfo);
	pCurData = (char*)pLoginShopModel;

	ShopGoodsMap* pGoodsMap = NULL;
	CShopData *conf = NULL;
	ShopConfigData *pData = NULL;
	int nFreshEndTimes = 0;
	int nTimes = 0;
	SLoginShopGoodsData *pGoodsData = NULL;
	for (std::map<int, ShopGoodsMap>::iterator ator = ShopMap.begin(); ator != ShopMap.end(); ++ator)
	{
		pGoodsMap = pShopModel->GetShopData(ator->first);
		CHECK_CONTINUE(NULL != pGoodsMap);

		conf = dynamic_cast<CShopData *>(CConfManager::getInstance()->getConf(CONF_SHOP));
		CHECK_CONTINUE(NULL != conf);
		pData = static_cast<ShopConfigData *>(conf->getData(ator->first));
		CHECK_CONTINUE(NULL != pData);
		CHECK_CONTINUE(pShopModel->GetShopInfo(ator->first, nTimes, nFreshEndTimes))
		SLoginShopData *pRefreshSC = reinterpret_cast<SLoginShopData *>(pCurData);
		SLoginShopData *pTempRefreshSC = pRefreshSC;
		pRefreshSC->cCurCount = pGoodsMap->size();
		pRefreshSC->nNextFreshTime = nFreshEndTimes;
		pRefreshSC->cShopID = ator->first;
		pRefreshSC->nTimes = nTimes;
		pRefreshSC++;
		pCurData = (char*)pRefreshSC;
		nOffset += sizeof(SLoginShopData);

		for (ShopGoodsMap::iterator iter = pGoodsMap->begin(); iter != pGoodsMap->end(); ++iter)
		{
			SShopGoods &CurGoods = iter->second;

			const ShopGoodsConfigData *pTemplateGoods = queryConfShopData(CurGoods.nShopGoodsID);

			if (pTemplateGoods == NULL)
			{
				pTempRefreshSC->cCurCount--;
				continue;
			}

			pGoodsData = reinterpret_cast<SLoginShopGoodsData *>(pCurData);

			pGoodsData->sGoodsShopID = CurGoods.nShopGoodsID;
			pGoodsData->nGoodsNum = CurGoods.nGoodsNum;
			pGoodsData->nCoinNum = pTemplateGoods->nCoinNum;
			pGoodsData->cCoinType = pTemplateGoods->nCoinType;
			pGoodsData->nGoodsID = pTemplateGoods->nGoodsID;
			pGoodsData->cIndex = iter->first;
			pGoodsData->cSale = pTemplateGoods->nSale;
			pGoodsData++;
			nOffset += sizeof(SLoginShopGoodsData);
			pCurData = (char*)pGoodsData;
		}
	}

	SLoginDiamondShopData *pDiamond = (SLoginDiamondShopData *)pCurData;
	pDiamond->cNum = MapDiamond.size();
	pDiamond++;
	nOffset += sizeof(SLoginDiamondShopData);
	SLoginDiamondData *pDiamondData = (SLoginDiamondData*)pDiamond;
	for (map<int, int>::iterator ator = MapDiamond.begin(); ator != MapDiamond.end(); ++ator)
	{
		pDiamondData->cPid = ator->first;
		pDiamondData->nTimeStamp = ator->second;
		pDiamondData++;
		nOffset += sizeof(SLoginDiamondData);
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_SHOPMODEL_SC, pSendData, nOffset);
	KxServer::kxMemMgrRecycle(pSendData, nLen);
}

void SendUnionShopModel(int uid, CUserUnionModel *pUserUnionModel)
{
	CHECK_RETURN_DEBUG_VOID(pUserUnionModel);
	int nUnionID;
	pUserUnionModel->getUserUnionValue(USER_UNION_ID, nUnionID);

	if (nUnionID == 0)
	{
		SLoginUnionShop UnionShop;
		UnionShop.sNum = 0;
		UnionShop.cFlag = 0;
		CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_UNIONSHOP_SC,(char*)&UnionShop,sizeof(UnionShop));
	}

	CUnionShopModel *pUnionShopModel = new CUnionShopModel();
	CUnionExpiditionModel *pUnionExpdiditionModel = new CUnionExpiditionModel();
	pUnionShopModel->init(nUnionID);
	pUnionExpdiditionModel->init(nUnionID);

	std::map<int, int> UnionShopMap = pUnionShopModel->getUnionShopMap();

	//计算总的buffer缓冲区长度
	int nLen = 0;
	int nOffset = 0;
	nLen += UnionShopMap.size() * sizeof(SLoginUnionShopGoodsData);
	nLen += sizeof(SLoginUnionShop);

	char *pSendData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nLen));
	SLoginUnionShop *pUnionShop = (SLoginUnionShop*)pSendData;
	pUnionShop->sNum = UnionShopMap.size();
	pUnionShop->cFlag = 0;
	int nExpiditionFinishTime = 0;
	int nRewardSendTime = 0;
	int nCurTime = time(NULL);
	pUnionExpdiditionModel->getExpiditionFieldValue(UNION_EXPIDITION_FINISHTIME, nExpiditionFinishTime);
	pUnionExpdiditionModel->getExpiditionFieldValue(UNION_EXPIDITION_REWARDSENDTIME, nRewardSendTime);
	if (nCurTime >= nExpiditionFinishTime && nCurTime < nRewardSendTime)
	{
		pUnionShop->cFlag = 1;
	}

	SLoginUnionShopGoodsData *pUnionGoodsData = reinterpret_cast<SLoginUnionShopGoodsData *>(pUnionShop + 1);
	nOffset += sizeof(SLoginUnionShop);
	for (map<int, int>::iterator ator = UnionShopMap.begin(); ator != UnionShopMap.end(); ++ator)
	{
		const ShopGoodsConfigData *pTemplateGoods = queryConfShopData(ator->first);

		if (pTemplateGoods == NULL)
		{
			pUnionShop->sNum--;
			continue;
		}

		pUnionGoodsData->sGoodsShopID = ator->first;
		pUnionGoodsData->cGoodsNum = static_cast<int>(ator->second);
		pUnionGoodsData->nCoinNum = pTemplateGoods->nCoinNum;
		pUnionGoodsData->cCoinType = pTemplateGoods->nCoinType;
		pUnionGoodsData->nGoodsID = pTemplateGoods->nGoodsID;
		pUnionGoodsData->cSale = pTemplateGoods->nSale;
		pUnionGoodsData++;
		nOffset += sizeof(SLoginUnionShopGoodsData);
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_UNIONSHOP_SC, pSendData, nOffset);
	KxServer::kxMemMgrRecycle(pSendData, nLen);
}

void SendActiveModel(int uid,CUserModel *pUserModel,CUserActiveModel *pUserActiveModel)
{
	CHECK_RETURN_DEBUG_VOID(NULL != pUserActiveModel);

	//活动总表
	CConfActiveTime *pConfActieTime = dynamic_cast<CConfActiveTime*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TIME));

	//活动商品表
	CConfActiveShop *pConfActiveShop = dynamic_cast<CConfActiveShop*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_SHOP));

	//活动掉落加成表
	CConfActiveExtraAdd *pActiveExtraAdd = dynamic_cast<CConfActiveExtraAdd*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_DROP));

	//活动任务表
	CConfActiveTask *pConfActiveTask = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));

	//活动月卡表
	CConfMonthActive *pConfActiveMonth = dynamic_cast<CConfMonthActive*>(
		CConfManager::getInstance()->getConf(CONF_MONTH_ACTIVE));

	//活动兑换表
	CConfExChangeActive *pConfActiveExChange = dynamic_cast<CConfExChangeActive*>(
		CConfManager::getInstance()->getConf(CONF_EXCHANGE_ACTIVE));

	map<int, void*> MapActiveData = pConfActieTime->getDatas();
	map<SConfActiveShopKey, SConfActiveShopData> MapShopData = pConfActiveShop->GetAllActiveShop();
	std::map<SConfActiveTaskKey, SConfActiveTaskData> MapTaskData = pConfActiveTask->GetAllTaskActiveMap();
	std::map<SConfActiveTaskKey, SConfExChangeActiveData> MapExChangeData = pConfActiveExChange->GetAllExChangeData();

	int nTotalLen = sizeof(SLoginActiveSC)+MapActiveData.size() * sizeof(SLoginActiveData)
		+sizeof(SLoginActiveShop)* pConfActiveShop->getShopActiveNum() + sizeof(SLoginActiveShopData)* MapShopData.size()
		+ sizeof(SLoginActiveTask)* pConfActiveTask->getTaskActiveNum() + sizeof(SLoginActiveTaskData)* MapTaskData.size()
		+ sizeof(SLoginActiveMonthCard)+pConfActiveMonth->GetAllMonthData().size()*sizeof(SLoginActiveMonthCardData)
		+sizeof(SLoginExChangeActive)+MapExChangeData.size() * sizeof(SLoginAcitveExChangeData);

	char *pSendData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nTotalLen));
	int nLen = 0;
	time_t CurTime;
	CurTime = time(NULL);
	SLoginActiveSC *pActiveSC = (SLoginActiveSC *)pSendData;

	pActiveSC->sCount = MapActiveData.size();
	SLoginActiveSC *pTempActiveSC = pActiveSC;
	pTempActiveSC++;
	nLen += sizeof(SLoginActiveSC);
	SLoginActiveData *pLoginActiveData = (SLoginActiveData *)pTempActiveSC;

	for (map<int, void*>::iterator ator = MapActiveData.begin(); ator != MapActiveData.end(); ++ator)
	{
		SConfActiveTime *pActiveTime = static_cast<SConfActiveTime *>(ator->second);
		if (pActiveTime->nTimeType == ACTOR_ACTIVETIME_TYPE)
		{
			int nCreatTime = 0;
			int nCurTime = static_cast<int>(time(NULL));
			pUserModel->GetUserFieldVal(USR_FD_CREATETIME, nCreatTime);
			nCreatTime = CTimeCalcTool::curTimeZero(static_cast<time_t>(nCreatTime));
			int nStartTime = nCreatTime + pActiveTime->nStartTime;
			int nEndTime = nCreatTime + pActiveTime->nEndTime;

			if (nCurTime > nEndTime)
			{
				pActiveSC->sCount--;
				continue;
			}

			if (nCurTime < nStartTime)
			{
				pActiveSC->sCount--;
				continue;
			}

			pLoginActiveData->nStartTime = nStartTime;
			pLoginActiveData->nEndTime = nEndTime;
		}
		else if (pActiveTime->nTimeType == SERVER_TIMEACTIVE_TYPE)
		{
			if (CurTime > pActiveTime->nEndTime)
			{
				pActiveSC->sCount--;
				continue;
			}

			pLoginActiveData->nStartTime = pActiveTime->nStartTime;
			pLoginActiveData->nEndTime = pActiveTime->nEndTime;
		}
		else
		{
			pLoginActiveData->nStartTime = pActiveTime->nStartTime;
			pLoginActiveData->nEndTime = pActiveTime->nEndTime;
		}

		pLoginActiveData->sActiveID = ator->first;
		pLoginActiveData->cLevLimit = pActiveTime->LvLimit;
		pLoginActiveData->cActiveType = pActiveTime->nActiveType;
		pLoginActiveData->cTimeType = pActiveTime->nTimeType;
		pLoginActiveData++;
		nLen += sizeof(SLoginActiveData);

		if (pActiveTime->nActiveType == SHOP_ACTIVE_TYPE)
		{
			SLoginActiveShop *pActiveShop = (SLoginActiveShop *)pLoginActiveData;
			vector<SConfActiveShopData> *pVectShopData = pConfActiveShop->GetActiveVector(ator->first);
			if (pVectShopData == NULL)
			{
				pActiveShop->sShopNum = 0;
				pActiveShop++;
				nLen += sizeof(SLoginActiveShop);
				pLoginActiveData = (SLoginActiveData *)pActiveShop;
				continue;
			}
			pActiveShop->sShopNum = pVectShopData->size();
			pActiveShop++;
			nLen += sizeof(SLoginActiveShop);
			SLoginActiveShopData *pLoginActiveShopData = (SLoginActiveShopData *)pActiveShop;

			for (vector<SConfActiveShopData>::iterator iter = pVectShopData->begin(); iter != pVectShopData->end(); ++iter)
			{
				int nValue = 0;
				SConfActiveShopData &ShopData = *iter;
				pUserActiveModel->GetShopActiveValue(ator->first, ShopData.nGiftID,nValue);
				pLoginActiveShopData->cBuyTimes = nValue;
				pLoginActiveShopData->cGiftID = ShopData.nGiftID;
				pLoginActiveShopData->cGoldType = ShopData.nGoldType;
				memset(pLoginActiveShopData->nGoodsID, 0, sizeof(pLoginActiveShopData->nGoodsID));
				memset(pLoginActiveShopData->nGoodsNum, 0, sizeof(pLoginActiveShopData->nGoodsNum));
				memcpy(pLoginActiveShopData->nGoodsID, ShopData.nGoodsID, sizeof(pLoginActiveShopData->nGoodsID));
				memcpy(pLoginActiveShopData->nGoodsNum, ShopData.nGoodsNum, sizeof(pLoginActiveShopData->nGoodsNum));
				pLoginActiveShopData->cMaxBuyTimes = ShopData.nMaxBuyTimes;
				pLoginActiveShopData->nPrice = ShopData.nPrice;
				pLoginActiveShopData->cSaleRate = ShopData.nSaleRate;
				pLoginActiveShopData++;
				nLen += sizeof(SLoginActiveShopData);
			}

			pLoginActiveData = (SLoginActiveData *)pLoginActiveShopData;
		}
		else if (pActiveTime->nActiveType == TASK_ACTIVE_TYPE)
		{
			SLoginActiveTask *pActiveTask = (SLoginActiveTask *)pLoginActiveData;
			std::map<int, SConfActiveTaskData>* pMapActiveTask = pConfActiveTask->GetTaskActiveTaskDataByActiveID(ator->first);
			if (pMapActiveTask == NULL)
			{
				pActiveTask->sActiveTaskNum = 0;
				pActiveTask++;
				nLen += sizeof(SLoginActiveTask);
				pLoginActiveData = (SLoginActiveData *)pActiveTask;
				continue;
			}
			pActiveTask->sActiveTaskNum = pMapActiveTask->size();
			pActiveTask++;
			nLen += sizeof(SLoginActiveTask);
			SLoginActiveTaskData * pActiveTaskData = (SLoginActiveTaskData *)pActiveTask;

			for (map<int, SConfActiveTaskData>::iterator iter = pMapActiveTask->begin(); iter != pMapActiveTask->end(); ++iter)
			{
				int nValue = 0;
				SConfActiveTaskData &TaskData = iter->second;

				pActiveTaskData->cTaskID = iter->first;
				memset(pActiveTaskData->nConditionParam, 0, sizeof(pActiveTaskData->nConditionParam));
				memcpy(pActiveTaskData->nConditionParam, TaskData.nConditionParam, sizeof(pActiveTaskData->nConditionParam));
				pActiveTaskData->sFinishCondition = TaskData.nFinishCondition;
				pUserActiveModel->GetTaskActiveValue(ator->first, iter->first, pActiveTaskData->nValue);
				pUserActiveModel->GetTaskActiveIndex(ator->first, iter->first,nValue);
				pActiveTaskData->cFinishFlag = nValue;
				pActiveTaskData->sRewardDimand = TaskData.nRewardDimand;
				pActiveTaskData->nRewardGold = TaskData.nRewardGold;
				memcpy(pActiveTaskData->nRewardGoodsID, TaskData.nRewardGoodsID, sizeof(pActiveTaskData->nRewardGoodsID));
				memcpy(pActiveTaskData->nRewardGoodsNum, TaskData.nRewardGoodsNum, sizeof(pActiveTaskData->nRewardGoodsNum));
				pActiveTaskData++;
				nLen += sizeof(SLoginActiveTaskData);
			}

			pLoginActiveData = (SLoginActiveData *)pActiveTaskData;
		}
		else if (pActiveTime->nActiveType == MONTHCARD_ACTIVE_TYPE)
		{
			SLoginActiveMonthCard * pMonthCard = (SLoginActiveMonthCard *)pLoginActiveData;
			map<int, SConfMonthActiveData>* pMonthMap = queryMonthActiveMap(ator->first);
			pMonthCard->cNum = pMonthMap->size();
			pMonthCard++;
			nLen += sizeof(SLoginActiveMonthCard);
			SLoginActiveMonthCardData *pMonthCardData = (SLoginActiveMonthCardData *)pMonthCard;

			for (map<int, SConfMonthActiveData>::iterator iter = pMonthMap->begin(); iter != pMonthMap->end(); ++iter)
			{
				SConfMonthActiveData &MonthData = iter->second;
				pMonthCardData->nMonthCardID = iter->first;
				if (MonthData.nDayLimit < 0)
				{
					pMonthCardData->cType = 0;
				}
				else
				{
					pMonthCardData->cType = 1;
				}

				//这种写法标识月卡只能有两种
				CUserActiveModel::getMonthCardValue(uid,MonthData.nPid, pMonthCardData->nFinishTimestamp);
				pUserActiveModel->GetTaskActiveValue(ator->first, iter->first, pMonthCardData->nRewardTime);
				pMonthCardData++;
				nLen += sizeof(SLoginActiveMonthCardData);
			}

			pLoginActiveData = (SLoginActiveData *)pMonthCardData;
		}
		else if (pActiveTime->nActiveType == EXCHANGE_ACTIVE_TYPE)
		{
			SLoginExChangeActive * pExChange = (SLoginExChangeActive *)pLoginActiveData;
			map<int, SConfExChangeActiveData>* pExChangeMap = queryExChangeActveMap(ator->first);
			pExChange->cNum = pExChangeMap->size();
			pExChange++;
			nLen += sizeof(SLoginExChangeActive);
			SLoginAcitveExChangeData *pExChangeData = (SLoginAcitveExChangeData *)pExChange;

			for (map<int, SConfExChangeActiveData>::iterator iter = pExChangeMap->begin(); iter != pExChangeMap->end(); ++iter)
			{
				int nValue = 0;
				SConfExChangeActiveData &ExChangeData = iter->second;
				pExChangeData->sActiveID = ator->first;
				pExChangeData->cTaskID = iter->first;
				pUserActiveModel->GetTaskActiveIndex(ator->first, iter->first,nValue);
				pExChangeData->cCurTimes = nValue;
				pExChangeData++;
				nLen += sizeof(SLoginAcitveExChangeData);
			}

			pLoginActiveData = (SLoginActiveData *)pExChangeData;
		}
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_ACTIVEMODEL_SC, pSendData, nLen);
	KxServer::kxMemMgrRecycle(pSendData,nTotalLen);
}

void Send7DayActiveModel(int uid,CUserModel *pUserModel,CUserActiveModel *pUserActiveModel)
{
	CHECK_RETURN_DEBUG_VOID(NULL != pUserActiveModel);
	CHECK_RETURN_DEBUG_VOID(NULL != pUserModel);

	//初始化7天活动
	CConf7DayActive *pConf7DayActive = dynamic_cast<CConf7DayActive*>(
		CConfManager::getInstance()->getConf(CONF_7DAY_ACTIVE));
	CHECK_RETURN_VOID(pConf7DayActive != NULL);

	//活动商品表
	CConfActiveShop *pConfActiveShop = dynamic_cast<CConfActiveShop*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_SHOP));

	//活动任务表
	CConfActiveTask *pConfActiveTask = dynamic_cast<CConfActiveTask*>(
		CConfManager::getInstance()->getConf(CONF_ACTIVE_TASK));

	int nCreatTime = 0;
	pUserModel->GetUserFieldVal(USR_FD_CREATETIME, nCreatTime);
	int nCreateDay = CCommonHelper::getTotalDay(nCreatTime);
	int nCurDay = CCommonHelper::getTotalDay();
	int nDay = nCurDay - nCreateDay;

	S7DayActive *pDayActive = static_cast<S7DayActive *>(pConf7DayActive->getData(nDay + 1));
	int nOffset = 0;
	if (pDayActive == NULL)
	{
		SLogin7DayActiveSC DayActiveSC;
		DayActiveSC.Day = nDay + 1;
		DayActiveSC.sNum = 0;
		CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_7DAYACTIVE_SC,(char*)&DayActiveSC,sizeof(DayActiveSC));
		return;
	}

	int nTotalLen = sizeof(SLogin7DayActiveSC)+sizeof(SLogin7DayActiveData)* pDayActive->VectActive.size();
	for (int i = 0; i < pDayActive->VectActive.size(); i++)
	{
		if (pDayActive->VectActive[i].nActiveType == SHOP_ACTIVE_TYPE)
		{
			vector<SConfActiveShopData> *pVectShopData = pConfActiveShop->GetActiveVector(pDayActive->VectActive[i].nActiveID);
			nTotalLen += sizeof(SLoginActiveShopData)* pVectShopData->size() + sizeof(SLoginActiveShop);
		}
		else if (pDayActive->VectActive[i].nActiveType == TASK_ACTIVE_TYPE)
		{
			std::map<int, SConfActiveTaskData>* pMapActiveTask = pConfActiveTask->GetTaskActiveTaskDataByActiveID(pDayActive->VectActive[i].nActiveID);
			nTotalLen += sizeof(SLoginActiveTaskData)* pMapActiveTask->size() + sizeof(SLoginActiveTask);
		}
	}

	char *pSendData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nTotalLen));
	SLogin7DayActiveSC *pActiveSC = (SLogin7DayActiveSC *)pSendData;
	pActiveSC->sNum = pDayActive->VectActive.size();
	pActiveSC->Day = nDay + 1;
	nOffset += sizeof(SLogin7DayActiveSC);

	for (int i = 0; i < pDayActive->VectActive.size(); i++)
	{
		SLogin7DayActiveData *pDayActiveData = (SLogin7DayActiveData *)(pSendData + nOffset);
		pDayActiveData->sActiveID = pDayActive->VectActive[i].nActiveID;
		pDayActiveData->cActiveType = pDayActive->VectActive[i].nActiveType;
		nOffset += sizeof(SLogin7DayActiveData);

		if (pDayActive->VectActive[i].nActiveType == SHOP_ACTIVE_TYPE)
		{
			SLoginActiveShop *pActiveShop = (SLoginActiveShop *)(pSendData + nOffset);
			vector<SConfActiveShopData> *pVectShopData = pConfActiveShop->GetActiveVector(pDayActive->VectActive[i].nActiveID);
			if (pVectShopData == NULL)
			{
				pActiveShop->sShopNum = pVectShopData->size();
				nOffset += sizeof(SLoginActiveShop);
				continue;
			}

			pActiveShop->sShopNum = pVectShopData->size();
			nOffset += sizeof(SLoginActiveShop);
			for (vector<SConfActiveShopData>::iterator iter = pVectShopData->begin(); iter != pVectShopData->end(); ++iter)
			{
				int nValue = 0;
				SConfActiveShopData &ShopData = *iter;
				SLoginActiveShopData *pLoginActiveShopData = (SLoginActiveShopData *)(pSendData + nOffset);
				pUserActiveModel->GetShopActiveValue(pDayActive->VectActive[i].nActiveID, ShopData.nGiftID,nValue);
				pLoginActiveShopData->cBuyTimes = nValue;
				pLoginActiveShopData->cGiftID = ShopData.nGiftID;
				pLoginActiveShopData->cGoldType = ShopData.nGoldType;
				memset(pLoginActiveShopData->nGoodsID, 0, sizeof(pLoginActiveShopData->nGoodsID));
				memset(pLoginActiveShopData->nGoodsNum, 0, sizeof(pLoginActiveShopData->nGoodsNum));
				memcpy(pLoginActiveShopData->nGoodsID, ShopData.nGoodsID, sizeof(pLoginActiveShopData->nGoodsID));
				memcpy(pLoginActiveShopData->nGoodsNum, ShopData.nGoodsNum, sizeof(pLoginActiveShopData->nGoodsNum));
				pLoginActiveShopData->cMaxBuyTimes = ShopData.nMaxBuyTimes;
				pLoginActiveShopData->nPrice = ShopData.nPrice;
				pLoginActiveShopData->cSaleRate = ShopData.nSaleRate;
				nOffset += sizeof(SLoginActiveShopData);
			}
		}
		else if (pDayActive->VectActive[i].nActiveType == TASK_ACTIVE_TYPE)
		{
			SLoginActiveTask *pActiveTask = (SLoginActiveTask *)(pSendData + nOffset);
			std::map<int, SConfActiveTaskData>* pMapActiveTask = pConfActiveTask->GetTaskActiveTaskDataByActiveID(pDayActive->VectActive[i].nActiveID);
			if (pMapActiveTask == NULL)
			{
				pActiveTask->sActiveTaskNum = 0;
				nOffset += sizeof(SLoginActiveTask);
				continue;
			}
			
			pActiveTask->sActiveTaskNum = pMapActiveTask->size();
			nOffset += sizeof(SLoginActiveTask);
			for (map<int, SConfActiveTaskData>::iterator iter = pMapActiveTask->begin(); iter != pMapActiveTask->end(); ++iter)
			{
				int nValue = 0;
				SConfActiveTaskData &TaskData = iter->second;
				SLoginActiveTaskData * pActiveTaskData = (SLoginActiveTaskData *)(pSendData + nOffset);
				pActiveTaskData->cTaskID = iter->first;
				memset(pActiveTaskData->nConditionParam, 0, sizeof(pActiveTaskData->nConditionParam));
				memcpy(pActiveTaskData->nConditionParam, TaskData.nConditionParam, sizeof(pActiveTaskData->nConditionParam));
				pActiveTaskData->sFinishCondition = TaskData.nFinishCondition;
				pUserActiveModel->GetTaskActiveValue(pDayActive->VectActive[i].nActiveID, iter->first, pActiveTaskData->nValue);
				pUserActiveModel->GetTaskActiveIndex(pDayActive->VectActive[i].nActiveID, iter->first,nValue);
				pActiveTaskData->cFinishFlag = nValue;
				pActiveTaskData->sRewardDimand = TaskData.nRewardDimand;
				pActiveTaskData->nRewardGold = TaskData.nRewardGold;
				memcpy(pActiveTaskData->nRewardGoodsID, TaskData.nRewardGoodsID, sizeof(pActiveTaskData->nRewardGoodsID));
				memcpy(pActiveTaskData->nRewardGoodsNum, TaskData.nRewardGoodsNum, sizeof(pActiveTaskData->nRewardGoodsNum));
				nOffset += sizeof(SLoginActiveTaskData);
			}
		}
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_7DAYACTIVE_SC, pSendData, nTotalLen);
	KxServer::kxMemMgrRecycle(pSendData, nTotalLen);
}

void sendBDActiveModel(int uid, CBlueDiamondModel *pModel)
{
	if (pModel == NULL)
	{
		return;
	}

	//蓝钻所有活动
	CConfBDActive *pConfActive = dynamic_cast<CConfBDActive*>(
		CConfManager::getInstance()->getConf(CONF_BLUEDIAMOND_ACTIVE));
	CHECK_RETURN_VOID(pConfActive != NULL);
	std::map<SBDActiveKey, SBDActiveData> MapActiveData = pConfActive->getBDAllData();

	int nTotalLen = sizeof(SLoginBDActiveSC)+sizeof(SLoginBDActiveData)* MapActiveData.size();
	int nOffset = 0;
	char *pSendData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nTotalLen));
	SLoginBDActiveSC *pActiveSC = (SLoginBDActiveSC *)pSendData;
	pActiveSC->nNum = MapActiveData.size();
	nOffset += sizeof(SLoginBDActiveSC);

	for (std::map<SBDActiveKey, SBDActiveData>::iterator ator = MapActiveData.begin(); ator != MapActiveData.end(); ++ator)
	{
		SLoginBDActiveData * pActiveTaskData = (SLoginBDActiveData *)(pSendData + nOffset);
		pActiveTaskData->nActiveID = ator->first.nActiveID;
		pActiveTaskData->nTaskID = ator->first.nTaskID;
		pModel->getQQActiveValue(ator->first.nActiveID, ator->first.nTaskID, pActiveTaskData->nFlag);
		nOffset += sizeof(SLoginBDActiveData);
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_QQACTIVE_SC, pSendData, nTotalLen);
	KxServer::kxMemMgrRecycle(pSendData, nTotalLen);
}

void sendHeadIDModel(int uid, CHeadModel *pHeadIDModel)
{
	if (pHeadIDModel == NULL)
	{
		return;
	}

	std::set<int> HeadIDSet = pHeadIDModel->GetAllHeadID();

	int len = sizeof(LoginHeadInfo)+HeadIDSet.size() * sizeof(int);
	char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(len));

	LoginGuideInfo *pHeadInfo = reinterpret_cast<LoginGuideInfo*>(buff);

	pHeadInfo->nNum = HeadIDSet.size();

	int *pHeadID = reinterpret_cast<int*>(pHeadInfo + 1);

	for (std::set<int>::iterator ator = HeadIDSet.begin(); ator != HeadIDSet.end(); ++ator)
	{
		(*pHeadID) = (*ator);
		pHeadID++;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_HEAD_SC, buff, len);
	KxServer::kxMemMgrRecycle(buff, len);
}

void CLoginService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
	case CMD_LOGIN_CS:
		processUserLogin(uid, buffer, len, commun);
		break;
	case CMD_LOGIN_RECONECT_CS:
		processUserReconect(uid, buffer, len, commun);
		break;
	case CMD_LOGIN_USERINFO_CS:
		processUserLogService(uid, buffer, len, commun);
		break;
    case CMD_LOGIN_GUIDEINFO_CS:
        processGuideLogService(uid, buffer, len, commun);
        break;
	case CMD_LOGIN_FRESH:
		processLoginFreshService(uid, buffer, len, commun);
	default:
		break;
	}
}

void CLoginService::processUserLogin(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	LoginCS *loginCS = reinterpret_cast<LoginCS*>(buffer);
	if (loginCS->userid != uid || loginCS->userid < 0)
	{
		return;
	}
	
	// 获得CGameUser
    CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    if (NULL == pGameUser)
    {
        //新用户
        pGameUser = CGameUserManager::getInstance()->newGameUser(uid);
        //角色创建日志
        CCommOssHelper::crateActorStatOss(uid);
    }
    else
    {
        // 如果不是新用户，断线后会在一段时间内自动移除
        // 改方法会剔除移除列表数据，不让它自动释放，因为我胡汉三又回来了
        CGameUserManager::getInstance()->donotDeleteUser(uid);
        pGameUser->refreshModel(MODELTYPE_MAIL);
        pGameUser->refreshModel(MODELTYPE_USER);
        pGameUser->refreshModel(MODELTYPE_USERUNION);
		pGameUser->refreshModel(MODELTYPE_SHOP);
    }

    int nCurTime = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
    //检查是否封停
    int nBanEndTimes = 0;
    pUserModel->GetUserFieldVal(USR_FD_BANTIME, nBanEndTimes);
    if (nBanEndTimes != 0)
    {
        if (nBanEndTimes > nCurTime)
        {
            CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_BAN_SC,
                reinterpret_cast<char *>(&nBanEndTimes), sizeof(nBanEndTimes));
            return;
        }
    }

    // 刷新一下每日重置的一些数据
    CGameUserManager::getInstance()->reSetGameUserData(uid, true);

	//检查充值数据
	CModelHelper::checkPayData(uid);

    // 获取所有模型
	CBagModel *pBagModel = dynamic_cast<CBagModel*>(pGameUser->getModel(MODELTYPE_BAG));
	CEquipModel *pEquipModel = dynamic_cast<CEquipModel*>(pGameUser->getModel(MODELTYPE_EQUIP));
	CSummonModel *pSummonerModel = dynamic_cast<CSummonModel*>(pGameUser->getModel(MODELTYPE_SUMMONER));
	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(pGameUser->getModel(MODELTYPE_HERO));
	CStageModel *pStageModel = dynamic_cast<CStageModel*>(pGameUser->getModel(MODELTYPE_STAGE));
	CTeamModel *pTeamModel = dynamic_cast<CTeamModel*>(pGameUser->getModel(MODELTYPE_TEAM));
	CTaskModel *pTaskModel = dynamic_cast<CTaskModel*>(pGameUser->getModel(MODELTYPE_TASK));
	CAchievementModel *pAchieveModel = dynamic_cast<CAchievementModel*>(pGameUser->getModel(MODELTYPE_ACHIEVEMENT));
	CGuideModel *pGuideModel = dynamic_cast<CGuideModel*>(pGameUser->getModel(MODELTYPE_GUIDE));
	//CInstanceModel *pInstanceModel = dynamic_cast<CInstanceModel*>(pGameUser->getModel(MODELTYPE_INSTANCE));
	CMailModel *pMailModel = dynamic_cast<CMailModel*>(pGameUser->getModel(MODELTYPE_MAIL));
	CGoldTestModel *pGoldTestModel = dynamic_cast<CGoldTestModel*>(pGameUser->getModel(MODELTYPE_GOLDTEST));
	CHeroTestModel *pHeroTestModel = dynamic_cast<CHeroTestModel*>(pGameUser->getModel(MODELTYPE_HEROTEST));
	CTowerTestModel *pTowerTestModel = dynamic_cast<CTowerTestModel*>(pGameUser->getModel(MODELTYPE_TOWERTEST));
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel*>(pGameUser->getModel(MODELTYPE_PVP));
	CPvpChestModel *pPvpChestModel = dynamic_cast<CPvpChestModel*>(pGameUser->getModel(MODELTYPE_PVPCHEST));
	CShopModel *pShopModel = dynamic_cast<CShopModel*>(pGameUser->getModel(MODELTYPE_SHOP));
	CUserActiveModel *pUserActiveModel = dynamic_cast<CUserActiveModel*>(pGameUser->getModel(MODELTYPE_ACTIVE));
	CHeadModel *pHeadmodel = dynamic_cast<CHeadModel*>(pGameUser->getModel(MODELTYPE_HEAD));
    CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
	CBlueDiamondModel *pBlueDiamondModel = dynamic_cast<CBlueDiamondModel*>(pGameUser->getModel(MODELTYPE_BDACTIVE));

	int capacity = 0;
	int nCreatTime = 0;
	pUserModel->GetUserFieldVal(USR_FD_BAGCAPACITY, capacity);
	pUserModel->GetUserFieldVal(USR_FD_CREATETIME, nCreatTime);
	
	//设置登陆时间
    pUserModel->SetUserFieldVal(USR_FD_LOGINTIME, nCurTime);
	CModelHelper::DispatchActionEvent(uid, ELA_OLTIME, NULL, 0);
	// 开始下发数据
	LoginSC loginSC;
    loginSC.serverStamp = nCurTime;
	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_SC,
		reinterpret_cast<char *>(&loginSC), sizeof(loginSC));

	// 发送用户数据
	sendUserModel(loginCS->userid, pUserModel);
	// 发送背包数据
	sendBagModel(loginCS->userid, capacity, pBagModel);
	// 发送装备数据
	sendEquipModel(loginCS->userid, pEquipModel);
	// 发送召唤师数据
	sendSummonModel(loginCS->userid, pSummonerModel);
	// 发送英雄数据
	sendHeroModel(loginCS->userid, 0, pHeroModel);
	// 发送关卡数据
	sendStageModel(loginCS->userid, pStageModel);
	// 发送队伍数据
	sendTeamModel(loginCS->userid, pTeamModel);
	// 发送任务数据
	sendTaskModel(loginCS->userid, pTaskModel);
	// 发送成就数据
	sendAchieveModel(loginCS->userid, pAchieveModel);
	// 发送剧情数据
	sendGuideModel(loginCS->userid, pGuideModel);
	// 发送公会数据
    sendUnionModel(loginCS->userid, pUserUnionModel);
	// 发送副本数据
	//sendActivityInstanceModel(loginCS->userid, pInstanceModel);
	// 发送邮件数据
	sendMailModel(loginCS->userid,pUserModel,pMailModel);
	// 发送金币试炼数据
	sendGoldTestModel(loginCS->userid, pGoldTestModel);
	// 发送英雄试炼数据
	sendHeroTestModel(loginCS->userid, pHeroTestModel);
	// 发送爬塔试练数据
	sendTowerTestModel(loginCS->userid, pTowerTestModel);
	// 发送pvpModel
	sendPvpModel(loginCS->userid, pPvpModel);
	// 发送pvp宝箱Id
	sendPvpChestModel(loginCS->userid, pPvpChestModel);
	//发送商店数据
	SendShopModel(loginCS->userid,pShopModel);
	//发送公会商店数据
	SendUnionShopModel(loginCS->userid, pUserUnionModel);
	//发送活动数据
	SendActiveModel(loginCS->userid,pUserModel,pUserActiveModel);
	//发送7日活动数据
	Send7DayActiveModel(loginCS->userid, pUserModel, pUserActiveModel);
	//发送QQ蓝钻活动数据
	sendBDActiveModel(loginCS->userid, pBlueDiamondModel);
	//发送头像数据
	sendHeadIDModel(loginCS->userid, pHeadmodel);
	// 下发数据结束
	CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_FINISH_SC, NULL, 0);

	KXLOGDEBUG("LoginFinish %d", loginCS->userid);
}

void CLoginService::processUserReconect(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	// 获得CGameUser
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	if (NULL == pGameUser)
	{
        return;
	}

	//设置routeId
	int battleId = 0;
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(uid, MODELTYPE_PVP));
	pPvpModel->GetPvpBattleId(battleId);
	if (battleId > 0)
	{
		CKxCommManager::getInstance()->setSessionRoute(uid, emRouteBattleKey, battleId);
	}

	//重置数据
	CGameUserManager::getInstance()->reSetGameUserData(uid);
    // 如果不是新用户，断线后会在一段时间内自动移除
    // 改方法会剔除移除列表数据，不让它自动释放，因为我胡汉三又回来了
	CGameUserManager::getInstance()->donotDeleteUser(uid);
}

void CLoginService::processUserLogService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	if (len != sizeof(LoginUserInfoCS) || buffer == NULL)
	{
		return;
	}
	LoginUserInfoCS *pUserInfoCS = reinterpret_cast<LoginUserInfoCS*>(buffer);

	if (pUserInfoCS->nIsNew)
	{
		//日志
		CCommOssHelper::registerStatOss(pUserInfoCS->nUid, pUserInfoCS->szMobile, pUserInfoCS->nChannelID, pUserInfoCS->szChannelName);
	}
	else
	{
		if (pUserInfoCS->nIsGuest)
		{
			CCommOssHelper::userChannelOss(pUserInfoCS->nUid, pUserInfoCS->nChannelID, pUserInfoCS->szChannelName);
		}
	}
}

void CLoginService::processGuideLogService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    if (len != sizeof(LoginGuideInfoCS) || buffer == NULL)
    {
        return;
    }

    LoginGuideInfoCS* pGuideInfoCS = reinterpret_cast<LoginGuideInfoCS*>(buffer);
    if (pGuideInfoCS->nStepId != 0)
    {
        //日志写入
        CCommOssHelper::newGuidOss(uid, pGuideInfoCS->nStepId);
    }
}

//处理用户零点或者其他时间点刷新数据请求
void CLoginService::processLoginFreshService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	// 获得CGameUser
	CGameUser* pGameUser = CGameUserManager::getInstance()->getGameUser(uid);

	if (pGameUser == NULL)
	{
		return;
	}

	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CUserActiveModel *pUserActiveModel = dynamic_cast<CUserActiveModel*>(pGameUser->getModel(MODELTYPE_ACTIVE));

	if (pUserModel == NULL || pUserActiveModel == NULL)
	{
		return;
	}

	CGameUserManager::getInstance()->reSetGameUserData(uid, true);
	//设置登陆时间
	int nLoginTime = (int)KxServer::KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
	KXLOGDEBUG("LoginTime is %d", nLoginTime);
	pUserModel->SetUserFieldVal(USR_FD_LOGINTIME,nLoginTime);
	//发送7日活动数据
	Send7DayActiveModel(uid, pUserModel, pUserActiveModel);
}