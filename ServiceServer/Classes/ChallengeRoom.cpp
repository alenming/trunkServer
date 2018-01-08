#include "ChallengeRoom.h"
#include "GameDef.h"
#include "ConfStage.h"
#include "ConfHall.h"
#include "ConfGameSetting.h"

#include "StageProtocol.h"
#include "InstanceProtocol.h"
#include "TowerTestProtocol.h"
#include "HeroTestProtocol.h"

#include "GameUserManager.h"
#include "ModelHelper.h"
#include "PropUseHelper.h"
#include "ItemDrop.h"
#include "CommStructs.h"
#include "KxMemPool.h"
#include <time.h>
#include "RankModel.h"
#include "CommOssHelper.h"

CChallengeRoom::CChallengeRoom()
: m_nUid(0)
, m_nStageId(0)
, m_nStageLevel(0)
, m_nBattleType(0)
, m_nExt1(0)
, m_nExt2(0)
, m_nBeginTime(0)
, m_nEndTime(0)
, m_nSummonerId(0)
, m_nHPPercent(0)
, m_nUseCrystal(0)
, m_nTick(0)
, m_nStar2Reason(0)
, m_nStar3Reason(0)
, m_pUserInfo(NULL)
, m_pStageConf(NULL)
{
    m_HeroList.clear();
	m_vectBuffID.clear();
}

CChallengeRoom::~CChallengeRoom()
{
}

bool CChallengeRoom::init(int uid, int battleType, int stageId, int stageLevel)
{
	m_nUid = uid;
	m_nStageId = stageId;
	m_nStageLevel = stageLevel;
	m_nBattleType = battleType;
	m_pUserInfo = CGameUserManager::getInstance()->getGameUser(uid);
	if (NULL == m_pUserInfo)
	{
		KXLOGDEBUG("CChallengeRoom::init NULL == m_pUserInfo");
		return false;
	}
	m_pStageConf = queryConfStage(m_nStageId);
	if (NULL == m_pStageConf)
	{
		KXLOGDEBUG("CChallengeRoom::init NULL == m_pStageConf");
		return false;
	}
	return true;
}

bool CChallengeRoom::setSummonerId(int summonerId)
{
	m_nSummonerId = summonerId;
	return true;
}

bool CChallengeRoom::addHeroId(int heroId)
{
	m_HeroList.push_back(heroId);
	return true;
}

void CChallengeRoom::addBuff(int nID)
{
	m_vectBuffID.push_back(nID);
}

void CChallengeRoom::getRoomData(char *data, int &len)
{
	//房间信息
	RoomData *pRoomData = reinterpret_cast<RoomData *>(data);
	pRoomData->BattleType = m_nBattleType;
	pRoomData->StageId = m_nStageId;
	pRoomData->StageLevel = m_nStageLevel;
	pRoomData->Ext1 = m_nExt1;
	pRoomData->Ext2 = m_nExt2;
	len += sizeof(RoomData);
	//玩家信息
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(m_pUserInfo->getModel(MODELTYPE_USER));
	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(m_pUserInfo->getModel(MODELTYPE_HERO));
	CEquipModel *pEquipModel = dynamic_cast<CEquipModel*>(m_pUserInfo->getModel(MODELTYPE_EQUIP));
    CHECK_RETURN_VOID(NULL != pUserModel && NULL != pHeroModel && NULL != pEquipModel);

	pRoomData->PlayerCount = 1;
	PlayerData *pPlayerData = reinterpret_cast<PlayerData *>(pRoomData + 1);
	pPlayerData->Camp = /*ECamp_Blue*/1;
	pPlayerData->UserId = m_nUid;
	pUserModel->GetUserFieldVal(USR_FD_USERLV, pPlayerData->UserLv);
	std::string name = pUserModel->GetName();
	memset(pPlayerData->UserName, 0, sizeof(pPlayerData->UserName));
	strncpys(pPlayerData->UserName, name.c_str(), strlen(name.c_str()));
	len += sizeof(PlayerData);

	pPlayerData->OuterBonusCount = m_vectBuffID.size();
	for (std::vector<int>::iterator iter = m_vectBuffID.begin();
		iter != m_vectBuffID.end(); ++iter)
	{
		int * pBuffID = (int *)(data + len);
		*pBuffID = *iter;
		len += sizeof(int);
	}

	pPlayerData->SoldierCount = m_HeroList.size();
	//玩家召唤师信息
	HeroCardData *pHeroCard = reinterpret_cast<HeroCardData*>(data + len);
	pHeroCard->HeroId = m_nSummonerId;

	len += sizeof(HeroCardData);

	//玩家英雄信息
	for (unsigned int i = 0; i < m_HeroList.size(); ++i)
	{
		SoldierCardData *pSoldierCard = reinterpret_cast<SoldierCardData*>(data + len);
		int heroId = m_HeroList[i];
		DBHeroAttr heroInfo;
		if (!pHeroModel->GetHero(heroId, heroInfo))
		{
			continue;
		}

		pSoldierCard->SoldierId = heroId;
		pSoldierCard->SoldierExp = heroInfo.exp;
		pSoldierCard->SoldierLv = heroInfo.level;
		pSoldierCard->SoldierStar = heroInfo.star;
        memcpy(pSoldierCard->SoldierTalent, heroInfo.talent, sizeof(heroInfo.talent));
		pSoldierCard->EquipCnt = 6;
		len += sizeof(SoldierCardData);
		//装备信息
		pSoldierCard->EquipCnt = 0;
		for (int i = WEAPON; i < TREASURE + 1; ++i)
		{
			int equipId = heroInfo.equipId[i];
			if (equipId <= 0)
			{
				continue;
			}
			SoldierEquip *pEquipData = reinterpret_cast<SoldierEquip*>(data + len);
			SEquipInfo *pEquipInfo = pEquipModel->GetEquipData(equipId);
			if (pEquipInfo == NULL)
			{
				KXLOGERROR("CChallengeRoom::getRoomData pEquipInfo == NULL");
				return;
			}
			pEquipData->confId = pEquipInfo->nItemID;

			for (int i = 0; i < MAX_EQUIP_EFFECT_NUM; i++)
			{
				pEquipData->cEffectID[i] = static_cast<unsigned char>(pEquipInfo->cEffectID[i]);
				pEquipData->sEffectValue[i] = static_cast<unsigned short>(pEquipInfo->sEffectValue[i]);
			}
			pSoldierCard->EquipCnt += 1;
			len += sizeof(SoldierEquip);
		}
	}
}

