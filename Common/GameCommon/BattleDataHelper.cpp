#include "BattleDataHelper.h"
#include "KxLog.h"
#include "CommStructs.h"
#include "ModelHelper.h"
#include "GameUserManager.h"
#include "UnionMercenaryModel.h"

bool CBattleDataHelper::mercenaryToBuffer(CUserModel *pUserModel, UnionMercenaryInfo* info, CBufferData& bufferData)
{
    // 填充佣兵SoldierCardData
    int offset = bufferData.getDataLength();
    if (!bufferData.writeEmptyBuffer(sizeof(SoldierCardData)))
    {
        return false;
    }

    SoldierCardData* pSoldierCard = reinterpret_cast<SoldierCardData*>(bufferData.getBuffer() + offset);
    memcpy(pSoldierCard, &info->stSoldierCardData, sizeof(SoldierCardData));
    int nUserLv = 0;
    pUserModel->GetUserFieldVal(USR_FD_USERLV, nUserLv);
    // 佣兵等级最多能比玩家大15级
    nUserLv += 15;
    if (pSoldierCard->SoldierLv > nUserLv)
    {
        pSoldierCard->SoldierLv = nUserLv;
    }

    pSoldierCard->EquipCnt = 0;
    for (int i = 0; i < EQUIPMENTSMAX; ++i)
    {
        if (info->stEquipments[i].confId > 0)
        {
            int offsetEquip = bufferData.getDataLength();
            if (!bufferData.writeEmptyBuffer(sizeof(SoldierEquip)))
            {
                KXLOGERROR("mercenaryToBuffer CBufferData Error, Model Error Or Memry Error");
                return false;
            }

            SoldierEquip *pSolderEquip = reinterpret_cast<SoldierEquip*>(bufferData.getBuffer() + offsetEquip);
            pSoldierCard->EquipCnt += 1;
            memcpy(pSolderEquip, &info->stEquipments[i], sizeof(SoldierEquip));
        }
    }
    return true;
}

bool CBattleDataHelper::playerToBuffer(int uid, const BattleRoomData& roomData, CUserModel *pUserModel, CBufferData& bufferData)
{
    // 填充PlayerData
    int offset = bufferData.getDataLength();
    if (!bufferData.writeEmptyBuffer(sizeof(PlayerData)))
    {
        return false;
    }
    PlayerData* player = reinterpret_cast<PlayerData*>(bufferData.getBuffer() + offset);
    player->UserId = uid;
    pUserModel->GetUserFieldVal(USR_FD_USERLV, player->UserLv);
    player->Camp = 1;
    player->OuterBonusCount = static_cast<int>(roomData.outerBuffs.size());
    player->SoldierCount = static_cast<int>(roomData.heroIds.size());
    player->MecenaryCount = roomData.mecenaryId > 0 ? 1 : 0;
    player->Identity = pUserModel->getIdentity();;
    std::string name = pUserModel->GetName();
    memset(player->UserName, 0, sizeof(player->UserName));
    memcpy(player->UserName, name.c_str(), strlen(name.c_str()));

    // 战斗外buff
    for (std::vector<int>::const_iterator iter = roomData.outerBuffs.begin();
        iter != roomData.outerBuffs.end(); ++iter)
    {
        int buffOffset = bufferData.getDataLength();
        if (!bufferData.writeEmptyBuffer(sizeof(int)))
        {
            return false;
        }

        int *pBuffId = reinterpret_cast<int*>(bufferData.getBuffer() + buffOffset);
        *pBuffId = *iter;
    }

    // 召唤师
    int summonerOffset = bufferData.getDataLength();
    if (!bufferData.writeEmptyBuffer(sizeof(int)))
    {
        return false;
    }

    int *pSummonerId = reinterpret_cast<int*>(bufferData.getBuffer() + summonerOffset);
    *pSummonerId = roomData.summonerId;

    return true;
}

