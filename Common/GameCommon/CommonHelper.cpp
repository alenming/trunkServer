#include "KxCore.h"
#include "CommonHelper.h"
#include "ConfArena.h"
#include "ConfHall.h"
#include "ConfGameSetting.h"
#include "ConfStage.h"
#include "GameDef.h"
#include "ItemDrop.h"
#include "Protocol.h"

#define STAGE_REF 10000

using namespace std;

int CCommonHelper::getUserMMRDan(int mmr)
{
	int dan = DAN_NEWBIE;
	for (dan = DAN_NEWBIE; dan < DAN_KING; ++dan)
    {
        const ArenaRankItem * pArenaRankItem = queryConfArenaRankItem(dan);
        if (pArenaRankItem != NULL)
        {
			if (pArenaRankItem->MMR_Range.size() >= 2)
            {
                if (pArenaRankItem->MMR_Range[0] <= mmr
                    && pArenaRankItem->MMR_Range[1] > mmr)
                {
                    break;
                }
            }
        }
    }
	return dan;
}

int CCommonHelper::getUserIntegralDan(int integral)
{
	int dan = DAN_NEWBIE;
	for (dan = DAN_NEWBIE; dan <= DAN_KING; ++dan)
	{
		const ArenaRankItem * pArenaRankItem = queryConfArenaRankItem(dan);
		if (pArenaRankItem->GNRank.size() >= 2)
		{
			if (pArenaRankItem->GNRank[0] <= integral
				&& pArenaRankItem->GNRank[1] > integral)
			{
				break;
			}
		}
	}
	return dan;
}

float CCommonHelper::getDanMMR_K(int dan)
{
	const ArenaRankItem * pArenaRankItem = queryConfArenaRankItem(dan);
	if (NULL != pArenaRankItem)
	{
		return pArenaRankItem->MMR_K;
	}
	return 0.0f;
}

float CCommonHelper::getDanMMR_Kx(int dan)
{
	const ArenaRankItem * pArenaRankItem = queryConfArenaRankItem(dan);
	if (NULL != pArenaRankItem)
	{
		return pArenaRankItem->MMR_kx;
	}
	return 0.0f;
}

float CCommonHelper::getDanArena_K(int dan)
{
	const ArenaRankItem * pArenaRankItem = queryConfArenaRankItem(dan);
	if (NULL != pArenaRankItem)
	{
		return pArenaRankItem->Arena_K;
	}

	return 0.0f;
}

int CCommonHelper::getDanRobotMatchTime(int dan)
{
	const ArenaSettingItem * pArenaSettingItem = queryArenaSetting();
	if (NULL != pArenaSettingItem)
	{
		return pArenaSettingItem->LimitWaitingTime;
	}
	return 0;
}

int CCommonHelper::getChapterTypeToTask(int chaperId)
{
	const ChapterConfItem *pChapterConf = queryConfChapter(chaperId);
	if (NULL != pChapterConf)
	{
		if (pChapterConf->Type == CHAPTERTYPE_NORMAL)
		{
			return STAGE_TASK_TYPE_NORMAL;
		}
		else if (pChapterConf->Type == CHAPTERTYPE_ELITE)
		{
			return STAGE_TASK_TYPE_ELITE;
		}
	}
    else
    {
        LOG("Execute False On File %s Line %d : %d", __FILE__, __LINE__, chaperId);
    }

	return STAGE_TASK_TYPE_NON;
}

int CCommonHelper::getStageTypeToTask(int stageId)
{
    if (stageId > STAGE_REF)
    {
        return STAGE_TASK_TYPE_ELITE;
    }

    return STAGE_TASK_TYPE_NORMAL;
}
/*
int CCommonHelper::getStageNeedEnergy(int chapterId, int stageId)
{
	const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
    if (NULL == pChapterConf)
    {
        LOG("Execute False On File %s Line %d : %d", __FILE__, __LINE__, chapterId);
        return -1;
    }

	std::map<int, StageInfo>::const_iterator iter = pChapterConf->Stages.find(stageId);
	if (iter == pChapterConf->Stages.end())
	{
		return -1;
	}
	//章节关卡配置
	const StageInfo &stageInfo = iter->second;
	return stageInfo.Energy;
}*/

int CCommonHelper::getStageNeedLevel(int chapterId, int stageId)
{
	const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
    if (NULL == pChapterConf)
    {
        LOG("Execute False On File %s Line %d : %d %d", __FILE__, __LINE__, chapterId, stageId);
        return -1;
    }

	std::map<int, StageInfo>::const_iterator iter = pChapterConf->Stages.find(stageId);
	if (iter == pChapterConf->Stages.end())
	{
		return -1;
	}
	//章节关卡配置
	const StageInfo &stageInfo = iter->second;
	if (stageInfo.ID.size() < 2)
	{
		return -1;
	}
	return stageInfo.ID[1];
}

