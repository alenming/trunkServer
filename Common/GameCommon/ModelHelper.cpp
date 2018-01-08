#include "ModelHelper.h"
#include "RandGenerator.h"
#include "UserActionListener.h"
#include "GameUserManager.h"
#include "KxCommManager.h"
#include "MailHelper.h"
#include "TaskAchieveHelper.h"
#include "CommOssHelper.h"
#include "ItemDrop.h"
#include "TimeCalcTool.h"

#include "GameDef.h"
#include "ModelDef.h"
#include "AchievementModel.h"
#include "BlueDiamondModel.h"
#include "RankModel.h"
#include "PvpRankModel.h"
#include "ShopModel.h"
#include "BagModel.h"
#include "EquipModel.h"
#include "GoldTestModel.h"
#include "HeroModel.h"
#include "MailModel.h"
#include "PassTeamModel.h"
#include "StageModel.h"
#include "GuideModel.h"
#include "SummonModel.h"
#include "TaskModel.h"
#include "TeamModel.h"
#include "HeroTestModel.h"
#include "TowerTestModel.h"
#include "UnionModel.h"
#include "UserModel.h"
#include "PvpModel.h"
#include "PvpTaskModel.h"
#include "GlobalMailModel.h"
#include "UnionMercenaryModel.h"
#include "PersonMercenaryModel.h"
#include "ActivityInstanceModel.h"
#include "StorageManager.h"
#include "RedisStorer.h"

#include "Protocol.h"
#include "ErrorCodeProtocol.h"
#include "PvpChestProtocol.h"
#include "UnionProtocol.h"
#include "ConfGameSetting.h"
#include "ConfHall.h"
#include "ConfRole.h"
#include "ConfStage.h"
#include "ConfArena.h"
#include "CommonHelper.h"
#include <time.h>

using namespace std;
using namespace KxServer;

const static int MAX_DAY_LIVENESS = 100;

IDBModel *CModelHelper::getModel(int uid, int modelType)
{
	CGameUser *pUserInfo = CGameUserManager::getInstance()->getGameUser(uid);
	if (NULL == pUserInfo)
	{
		return NULL;
	}

	return pUserInfo->getModel(modelType);
}

bool CModelHelper::addGold(int uid, int gold)
{
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL == pUserModel)
	{
		return false;
	}
	int haveGold = 0;
	if (!pUserModel->GetUserFieldVal(USR_FD_GOLD, haveGold, true))
	{
		return false;
	}
	
    if (0 == gold)
    {
        return true;
    }

	int nGoldCost = 0;
	if (gold < 0)
	{
		nGoldCost = -1 * gold;
	}

    if (pUserModel->SetUserFieldVal(USR_FD_GOLD, haveGold + gold)
        && pUserModel->AddUserFieldVal(USR_FD_ACCUMULATEGOLD, gold))
    {
        DispatchActionEvent(uid, ELA_ACCUMULATE_GOLD, &gold, sizeof(int));

		if (nGoldCost != 0)
		{
			DispatchActionEvent(uid, ELA_GOLDCOSTNUM, &nGoldCost, sizeof(nGoldCost));
		}
        return true;
    }

    return false;
}

bool CModelHelper::addExp(int uid, int exp)
{
    int newLv = 0, newExp = 0;
    return addExp(uid, exp, newLv, newExp);
}

bool CModelHelper::addExp(int uid, int exp, int &newLv, int &newExp)
{
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL == pUserModel)
	{
		return false;
	}

	int userLv = 0;
	int userExp = 0;
	if (!pUserModel->GetUserFieldVal(USR_FD_USERLV, userLv, true)
		|| !pUserModel->GetUserFieldVal(USR_FD_EXP, userExp, true))
	{
		return false;
	}

	userExp += exp;
    int oldLv = userLv;
	CConfUserLevelSetting *conf = dynamic_cast<CConfUserLevelSetting*>(
		CConfManager::getInstance()->getConf(CONF_USER_LEVEL_SETTING));
	// 最高等级
	int nUserMaxLv = conf->GetUserMaxLv();
	const UserLevelSettingItem *pUserLevelItem = static_cast<UserLevelSettingItem*>(conf->getData(userLv));
	while (pUserLevelItem)
	{
		if (userExp >= pUserLevelItem->Exp)
		{
			if (userLv >= nUserMaxLv)
			{
				userExp = pUserLevelItem->Exp;
				break;
			}
			else
			{
				userExp -= pUserLevelItem->Exp;
				userLv += 1;
			}

			pUserLevelItem = queryConfUserLevel(userLv);
		}
		else
		{
			// 经验不够升级
			break;
		}
	}

	if (!pUserModel->SetUserFieldVal(USR_FD_EXP, userExp) || !pUserModel->SetUserFieldVal(USR_FD_USERLV, userLv))
	{
		return false;
	}

    if (oldLv != userLv)
    {
        DispatchActionEvent(uid, ELA_USER_LEVEL_UP, &(userLv), sizeof(int));
		DispatchActionEvent(uid, ELA_FUND,NULL,0);
		CRankModel::getInstance()->AddRankData(LEVEL_RANK_TYPE, uid, userLv);
		CCommOssHelper::userLevelUpOss(uid);

        UnionForwardSS unionForwardSS;
        memset(&unionForwardSS, 0, sizeof(UnionForwardSS));
        unionForwardSS.type = UNION_FORWARD_USERLV;
        unionForwardSS.val = userLv;
        // 转发到公会服务器
        CKxCommManager::getInstance()->forwardData(uid, CMD_UNION, CMD_UNION_FORWARD_SS,
            reinterpret_cast<char*>(&unionForwardSS), sizeof(UnionForwardSS));
    }

	return true;
}

bool CModelHelper::addDiamond(int uid, int diamond,int nParam1,int nParam2)
{
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL == pUserModel)
	{
		return false;
	}
	int haveDiamond = 0;
	if (!pUserModel->GetUserFieldVal(USR_FD_DIAMOND, haveDiamond, true))
	{
		return false;
	}
	 
	if (!pUserModel->SetUserFieldVal(USR_FD_DIAMOND, haveDiamond + diamond))
	{
		return false;
	}

	if (diamond < 0)
	{
		int nCostdiamond = -1 * diamond;
		DispatchActionEvent(uid, ELA_DIAMONDCOSTNUM,&nCostdiamond,sizeof(nCostdiamond));
	}

	return true;
}
/*
bool CModelHelper::addEnergy(int uid, int count)
{
	int nCurEnergy = 0;
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	CModelHelper::updateUserModelReset(uid);
	if (!pUserModel->GetUserFieldVal(USR_FD_ENERGY, nCurEnergy, true))
	{
		return false;
	}

    if (count < 0)
    {
        if (!addDayLiveness(uid, UNION_DAY_STAGELIVENESS, -count))
        {
            KXLOGERROR("add day energyliveness error, uid %d energy %d", uid, -count);
        }
    }

	return pUserModel->AddUserFieldVal(USR_FD_ENERGY, count);
}

int CModelHelper::getEnergy(int uid)
{
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL != pUserModel)
	{
		int energy = 0;
		CModelHelper::updateUserModelReset(uid);
		pUserModel->GetUserFieldVal(USR_FD_ENERGY, energy);
		return energy;
	}
	return 0;
}*/

bool CModelHelper::addPvpCoin(int uid, int pvpCoin)
{
    CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
    if (NULL == pUserModel)
    {
        return false;
    }

	int nPvpCoinCost = 0;
	if (pvpCoin < 0)
	{
		nPvpCoinCost = -1 * pvpCoin;
	}

	if (!pUserModel->AddUserFieldVal(USR_FD_PVPCOIN, pvpCoin))
	{
		return false;
	}

	if (nPvpCoinCost != 0)
	{
		DispatchActionEvent(uid, ELA_PVPCOINCOSTNUM, &nPvpCoinCost, sizeof(nPvpCoinCost));
	}

    return true;
}

bool CModelHelper::addTowerCoin(int uid, int towerCoin)
{
    CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
    if (NULL == pUserModel)
    {
        return false;
    }

	int nTowerCoinCost = 0;
	if (towerCoin < 0)
	{
		nTowerCoinCost = -1 * towerCoin;
	}

	if (!pUserModel->AddUserFieldVal(USR_FD_TOWERCOIN, towerCoin))
	{
		return false;
	}

	if (nTowerCoinCost != 0)
	{
		DispatchActionEvent(uid, ELA_TOWERCOINCOSTNUM, &nTowerCoinCost, sizeof(nTowerCoinCost));
	}

    return true;
}

bool CModelHelper::addGuildContrib(int uid, int guildContrib)
{
    CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
    CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(getModel(uid, MODELTYPE_USERUNION));
    CHECK_RETURN(NULL != pUserModel && NULL != pUserUnionModel);
    if (!pUserModel->AddUserFieldVal(USR_FD_UNIONCONTRIB, guildContrib))
    {
        KXLOGERROR("CModelHelper::addGuildContrib, UserModel add contrib error!uid:%d, guildContrib:%d", uid, guildContrib);
    }

    // 累计贡献
    if (guildContrib > 0)
    {
        if (!pUserUnionModel->addUserUnionValue(USER_UNION_CONTRIBUTION, guildContrib))
        {
            KXLOGERROR("CModelHelper::addGuildContrib, UserUnionModel add contrib error!uid:%d, guildContrib:%d", uid, guildContrib);
        }
        else
        {
            UnionForwardSS unionForwardSS;
            memset(&unionForwardSS, 0, sizeof(UnionForwardSS));
            unionForwardSS.type = UNION_FORWARD_CONTRIBUTION;
            unionForwardSS.val = guildContrib;
            // 转发到公会服务器
            CKxCommManager::getInstance()->forwardData(uid, CMD_UNION, CMD_UNION_FORWARD_SS,
                reinterpret_cast<char*>(&unionForwardSS), sizeof(UnionForwardSS));
        }
    }

    return true;
}

bool CModelHelper::addFlashCard(int uid, int flashcard)
{
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL == pUserModel)
	{
		return false;
	}

	return pUserModel->AddUserFieldVal(USR_FD_FLASHCARD, flashcard);
}

bool CModelHelper::addFlashCard10(int uid, int flashcard10)
{
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL == pUserModel)
	{
		return false;
	}

	return pUserModel->AddUserFieldVal(USR_FD_FLASHCARD10, flashcard10);
}

std::string CModelHelper::getUserName(int uid)
{
	std::string userName = "";
    /*CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
    if (NULL != pUserModel) // 在线
    {
        userName = pUserModel->GetName();
    }
    else
    {*/
        Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_USER);
        CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(pStorage->GetStorer(uid));
        if (NULL != pStorer)
        {
            std::string key = ModelKey::UsrKey(uid);
            if (SUCCESS == pStorer->ExistKey(key)
                && SUCCESS == pStorer->GetHashByField(key, USR_FD_USERNAME, userName))
            {
                return userName;
            }
        }
    //}

	return userName;
}

bool CModelHelper::getUserInfo(int uid, UserBasicInfo &info)
{
    IKxComm * pKxComm = CKxCommManager::getInstance()->getKxComm(uid);
    if (NULL != pKxComm) // 在线
    {
        CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
        CHECK_RETURN(NULL != pGameUser);
        CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
        CHECK_RETURN(NULL != pUserModel);
        pUserModel->GetUserFieldVal(USR_FD_USERLV, info.userLv, true);
        pUserModel->GetUserFieldVal(USR_FD_LOGINTIME, info.lastLoginTime, true);
        info.name = pUserModel->GetName();
        info.identity = pUserModel->getIdentity();
    }
    else
    {
        Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_USER);
        CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(pStorage->GetStorer(uid));
        if (NULL != pStorer)
        {
            std::string key = ModelKey::UsrKey(uid);
            std::map<int, int> mapUserInfo;
            if (SUCCESS != pStorer->ExistKey(key)
                || SUCCESS != pStorer->GetHash(key, mapUserInfo)
                || SUCCESS != pStorer->GetHashByField(key, USR_FD_USERNAME, info.name))
            {
                return false;
            }
            
            // 蓝钻信息
            std::string QQVipKey = ModelKey::ckExtraDataKey(uid);
            std::string strQQExtraData;
            info.identity = 0;
            //蓝钻
            if (SUCCESS == pStorer->GetString(QQVipKey, strQQExtraData))
            {
                //有蓝钻，数据必须合法
                VecInt VectValue;
                strQQExtraData = "[" + strQQExtraData + "]";
                CConfAnalytic::ToJsonInt(strQQExtraData, VectValue);
                if (VectValue.size() == 3)
                {
                    if (time(NULL) > VectValue[2])
                    {
                        info.identity = 0;
                    }
                    else
                    {
                        info.identity = VectValue[1] * 10 + VectValue[0];
                    }
                }
            }

            info.userLv = mapUserInfo[USR_FD_USERLV];
            info.lastLoginTime = mapUserInfo[USR_FD_LOGINTIME];
        }
    }

	return true;
}