int CChallengeRoom::getRoomDataLength()
{
	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(m_pUserInfo->getModel(MODELTYPE_HERO));
	int length = sizeof(RoomData);
	length += sizeof(PlayerData);
	for (std::vector<int>::iterator iter = m_vectBuffID.begin();
		iter != m_vectBuffID.end(); ++iter)
	{
		length += sizeof(int);
	}

	length += sizeof(HeroCardData);
	length += sizeof(SoldierCardData)* m_HeroList.size();
	for (unsigned int i = 0; i < m_HeroList.size(); ++i)
	{
		int heroId = m_HeroList[i];
		DBHeroAttr heroInfo;
		if (!pHeroModel->GetHero(heroId, heroInfo))
		{
			continue;
		}
		for (int i = WEAPON; i < TREASURE + 1; ++i)
		{
			if (heroInfo.equipId[i] > 0)
			{
				length += sizeof(SoldierEquip);
			}
		}
	}

	return length;
}

void CChallengeRoom::challengeBegin()
{
	m_nBeginTime = static_cast<int>(time(NULL));
}

void CChallengeRoom::challengeEnd()
{
	m_nEndTime = static_cast<int>(time(NULL));
}

void CChallengeRoom::cancelChallenge()
{

}

int CChallengeRoom::getStarInfo()
{
	int star = 1;
	int battleTime = 0;
	// 第一颗星
	switch (m_pStageConf->WinStar1)
	{
	case 1:
		if (m_nHPPercent >= m_pStageConf->WinStar1Param)
		{
			star += 1;
			m_nStar2Reason = m_pStageConf->WinStar1;
		}
		break;
	case 2:
		battleTime = m_nTick;
		if (battleTime <= m_pStageConf->WinStar1Param)
		{
			star += 1;
			m_nStar2Reason = m_pStageConf->WinStar1;
		}
		break;
	case 3:
		if (m_nUseCrystal <= m_pStageConf->WinStar1Param)
		{
			star += 1;
			m_nStar2Reason = m_pStageConf->WinStar1;
		}
		break;
	default:
		break;
	}
	// 第2颗星
	switch (m_pStageConf->WinStar2)
	{
	case 1:
		if (m_nHPPercent >= m_pStageConf->WinStar2Param)
		{
			star += 1;
			m_nStar3Reason = m_pStageConf->WinStar2;
		}
		break;
	case 2:
		battleTime = m_nTick;
		if (battleTime <= m_pStageConf->WinStar2Param)
		{
			star += 1;
			m_nStar3Reason = m_pStageConf->WinStar2;
		}
		break;
	case 3:
		if (m_nUseCrystal <= m_pStageConf->WinStar2Param)
		{
			star += 1;
			m_nStar3Reason = m_pStageConf->WinStar2;
		}
		break;
	default:
		break;
	}
	return star;
}