bool CCommonHelper::getNextChapterStage(int chapterId, int stageId, int &nextChapter, int &nextStage)
{
	const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
    if (NULL == pChapterConf)
    {
        LOG("Execute False On File %s Line %d : %d", __FILE__, __LINE__, chapterId);
        return false;
    }

	std::map<int, StageInfo>::const_iterator iter = pChapterConf->Stages.find(stageId);
	if (iter == pChapterConf->Stages.end())
	{
		nextChapter = 0;
		nextStage = 0;
		return false;
	}

	const StageInfo &stageInfo = iter->second;
	// 关卡最后一关
	if (stageInfo.ExInfo.NextID == 0)
	{
		nextChapter = pChapterConf->ExInfo.NextID;
		const ChapterConfItem *pNextChapter = queryConfChapter(nextChapter);
        if (NULL == pNextChapter)
        {
            LOG("Execute False On File %s Line %d : %d", __FILE__, __LINE__, nextChapter);
            return false;
        }

		if (nextChapter == 0)
		{
			nextChapter = 0;
			nextStage = 0;
			return false;
		}
		nextStage = pNextChapter->ExInfo.FirstStageID;
	}
	else
	{
		// 正常
		nextChapter = chapterId;
		nextStage = stageInfo.ExInfo.NextID;
	}

	return true;
}

int CCommonHelper::getHeroTopStar(int heroId)
{
	const SoldierUpRateItem *pSoldierStar = queryConfSoldierUpRateItem(heroId);
	if (NULL != pSoldierStar)
	{
		return pSoldierStar->TopStar;
	}
	return 0;
}

void CCommonHelper::classifyItems(const std::vector<DropItemInfo> &dropItems, std::vector<DropItemInfo> &bagItems,
	std::vector<DropItemInfo> &heroItems, std::vector<DropItemInfo> &summonerItems, std::vector<DropItemInfo> &resourceItems)
{
	for (size_t i = 0; i < dropItems.size(); i++)
	{
		const PropItem *pPropConf = queryConfProp(dropItems[i].id);
		if (NULL == pPropConf)
		{
            LOG("Execute False On File %s Line %d : %d", __FILE__, __LINE__, dropItems[i].id);
			continue;
		}

		switch (pPropConf->Type)
		{
			case PROPTYPE_HEROCARD:		//英雄卡片
			{
				heroItems.push_back(dropItems[i]);
			}
			break;
			case PROPTYPE_SUMMONERCARD:	//召唤师卡片
			{
				summonerItems.push_back(dropItems[i]);
			}
			break;
			case PROPTYPE_RESOURCE:
			case PROPTYPE_HEAD:
			case PROPTYPE_HEROFRAGMENT:
			{
				resourceItems.push_back(dropItems[i]);
			}
			break;
			default:
			{
				bagItems.push_back(dropItems[i]);
			}
			break;
		}
	}
}

void CCommonHelper::classifyItems(const std::vector<DropItemInfo> &dropItems,
    std::vector<DropItemInfo> &bagItems, std::vector<DropItemInfo> &otherItems)
{
    for (unsigned int i = 0; i < dropItems.size(); ++i)
    {
		const PropItem *pPropConf = queryConfProp(dropItems[i].id);
        if (NULL == pPropConf)
        {
			KXLOGERROR("Execute False On File %s Line %d : %d", __FILE__, __LINE__, dropItems[i].id);
        }
        else
        {
            switch (pPropConf->Type)
            {
            case PROPTYPE_HEROCARD:
            case PROPTYPE_SUMMONERCARD:
            case PROPTYPE_RESOURCE:
            case PROPTYPE_HEAD:
            case PROPTYPE_HEROFRAGMENT:
				otherItems.push_back(dropItems[i]);
                break;
            default:
                // 将需要放到背包中的挪到bagItems中
				bagItems.push_back(dropItems[i]);
                break;
            }
        }
    }
}
int CCommonHelper::getCurMinTime()
{
//	LARGE_INTEGER liTime, liFreq;
//	QueryPerformanceFrequency(&liFreq);
//	QueryPerformanceCounter(&liTime);
//	int nSec = (long)(liTime.QuadPart / liFreq.QuadPart);
//	int nUSec = (long)(liTime.QuadPart * 1000000.0 / liFreq.QuadPart - nSec * 1000000.0);
//
//	int nMinSec = (nSec * 1000) + (nUSec / 1000);
//
//	return nMinSec;
		return 0;
}

void CCommonHelper::getCurTimeStr(char *pBuf, int nLen)
{
	if (pBuf == NULL || nLen < 64)
	{
		return;
	}

	memset(pBuf, 0, nLen);
	time_t t;  //秒时间 
	t = time(NULL); //获取目前秒时间  
	tm Ctm;
	stlocaltime(&t, &Ctm);
	strftime(pBuf, 64, "%Y-%m-%d %H:%M:%S", &Ctm);
}

//获取当前总天数
int CCommonHelper::getTotalDay(int nTime)
{
	time_t TotalSec = 0;
	if (nTime == 0)
	{
		TotalSec = time(NULL); //获取目前秒时间
	}
	else
	{
		TotalSec = static_cast<time_t>(nTime);
	}
	
	//当前时区总秒数
	TotalSec = TotalSec + TIME_SQUARE_INTERVAL;

	return (int)TotalSec / DAYOFSECOND;
}