bool CModelHelper::getUserUnionInfo(int uid, UserUnionBasicInfo &info)
{
    IKxComm * pKxComm = CKxCommManager::getInstance()->getKxComm(uid);
    int nUnionRestStamp = 0;
    if (NULL != pKxComm) // 在线
    {
        CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
        CHECK_RETURN(NULL != pGameUser);
        CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
        CHECK_RETURN(NULL != pUserUnionModel);
        pUserUnionModel->getUserUnionValue(USER_UNION_DAYPVPLIVENESS, info.todayPvpLiveness);
        pUserUnionModel->getUserUnionValue(USER_UNION_DAYSTAGELIVENESS, info.todayStageLiveness);
        pUserUnionModel->getUserUnionValue(USER_UNION_CONTRIBUTION, info.totalContribution);
        pUserUnionModel->getUserUnionValue(USER_UNION_RESETSTAMP, nUnionRestStamp);
    }
    else
    {
        Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_UNION);
        CRedisStorer *pStorer = reinterpret_cast<CRedisStorer*>(pStorage->GetStorer(uid));
        if (NULL != pStorer)
        {
            std::string key = ModelKey::UserUnionKey(uid);
            std::map<int, int> mapUserInfo;
            if (SUCCESS != pStorer->ExistKey(key)
                || SUCCESS != pStorer->GetHash(key, mapUserInfo))
            {
                return false;
            }

            info.todayPvpLiveness = mapUserInfo[USER_UNION_DAYPVPLIVENESS];
            info.todayStageLiveness = mapUserInfo[USER_UNION_DAYSTAGELIVENESS];
            info.totalContribution = mapUserInfo[USER_UNION_CONTRIBUTION];
            nUnionRestStamp = mapUserInfo[USER_UNION_RESETSTAMP];
        }
    }

    if (time(NULL) > nUnionRestStamp)
    {
        info.todayStageLiveness = 0;
        info.todayPvpLiveness = 0;
    }

    return true;
}
/*
bool CModelHelper::isEnergyEnough(int uid, int energy, int times)
{
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL == pUserModel)
	{
		return false;
	}

	int haveEnergy = CModelHelper::getEnergy(uid);

	if (haveEnergy < energy * times)
	{
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_BATTLE_ENERGYERROR;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
	}

	return haveEnergy >= energy * times;
}*/

bool CModelHelper::isUserLevelEnough(int uid, int needLv)
{
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL == pUserModel)
	{
		return false;
	}

	int userlevel = 0;
	pUserModel->GetUserFieldVal(USR_FD_USERLV, userlevel);
	return userlevel >= needLv;
}

//添加头像
bool CModelHelper::AddHeadID(int uid, int HeadID)
{
	const std::map<int, SSystemHeadIconItem>& mapAllHeadIcon = queryConfHeadIcon();

	//头像不存在
	if (mapAllHeadIcon.find(HeadID) == mapAllHeadIcon.end())
	{
		return false;
	}

	CHeadModel *pHeadModel = dynamic_cast<CHeadModel*>(getModel(uid, MODELTYPE_HEAD));
	if (NULL == pHeadModel)
	{
		return false;
	}

	return pHeadModel->AddHeadID(HeadID);
}

bool CModelHelper::addDayLiveness(int uid, int type, int count /*= 1*/)
{
    CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(getModel(uid, MODELTYPE_USERUNION));
    CHECK_RETURN(NULL != pUserUnionModel);
    int nUnionId = 0;
    CHECK_RETURN(pUserUnionModel->getUserUnionValue(USER_UNION_ID, nUnionId));

    CHECK_RETURN(nUnionId > 0 && count > 0);

    int nField = 0;
    if (UNION_DAY_PVPLIVENESS == type)
    {
        nField = USER_UNION_DAYPVPLIVENESS;
        count *= 10;
    }
    else if (UNION_DAY_STAGELIVENESS == type)
    {
        nField = USER_UNION_DAYSTAGELIVENESS;
    }
    else
    {
        return false;
    }

    updateUserUnionInfo(uid);

    int nDayLiveness = 0;
    CHECK_RETURN(pUserUnionModel->getUserUnionValue(nField, nDayLiveness));

    if (nDayLiveness < MAX_DAY_LIVENESS)
    {
        int nTempDayLiveness = nDayLiveness;
        nDayLiveness += count;
        if (nDayLiveness > MAX_DAY_LIVENESS)
        {
            nDayLiveness = MAX_DAY_LIVENESS;
        }

        CHECK_RETURN(pUserUnionModel->setUserUnionValue(nField, nDayLiveness));

        nTempDayLiveness = nDayLiveness - nTempDayLiveness;

        UnionForwardSS unionForwardSS;
        memset(&unionForwardSS, 0, sizeof(UnionForwardSS));
        unionForwardSS.type = UNION_DAY_PVPLIVENESS == type ? UNION_FORWARD_PVPLIVENESS : UNION_FORWARD_STAGELIVENESS;
        unionForwardSS.val = nTempDayLiveness;
        // 转发到公会服务器
        CKxCommManager::getInstance()->forwardData(uid, CMD_UNION, CMD_UNION_FORWARD_SS,
            reinterpret_cast<char*>(&unionForwardSS), sizeof(UnionForwardSS));
    }

    return true;
}

bool GetEquipExtraProp(vector<EffectData> &VectData, unsigned char &EffectID, unsigned short &EffectValue)
{
	int nTotalWeight = 0;
	vector<EffectData>::iterator ator;

	for (ator = VectData.begin(); ator != VectData.end(); ator++)
	{
		nTotalWeight = nTotalWeight + ator->nWeight;
	}

	int nTempWeight = g_RandGenerator.MakeRandNum(0, nTotalWeight);
	int nCurWeight = 0;
	ator = VectData.begin();
	for (; ator != VectData.end();)
	{
		nCurWeight += ator->nWeight;

		if (nCurWeight >= nTempWeight)
		{
			break;
		}
	}

	if (ator == VectData.end())
	{
		return false;
	}

	EffectID = static_cast<unsigned char>(ator->nEffectID);
	EffectValue = g_RandGenerator.MakeRandNum(ator->nMinValue, ator->nMaxValue);
	VectData.erase(ator);
	return true;
}


bool GeneralEquip(int nUid, int EquipCreateID, SEquipInfo& EquipInfo)
{
	CEquipModel *pEquipModel = dynamic_cast<CEquipModel*>(CModelHelper::getModel(nUid, MODELTYPE_EQUIP));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(nUid, MODELTYPE_USER));
	if (NULL == pEquipModel || NULL == pUserModel)
	{
		return false;
	}

	// 先修改装备id
	EquipInfo.nDnycEquipID = pUserModel->IncreaseFieldVal(USR_FD_EQUIPREF);
	if (EquipInfo.nDnycEquipID == 0)
	{
		return false;
	}

	const EquipPropCreate * pEquipPropCreate = queryConfEquipCreat(EquipCreateID);

	if (pEquipPropCreate == NULL)
	{
        LOG("Execute False On File %s Line %d : %d", __FILE__, __LINE__, EquipCreateID);
		return false;
	}

	const PropItem *pPropConf = queryConfProp(pEquipPropCreate->nEquipID);
	if (NULL == pPropConf)
	{
        LOG("Execute False On File %s Line %d : %d", __FILE__, __LINE__, pEquipPropCreate->nEquipID);
		return false;
	}

	int nPropNum = queryConfQualityProp(pPropConf->Quality);

	int nIndex = 0;
	bool bAddExtra = true;
	for (vector<EffectData>::const_iterator ator = pEquipPropCreate->VectBaseProp.begin();
		ator != pEquipPropCreate->VectBaseProp.end(); ator++)
	{
		EquipInfo.cEffectID[nIndex] = ator->nEffectID;
		EquipInfo.sEffectValue[nIndex] = g_RandGenerator.MakeRandNum(ator->nMinValue, ator->nMaxValue);
		nIndex++;

		if (nIndex >= nPropNum)
		{
			bAddExtra = false;
			break;
		}
	}
	//记录主属性个数
	EquipInfo.cMainPropNum = static_cast<unsigned char>(nIndex);

	if (bAddExtra && pEquipPropCreate->VectExtraProp.size() != 0)
	{
		vector<EffectData> VectTemp;
		VectTemp.assign(pEquipPropCreate->VectExtraProp.begin(), pEquipPropCreate->VectExtraProp.end());

		for (; nIndex < nPropNum;)
		{
			if (!GetEquipExtraProp(VectTemp, EquipInfo.cEffectID[nIndex], EquipInfo.sEffectValue[nIndex]))
			{
				break;
			}

			nIndex++;
		}
	}

	EquipInfo.nItemID = pEquipPropCreate->nEquipID;
	EquipInfo.nType = PROPTYPE_EQUIP;

	return true;
}

bool CModelHelper::addBagCapacity(int uid, int capacity)
{
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL != pUserModel)
	{
		return pUserModel->AddUserFieldVal(USR_FD_BAGCAPACITY, capacity);
	}
	return false;
}

bool CModelHelper::addHeroCapacity(int uid, int capacity)
{
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL != pUserModel)
	{
		return pUserModel->AddUserFieldVal(USR_FD_HEROCAPCITY, capacity);
	}
	return false;
}

bool CModelHelper::addEquip(int uid, int CreateId, SEquipInfo &equipData)
{
	CBagModel *pBagModel = dynamic_cast<CBagModel*>(getModel(uid, MODELTYPE_BAG));
	CEquipModel *pEquipModel = dynamic_cast<CEquipModel*>(getModel(uid, MODELTYPE_EQUIP));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	const EquipPropCreate * pEquipPropCreate = queryConfEquipCreat(CreateId);
	if (NULL == pBagModel || NULL == pEquipModel || NULL == pUserModel || NULL == pEquipPropCreate)
	{
		return false;
	}

	if (!GeneralEquip(uid,CreateId, equipData))
	{
		return false;
	}
	// 添加到背包
	if (!pBagModel->AddItem(equipData.nDnycEquipID, pEquipPropCreate->nEquipID))
	{
		//添加失败记录日志
		return false;
	}

	//背包当前占用格子数+1
	if (pBagModel->AddCapacity(1) < 0)
	{
		return false;
	}

	// 装备数据
	return pEquipModel->AddEquip(equipData);
}

bool CModelHelper::addEquips(int uid, std::vector<int>& equipConfIds, std::vector<int>& equipIds)
{
	CBagModel *pBagModel = dynamic_cast<CBagModel*>(getModel(uid, MODELTYPE_BAG));
	CEquipModel *pEquipModel = dynamic_cast<CEquipModel*>(getModel(uid, MODELTYPE_EQUIP));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL == pBagModel || NULL == pEquipModel || NULL == pUserModel)
	{
		return false;
	}

	// 先修改装备id
	int equipRef = pUserModel->IncreaseFieldVal(USR_FD_EQUIPREF, equipConfIds.size());
	if (equipRef == 0)
	{
		return false;
	}

	for (unsigned int i = 0; i < equipConfIds.size(); ++i)
	{
		int equipId = equipRef - i;
		if (!pBagModel->AddItem(equipId, equipConfIds[i]))
		{
			//添加失败记录日志
			return false;
		}

		//背包当前占用格子数+1
		if (pBagModel->AddCapacity(1) < 0)
		{
			return false;
		}

		equipIds.push_back(equipId);
	}

	return true;
}

bool CModelHelper::addItem(int uid, int itemConfId, int count)
{
	CBagModel *pBagModel = dynamic_cast<CBagModel*>(getModel(uid, MODELTYPE_BAG));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL == pBagModel || NULL == pUserModel)
	{
		return false;
	}

	if (!pBagModel->ExistItem(itemConfId))
	{
		//判断格子数是否足够	
		if (1 > CModelHelper::getBagStoreCapacity(uid))
		{
			return false;
		}

		if (pBagModel->AddCapacity(1) < 0)
		{
			return false;
		}
	}

	if (!pBagModel->AddItem(itemConfId, count))
	{
		return false;
	}

	return true;
}

bool CModelHelper::removeItem(int uid, int itemConfId, int count)
{
	CBagModel *pBagModel = dynamic_cast<CBagModel*>(getModel(uid, MODELTYPE_BAG));
    CEquipModel *pEquipModel = dynamic_cast<CEquipModel*>(getModel(uid, MODELTYPE_EQUIP));
    if (NULL == pBagModel && NULL == pEquipModel)
	{
		return false;
	}
	// 是否拥有该物品(包含装备),如果是装备获取的是ConfID,否则为个数
	int nVal = 0;
    if (pBagModel->GetItem(itemConfId, nVal))
    {
        if (itemConfId <= DEFAULT_EQUIPREF) // 大于则是装备
        {
            if (nVal > count)
            {
                return pBagModel->AddItem(itemConfId, -1 * count);
            }
        }
        else
        {
            if (!pEquipModel->RemoveEquip(itemConfId))
            {
                return false;
            }
        }

		if (!pBagModel->RemoveItem(itemConfId))
		{
			return false;
		}

		if (pBagModel->AddCapacity(-1) < 0)
		{
			return false;
		}

        return true;
	}
	else
	{
		ErrorCodeData CodeData;
		CodeData.nCode = ERROR_BAG_ITEMNOTEXIST;
		CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL, (char*)&CodeData, sizeof(CodeData));
	}
	
	return false;
}