bool CBattleDataHelper::soldierToBuffer(int heroId, CEquipModel* equipModel, CHeroModel* heroModel, CBufferData& bufferData)
{
    DBHeroAttr heroInfo;
    if (!heroModel->GetHero(heroId, heroInfo))
    {
        KXLOGERROR("soldierToBuffer GetHero faile heroId %d", heroId);
        return false;
    }

    int offsetSoldierCard = bufferData.getDataLength();
    if (!bufferData.writeEmptyBuffer(sizeof(SoldierCardData)))
    {
        KXLOGERROR("soldierToBuffer CBufferData Error, Model Error Or Memry Error");
        return false;
    }

    SoldierCardData* soldierCard = reinterpret_cast<SoldierCardData*>(
        bufferData.getBuffer() + offsetSoldierCard);

    // 卡片信息
    soldierCard->SoldierId = heroId;
    soldierCard->SoldierExp = heroInfo.exp;
    soldierCard->SoldierLv = heroInfo.level;
    soldierCard->SoldierStar = heroInfo.star;
    memcpy(soldierCard->SoldierTalent, heroInfo.talent, sizeof(heroInfo.talent));

    // 装备信息
    soldierCard->EquipCnt = 0;
    for (int i = WEAPON; i < TREASURE + 1; ++i)
    {
        int equipId = heroInfo.equipId[i];
        // 正常，该部位没有装备
        if (0 < equipId)
        {
            SEquipInfo *pEquipInfo = equipModel->GetEquipData(equipId);
            if (pEquipInfo == NULL)
            {
                KXLOGERROR("soldierToBuffer pEquipInfo == NULLequipId %d", equipId);
                return false;
            }

            int offsetEquip = bufferData.getDataLength();
            if (!bufferData.writeEmptyBuffer(sizeof(SoldierEquip)))
            {
                KXLOGERROR("soldierToBuffer CBufferData Error, Model Error Or Memry Error");
                return false;
            }

            SoldierEquip *equipData = reinterpret_cast<SoldierEquip*>(
                bufferData.getBuffer() + offsetEquip);
            equipData->confId = pEquipInfo->nItemID;
            for (int i = 0; i < MAX_EQUIP_EFFECT_NUM; i++)
            {
                equipData->cEffectID[i] = pEquipInfo->cEffectID[i];
                equipData->sEffectValue[i] = pEquipInfo->sEffectValue[i];
            }
            soldierCard->EquipCnt += 1;
        }
    }

    return true;
}

bool CBattleDataHelper::roomDataToBuffer(int uid, const BattleRoomData& roomData, CBufferData& bufferData)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    if (pGameUser == NULL)
    {
        return false;
    }

    if (!CModelHelper::haveSummoner(uid, roomData.summonerId))
    {
        return false;
    }

    // 检查佣兵，如果携带了，且成功获取该佣兵信息
    UnionMercenaryInfo mercenaryInfo;
    if (roomData.mecenaryId > 0)
    {
        CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
        CHECK_RETURN(pUserUnionModel != NULL);
        // 获取玩家公会id
        int nUnionId = 0;
        pUserUnionModel->getUserUnionValue(USER_UNION_ID, nUnionId);

        if (nUnionId <= 0 || !CUnionMercenaryModel::GetSingleMercenaryInfo(nUnionId, roomData.mecenaryId, &mercenaryInfo))
        {
            KXLOGDEBUG("GetSingleMercenaryInfo is fail!!, uid %d, nUnionId %d, nMercenaryID %d", uid, nUnionId, roomData.mecenaryId);
            return false;
        }
    }

    CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
    CHeroModel *pHeroModel = dynamic_cast<CHeroModel*>(pGameUser->getModel(MODELTYPE_HERO));
    CEquipModel *pEquipModel = dynamic_cast<CEquipModel*>(pGameUser->getModel(MODELTYPE_EQUIP));
	CHECK_RETURN(pUserModel != NULL && pHeroModel != NULL && pEquipModel != NULL);

    // 填充RoomData
    int offset = bufferData.getDataLength();
    if (!bufferData.writeEmptyBuffer(sizeof(RoomData)))
    {
        return false;
    }
    RoomData* room = reinterpret_cast<RoomData*>(bufferData.getBuffer() + offset);
    room->BattleType = roomData.battleType;
    room->StageId = roomData.stageId;
    room->StageLevel = roomData.stageLv;
    room->Ext1 = roomData.ext1;
    room->Ext2 = roomData.ext2;
    room->BuffCount = 0;
    room->PlayerCount = 1;

    // buff无法进行验证
    for (std::vector<BuffData>::const_iterator iter = roomData.buffs.begin();
        iter != roomData.buffs.end(); ++iter)
    {
        int buffOffset = bufferData.getDataLength();
        if (!bufferData.writeEmptyBuffer(sizeof(BuffData)))
        {
            KXLOGERROR("roomDataToBuffer uid %d, add buff %d", uid, *iter);
            return false;
        }

        BuffData* buffData = reinterpret_cast<BuffData*>(bufferData.getBuffer() + buffOffset);
        memcpy(buffData, &(*iter), sizeof(BuffData));

        room->BuffCount++;
    }

    // 填充玩家信息
    if (!playerToBuffer(uid, roomData, pUserModel, bufferData))
    {
        KXLOGERROR("roomDataToBuffer uid %d, playerToBuffer faile", uid);
        return false;
    }

    // 顺序初始化士兵
    for (std::vector<int>::const_iterator iter = roomData.heroIds.begin();
        iter != roomData.heroIds.end(); ++iter)
    {
        if (!soldierToBuffer(*iter, pEquipModel, pHeroModel, bufferData))
        {
            KXLOGERROR("roomDataToBuffer uid %d, soldierToBuffer %d faile", uid, *iter);
            return false;
        }
    }

    // 如果带了佣兵则初始化佣兵
    if (!mercenaryToBuffer(pUserModel, &mercenaryInfo, bufferData))
    {
        return false;
    }

    return true;
}