void CCommonHelper::encryptProtocolBuff(int mainCmd, int subCmd, char *buff, int len)
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

const STalentArrangeData * CCommonHelper::getRaceTalentArrange(int race)
{
    int nRaceTalent = 0;
    switch (race)
    {
    case 1:
        nRaceTalent = TALENT_HUMAN;
        break;
    case 2:
        nRaceTalent = TALENT_NATURE;
        break;
    case 3:
        nRaceTalent = TALENT_GHOST;
        break;
    }

    return queryTalentArrangeData(nRaceTalent);
}

const STalentArrangeData * CCommonHelper::getVocationTalentArrange(int vocation)
{
    int nVocationTalent = 0;
    switch (vocation)
    {
    case 1:
        nVocationTalent = TALENT_SOLIDER;
        break;
    case 2:
        nVocationTalent = TALENT_ASSASSIN;
        break;
    case 3:
        nVocationTalent = TALENT_SHOOT;
        break;
    case 4:
        nVocationTalent = TALENT_MAGIC;
        break;
    case 5:
        nVocationTalent = TALENT_ASSIST;
        break;
    case 6:
        nVocationTalent = TALENT_BODYGUARD;
        break;
    }

    return queryTalentArrangeData(nVocationTalent);
}

const ArenaChestItem* CCommonHelper::getArenaChest(int pvpLv, int userLv, int quality)
{
	// 默认普通宝箱
	int chestType = 1;
	int chestLevel = pvpLv;
	// 获得段位对应宝箱列表
	CConfArenaChest *pArenaChestConf = reinterpret_cast<CConfArenaChest*>(
		CConfManager::getInstance()->getConf(CONF_ARENA_CHEST));
	const ArenaSettingItem *pArenaSettingConf = queryArenaSetting();
	if (NULL == pArenaChestConf || NULL == pArenaSettingConf)
	{
		return NULL;
	}
	// 获得活动宝箱概率, 是否是活动宝箱
	int randActivity = rand() % 100;
	if (randActivity < pArenaSettingConf->ActivityChestRate)
	{
		chestType = 2;
	}
	// 配置表所有的宝箱
	const std::vector<ArenaChestItem*>& vecChests = pArenaChestConf->getAreanChest(chestType, chestLevel);
	if (vecChests.empty())
	{
		KXLOGERROR("Arena_Chest.csv config chest empty! type=%d, level=%d", chestType, chestLevel);
		return NULL;
	}
	// 过滤之后的宝箱
	// 过滤品质列表 & 过滤等级
	std::vector<ArenaChestItem*> chooseChests;
	for (std::vector<ArenaChestItem*>::const_iterator iter = vecChests.begin();
		iter != vecChests.end(); ++iter)
	{
		if (quality == (*iter)->ChestQuality
			&& userLv >= (*iter)->ChestAppearLevel[0] 
			&& userLv <= (*iter)->ChestAppearLevel[1])
		{
			chooseChests.push_back(*iter);
		}
	}

	if (chooseChests.empty())
	{
		KXLOGERROR("Arena_Chest.csv config chest empty! chesttype=%d, quality=%d, usrlevel=%d", chestType, quality, userLv);
		return NULL;
	}
	// 根据权值获得一个宝箱
	int totalRate = 0;
	for (std::vector<ArenaChestItem*>::iterator iter = chooseChests.begin();
		iter != chooseChests.end(); ++iter)
	{
		totalRate += (*iter)->ChestWeight;
	}

	int rate = rand() % totalRate;
	totalRate = 0;
	for (std::vector<ArenaChestItem*>::iterator iter = chooseChests.begin();
		iter != chooseChests.end(); ++iter)
	{
		totalRate += (*iter)->ChestWeight;
		if (rate <= totalRate)
		{
			return *iter;
		}
	}
	return NULL;
}

const int CCommonHelper::getArenaDrop(int userLv, int dayWinTimes)
{
	const ArenaSettingItem *pArenaSettingConf = queryArenaSetting();
	if (NULL == pArenaSettingConf)
	{
		return 0;
	}
	int limit1 = 10;
	int limit2 = 20;
	int dropIndex = 0;
	if (pArenaSettingConf->WinTimesLimit.size() >= 2)
	{
		limit1 = pArenaSettingConf->WinTimesLimit[0];
		limit2 = pArenaSettingConf->WinTimesLimit[1];
	}
	if (dayWinTimes <= limit1)
	{
		dropIndex = 0;
	}
	else if (dayWinTimes <= limit2)
	{
		dropIndex++;
	}
	else
	{
		dropIndex++;
	}
	for (int i = 0; i < pArenaSettingConf->WinDrops.size(); ++i)
	{
		const ArenaWinDrop &winDrop = pArenaSettingConf->WinDrops[i];
		if (userLv >= winDrop.WinLevelLimit[0]
			&& userLv <= winDrop.WinLevelLimit[1])
		{
			return winDrop.DropId[dropIndex];
		}
	}
	return 0;
}
