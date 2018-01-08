#include "LookService.h"
#include "LookProtocol.h"
#include "LoginProtocol.h"
#include "Protocol.h"
#include "KxCommManager.h"
#include "GameUserManager.h"

using namespace KxServer;

void CLookService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
    case CMD_LOOK_HERO_CS:
        ProcessLookHero(uid, buffer, len, commun);
		break;
    case CMD_LOOK_EQUIP_CS:
        ProcessLookEquip(uid, buffer, len, commun);
        break;
	}
}

void CLookService::ProcessLookHero(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(len == sizeof(LookHeroCS));
    LookHeroCS *pLookHeroCS = reinterpret_cast<LookHeroCS*>(buffer);
    
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(pLookHeroCS->uid, false);
    if (NULL != pGameUser)
    {
        CHeroModel *pHeroModel = dynamic_cast<CHeroModel *>(pGameUser->getModel(MODELTYPE_HERO));
        CEquipModel *pEquipModel = dynamic_cast<CEquipModel *>(pGameUser->getModel(MODELTYPE_EQUIP));
        CHECK_RETURN_VOID(NULL != pHeroModel && NULL != pEquipModel);

        DBHeroAttr heroAttr;
        CHECK_RETURN_VOID(pHeroModel->GetHero(pLookHeroCS->heroId, heroAttr));

        int nSendLen = sizeof(LookHeroSC) + heroAttr.equipId.size() * sizeof(EquipItemInfo);
        char *pSendData = static_cast<char*>(kxMemMgrAlocate(nSendLen));

        int nOffset = sizeof(LookHeroSC);
        LookHeroSC *pLookHeroSC = reinterpret_cast<LookHeroSC*>(pSendData);
        pLookHeroSC->heroId = pLookHeroCS->heroId;
        pLookHeroSC->heroExp = heroAttr.exp;
        pLookHeroSC->heroLv = heroAttr.level;
        pLookHeroSC->heroStar = heroAttr.star;
        pLookHeroSC->equipCount = 0;
        memcpy(pLookHeroSC->heroTalent, heroAttr.talent, sizeof(pLookHeroSC->heroTalent));

        std::map<int, int>::iterator iter = heroAttr.equipId.begin();
        for (; iter != heroAttr.equipId.end(); ++iter)
        {
            if (iter->second <= 0)
            {
                continue;
            }

            SEquipInfo* pEquipInfo = pEquipModel->GetEquipData(iter->second);
            if (NULL == pEquipInfo)
            {
                continue;
            }

            ++pLookHeroSC->equipCount;

            EquipItemInfo *pEquipItemInfo = reinterpret_cast<EquipItemInfo*>(pSendData + nOffset);
            pEquipItemInfo->equipId = pEquipInfo->nDnycEquipID;
            pEquipItemInfo->confId = pEquipInfo->nItemID;
            pEquipItemInfo->cMainPropNum = pEquipInfo->cMainPropNum;
            memcpy(pEquipItemInfo->cEffectID, pEquipInfo->cEffectID, sizeof(pEquipInfo->cEffectID));
            memcpy(pEquipItemInfo->sEffectValue, pEquipInfo->sEffectValue, sizeof(pEquipInfo->sEffectValue));
            nOffset += sizeof(EquipItemInfo);
        }

        CKxCommManager::getInstance()->sendData(uid, CMD_LOOK, CMD_LOOK_HERO_SC,
            reinterpret_cast<char *>(pSendData), nOffset);
    }
    else
    {
        int heroId = -1;
        CKxCommManager::getInstance()->sendData(uid, CMD_LOOK, CMD_LOOK_HERO_SC, 
            reinterpret_cast<char *>(&heroId), sizeof(int));
    }
}

void CLookService::ProcessLookEquip(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CHECK_RETURN_VOID(len == sizeof(LookEquipCS));
    LookEquipCS *pLookEquipCS = reinterpret_cast<LookEquipCS*>(buffer);

    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(pLookEquipCS->uid, false);
    if (NULL != pGameUser)
    {
        CEquipModel *pEquipModel = dynamic_cast<CEquipModel *>(pGameUser->getModel(MODELTYPE_EQUIP));
        CHECK_RETURN_VOID(NULL != pEquipModel);

        SEquipInfo* pEquipInfo = pEquipModel->GetEquipData(pLookEquipCS->equipDynId);
        CHECK_RETURN_VOID(NULL != pEquipInfo);

        int nSendLen = sizeof(LookEquipSC)+sizeof(EquipItemInfo);
        char *pSendData = static_cast<char*>(kxMemMgrAlocate(nSendLen));

        int *pUid = reinterpret_cast<int*>(pSendData);
        *pUid = pLookEquipCS->uid;

        EquipItemInfo *pEquipItemInfo = reinterpret_cast<EquipItemInfo*>(pUid + 1);;
        pEquipItemInfo->equipId = pEquipInfo->nDnycEquipID;
        pEquipItemInfo->confId = pEquipInfo->nItemID;
        pEquipItemInfo->cMainPropNum = pEquipInfo->cMainPropNum;
        memcpy(pEquipItemInfo->cEffectID, pEquipInfo->cEffectID, sizeof(pEquipInfo->cEffectID));
        memcpy(pEquipItemInfo->sEffectValue, pEquipInfo->sEffectValue, sizeof(pEquipInfo->sEffectValue));

        CKxCommManager::getInstance()->sendData(uid, CMD_LOOK, CMD_LOOK_EQUIP_SC, pSendData, nSendLen);
    }
    else
    {
        int uid = -1;
        CKxCommManager::getInstance()->sendData(uid, CMD_LOOK, CMD_LOOK_EQUIP_SC,
            reinterpret_cast<char *>(&uid), sizeof(int));
    }
}