CInstanceRoom::CInstanceRoom()
: m_nInstanceId(0)
, m_nDifficulty(0)
, m_pInstanceConf(NULL)
{
}

CInstanceRoom::~CInstanceRoom()
{
}

bool CInstanceRoom::init(int uid, int instanceId, int difficulty)
{
	m_nInstanceId = instanceId;
	m_nDifficulty = difficulty;
	m_pInstanceConf = queryConfActivityInstance(instanceId);
	if (NULL == m_pInstanceConf)
	{
		KXLOGDEBUG("CInstanceRoom::init NULL == m_pInstanceConf");
		return false;
	}

	int stageId = 0;
	if (difficulty < (int)m_pInstanceConf->Diffcult.size())
	{
		stageId = m_pInstanceConf->Diffcult[difficulty].DiffID;
	}

	int stageLevel = 0;
	CGameUser *pUserInfo = CGameUserManager::getInstance()->getGameUser(uid);
	if (NULL != pUserInfo)
	{
		CUserModel *pUserModel = dynamic_cast<CUserModel*>(pUserInfo->getModel(MODELTYPE_USER));
		int userLevel = 0;
		pUserModel->GetUserFieldVal(USR_FD_USERLV, userLevel);
		stageLevel = userLevel + m_pInstanceConf->Diffcult[difficulty].ExLevel;
		if (stageLevel > m_pInstanceConf->Diffcult[difficulty].MaxLevel)
		{
			stageLevel = m_pInstanceConf->Diffcult[difficulty].MaxLevel;
		}
		else if (stageLevel < m_pInstanceConf->Diffcult[difficulty].BasicLevel)
		{
			stageLevel = m_pInstanceConf->Diffcult[difficulty].BasicLevel;
		}
	}
	return CChallengeRoom::init(uid, EBATTLE_INSTANCE, stageId, stageLevel);
}

void CInstanceRoom::challengeEnd()
{
	CChallengeRoom::challengeEnd();
	//扣挑战次数

	//获得副本奖励
	getRewardData();
	//副本结算
	CModelHelper::finishInstance(m_nUid, m_nInstanceId, m_nDifficulty, m_nFinalStar);
}

void CInstanceRoom::getRewardData()
{
	int dropId = 0;
	//星星信息
	m_nFinalStar = getStarInfo();
	//获得配置物品
	std::vector<DropItemInfo> dropItems;
	//掉落id
	dropId = m_pStageConf->ItemDrop[m_nFinalStar - 1];
	CItemDrop::Drop(dropId,dropItems);
	//只会有一个, 生成多的丢弃
	CPropUseHelper::getInstance()->AddItems(m_nUid, dropItems, true, WORDSTAGE_GAIN_REASON);

	int nSendLen = sizeof(InstanceFinishSC)
		+sizeof(StageReward)
		+sizeof(int)* 2 //2个星级原因
		+sizeof(DropItemInfo)*dropItems.size();

	//分配内存
	char *pSendBuffer = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(nSendLen));
	InstanceFinishSC *pFinishSC = reinterpret_cast<InstanceFinishSC *>(pSendBuffer);
	pFinishSC->activityId = m_nInstanceId;
	pFinishSC->difficulty = m_nDifficulty;
	pFinishSC->wonOrFailed = CHALLENGE_WON;

	StageReward *pStageReward = reinterpret_cast<StageReward*>(pFinishSC + 1);
	pStageReward->star = m_nFinalStar;
	pStageReward->star2Reason = m_nStar2Reason;
	pStageReward->star3Reason = m_nStar3Reason;
	pStageReward->rewardCount = dropItems.size();

	//这两个星星的获得理由
	int *pStarReason = reinterpret_cast<int*>(pStageReward + 1);

	//具体的物品信息
	DropItemInfo *pPropInfo = reinterpret_cast<DropItemInfo *>(pStarReason + 1);
	for (std::vector<DropItemInfo>::iterator iter = dropItems.begin();
		iter != dropItems.end(); ++iter)
	{
		memcpy(pPropInfo, &(*iter), sizeof(DropItemInfo));
		pPropInfo += 1;
	}
	//发送
// 	CServiceHelper::SentMsg(m_nUid, CMD_INSTANCE, CMD_INSTANCE_FINISH_SC,
// 		pSendBuffer, nSendLen, NULL);
	//内存回收
	KxServer::kxMemMgrRecycle(pSendBuffer, nSendLen);
}