int CModelHelper::getBagStoreCapacity(int uid)
{
	CBagModel *pBagModel = dynamic_cast<CBagModel*>(getModel(uid, MODELTYPE_BAG));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL == pBagModel || NULL == pUserModel)
	{
		return false;
	}

	int capacity = 0;
	int remainCapacity = 0;

	pUserModel->GetUserFieldVal(USR_FD_BAGCAPACITY, capacity);
	remainCapacity = capacity - pBagModel->GetCapacity();
	return remainCapacity;
}

bool CModelHelper::checkBagCapacity(int uid, const std::vector<DropItemInfo> &awards)
{
    CBagModel *pBagModel = dynamic_cast<CBagModel*>(getModel(uid, MODELTYPE_BAG));
    CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (pBagModel == NULL || pUserModel == NULL)
	{
		return false;
	}

    // 计算背包所需额外容量
	int nNeedBagCapacity = 0;
	int nCurMaxBagCapacity = 0;
	for (unsigned int i = 0; i < awards.size(); ++i)
	{
		const PropItem* pPropItem = queryConfProp(awards[i].id);
		if (NULL == pPropItem)
		{
            KXLOGERROR("checkBagCapacity queryConfProp %d Error", awards[i].id);
			continue;
		}
        switch (pPropItem->Type)
        {
        case PROPTYPE_HEROCARD:
        case PROPTYPE_SUMMONERCARD:
        case PROPTYPE_RESOURCE:
        case PROPTYPE_HEAD:
        case PROPTYPE_HEROFRAGMENT:
            break;
            // 装备占用
        case PROPTYPE_EQUIP:
            nNeedBagCapacity += awards[i].num;
            break;
        default:
            if (!pBagModel->ExistItem(awards[i].id))
            {
                ++nNeedBagCapacity;
            }
            break;
        }
	}

    // 不需要占用额外的空间
    if (nNeedBagCapacity <= 0)
    {
        return true;
    }

    // 背包有上限
    int nBagCapacity = 0;
    pUserModel->GetUserFieldVal(USR_FD_BAGCAPACITY, nBagCapacity);
	nCurMaxBagCapacity = pBagModel->AddCapacity(nNeedBagCapacity);
	pBagModel->AddCapacity(-1 * nNeedBagCapacity);
	if (nCurMaxBagCapacity > nBagCapacity)
	{
		return false;
	}
	return true;
}


bool CModelHelper::checkAllCapacity(int uid, const std::vector<DropItemInfo> &awards)
{
    std::vector<DropItemInfo> bagItems;
    std::vector<DropItemInfo> otherItems;
    CCommonHelper::classifyItems(awards, bagItems, otherItems);
	return CModelHelper::checkBagCapacity(uid, bagItems);
}

bool CModelHelper::isHeroCompletion(int uid, int heroId)
{
	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(getModel(uid, MODELTYPE_HERO));
	if (NULL == pHeroModel)
	{
		return false;
	}

	if (pHeroModel->ExsitHero(heroId))
	{
		DBHeroAttr heroAttr;
		if (!pHeroModel->GetHero(heroId, heroAttr))
		{
			return false;
		}
		//星级或等级大于0则为整卡
		return heroAttr.star > 0 || heroAttr.level > 0;
	}
	return false;
}

bool CModelHelper::addHeroFragment(int uid, int heroId, int fragment)
{
	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(getModel(uid, MODELTYPE_HERO));
	if (NULL == pHeroModel)
	{
		return false;
	}
	DBHeroAttr dbheroAttr;
	if (pHeroModel->GetHero(heroId, dbheroAttr))
	{
		int topStar = CCommonHelper::getHeroTopStar(heroId);
		if (dbheroAttr.star >= topStar)
		{
			//转成金币
			const CardGambleSettingItem& pTurnRateConf = queryConfCardGambleSetting();
			fragment *= pTurnRateConf.exchangeRatio;
			return addGold(uid, fragment);
		}
	}
	// 无论有无英雄, 直接增加英雄碎片
	return pHeroModel->UpdateHeroFragment(heroId, fragment);
}

bool CModelHelper::addHero(int uid, int heroId, int star, int lv)
{
	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(getModel(uid, MODELTYPE_HERO));
	if (NULL == pHeroModel)
	{
		return false;
	}

	DBHeroAttr heroAttr;
	// 如果是整卡, 转换成碎片或粉尘
	if (isHeroCompletion(uid, heroId))
	{
		const SoldierStarSettingItem * pSoldierStarConf = queryConfSoldierStarSetting(star);
		if (NULL == pSoldierStarConf)
		{
            KXLOGERROR("queryConfSoldierStarSetting %d error", star);
			return false;
		}
		// 转成碎片
		return addHeroFragment(uid, heroId, pSoldierStarConf->TurnFragCount);
	}
	else
	{	
		if (!pHeroModel->GetHero(heroId, heroAttr))
		{
			//如果之前没有数据, 碎片为0, 否则碎片为之前碎片
			heroAttr.fragment = 0;
		}

		heroAttr.star = star;
		heroAttr.level = lv;
		heroAttr.exp = 0;
	}

	if (!pHeroModel->SetHero(heroId, heroAttr))
	{
		return false;
	}

	DispatchActionEvent(uid, ELA_HAVE_HERO_X_STAR, NULL, 0);
	DispatchActionEvent(uid, ELA_OWN_DIFFEREN_HERO, NULL, 0);
	DispatchActionEvent(uid, ELA_OWNCOLORHERO, NULL, 0);
	return true;
}

bool CModelHelper::removeHero(int uid, int heroId)
{
	// 暂不会有删除操作
	// 如有, 1. 移除英雄装备; 2. 从组队界面移除

	return false;
}

bool CModelHelper::checkHeros(int uid, std::vector<int> &heroIds)
{
	CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(getModel(uid, MODELTYPE_HERO));
	if (NULL != pHeroModel)
	{
		for (unsigned int i = 0; i < heroIds.size(); ++i)
		{
			if (!pHeroModel->ExsitHero(heroIds[i]))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

bool CModelHelper::haveSummoner(int uid, int summonerId)
{
	CSummonModel *pSummonModel = dynamic_cast<CSummonModel*>(getModel(uid, MODELTYPE_SUMMONER));
	if (NULL == pSummonModel)
	{
		return false;
	}
	return pSummonModel->HaveSummoner(summonerId);
}

bool CModelHelper::addSummoner(int uid, int summonerId)
{
	CSummonModel *pSummonModel = dynamic_cast<CSummonModel*>(getModel(uid, MODELTYPE_SUMMONER));
	if (NULL == pSummonModel)
	{
		return false;
	}

	if (pSummonModel->HaveSummoner(summonerId))
	{
		return false;
	}
	return pSummonModel->AddSummon(summonerId);
}

bool CModelHelper::isFirstChllange(int uid, int chapterId, int stageId)
{
	CStageModel *pStageModel = dynamic_cast<CStageModel*>(getModel(uid, MODELTYPE_STAGE));
	const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
	CHECK_RETURN(NULL != pStageModel && NULL != pChapterConf);

	if (pChapterConf->Type == CHAPTERTYPE_NORMAL)
	{
		return pStageModel->GetStageStatus(stageId) <= STAGESTATUS_UNLOCK;
	}
	else if (pChapterConf->Type == CHAPTERTYPE_ELITE)
	{
		return pStageModel->GetEliteStatus(stageId) <= STAGESTATUS_UNLOCK;
	}

	return false;
}

bool CModelHelper::canChallengeStage(int uid, int chapterId, int stageId, int times)
{
	CStageModel *pStageModel = dynamic_cast<CStageModel*>(getModel(uid, MODELTYPE_STAGE));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
	const StageConfItem *pStageConf = queryConfStage(stageId);

	if (NULL == pStageModel || NULL == pUserModel 
		|| NULL == pChapterConf || pStageConf == NULL)
	{
		KXLOGERROR("NULL == pStageModel || NULL == pUserModel || NULL == pChapterConf || pStageConf == NULL");
		return false;
	}

 //   int needEnergy = CCommonHelper::getStageNeedEnergy(chapterId, stageId);
	////玩家体力
	//if (!CModelHelper::isEnergyEnough(uid, needEnergy, times))
	//{
	//	KXLOGERROR("uid %d challenge stage not enough energy!", uid);
	//	return false;
	//}
	// 等级是否满足
	if (!CModelHelper::isUserLevelEnough(uid, pChapterConf->UnlockLevel))
	{
		KXLOGERROR("uid %d challenge stage not enough level! need %d", uid, pChapterConf->UnlockLevel);
		return false;
	}
	//关卡进度
	if (pChapterConf->Type == CHAPTERTYPE_NORMAL)
	{
		if (pStageModel->GetChapterStatus(chapterId) >= CHAPTERSTATUS_UNLOCK)
		{
			int curStageId = pStageModel->GetCurStage();
			if (stageId > curStageId)
			{
				KXLOGERROR("uid %d %s stageId > curStageId", uid, __FUNCTION__);
				return false;
			}
		}
		else
		{
			//版本更新检查
			if (pChapterConf->ExInfo.PrevID != 0 && pStageModel->GetChapterStatus(pChapterConf->ExInfo.PrevID) < CHAPTERSTATUS_FINISH)
			{
				KXLOGERROR("%s pStageModel->GetChapterStatus(pChapterConf->ExInfo.PrevID) < CHAPTERSTATUS_FINISH", __FUNCTION__);
				return false;
			}
			else
			{
				//且不是第一关
				if (pChapterConf->ExInfo.FirstStageID != stageId)
				{
					KXLOGERROR("uid %d %s pChapterConf->ExInfo.FirstStageID != stageId", uid, __FUNCTION__);
					return false;
				}
                else
                {
                    // 版本更新的第一关， 先设置为解锁
					if (!pStageModel->SetChapterStatus(chapterId, CHAPTERSTATUS_UNLOCK) 
						|| !pStageModel->SetCurStage(stageId))
					{
						KXLOGERROR("user %d update version or after level enough unlock normal chapter error!", uid);
					}
                }
			}
		}
	}
	else if (pChapterConf->Type == CHAPTERTYPE_ELITE)
	{
		if (pStageModel->GetEliteChapterStatus(chapterId) >= CHAPTERSTATUS_UNLOCK)
		{
			int lastStageId = pStageModel->GetCurElite();
			if (stageId > lastStageId)
			{
				KXLOGERROR("uid %d %s stageId > lastStageId", uid, __FUNCTION__);
				return false;
			}
		}
		else
		{
			//版本更新检查
			if (pChapterConf->ExInfo.PrevID != 0 && pStageModel->GetEliteChapterStatus(pChapterConf->ExInfo.PrevID) < CHAPTERSTATUS_FINISH)
			{
				KXLOGERROR("uid %d %s pStageModel->GetEliteChapterStatus(pChapterConf->ExInfo.PrevID) < CHAPTERSTATUS_FINISH", uid, __FUNCTION__);
				return false;
			}
			else
			{
				//且不是第一关
				if (pChapterConf->ExInfo.FirstStageID != stageId)
				{
					KXLOGERROR("uid %d %s pChapterConf->ExInfo.FirstStageID != stageId", uid, __FUNCTION__);
					return false;
				}
                else
                {
                    // 版本更新的第一关， 先设置基本信息
					if (!pStageModel->SetEliteChapterStatus(chapterId, CHAPTERSTATUS_UNLOCK) 
						|| !pStageModel->SetCurElite(stageId))
					{
						KXLOGERROR("user %d update version or after level enough unlock elite chapter error!", uid);
					}
                }
			}
		}

		//挑战次数用完
		DBEliteStageInfo eliteInfo;
		if (!CModelHelper::getEliteInfo(uid, chapterId, stageId, eliteInfo))
		{
			KXLOGERROR("uid %d challenge elite get db info error!", uid);
			return false;
		}
  //      //次数不够
		//if (eliteInfo.challengeTimes + times > pChapterConf->ChallengeTimes)
		//{
		//	KXLOGERROR("uid %d challenge elite has %d need %d times maxtimes %d not enough!", 
		//		uid, eliteInfo.challengeTimes, times, pChapterConf->ChallengeTimes);
		//	return false;
		//}
	}
	
	return true;
}

void CModelHelper::unlockChapter(int uid, const std::vector<int> &chapters)
{
	CStageModel *pStageModel = dynamic_cast<CStageModel*>(getModel(uid, MODELTYPE_STAGE));
    CHECK_RETURN_VOID(pStageModel);
	for (std::vector<int>::const_iterator iter = chapters.begin();
		iter != chapters.end(); ++iter)
	{
		int chapterId = *iter;
		const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
        CHECK_RETURN_VOID(pChapterConf);

		if (pChapterConf->Type == CHAPTERTYPE_NORMAL)
		{
			//解锁新普通章节
            CHECK_RETURN_VOID(pStageModel->SetChapterStatus(chapterId, CHAPTERSTATUS_UNLOCK));
		}
		else
		{
			CHECK_RETURN_VOID(pStageModel->SetEliteChapterStatus(chapterId, CHAPTERSTATUS_UNLOCK));
		}
	}
}

bool CModelHelper::isChapterUnlock(int uid, int chapterId)
{
	CStageModel *pStageModel = dynamic_cast<CStageModel*>(getModel(uid, MODELTYPE_STAGE));
	const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
	CHECK_RETURN(NULL != pStageModel && NULL != pChapterConf);

	return pStageModel->GetChapterStatus(chapterId) >= CHAPTERSTATUS_UNLOCK;
}

bool CModelHelper::checkChapterUnlock(int uid, int integral)
{
	// 获得现在玩家的t级
	int tlevel = CCommonHelper::getUserIntegralDan(integral);
	CConfChapter *conf = dynamic_cast<CConfChapter *>(
		CConfManager::getInstance()->getConf(CONF_CHAPTER));
	CHECK_RETURN(conf != NULL);

	// 找到对应的章节, 一个T级对应解锁一个章节
	ChapterConfItem *pChapterConf = NULL;
	std::map<int, void*> &allChapterConf = conf->getDatas();
	for (std::map<int, void*>::iterator iter = allChapterConf.begin();
		iter != allChapterConf.end(); ++iter)
	{
		ChapterConfItem *temp = reinterpret_cast<ChapterConfItem*>(iter->second);
		if (temp->UnlockLevel == tlevel)
		{
			pChapterConf = temp;
			break;
		}
	}
	CHECK_RETURN(NULL != pChapterConf);

	std::vector<int> unlockChapterId;
	// 如果已经解锁, 不用处理下面的
	if (CModelHelper::isChapterUnlock(uid, pChapterConf->ID))
	{
		return false;
	}
	//需要解锁这个
	unlockChapterId.push_back(pChapterConf->ID);
	// 处理可能跳级的情况
	int prevChapterId = pChapterConf->ExInfo.PrevID;
	while (prevChapterId != 0)
	{
		const ChapterConfItem *pPrevChapterConf = queryConfChapter(prevChapterId);
		if (NULL == pPrevChapterConf)
		{
			continue;
		}
		// 前一个如果解锁了, 应该是正常解锁了的, 不用继续查找之前的
		if (CModelHelper::isChapterUnlock(uid, prevChapterId))
		{
			break;
		}
		unlockChapterId.push_back(prevChapterId);
		prevChapterId = pPrevChapterConf->ExInfo.PrevID;
	}

	CModelHelper::unlockChapter(uid, unlockChapterId);
	return true;
}

void CModelHelper::finishChapterStage(int uid, int chapterId, int stageId, int status, std::vector<int> &unlockVec)
{
	CStageModel *pStageModel = dynamic_cast<CStageModel*>(getModel(uid, MODELTYPE_STAGE));
	const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
    CHECK_RETURN_VOID(NULL != pStageModel && NULL != pChapterConf);

	std::map<int, StageInfo>::const_iterator iter = pChapterConf->Stages.find(stageId);
	if (iter == pChapterConf->Stages.end())
	{
		KXLOGDEBUG("uid %d finish chapter=%d stageid=%d, can't find stageInfo, can't unlock!", uid, chapterId, stageId);
		return;
	}

	const StageInfo &stageInfo = iter->second;
	bool isLastStage = (stageInfo.ExInfo.NextID == 0);
	// 下个关卡id
	int nextStageId = stageInfo.ExInfo.NextID;
	if (pChapterConf->Type == CHAPTERTYPE_NORMAL)
	{
		//更新关卡状态
		CHECK_RETURN_VOID(pStageModel->SetStageStatus(stageId, status, !isLastStage));
		//如果关卡的进度大于之前进度
		int curStage = pStageModel->GetCurStage();
		if (nextStageId > curStage)
		{
			unlockVec.push_back(nextStageId);
            CHECK_RETURN_VOID(pStageModel->SetCurStage(stageInfo.ExInfo.NextID));
		}
	}
	else if (pChapterConf->Type == CHAPTERTYPE_ELITE)
	{
		//更新精英关卡状态
		CHECK_RETURN_VOID(pStageModel->SetEliteStatus(stageId, status, !isLastStage));
		//挑战次数
		CModelHelper::addEliteUseTimes(uid, chapterId, stageId, 1);
		
		int lastElite = pStageModel->GetCurElite();
		if (nextStageId > lastElite)
		{
			unlockVec.push_back(nextStageId);
			//更新精英关卡信息
			DBEliteStageInfo eliteInfo = { 0, 0, 0, 0 };
            CHECK_RETURN_VOID(pStageModel->SetEliteInfo(stageInfo.ExInfo.NextID, eliteInfo));
			//更新最新精英关卡
            CHECK_RETURN_VOID(pStageModel->SetCurElite(stageInfo.ExInfo.NextID));
		}
	}

	
	//// 是否为最后一个关卡
	//if (isLastStage)
	//{
	//	//章节结束, 解锁新章节
	//	bool unlockNext = false;
	//	if (pChapterConf->Type == CHAPTERTYPE_NORMAL)
	//	{
	//		if (pStageModel->GetChapterStatus(chapterId) < CHAPTERSTATUS_FINISH)
	//		{
	//			CHECK_RETURN_VOID(pStageModel->SetChapterStatus(chapterId, CHAPTERSTATUS_FINISH));
	//			unlockNext = true;
	//		}
	//	}
	//	else
	//	{
	//		if (pStageModel->GetEliteChapterStatus(chapterId) < CHAPTERSTATUS_FINISH)
	//		{
	//			CHECK_RETURN_VOID(pStageModel->SetEliteChapterStatus(chapterId, CHAPTERSTATUS_FINISH));
	//			unlockNext = true;
	//		}
	//	}

	//	if (unlockNext)
	//	{
	//		for (std::vector<int>::const_iterator iter = pChapterConf->UnlockChapters.begin();
	//			iter != pChapterConf->UnlockChapters.end(); ++iter)
	//		{
	//			int unlockChapterId = *iter;
	//			const ChapterConfItem *pNextChapterConf = queryConfChapter(unlockChapterId);
	//			if (NULL == pNextChapterConf)
	//			{
	//				continue;
	//			}

	//			// 如果等级不足不解锁
	//			if (!CModelHelper::isUserLevelEnough(uid, pNextChapterConf->UnlockLevel))
	//			{
	//				KXLOGDEBUG("User level less than unlock level, don't unlock!");
	//				continue;
	//			}

	//			if (pNextChapterConf->Type == CHAPTERTYPE_NORMAL)
	//			{
	//				// 解锁章节
	//				CHECK_RETURN_VOID(pStageModel->SetChapterStatus(unlockChapterId, CHAPTERSTATUS_UNLOCK));
	//				// 设置新关卡id
	//				CHECK_RETURN_VOID(pStageModel->SetCurStage(pNextChapterConf->ExInfo.FirstStageID));
	//			}
	//			else if (pNextChapterConf->Type == CHAPTERTYPE_ELITE)
	//			{
	//				// 解锁章节
	//				CHECK_RETURN_VOID(pStageModel->SetEliteChapterStatus(unlockChapterId, CHAPTERSTATUS_UNLOCK));
	//				// 设置新关卡id
	//				CHECK_RETURN_VOID(pStageModel->SetCurElite(pNextChapterConf->ExInfo.FirstStageID));
	//			}
	//			unlockVec.push_back(pNextChapterConf->ExInfo.FirstStageID);
	//		}
	//	}
	//}

	//CModelHelper::addEnergy(uid, -1 * stageInfo.Energy);
}

void CModelHelper::finishInstance(int uid, int instance, int difficulty, int star)
{
	CInstanceModel *pInstanceModel = dynamic_cast<CInstanceModel*>(getModel(uid, MODELTYPE_INSTANCE));
	const ActivityInstanceItem *pInstanceConf = queryConfActivityInstance(instance);
	if (NULL == pInstanceModel || NULL == pInstanceConf)
	{
		return;
	}

	DBActivityInstanceInfo instanceInfo;
	if (!pInstanceModel->GetInstanceInfo(instance, instanceInfo))
	{
		instanceInfo.useTimes = pInstanceConf->CompleteTimes;
		instanceInfo.useStamp = 0;
		instanceInfo.buyTimes = pInstanceConf->BuyTimes;
		instanceInfo.buyStamp = 0;
		instanceInfo.easy = 0;
		instanceInfo.normal = 0;
		instanceInfo.difficult = 0;
		instanceInfo.hell = 0;
		instanceInfo.legend = 0;
	}

	instanceInfo.useTimes -= 1;
	if (instanceInfo.useTimes < 0)
	{
		//log报错, 因为如果使用次数=0是不会进行挑战的
		instanceInfo.useTimes = 0;
	}

	switch (difficulty)
	{
	case DIFFICULTY_EASY:
		if (instanceInfo.easy < star)
		{
			instanceInfo.easy = star;
		}
		break;
	case DIFFICULTY_NORMAL:
		if (instanceInfo.normal < star)
		{
			instanceInfo.normal = star;
		}
		break;
	case DIFFICULTY_DIFFICULT:
		if (instanceInfo.difficult < star)
		{
			instanceInfo.difficult = star;
		}
		break;
	case DIFFICULTY_HELL:
		if (instanceInfo.hell < star)
		{
			instanceInfo.hell = star;
		}
		break;
	case DIFFICULTY_LEGEND:
		if (instanceInfo.legend < star)
		{
			instanceInfo.legend = star;
		}
		break;
	default:
		break;
	}

	if (!pInstanceModel->SetInstanceInfo(instance, instanceInfo))
	{
		// log
	}
}

bool CModelHelper::getEliteInfo(int uid, int chapterId, int stageId, DBEliteStageInfo &eliteInfo)
{
	CStageModel *pStageModel = dynamic_cast<CStageModel*>(getModel(uid, MODELTYPE_STAGE));
	const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
	const StageConfItem * pStageConf = queryConfStage(stageId);
	if (NULL == pStageModel || NULL == pChapterConf || NULL == pStageConf)
	{
		return false;
	}

	updateEliteInfo(uid, chapterId, stageId);
	memset(&eliteInfo, 0, sizeof(eliteInfo));
	if (!pStageModel->GetEliteInfo(stageId, eliteInfo))
	{
		eliteInfo.buyRecoverStamp = 0;
		eliteInfo.buyTimes = 0;
		eliteInfo.challengeTimes = 0;
		eliteInfo.useRecoverStamp = 0;
	}
	return true;
}

bool CModelHelper::addEliteUseTimes(int uid, int chapterId, int stageId, int times)
{
	CStageModel *pStageModel = dynamic_cast<CStageModel*>(getModel(uid, MODELTYPE_STAGE));
	const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
	if (pChapterConf->Type == CHAPTERTYPE_ELITE)
	{
		DBEliteStageInfo eliteInfo;
		pStageModel->GetEliteInfo(stageId, eliteInfo);
		eliteInfo.challengeTimes += times;
		//首次消耗精英关卡挑战次数, 记录恢复时间
		if (eliteInfo.challengeTimes == 0)
		{
			time_t timeNowStamp = time(NULL);
			const TimeRecoverItem *pTimeRecover = queryConfTimeRecoverSetting();
			eliteInfo.useRecoverStamp = CTimeCalcTool::nextTimeStampToZero(
				timeNowStamp, pTimeRecover->AllTimeReset);
		}
		
		if (!pStageModel->SetEliteInfo(stageId, eliteInfo))
		{
			KXLOGERROR("CModelHelper::addEliteUseTimes uid=%d stageId=%d add times = %d", uid, stageId, times);
		}
		return true;
	}
	return false;
}

int CModelHelper::getStageChapterStar(int uid, int chapterId)
{
    int nStar = 0;
    CStageModel *pStageModel = dynamic_cast<CStageModel *>(getModel(uid, MODELTYPE_STAGE));
    const ChapterConfItem *pChapterConfItem = queryConfChapter(chapterId);
    if (pStageModel && pChapterConfItem)
    {
		const std::map<int, StageInfo> &Stages = pChapterConfItem->Stages;
		for (std::map<int, StageInfo>::const_iterator iter = Stages.begin();
			iter != Stages.end(); ++iter)
		{
            if (iter->second.ID.size() < 2)
            {
                continue;
            }

			// getChapterStageState 会区分精英或普通关卡
			int stageStatus = CModelHelper::getChapterStageState(uid, chapterId, iter->second.ID[0]);
			if (stageStatus > STAGESTATUS_UNLOCK)
			{
				nStar += stageStatus - STAGESTATUS_UNLOCK;
			}
		}
    }

    return nStar;
}

int CModelHelper::getChapterRewardStar(int uid, int chapterId)
{
	int nStar = 0;
	CStageModel *pStageModel = dynamic_cast<CStageModel *>(getModel(uid, MODELTYPE_STAGE));
	const ChapterConfItem *pChapterConfItem = queryConfChapter(chapterId);
	if (pStageModel && pChapterConfItem)
	{
		const std::map<int, StageInfo> &Stages = pChapterConfItem->Stages;
		int i = 1;
		for (std::map<int, StageInfo>::const_iterator iter = Stages.begin();
			iter != Stages.end(); ++iter, ++i)
		{
			if (pChapterConfItem->Type == CHAPTERTYPE_NORMAL && i % 3 != 0)
			{
				continue;
			}

            if (iter->second.ID.size() < 2)
            {
                continue;
            }

			// getChapterStageState 会区分精英或普通关卡
			int stageStatus = CModelHelper::getChapterStageState(uid, chapterId, iter->second.ID[0]);
			if (stageStatus > STAGESTATUS_UNLOCK)
			{
				nStar += stageStatus - STAGESTATUS_UNLOCK;
			}
		}
	}

	return nStar;
}

int CModelHelper::getChapterStageState(int uid, int chapterId, int stageId)
{
	CStageModel *pStageModel = dynamic_cast<CStageModel *>(getModel(uid, MODELTYPE_STAGE));
	if (NULL == pStageModel)
	{
		return STAGESTATUS_HIDE;
	}

	const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
    if (NULL == pChapterConf)
    {
        LOG("Execute False On File %s Line %d : %d", __FILE__, __LINE__, chapterId);
        return STAGESTATUS_HIDE;
    }

	std::map<int, StageInfo>::const_iterator iter = pChapterConf->Stages.find(stageId);
	if (iter == pChapterConf->Stages.end())
	{
		return STAGESTATUS_HIDE;
	}

	const StageInfo &stageInfo = iter->second;
	int stageState = 0;
	int chapterState = 0;
	if (pChapterConf->Type == CHAPTERTYPE_ELITE)
	{
		chapterState = pStageModel->GetEliteChapterStatus(chapterId);
		stageState = pStageModel->GetEliteStatus(stageId);
	}
	else
	{
		chapterState = pStageModel->GetChapterStatus(chapterId);
		stageState = pStageModel->GetStageStatus(stageId);
	}
	//如果是最后最后一关
	if (stageState == STAGESTATUS_LOCK && stageInfo.ExInfo.NextID == 0 && chapterState == CHAPTERSTATUS_FINISH)
	{
		stageState = STAGESTATUS_STAR3;
	}

	return stageState;
}

void CModelHelper::setGoldTestData(int uid, int damage, int count /*= 1*/)
{
    CGoldTestModel *pGoldTestModel = dynamic_cast<CGoldTestModel*>(getModel(uid, MODELTYPE_GOLDTEST));
    // 可能过了挑战时间,再次挑战时会重置
    pGoldTestModel->AddFieldNum(GT_FD_DAMAGE, damage);
    pGoldTestModel->AddFieldNum(GT_FD_USETIMES, count);
}

int CModelHelper::markGoldTestChest(int uid, std::vector<int> &chests)
{
	CGoldTestModel *pGoldTestModel = dynamic_cast<CGoldTestModel*>(getModel(uid, MODELTYPE_GOLDTEST));
	int flag = 0;
	if (NULL == pGoldTestModel || !pGoldTestModel->GetFieldNum(GT_FD_CHESTFLAG, flag))
	{
		KXLOGERROR("%s user %d get gold test model data error!", __FUNCTION__, uid);
		return 0;
	}

	for (std::vector<int>::iterator iter = chests.begin();
		iter != chests.end(); ++iter)
	{
		int chest = *iter - 1;
		int mark = 0x1 << chest;
		flag |= mark;
	}

	if (!pGoldTestModel->SetFieldNum(GT_FD_CHESTFLAG, flag))
	{
		KXLOGERROR("%s user %d mark gold test chest flag error!", __FUNCTION__, uid);
		return 0;
	}

	return flag;
}

bool CModelHelper::canMercenaryUse(int uid, int mercenary)
{
	CPersonMercenaryModel *pMerModel = dynamic_cast<CPersonMercenaryModel*>(getModel(uid, MODELTYPE_MERCENARY));
	if (NULL == pMerModel)
	{
		return false;
	}
	std::set<int>& employees = pMerModel->GetPersonEmployedMercenaryInfo();
	return employees.find(mercenary) == employees.end();
}

void CModelHelper::addMercenaryUseList(int uid, int mercenary)
{
	CPersonMercenaryModel *pMerModel = dynamic_cast<CPersonMercenaryModel*>(getModel(uid, MODELTYPE_MERCENARY));
	if (NULL == pMerModel)
	{
		return;
	}
	pMerModel->AddEmployedMercenaryInfo(mercenary);

	CUserUnionModel *pUsrUnion = dynamic_cast<CUserUnionModel *>(getModel(uid, MODELTYPE_USERUNION));
	if (NULL == pUsrUnion)
	{
		return;
	}
	int nUnionID = 0;
	pUsrUnion->getUserUnionValue(USER_UNION_ID, nUnionID);
	CUnionMercenaryModel *pMercenaryModel = new CUnionMercenaryModel();
	pMercenaryModel->init(nUnionID);
	pMercenaryModel->EmployedMercenary(mercenary);
	delete pMercenaryModel;
}

void CModelHelper::updatePvpInfo(int uid)
{
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel *>(getModel(uid, MODELTYPE_PVP));
    CHECK_RETURN_VOID(pPvpModel != NULL);
	int resetStamp = 0;
    int nowStamp = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
	pPvpModel->GetPvpField(PVPCOMM_FD_DAYRESETSTAMP, resetStamp);
	// 每日任务重置
    if (resetStamp == 0 || nowStamp >= resetStamp)
	{
		resetPvpDayInfo(uid);
	}
}

bool CModelHelper::persistPvpBattleKey(int uid)
{
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel *>(getModel(uid, MODELTYPE_PVP));
	if (NULL != pPvpModel)
	{
		return pPvpModel->PersistPvpBattleKey();
	}
	return false;
}

bool CModelHelper::deletePvpBattleKey(int uid)
{
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel *>(getModel(uid, MODELTYPE_PVP));
	if (NULL != pPvpModel)
	{
		return pPvpModel->DeletePvpBattleKey();
	}
	return false;
}

bool CModelHelper::setPvpBattleId(int uid, int battleId, int robotId)
{
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel *>(getModel(uid, MODELTYPE_PVP));
	if (NULL != pPvpModel)
	{
		return pPvpModel->SetPvpBattleId(battleId, robotId);
	}
	return false;
}

bool CModelHelper::getUserPreChampionRank(int uid, int &Rank)
{
	/*CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN(NULL != pGameUser);
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel*>(pGameUser->getModel(MODELTYPE_PVP));
	CHECK_RETURN(NULL != pPvpModel);

	int nFightTime = 0;
	int nCurTime = (int)time(NULL);
	pPvpModel->GetPvpField(CPN_FD_WEEKRESETSTAMP, nFightTime);
	int nNextTime = CTimeCalcTool::nextTimeStamp(nFightTime, 0, 0, 2);
	if (nFightTime > nCurTime)
	{
		return false;
	}

	int nDay = CCommonHelper::getTotalDay(nNextTime);
	if (!CPvpRankModel::getInstance()->getChampionRank(uid, Rank, nDay))
	{
		Rank = 0;
		return false;
	}
    return true;
    */

	return false;
}

bool CModelHelper::resetPvpDayInfo(int uid)
{
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel *>(getModel(uid, MODELTYPE_PVP));
	CHECK_RETURN(NULL != pPvpModel);

	int resetStamp = 0;
    int nowStamp = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
	int nextMin = 0;
	int nextHour = 0;

	std::map<int, int> mapValues;
	resetStamp = CTimeCalcTool::nextTimeStamp(nowStamp, nextMin, nextHour);
	mapValues[PVPCOMM_FD_DAYRESETSTAMP] = resetStamp;
	mapValues[PVPCOMM_FD_DAYWINTIMES] = 0;
	mapValues[PVPCOMM_FD_DAYCONTINUSWINTIMES] = 0;
	mapValues[PVPCOMM_FD_DAYMAXCONITNUSWINTIMES] = 0;
	mapValues[PVPCOMM_FD_DAYBATTLETIMES] = 0;
	mapValues[PVPCOMM_FD_DAYBUYCHESTTIMES] = 0;
	mapValues[PVPCOMM_FD_REWARDFLAG] = 0;

	if (!pPvpModel->SetPvpField(mapValues))
	{
		KXLOGERROR("set pvp field error! %s", __FUNCTION__);
		return false;
	}

	return true;
}

bool CModelHelper::resetPvpCPN(int uid)
{
	/*CUserModel *pUserModel = dynamic_cast<CUserModel *>(getModel(uid, MODELTYPE_USER));
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel *>(getModel(uid, MODELTYPE_PVP));
	CConfPvpGradingInitial *pGradingConf = dynamic_cast<CConfPvpGradingInitial*>(
		CConfManager::getInstance()->getConf(CONF_PVP_GRADING_INITIAL));

	if (pUserModel == NULL || pPvpModel == NULL || pGradingConf == NULL)
	{
		return false;
	}

	std::vector<PvpGradingInitial> &initialConf = pGradingConf->GetGradingInitial();
	int userLevel = 0;
	int mmr = 0;
	int integral = 0;

	if (!pUserModel->GetUserFieldVal(USR_FD_USERLV, userLevel))
	{
		return false;
	}

	for (std::vector<PvpGradingInitial>::iterator iter = initialConf.begin();
		iter != initialConf.end(); ++iter)
	{
		if (userLevel >= iter->PlayerLevelLower 
			&& userLevel < iter->PlayerLevelUpper)
		{
			mmr = iter->StartMMR;
			integral = iter->StartPoint;
			break;
		}
	}
	
    CConfPvpSetting *pPvpSetting = dynamic_cast<CConfPvpSetting*>(
        CConfManager::getInstance()->getConf(CONF_PVP_SETTING));
    if (NULL == pPvpSetting 
        || pPvpSetting->m_PvpSetting.ArenaDay.size() < 2
        || pPvpSetting->m_PvpSetting.ArenaTime.size() < 4)
    {
        KXLOGERROR("CConfPvpSetting error! %s", __FUNCTION__);
        return false;
    }

    int weekResetStamp = 0;
    time_t nowStamp = time(NULL);
    std::map<int, int> mapValues;
    //配置表时间
    weekResetStamp = CTimeCalcTool::nextTimeStamp(nowStamp, 0, 0, pPvpSetting->m_PvpSetting.ArenaDay[1] + 1);
	
    mapValues[CPN_FD_WEEKRESETSTAMP] = weekResetStamp;
	mapValues[CPN_FD_GRADINGNUM] = 0;
	mapValues[CPN_FD_GRADINGDVAL] = 0;
	mapValues[CPN_FD_MMR] = mmr;
	mapValues[CPN_FD_INTEGRAL] = integral;
	mapValues[CPN_FD_CONTINUSWINTIMES] = 0;
	mapValues[CPN_FD_BATTLETIMES] = 0;
	mapValues[CPN_FD_DANCONTINUSWINTIMES] = 0;
	mapValues[CPN_FD_TOTALWINTIMES] = 0;

	if (!pPvpModel->SetPvpField(mapValues))
	{
		KXLOGERROR("set pvp field error! %s", __FUNCTION__);
		return false;
	}
	return true;*/
    return false;
}

int CModelHelper::addPvpChest(int uid)
{
    CPvpModel *pPvpModel = dynamic_cast<CPvpModel *>(getModel(uid, MODELTYPE_PVP));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
    CPvpChestModel *pPvpChestModel = dynamic_cast<CPvpChestModel *>(getModel(uid, MODELTYPE_PVPCHEST));
	CHECK_RETURN_INT(NULL != pPvpModel && NULL != pPvpChestModel && NULL != pUserModel);

    const ArenaSettingItem* pChestSetting = queryArenaSetting(); 
	if (pChestSetting == NULL)
	{
		KXLOGERROR("uid %d config ArenaSettingItem is NULL", uid);
		return -1;
	}

	int chestOrder = 0, integral = 0, pvplv = 0, userlv = 0, quality = 0;
	// 获得pvp宝箱次序, 积分, 玩家等级
	if (!pPvpModel->GetPvpField(PVPCOMM_FD_CHESTORDER, chestOrder)
		|| !pPvpModel->GetPvpField(PVP_FD_INTEGRAL, integral)
		|| !pUserModel->GetUserFieldVal(USR_FD_USERLV, userlv))
	{
		return -1;
	}

	if (pChestSetting->ChestOrders.empty())
	{
		KXLOGERROR("uid %d config can't found more chest, pChestSetting->ChestOrders is empty!", uid);
		return -1;
	}

	if ((size_t)chestOrder >= pChestSetting->ChestOrders.size()) 
	{
		chestOrder = 0;
	}
	quality = pChestSetting->ChestOrders[chestOrder];
	pvplv = CCommonHelper::getUserIntegralDan(integral);
	const ArenaChestItem *pChestConf = CCommonHelper::getArenaChest(pvplv, userlv, quality);
	if (pChestConf == NULL)
    {
		KXLOGERROR("uid %d config can't found more chest, condition: pvplv%d, uselv%d, quality%d", uid, pvplv, userlv, quality);
        return -1;
    }
    // 设置次序及宝箱
	if (!pPvpModel->SetPvpField(PVPCOMM_FD_CHESTORDER, ++chestOrder)
		|| !pPvpChestModel->addChestId(pChestConf->ChestId))
	{
		KXLOGERROR("uid %d set pvp field or add chest id error!", uid);
		return -1;
	}
	return pChestConf->ChestId;
}

bool CModelHelper::addPvpChest(int uid, int chestId)
{
	CPvpChestModel *pPvpChestModel = dynamic_cast<CPvpChestModel *>(getModel(uid, MODELTYPE_PVPCHEST));
	CHECK_RETURN(NULL != pPvpChestModel);
	if (!pPvpChestModel->addChestId(chestId))
	{
		return false;
	}
	return true;
}

int CModelHelper::getWinDropId(int uid)
{
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel*>(getModel(uid, MODELTYPE_PVP));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	if (NULL == pPvpModel || NULL == pUserModel)
	{
		return 0;
	}

	int userLv = 0;
	int dayWinTimes = 0;
	if (!pPvpModel->GetPvpField(PVPCOMM_FD_DAYWINTIMES, dayWinTimes))
	{
		KXLOGERROR("%d get pvp field error when getWinDropId!", uid);
		return 0;
	}
	if (!pUserModel->GetUserFieldVal(USR_FD_USERLV, userLv))
	{
		KXLOGERROR("%d get user field error when getWinDropId!", uid);
		return 0;
	}

	return CCommonHelper::getArenaDrop(userLv, dayWinTimes);
}

int CModelHelper::popPvpChest(int uid)
{
	CPvpChestModel *pModel = dynamic_cast<CPvpChestModel*>(getModel(uid, MODELTYPE_PVPCHEST));
	if (NULL == pModel)
	{
		return 0;
	}

	return pModel->popChestId();
}

bool CModelHelper::isPvpChestFull(int uid)
{
	CPvpChestModel *pModel = dynamic_cast<CPvpChestModel*>(getModel(uid, MODELTYPE_PVPCHEST));
	if (NULL == pModel)
	{
		return false;
	}
	const std::list<int> &chestList = pModel->getChestIds();
	return (int)chestList.size() >= 5;
}

bool CModelHelper::isPvpChestEmpty(int uid)
{
	CPvpChestModel *pModel = dynamic_cast<CPvpChestModel*>(getModel(uid, MODELTYPE_PVPCHEST));
	if (NULL == pModel)
	{
		return false;
	}

	const std::list<int> &chestList = pModel->getChestIds();
	return chestList.empty();
}

void CModelHelper::refreshPvpChest(int uid)
{
    CPvpModel *pPvpModel = dynamic_cast<CPvpModel*>(CModelHelper::getModel(uid, MODELTYPE_PVP));
    CHECK_RETURN_VOID(NULL != pPvpModel);
    CPvpChestModel *pPvpChestModel = dynamic_cast<CPvpChestModel*>(CModelHelper::getModel(uid, MODELTYPE_PVPCHEST));
    CHECK_RETURN_VOID(NULL != pPvpChestModel);

	const ArenaSettingItem* pChestSetting = queryArenaSetting();
    CHECK_RETURN_VOID(NULL != pChestSetting);

    // 查询是否宝箱是否已满
    int nCurChestCount = static_cast<int>(pPvpChestModel->getChestIds().size());
    CHECK_RETURN_VOID(nCurChestCount < MAX_CHEST_NUM);

    int timeNow = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
    int lastGenTime = 0;
    if (!pPvpModel->GetPvpField(PVPCOMM_FD_LASTCHESTGENTIME, lastGenTime))
    {
        KXLOGERROR("%d get pvp field error! when refresh pvp chest", uid);
        return;
    }

    int interval = timeNow - lastGenTime;
	CHECK_RETURN_VOID(interval >= pChestSetting->ChestRefreshTime);

    std::vector<int> chestIds;
	while (interval >= pChestSetting->ChestRefreshTime)
    {
        int chestId = CModelHelper::addPvpChest(uid);
        if (chestId <= 0)
        {
            KXLOGERROR("refreshPvpChest addPvpChest error! uid %d", uid);
            break;
        }
        chestIds.push_back(chestId);
		interval -= pChestSetting->ChestRefreshTime;
        nCurChestCount++;
        if (nCurChestCount >= 5)
        {
            break;
        }
    }
	// 设置最后一次生成时间
	if (nCurChestCount >= 5)
	{
		if (!pPvpModel->SetPvpField(PVPCOMM_FD_LASTCHESTGENTIME, 0))
		{
			KXLOGERROR("%d set pvp field:PVPCOMM_FD_LASTCHESTGENTIME error when refreshPvpChest!", uid);
			return;
		}
	}
	else
	{
        if (!pPvpModel->SetPvpField(PVPCOMM_FD_LASTCHESTGENTIME, 
			lastGenTime + pChestSetting->ChestRefreshTime * chestIds.size()))
		{
			KXLOGERROR("%d set pvp field:PVPCOMM_FD_LASTCHESTGENTIME error when refreshPvpChest!", uid);
			return;
		}
	}

    int nSendLen = sizeof(PvpChestRefreshSC)+sizeof(int)*chestIds.size();
    char *pSendData = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(nSendLen));
    PvpChestRefreshSC *pRefreshSC = reinterpret_cast<PvpChestRefreshSC*>(pSendData);
    pRefreshSC->chestCount = chestIds.size();
    int *pChestId = reinterpret_cast<int*>(pRefreshSC + 1);

    for (std::vector<int>::iterator iter = chestIds.begin();
        iter != chestIds.end(); ++iter)
    {
        *pChestId = *iter;
        pChestId += 1;
    }

    CKxCommManager::getInstance()->sendData(uid,
        CMD_PVPCHEST, CMD_PVPCHEST_REFRESH_SC, pSendData, nSendLen);
    //内存回收
    KxServer::kxMemMgrRecycle(pSendData, nSendLen);
}

// 生成符合指定等级的物品列表，并顺带生成总权重
void genGoodsListAndWeightByLevel(int usrLv, int& weight, const list<ShopGoodsConfigData*>& src, list<ShopGoodsConfigData*>& dst)
{
    for (list<ShopGoodsConfigData*>::const_iterator iter = src.begin();
        iter != src.end(); ++iter)
    {
        if (usrLv < (*iter)->nFreshMinLev || usrLv >(*iter)->nFreshMaxLev)
        {
            continue;
        }
        weight += (*iter)->nWeight;
        dst.push_back(*iter);
    }
}

list<ShopGoodsConfigData*>::iterator randGoodsIter(int uid, int &wight, std::list<ShopGoodsConfigData *>& ShopList)
{
    // 随机出权重
    int nTempWeight = g_RandGenerator.MakeRandNum(0, wight);
    int nCurWeight = 0;
    list<ShopGoodsConfigData*>::iterator iter = ShopList.begin();
    for (; iter != ShopList.end(); ++iter)
    {
        nCurWeight += (*iter)->nWeight;
        if (nTempWeight > nCurWeight)
        {
            continue;
        }
        else
        {
            // 更新权重
            wight -= (*iter)->nWeight;
            return iter;
        }
    }
    return ShopList.end();
}

// 刷新商店
bool CModelHelper::FreshShop(int uid, int nShopID, int userLv, std::map<int, SShopGoods> &ShopMap)
{
	CShopData *conf = dynamic_cast<CShopData *>(CConfManager::getInstance()->getConf(CONF_SHOP));
	CHECK_RETURN(conf != NULL);
	ShopConfigData *pData = static_cast<ShopConfigData *>(conf->getData(nShopID));
	CHECK_RETURN(pData != NULL && pData->VectNum.size() > 0);

	//读取配置表生成数据
	const std::list<ShopGoodsConfigData*>* ShopList = queryConfShopList(nShopID);
	CHECK_RETURN(ShopList != NULL);

	// 通过段位计算商店数目
	CPvpModel *pPvpModel = dynamic_cast<CPvpModel *>(getModel(uid, MODELTYPE_PVP));
	CHECK_RETURN(NULL != pPvpModel);
	int nPvpMScore = 0;
	pPvpModel->GetPvpField(PVP_FD_HISTORYHIGHESTINTEGRAL, nPvpMScore);
	int nPvpLev = CCommonHelper::getUserIntegralDan(nPvpMScore);
	int nShopNum = 0;
    if (nPvpLev > static_cast<int>(pData->VectNum.size()))
	{
        nShopNum = pData->VectNum[pData->VectNum.size() - 1];
	}
	else
	{
		if (nPvpLev < DAN_BRONZE || nPvpLev > DAN_KING)
		{
			nShopNum = pData->VectNum[0];
		}
		else
		{
			nShopNum = pData->VectNum[nPvpLev - 1];
		}
	}

    // 取出符合指定等级的物品列表，并顺带生成总权重
	list<ShopGoodsConfigData*> ResourceList;
    int wight = 0;
    genGoodsListAndWeightByLevel(userLv, wight, *ShopList, ResourceList);

	while (static_cast<int>(ShopMap.size()) < nShopNum 
        && ResourceList.size() != 0)
	{
		// 从
        std::list<ShopGoodsConfigData*>::iterator iter = randGoodsIter(uid, wight, ResourceList);
		CHECK_BREAK(iter != ResourceList.end());

		int nIndex = ShopMap.size() + 1;
		SShopGoods Goods;
		Goods.nShopGoodsID = (*iter)->nShopGoodsID;
		Goods.nGoodsNum = (*iter)->nGoodsNum;
		ShopMap[nIndex] = Goods;
		ResourceList.erase(iter);
	}

	return true;
}

void CModelHelper::DispatchActionEvent(int uid, int actID, void *data, int len)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    if (NULL != pGameUser)
    {
        pGameUser->getUserActionManager()->UserAction(actID, data, len);
    }
}

bool CModelHelper::removeExpireMail(int uid)
{
    CMailModel *pMailModel = dynamic_cast<CMailModel *>(getModel(uid, MODELTYPE_MAIL));
    CGlobalMailModel *pGMailModel = CGlobalMailModel::getInstance();
    CHECK_RETURN(pMailModel != NULL);
    CHECK_RETURN(pGMailModel != NULL);

    // 处理过期的全局邮件
    map<int, int> userGlobalMap = pMailModel->GetWebMails();
    std::map<int, BaseMailInfo>& globalList = pGMailModel->GetGlobalMailBase();
    for (std::map<int, int>::iterator iter = userGlobalMap.begin();
        iter != userGlobalMap.end(); ++iter)
    {
        int gMailId = iter->first;
        if (globalList.find(gMailId) == globalList.end())
        {
            pMailModel->RemoveWebMail(gMailId);
        }
    }

    return true;
}

bool CModelHelper::removeExpireGlobalMail()
{
	CGlobalMailModel *pGMailModel = CGlobalMailModel::getInstance();
	time_t nowStamp = time(NULL);

	std::map<int, BaseMailInfo> mapGMailBasic = pGMailModel->GetGlobalMailBase();
	for (std::map<int, BaseMailInfo>::iterator iter = mapGMailBasic.begin();
		iter != mapGMailBasic.end(); ++iter)
	{
		if (nowStamp >= iter->second.sendTimeStamp + 30 * 24 * 3600)
		{
			pGMailModel->RemoveGlobalMail(iter->first);
		}
	}
	return true;
}

void CModelHelper::updateUserModelReset(int uid, bool bLogin)
{
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
	CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(getModel(uid, MODELTYPE_USERUNION));
	CBlueDiamondModel *pBlueDiamondModel = dynamic_cast<CBlueDiamondModel*>(getModel(uid, MODELTYPE_BDACTIVE));
	const TimeRecoverItem *pRecoverConf = queryConfTimeRecoverSetting();
	if (NULL != pUserModel && NULL != pRecoverConf)
	{
		int timeNow = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
		int resetTimeStamp = 0;
		pUserModel->GetUserFieldVal(USR_FD_RESETTIMESTAMP, resetTimeStamp);
		if (resetTimeStamp == 0 || timeNow >= resetTimeStamp)
		{
			int buyGoldTimes = 0;
			int nLoginTime = 0;

			std::map<int, int> userValues;
			userValues[USR_FD_RESETTIMESTAMP] = CTimeCalcTool::nextTimeStampToZero(timeNow, pRecoverConf->AllTimeReset);

            pUserModel->GetUserFieldVal(USR_FD_BUYGOLDTIMES, buyGoldTimes);
			pUserModel->GetUserFieldVal(USR_FD_LOGINTIME, nLoginTime);

			//得根据vip配表来
			userValues[USR_FD_BUYGOLDTIMES] = 0;
			userValues[USR_FD_FREEHEROTIMES] = 1;

			//签到信息重置
			userValues[USR_FD_DAYFLAG] = 0;
			time_t tCurTime = time(NULL);
			tm Curtm;
            stlocaltime(&tCurTime, &Curtm);
			time_t tLastLoiginTime = nLoginTime;
			tm LastLoginTm;
			stlocaltime(&tLastLoiginTime, &LastLoginTm);
			//每个月第一天
			if (Curtm.tm_mday == 1 || LastLoginTm.tm_mon != Curtm.tm_mon)
			{
				userValues[USR_FD_MONTHDAY] = 0;
			}

			//每日首次登陆，记录累计登陆天数和连续登陆天数
			int nDay = 0;
			pUserModel->GetUserFieldVal(USR_FD_LOGIN_DAY, nDay);
			userValues[USR_FD_LOGIN_DAY] = nDay + 1;
			int nLastLoginDay = CCommonHelper::getTotalDay(nLoginTime);
			int nCurLoginDay = CCommonHelper::getTotalDay();
			nDay = 0;
			pUserModel->GetUserFieldVal(USR_FC_CONLOGIN_DAY, nDay);
			if (nLastLoginDay + 1 == nCurLoginDay)
			{
				userValues[USR_FC_CONLOGIN_DAY] = nDay + 1;
			}
			else
			{
				userValues[USR_FC_CONLOGIN_DAY] = 1;
			}

			if (!pUserModel->SetUserFieldVal(userValues))
			{
				KXLOGDEBUG("user %d SetUserFieldVal update energy error!", uid);
			}
			if (pUserUnionModel != NULL)
			{
				pUserUnionModel->setUserUnionValue(USER_UNION_EXPIDITION_TIME, 0);
			}

			if (pBlueDiamondModel != NULL)
			{
				pBlueDiamondModel->reSetQQActiveValue();
			}
			
			// pvp日奖励发放
			//PvpMailSend(uid);
			TowerMailSend(uid);

			if (bLogin)
			{
				//登陆日志记录
				CCommOssHelper::userLoginOss(uid, 1);
			}
			
			DispatchActionEvent(uid, ELA_CONTINUELOGIN, &(userValues[USR_FC_CONLOGIN_DAY]), sizeof(int));
			DispatchActionEvent(uid, ELA_HAVE_HERO_X_STAR, NULL, 0);
			DispatchActionEvent(uid, ELA_USER_LEVEL_UP, NULL, 0);
			DispatchActionEvent(uid, ELA_OWN_SUMMONER_COUNT, NULL, 0);
			DispatchActionEvent(uid, ELA_OWN_DIFFEREN_HERO, NULL, 0);
			DispatchActionEvent(uid, ELA_STAGE_STAR, NULL, 0);
			DispatchActionEvent(uid, ELA_OWNCOLORHERO, NULL, 0);
			DispatchActionEvent(uid, ELA_OWNLEVELHERO, NULL, 0);
			DispatchActionEvent(uid, ELA_TOTALLOGIN, NULL, 0);
		}
		else
		{
			if (bLogin)
			{
				//登陆日志记录
				CCommOssHelper::userLoginOss(uid, 0);
			}
			
		}

		DispatchActionEvent(uid, ELA_FUND, NULL, 0);
	}
}

void CModelHelper::updateInstanceInfo(int uid, int activityId)
{
	CInstanceModel *pInstanceModel = dynamic_cast<CInstanceModel*>(getModel(uid, MODELTYPE_INSTANCE));
	std::map<int, DBActivityInstanceInfo> &mapInstance = pInstanceModel->GetAllInstanceInfo();
	std::map<int, DBActivityInstanceInfo>::iterator iter = mapInstance.find(activityId);
	if (iter != mapInstance.end())
	{
		//副本配置
		const ActivityInstanceItem *pInstanceConf = queryConfActivityInstance(activityId);
		if (NULL == pInstanceConf)
		{
			return;
		}
		DBActivityInstanceInfo instanceInfo = iter->second;
		int times = 0;
		int buyTimes = 0;
		int fullTimes = pInstanceConf->CompleteTimes;
		int recoverType = pInstanceConf->RecoverType;
		time_t useStamp = instanceInfo.useStamp;
		time_t buyStamp = instanceInfo.buyStamp;
		//溢出的, 直接返回
		if (instanceInfo.useTimes > fullTimes)
		{
			return;
		}

		switch (recoverType)
		{
		case RCVTYPE_PERSECOND:
			if (pInstanceConf->RecoverTime.size() >= 0)
			{
				int perSecond = pInstanceConf->RecoverTime[0];
				//检查使用时间戳
				if (useStamp > 0)
				{
					times = CTimeCalcTool::overSecond(useStamp, perSecond);
				}
				if (buyStamp > 0)
				{
					buyTimes = CTimeCalcTool::overSecond(buyStamp, perSecond);
				}
			}
			break;
		case RCVTYPE_DAY:
			if (pInstanceConf->RecoverTime.size() >= 3)
			{
				DayTime dayTime;
				dayTime.hour = pInstanceConf->RecoverTime[0];
				dayTime.minutes = pInstanceConf->RecoverTime[1];
				dayTime.second = pInstanceConf->RecoverTime[2];
				if (useStamp > 0)
				{
					times = CTimeCalcTool::overDay(useStamp, dayTime);
				}
				if (buyStamp > 0)
				{
					buyTimes = CTimeCalcTool::overDay(buyStamp, dayTime);
				}
			}
			break;
		case RCVTYPE_WEEK:
			if (pInstanceConf->RecoverTime.size() >= 4)
			{
				WeekTime weekTime;
				weekTime.weekDay = pInstanceConf->RecoverTime[0];
				weekTime.dayTime.hour = pInstanceConf->RecoverTime[1];
				weekTime.dayTime.minutes = pInstanceConf->RecoverTime[2];
				weekTime.dayTime.second = pInstanceConf->RecoverTime[3];
				times = CTimeCalcTool::overWeek(useStamp, weekTime);
				if (useStamp > 0)
				{
					times = CTimeCalcTool::overWeek(useStamp, weekTime);
				}
				if (buyStamp > 0)
				{
					buyTimes = CTimeCalcTool::overWeek(buyStamp, weekTime);
				}
			}
			break;
		case RCVTYPE_OPEN:
		case RCVTYPE_DONTRECOVER:
		default:
			break;
		}

		bool isNeedSave = false;
		if (times > 0)
		{
			// 恢复相对应次数
			instanceInfo.useTimes += pInstanceConf->RecoverParam * times;
			if (instanceInfo.useTimes > fullTimes)
			{
				instanceInfo.useTimes = fullTimes;
			}
			isNeedSave = true;
		}

		if (buyTimes > 0)
		{
			//恢复购买次数, 重置购买时间戳
			instanceInfo.buyTimes = pInstanceConf->BuyTimes;
			iter->second.buyStamp = 0;
			isNeedSave = true;
		}
		// 需要设置新数据再设置
		if (isNeedSave)
		{
            CHECK_RETURN_VOID(pInstanceModel->SetInstanceInfo(activityId, instanceInfo));
		}
	}
}

void CModelHelper::updateTowerTestInfo(int uid)
{
	CTowerTestModel * pTowerTestModel = dynamic_cast<CTowerTestModel *>(getModel(uid, MODELTYPE_TOWERTEST));
    CHECK_RETURN_VOID(pTowerTestModel != NULL);

	int recoverStamp = pTowerTestModel->GetTowerTestField(TOWER_FD_TIMESTAMP);
    int nowStamp = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
	if (recoverStamp == 0 || nowStamp >= recoverStamp)
	{
		const TowerTestSettingItem * pSettingConf = queryConfTowerSetting();
		CHECK_RETURN_VOID(NULL != pSettingConf)
		const TimeRecoverItem * pRecoverConf = queryConfTimeRecoverSetting();
		if (NULL != pRecoverConf)
		{
			recoverStamp = CTimeCalcTool::nextTimeStamp(nowStamp,0,0,pSettingConf->nStartWeekDay);
		}
		else
		{
			recoverStamp = CTimeCalcTool::nextTimeStamp(nowStamp, 0, 5);
		}

		pTowerTestModel->ResetTowerTest();
		std::map<int, int> towerReset;
		towerReset[TOWER_FD_TIMESTAMP] = recoverStamp;
		towerReset[TOWER_FD_CRYSTAL] = pSettingConf->FirstCrystal;
        CHECK_RETURN_VOID(pTowerTestModel->SetTowerTestField(towerReset));
	}
}

void CModelHelper::updateGoldTestInfo(int uid)
{
    CGoldTestModel *pGoldTestModel = dynamic_cast<CGoldTestModel *>(getModel(uid, MODELTYPE_GOLDTEST));
    CHECK_RETURN_VOID(pGoldTestModel != NULL);
    int recoverStamp = 0;
    int nowStamp = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();

	pGoldTestModel->GetFieldNum(GT_FD_RESETSTAMP, recoverStamp);
	if (recoverStamp <= 0 || nowStamp >= recoverStamp)
	{
		const TimeRecoverItem *pRecoverConf = queryConfTimeRecoverSetting();
        CHECK_RETURN_VOID(NULL != pRecoverConf);
		//重置
		recoverStamp = CTimeCalcTool::nextTimeStampToZero(nowStamp, pRecoverConf->AllTimeReset);
		if (!pGoldTestModel->ResetGoldTest(recoverStamp))
		{
			KXLOGDEBUG("update gold test set recover stamp failed! uid=%d", uid);
		}
	}
}

void CModelHelper::updateHeroTestInfo(int uid)
{
    CHeroTestModel *pHeroTestModel = dynamic_cast<CHeroTestModel *>(getModel(uid, MODELTYPE_HEROTEST));
    CHECK_RETURN_VOID(pHeroTestModel != NULL);
    int nowStamp = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
	int nResetStamp = pHeroTestModel->GetResetStamp(); 
	if (nResetStamp <= 0 || nowStamp >= nResetStamp)
	{
		const TimeRecoverItem *pRecoverConf = queryConfTimeRecoverSetting();
        CHECK_RETURN_VOID(NULL != pRecoverConf);

		nResetStamp = CTimeCalcTool::nextTimeStampToZero(nowStamp, pRecoverConf->AllTimeReset);
		pHeroTestModel->ResetHeroTest((int)nResetStamp);
	}
}

void CModelHelper::updateAllEliteInfo(int uid)
{
	/*CStageModel *pStageModel = dynamic_cast<CStageModel*>(getModel(uid, MODELTYPE_STAGE));
	const TimeRecoverItem *pTimeRecover = queryConfTimeRecoverSetting();
	if (NULL == pStageModel || NULL == pTimeRecover)
	{
		return;
	}

	time_t timeNowStamp = time(NULL);

	std::map<int, DBEliteStageInfo> dbEliteInfoMap = pStageModel->GetEliteStageInfo();
	for (std::map<int, DBEliteStageInfo>::iterator iter = dbEliteInfoMap.begin();
		iter != dbEliteInfoMap.end(); ++iter)
	{
		DBEliteStageInfo dbStageInfo = iter->second;
		bool bRecover = false;
		int nextRecoverTime = CTimeCalcTool::nextTimeStampToZero(timeNowStamp, pTimeRecover->AllTimeReset);
		if (dbStageInfo.buyRecoverStamp < timeNowStamp)
		{
			bRecover = true;
			dbStageInfo.buyTimes = 0;
			dbStageInfo.buyRecoverStamp = nextRecoverTime;
		}
		if (dbStageInfo.useRecoverStamp < timeNowStamp)
		{
			bRecover = true;
			dbStageInfo.challengeTimes = 0;
			dbStageInfo.useRecoverStamp = nextRecoverTime;
		}

		if (bRecover)
		{
			if (!pStageModel->SetEliteInfo(iter->first, dbStageInfo))
			{
				KXLOGERROR("update all elite info error !!!! uid = %d, stageId = %d", uid, iter->first);
			}
		}
	}*/
}

void CModelHelper::updateEliteInfo(int uid, int chapterId, int stageId)
{
	/*CStageModel *pStageModel = dynamic_cast<CStageModel*>(getModel(uid, MODELTYPE_STAGE));
	const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
	const StageConfItem * pStageConf = queryConfStage(stageId);
	const TimeRecoverItem *pTimeRecover = queryConfTimeRecoverSetting();
	if (NULL == pStageModel || NULL == pChapterConf 
		|| NULL == pStageConf || NULL == pTimeRecover)
	{
		return;
	}

	time_t timeNowStamp = time(NULL);

	DBEliteStageInfo dbStageInfo;
	memset(&dbStageInfo, 0, sizeof(dbStageInfo));
	if (!pStageModel->GetEliteInfo(stageId, dbStageInfo))
	{
		//该关卡不需要刷新
		return;
	}
	else
	{
		bool bRecover = false;
		int nextRecoverTime = CTimeCalcTool::nextTimeStampToZero(timeNowStamp, pTimeRecover->AllTimeReset);
		if (dbStageInfo.buyRecoverStamp < timeNowStamp)
		{
			bRecover = true;
			dbStageInfo.buyTimes = 0;
			dbStageInfo.buyRecoverStamp = nextRecoverTime;
		}
		if (dbStageInfo.useRecoverStamp < timeNowStamp)
		{
			bRecover = true;
			dbStageInfo.challengeTimes = 0;
			dbStageInfo.useRecoverStamp = nextRecoverTime;
		}
		
		if (bRecover)
		{
			if (!pStageModel->SetEliteInfo(stageId, dbStageInfo))
			{
				KXLOGERROR("update elite info error !!!! uid = %d, chapterId = %d, stageId = %d", uid, chapterId, stageId);
			}
		}
	}*/
}

void CModelHelper::updateTaskInfo(int uid)
{
    CTaskModel *pTaskModel = dynamic_cast<CTaskModel*>(getModel(uid, MODELTYPE_TASK));
    if (NULL == pTaskModel)
    {
        return;
    }

    std::map<int, TaskDBInfo>& mapAllTask = pTaskModel->GetTasks();
    std::map<int, TaskDBInfo>::iterator iter = mapAllTask.begin();
    for (; iter != mapAllTask.end(); ++iter)
    {
        CTaskAchieveHelper::checkTask(uid, iter->first, iter->second);
    }
}

void CModelHelper::updateUserUnionInfo(int uid, bool bRefresh /*= true*/)
{
    CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(getModel(uid, MODELTYPE_USERUNION));
    CHECK_RETURN_VOID(NULL != pUserUnionModel);

    int nRestStamp = 0;
    pUserUnionModel->getUserUnionValue(USER_UNION_RESETSTAMP, nRestStamp);

    bool bReset = false;
    int nNow = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
    if (0 == nRestStamp || nRestStamp < nNow)
    {
        if (bRefresh)
        {
            // 查看数据库中是否有重置了(不同服务器原因)
            if (!pUserUnionModel->Refresh())
            {
                KXLOGERROR("day reset refresh CUserUnionModel error!!!");
                return;
            }

            pUserUnionModel->getUserUnionValue(USER_UNION_RESETSTAMP, nRestStamp);
            if (0 == nRestStamp || nRestStamp < nNow)
            {
                bReset = true;
            }
        }
        else
        {
            bReset = true;
        }
    }

    if (bReset)
    {
        nRestStamp = CTimeCalcTool::nextTimeStampToZero(nNow, queryConfTimeRecoverSetting()->AllTimeReset);
        std::map<int, int> mapValue;
        mapValue[USER_UNION_RESETSTAMP] = nRestStamp;
        mapValue[USER_UNION_WELFARE] = 0;
        mapValue[USER_UNION_DAYPVPLIVENESS] = 0;
        mapValue[USER_UNION_DAYSTAGELIVENESS] = 0;
        mapValue[USER_UNION_DAYAPPLAYCOUNT] = 0;

        if (!pUserUnionModel->setUserUnionValues(mapValue))
        {
            KXLOGERROR("CModelHelper::updateUserUnionInfo setUserUnionValues fail!!! %d", uid);
        }
    }
}

// 重置商店数据，理论上只有新玩家才会走到这里
bool CModelHelper::updateShopData(int uid)
{
	CShopModel *pShopModel = dynamic_cast<CShopModel*>(getModel(uid, MODELTYPE_SHOP));
	if (NULL == pShopModel)
	{
		return false;
	}

	//读取配置表，生成数据
	CShopData *conf = dynamic_cast<CShopData *>(CConfManager::getInstance()->getConf(CONF_SHOP));
	CHECK_RETURN(conf != NULL);
	std::map<int, void *> mapShopID = conf->getDatas();

    // 当玩家首次登录时应该刷新商店数据
	map<int, SShopInfo> MapShopIDInfo = pShopModel->GetShopIDInfo();
	if (MapShopIDInfo.size() == 0)
	{
        // 如果此时
        int nowTime = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
		for (std::map<int, void *>::iterator ator = mapShopID.begin();
            ator != mapShopID.end(); ++ator)
        {
            ShopConfigData *pData = reinterpret_cast<ShopConfigData *>(ator->second);
            int freshDt = pData->nTimeInterval * 60;    // 刷新间隔时间
            int nextRefreshTime = (nowTime / DAYSECOND) * DAYSECOND; // 当天0点
            if (freshDt > 0)
            {
                // 距离当前最近的下一个刷新时间点
                nextRefreshTime += ((nowTime % DAYSECOND) / freshDt + 1) * freshDt; 
            }
            pShopModel->SetShopInfo(ator->first, 0, nextRefreshTime);
		}
	}

	map<int, ShopGoodsMap> MapShopGoods = pShopModel->GetAllShop();
	if (MapShopGoods.size() == 0)
    {
        // 先获取玩家等级
        CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
        CHECK_RETURN(pUserModel != NULL);
        int usrLv = 0;
        pUserModel->GetUserFieldVal(USR_FD_USERLV, usrLv);

		ShopGoodsMap GoodsMap;
		for (std::map<int, void *>::iterator ator = mapShopID.begin();
            ator != mapShopID.end(); ++ator)
		{
			//随机生成商店物品列表
            CHECK_CONTINUE(CModelHelper::FreshShop(uid, ator->first, usrLv, GoodsMap));
            pShopModel->AddShopData(ator->first, GoodsMap);
            GoodsMap.clear();
		}
	}

	return true;
}

bool CModelHelper::checkShopFreshTimes(int uid)
{
	CShopModel *pShopModel = dynamic_cast<CShopModel*>(getModel(uid, MODELTYPE_SHOP));
	if (NULL == pShopModel)
	{
		return false;
	}

	time_t nTime = time(NULL);
	int nValue = 0;

	pShopModel->GetFieldValue(enFreshStartTime, nValue);

	//过了一天，重置次数
	if (nTime >= nValue)
	{
		int nFreshEndTimes = 0;
		int nTimes = 0;

		CShopData *conf = dynamic_cast<CShopData *>(CConfManager::getInstance()->getConf(CONF_SHOP));
		CHECK_RETURN(conf != NULL);

		std::map<int, void*> MapShop = conf->getDatas();

		for (std::map<int, void*>::iterator ator = MapShop.begin(); ator != MapShop.end(); ++ator)
		{
			CHECK_CONTINUE(pShopModel->GetShopInfo(ator->first, nTimes, nFreshEndTimes));

			if (!pShopModel->SetShopInfo(ator->first, 0, nFreshEndTimes))
			{
				continue;
			}
		}

		//获取到当天凌晨时间戳
		tm Ctm;
		stlocaltime(&nTime, &Ctm);
		int nNextTime = DAYSECOND - Ctm.tm_hour * 60 * 60 - Ctm.tm_min * 60 - Ctm.tm_sec;

		if (nNextTime == 0)
		{
			nValue = (int)nTime + DAYSECOND;
		}
		else
		{
			nValue = (int)nTime + nNextTime;
		}

		if (!pShopModel->SetFieldValue(enFreshStartTime, nValue))
		{
			return false;
		}
	}

	return true;
}

bool CModelHelper::PvpMailSend(int uid)
{
	//每日逻辑处理,目前写死7天
	int nCurDay = CCommonHelper::getTotalDay();
	CPvpRankModel::getInstance()->cleanPvpRewardFlag(uid, nCurDay - 7);

	for (int i = nCurDay - 7; i < nCurDay; i++)
	{
		//有就标识领取过了
		if (!CPvpRankModel::getInstance()->getPvpRewardFlag(uid, i))
		{
			continue;
		}

		int nRank = 0;
		CPvpRankModel::getInstance()->GetPvpRank(uid, nRank, i);

		ArenaRewardItem *pRewardItem = queryConfPvpRankReward(nRank);

		if (pRewardItem == NULL)
		{
			continue;
		}
		vector<DropItemInfo> DropItem;
		CItemDrop::Drop(pRewardItem->Award_Items, DropItem);

		if (!CMailHelper::sendPvpRankMail(uid, DropItem))
		{
			continue;
		}

		CPvpRankModel::getInstance()->setPvpRewardFlag(uid, i);
	}

	return true;
}

bool CModelHelper::TowerMailSend(int uid)
{
	const TowerTestSettingItem * pSettingConf = queryConfTowerSetting();
	CHECK_RETURN(NULL != pSettingConf);
	int nCurTime = static_cast<time_t>(time(NULL));
	int nStartTime = CTimeCalcTool::curDayTimeZero(pSettingConf->nStartWeekDay);
	int nEndTime = nStartTime + pSettingConf->nInterval * 3600;
	
	if (nCurTime >= nStartTime && nCurTime < nEndTime)
	{
		//爬塔进行中，没有奖励
		return true;
	}

	//这个是上个爬塔结束，但未开始，可以结算发送邮件
	if (nCurTime < nStartTime || nCurTime >= nEndTime)
	{
		//上周开始时间
		int nPreStartTime = 0;
		if (nCurTime < nStartTime)
		{
			//上周开始时间
			nPreStartTime = nStartTime - WEEKSECOND;
		}
		else if (nCurTime >= nEndTime)
		{
			nPreStartTime = nStartTime;
		}
		
		//有就标识领取过了
		if (!CRankModel::getInstance()->getRankRewardFlag(uid, TOWER_RANK_TYPE,nPreStartTime))
		{
			return true;
		}

		int nRank = 0;
		CRankModel::getInstance()->GetRankData(TOWER_RANK_TYPE, uid, nRank);
		const TowerRankItem *pTowerRankItem = queryConfTowerRankReward(nRank);

		if (pTowerRankItem == NULL)
		{
			return false;
		}

		vector<DropItemInfo> DropItem;
		for (unsigned int i = 0; i < pTowerRankItem->Item.size(); i++)
		{
			DropItemInfo Info;
			Info.id = pTowerRankItem->Item[i].ID;
			Info.num = pTowerRankItem->Item[i].Num;
			DropItem.push_back(Info);
		}

		if (!CMailHelper::sendTowerMail(uid, DropItem))
		{
			return false;
		}

		CRankModel::getInstance()->setRankRewardFlag(uid, TOWER_RANK_TYPE, nPreStartTime);
	}

	return true;
}

bool CModelHelper::addExistEquipToBag(int uid, int equipId)
{
    CUserModel *pUserModel = dynamic_cast<CUserModel*>(getModel(uid, MODELTYPE_USER));
    CBagModel *pBagModel = dynamic_cast<CBagModel*>(getModel(uid, MODELTYPE_BAG));
    CEquipModel *pEquipModle = dynamic_cast<CEquipModel*>(getModel(uid, MODELTYPE_EQUIP));
    CHECK_RETURN(NULL != pUserModel && NULL != pBagModel && NULL != pEquipModle);

    SEquipInfo* pEquipInfo = pEquipModle->GetEquipData(equipId);
    CHECK_RETURN(NULL != pEquipInfo);

    int nBagCapacity = 0;
    CHECK_RETURN(pUserModel->GetUserFieldVal(USR_FD_BAGCAPACITY, nBagCapacity));
    int nCurMaxBagCapacity = pBagModel->AddCapacity(1);
    if (nCurMaxBagCapacity > nBagCapacity)
    {
        pBagModel->AddCapacity(-1);
        return false;
    }

    if (!pBagModel->AddItem(equipId, pEquipInfo->nItemID))
    {
        return false;
    }

    return true;
}

bool CModelHelper::removeExistEquipFromBag(int uid, int equipId)
{
    CBagModel *pBagModel = dynamic_cast<CBagModel*>(getModel(uid, MODELTYPE_BAG));
    CEquipModel *pEquipModle = dynamic_cast<CEquipModel*>(getModel(uid, MODELTYPE_EQUIP));
    CHECK_RETURN(NULL != pBagModel && NULL != pEquipModle);

    if (NULL == pEquipModle->GetEquipData(equipId))
    {
        return false;
    }

    if (!pBagModel->RemoveItem(equipId)
        || pBagModel->AddCapacity(-1) < 0)
    {
        return false;
    }

    return true;
}

int CModelHelper::getItemType(int itemId)
{
    if (itemId >= DEFAULT_EQUIPREF)
    {
        return PROPTYPE_EQUIP;
    }

    return PROPTYPE_NONE;
}

void CModelHelper::checkPayData(int uid)
{
	Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGE_PAY);
	CHECK_RETURN_DEBUG_VOID(pStorage != NULL);
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(pStorage->GetStorer(uid));
	CHECK_RETURN_DEBUG_VOID(pRedisStorer != NULL);

	string strBillKey = ModelKey::AnySDKPayKey(uid);
	vector<string> vecBill;

	if (SUCCESS != pRedisStorer->ExistKey(strBillKey))
	{
		return;
	}

	CHECK_RETURN_DEBUG_VOID(SUCCESS == pRedisStorer->ListRange(strBillKey, vecBill));
	for (size_t i = 0; i < vecBill.size(); i++)
	{
		VecInt vec;
		CConfAnalytic::ToJsonInt(vecBill[i], vec);
		CHECK_CONTINUE(vec.size() >= 6);
		int nResult = vec[2];
		int nPayNum = vec[3];
		int nGetDiamond = vec[4];
		int nPid = vec[5];

		//充值成功
		if (nResult == PAYBACK_SUCESS_TYPE)
		{
			if (nPid == FOND_PID)
			{
				CModelHelper::DispatchActionEvent(uid, ELA_FUND, NULL, 0);
			}
			else
			{
				CModelHelper::DispatchActionEvent(uid, ELA_PAYDIAMOND, &nGetDiamond, sizeof(nGetDiamond));
			}

			CModelHelper::DispatchActionEvent(uid, ELA_RMVBCOST, &nPayNum, sizeof(nPayNum));
		}
	}

	if (SUCCESS != pRedisStorer->DelKey(strBillKey))
	{
		KXLOGDEBUG("uid %d DelKey %s failed", uid, strBillKey);
		return;
	}
}

