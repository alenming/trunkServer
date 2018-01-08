#include "BattleModels.h"
#include "RoleComm.h"
#include "ConfStage.h"
#include "ConfGameSetting.h"
#include "ConfLanguage.h"

#ifndef RunningInServer
#include "Game.h"
#endif

using namespace std;

bool judgeValue(int judgeType, int left, int right)
{
	switch (judgeType)
	{
	case Greater:
		return left > right;
	case Less:
		return left < right;
	case Equal:
		return left == right;
	case GreaterEqual:
		return left >= right;
	case LessEqual:
		return left <= right;
	}
	return false;
}

void addBonusEffect(CRoleModel* role, int bounsId)
{
	CHECK_RETURN_VOID(role);
	CSoldierModel* solider = reinterpret_cast<CSoldierModel*>(role);
	const OutterBonusItem* bonus = queryConfOutterBonusItem(bounsId);
	CHECK_RETURN_VOID(bonus);
	for (size_t i = 0; i < bonus->EnhanceConditions.size(); ++i)
	{
		bool b = false;
		const VecInt& params = bonus->EnhanceConditions[i].Param;
		switch (bonus->EnhanceConditions[i].Type)
		{
		case EnhanceConditionId:
			for (size_t j = 0; j < params.size(); ++j)
			{
				if (params[j] == role->getRoleComm()->ClassID)
				{
					b = true;
					break;
				}
			}
			break;
		case EnhanceConditionRoleType:
			for (size_t j = 0; j < params.size(); ++j)
			{
				if (params[j] == role->getRoleType())
				{
					b = true;
					break;
				}
			}
			break;
		case EnhanceConditionStar:
			if (solider && params.size() == 2)
			{
				if (judgeValue(params[0], solider->getStar(), params[1]))
				{
					continue;
				}
			}
			break;
		case EnhanceConditionCrystalCost:
			if (solider && params.size() == 2)
			{
				if (judgeValue(params[0], solider->CurCost, params[1]))
				{
					continue;
				}
			}
			break;
		case EnhanceConditionRace:
			for (size_t j = 0; j < params.size(); ++j)
			{
				if (params[j] == role->getClassInt(ECardRace))
				{
					b = true;
					break;
				}
			}
			break;
		case EnhanceConditionSex:
			for (size_t j = 0; j < params.size(); ++j)
			{
				if (params[j] == role->getClassInt(ECardSex))
				{
					b = true;
					break;
				}
			}
			break;
		case EnhanceConditionVocation:
			for (size_t j = 0; j < params.size(); ++j)
			{
				if (params[j] == role->getClassInt(ECardVocation))
				{
					b = true;
					break;
				}
			}
			break;
		case EnhanceConditionAttackType:
			for (size_t j = 0; j < params.size(); ++j)
			{
				if (params[j] == role->getClassInt(ECardAttackType))
				{
					b = true;
					break;
				}
			}
			break;
		}
		// 任何条件不满足，都直接返回
		if (!b)
		{
			return;
		}
	}

	for (size_t i = 0; i < bonus->Enhances.size(); ++i)
	{
		role->addEffect(bonus->Enhances[i].EffectId, bonus->Enhances[i].Param);
	}
}

CRoleModel::CRoleModel() :m_nLevel(0)
, m_nRoleType(0)
, m_pRole(NULL)
, m_bEncrypt(false)
{
	m_ClassIntMap.clear();
	m_ClassFloatMap.clear();
	m_IntAddMap.clear();
	m_IntPercentMap.clear();
	m_FloatAddMap.clear();
	m_FloatPercentMap.clear();
}

CRoleModel::~CRoleModel()
{

}

bool CRoleModel::init(int roleType, int level, const Role* role, int battleType)
{
	CHECK_RETURN(role);
	m_nLevel = level;
	m_nRoleType = roleType;
	m_pRole = role;
    m_bEncrypt = false;

	if (role->HPLine.size() == 3)
	{
		m_ClassIntMap[EClassHPLine] = role->HPLine[0];
		m_ClassIntMap[EClassHPPosX] = role->HPLine[1];
		m_ClassIntMap[EClassHPPosY] = role->HPLine[2];
	}
	else
	{
		m_ClassIntMap[EClassHPPosX] = 0;
		m_ClassIntMap[EClassHPPosY] = 0;
	}
	m_ClassIntMap[EClassStatusID] = role->StatusID;
	m_ClassIntMap[EClassAIID] = role->AIID;

	// 卡片属性
	m_ClassIntMap[ECardRace] = role->Race;
	m_ClassIntMap[ECardSex] = role->Sex;
	m_ClassIntMap[ECardAttackType] = role->AttackType;
	m_ClassIntMap[ECardVocation] = role->Vocation;

	// 成长Class属性 —— 可以不读
	m_ClassIntMap[EClassHPGrowUp] = role->HPGrowUp;
	m_ClassIntMap[EClassPAttackGrowUp] = role->PAttackGrowUp;
	m_ClassIntMap[EClassMAttackGrowUp] = role->MAttackGrowUp;
	m_ClassIntMap[EClassPGuardGrowUp] = role->PGuardGrowUp;
	m_ClassIntMap[EClassMGuardGrowUp] = role->MGuardGrowUp;
	m_ClassFloatMap[EClassCritGrowUp] = role->CritGrowUp;

	int lv = level - 1;
	// Class属性 —— int
	m_ClassIntMap[EClassAnimationId] = role->AnimationID;
	m_ClassIntMap[EClassFireRange] = role->FireRange;
	m_ClassIntMap[EClassFarFireRange] = role->FarFireRange;
	m_ClassIntMap[EClassPAttack] = role->PAttack + lv * role->PAttackGrowUp;
	m_ClassIntMap[EClassMAttack] = role->MAttack + lv * role->MAttackGrowUp;
	m_ClassIntMap[EClassPGuard] = role->PGuard + lv * role->PGuardGrowUp;
	m_ClassIntMap[EClassMGuard] = role->MGuard + lv * role->MGuardGrowUp;
	m_ClassIntMap[EClassPPenetrate] = role->PPenetrate;
	m_ClassIntMap[EClassMPenetrate] = role->MPenetrate;
	m_ClassIntMap[EClassAttackSpeed] = role->AttackSpeed;
	m_ClassIntMap[EClassSpeed] = role->Speed;
	m_ClassIntMap[EClassHP] = role->HP + lv * role->HPGrowUp;;
	m_ClassIntMap[EClassRage] = role->Rage;
	m_ClassIntMap[EClassRageRecover] = role->RageRecover;
	m_ClassIntMap[EClassMP] = role->MP;
	m_ClassIntMap[EClassMPRecover] = role->MPRecover;
	m_ClassIntMap[EClassStrong] = role->Strong;
	m_ClassIntMap[EClassStrongRecover] = role->StrongRecover;
	m_ClassIntMap[EClassHaterd] = role->Haterd;

	// Float属性 —— float
	m_ClassFloatMap[EClassScale] = role->Scale;
	m_ClassFloatMap[EClassEffectScale] = role->EffectScale;
	m_ClassFloatMap[EClassDefend] = role->Defend;
	m_ClassFloatMap[EClassResustance] = role->Resustance;
	m_ClassFloatMap[EClassVampire] = role->Vampire;
	m_ClassFloatMap[EClassRebound] = role->Rebound;
	m_ClassFloatMap[EClassMiss] = role->Miss;
	m_ClassFloatMap[EClassCrit] = role->Crit + lv * role->CritGrowUp;
	m_ClassFloatMap[EClassCritDamage] = role->CritDamage;
	m_ClassFloatMap[EClassMass] = role->Mass;

    if (battleType != EBATTLE_PVP)
    {
        // 非pvp需要加密
        m_bEncrypt = true;

        std::map<EAttributeTypes, int>::iterator iter1 = m_ClassIntMap.begin();
        for (; iter1 != m_ClassIntMap.end(); ++iter1)
        {
            m_ClassIntMap[iter1->first] = encryptNumber(iter1->first, iter1->second);
        }
        std::map<EAttributeTypes, float>::iterator iter2 = m_ClassFloatMap.begin();
        for (; iter2 != m_ClassFloatMap.end(); ++iter2)
        {
            m_ClassFloatMap[iter2->first] = encryptNumber(iter2->first, iter2->second);
        }
    }

	return true;
}

void CRoleModel::addEffect(int effId, int effValue)
{
	// 从效果配置中获取其对于的属性类型和加成方式
	const OutterBonusSetting* setting = queryConfOutterBonusSetting(effId);
	if (setting == NULL)
	{
		LOG("Error! queryConfOutterBonusSetting %d is NULL", effId);
		return;
	}
	EAttributeTypes type = static_cast<EAttributeTypes>(setting->AttributeID);
	EEquipAddType addType = static_cast<EEquipAddType>(setting->Method);
	switch (addType)
	{
	case EEquipAdd:
		if (isClassInt(type))
		{
			m_IntAddMap[type] += effValue;
		}
		else if (isClassFloat(type))
		{
			m_FloatAddMap[type] += effValue;
		}
		break;
	case EEquipPercent:
		if (isClassInt(type))
		{
			m_IntPercentMap[type] += effValue * 1.0f / 100.0f;
		}
		else if (isClassFloat(type))
		{
			m_FloatPercentMap[type] += effValue * 1.0f / 100.0f;
		}
		break;
    case EEquipAddPercent:
        if (isClassInt(type))
        {
            m_IntAddMap[type] += effValue * 1.0f / 100.0f;
        }
        else if (isClassFloat(type))
        {
            m_FloatAddMap[type] += effValue * 1.0f / 100.0f;
        }
        break;
	default:
		break;
	}
}

// 应用附加效果 - 先计算百分比对基础属性的加成数值，再计算固定加成的总和
void CRoleModel::applyEffect()
{
	for (map<EAttributeTypes, float>::iterator iter = m_IntPercentMap.begin();
		iter != m_IntPercentMap.end(); ++iter)
	{
        setClassInt(iter->first, getClassInt(iter->first) * (1.0f + iter->second));
	}
	m_IntPercentMap.clear();
	for (map<EAttributeTypes, int>::iterator iter = m_IntAddMap.begin();
		iter != m_IntAddMap.end(); ++iter)
	{
        setClassInt(iter->first, getClassInt(iter->first) + iter->second);
	}
	m_IntAddMap.clear();
	for (map<EAttributeTypes, float>::iterator iter = m_FloatPercentMap.begin();
		iter != m_FloatPercentMap.end(); ++iter)
	{
        setClassFloat(iter->first, getClassFloat(iter->first) * (1.0f + iter->second));
	}
	m_FloatPercentMap.clear();
	for (map<EAttributeTypes, float>::iterator iter = m_FloatAddMap.begin();
		iter != m_FloatAddMap.end(); ++iter)
	{
        setClassFloat(iter->first, getClassFloat(iter->first) + iter->second);
	}
	m_FloatAddMap.clear();
}

bool CRoleModel::classIntKeyExist(const EAttributeTypes& key) const
{
    return m_ClassIntMap.find(key) != m_ClassIntMap.end();
}

bool CRoleModel::classFloatKeyExist(const EAttributeTypes& key) const
{
    return m_ClassFloatMap.find(key) != m_ClassFloatMap.end();
}

int CRoleModel::getClassInt(EAttributeTypes type)
{
	map<EAttributeTypes, int>::const_iterator iter = m_ClassIntMap.find(type);
	if (iter != m_ClassIntMap.end())
	{
        int num = decodeNumber(type, iter->second);
        switch (type)
        {
        case EClassFireRange:
        case EClassFarFireRange:
            roleTypeLimit(num, 0, 5000, 0, 5000);
            break;
        case EClassSpeed:
            roleTypeLimit(num, 33, 600, 33, 600);
            break;
        case EClassHP:
        case EClassPAttack:
        case EClassMAttack:
            roleTypeLimit(num, 0, 200000, 0, 99999999);
            break;
        case EClassHPGrowUp:
        case EClassPAttackGrowUp:
        case EClassMAttackGrowUp:
            roleTypeLimit(num, 0, 20000, 0, 9999999);
            break;
        case EClassPGuard:
        case EClassMGuard:
        case EClassPPenetrate:
            roleTypeLimit(num, 0, 75000, 0, 75000);
            break;
        case EClassPGuardGrowUp:
        case EClassMGuardGrowUp:
        case EClassMPenetrate:
            roleTypeLimit(num, 0, 7500, 0, 7500);
            break;
        case EClassAttackSpeed:
            roleTypeLimit(num, 0, 1000, 0, 1000);
            break;
        case EClassRage:
        case EClassRageRecover:
        case EClassMP:
        case EClassMPRecover:
            roleTypeLimit(num, 0, 10000, 0, 10000);
            break;
        case EClassStrong:
            roleTypeLimit(num, 1, 100, 1, 100);
            break;
        case EClassStrongRecover:
        case EClassHaterd:
            roleTypeLimit(num, 1, 10000, 1, 10000);
            break;
        default:
            break;
        }
        return num;
	}
	return 0;
}

float CRoleModel::getClassFloat(EAttributeTypes type)
{
	map<EAttributeTypes, float>::const_iterator iter = m_ClassFloatMap.find(type);
	if (iter != m_ClassFloatMap.end())
	{
        float num = decodeNumber(type, iter->second);
        switch (type)
        {
        case EClassDefend:
        case EClassResustance:
            roleTypeLimit(num, 0.f, 1.f, 0.f, 1.f);
            break;
        case EClassVampire:
        case EClassRebound:
        case EClassMiss:
        case EClassCrit:
        case EClassCritGrowUp:
            roleTypeLimit(num, 0.f, 100.f, 0.f, 100.f);
            break;
        case EClassCritDamage:
            roleTypeLimit(num, 0.f, 1000.f, 0.f, 1000.f);
            break;
        case EClassMass:
            roleTypeLimit(num, 0.001f, 1000.f, 0.001f, 1000.f);
            break;
        default:
            break;
        }
        return num;
	}
	return 0.0f;
}

void CRoleModel::setClassInt(const EAttributeTypes& key, int value)
{
    m_ClassIntMap[key] = encryptNumber(key, value);
}

void CRoleModel::setClassFloat(const EAttributeTypes& key, float value)
{
    m_ClassFloatMap[key] = encryptNumber(key, value);
}

std::map<EAttributeTypes, int> CRoleModel::getClassIntMap()
{
    std::map<EAttributeTypes, int> tempMap;
    std::map<EAttributeTypes, int>::iterator iter = m_ClassIntMap.begin();
    for (; iter != m_ClassIntMap.end(); ++iter)
    {
        tempMap[iter->first] = decodeNumber(iter->first, iter->second);
    }
    return tempMap;
}

std::map<EAttributeTypes, float> CRoleModel::getClassFloatMap()
{
    std::map<EAttributeTypes, float> tempMap;
    std::map<EAttributeTypes, float>::iterator iter = m_ClassFloatMap.begin();
    for (; iter != m_ClassFloatMap.end(); ++iter)
    {
        tempMap[iter->first] = decodeNumber(iter->first, iter->second);
    }
    return tempMap;
}

void CRoleModel::roleTypeLimit(int v, int min1, int max1, int min2, int max2)
{
    if (m_nRoleType == RT_HERO || m_nRoleType == RT_SOLDIER)
    {
        limitValue(v, min1, max1);
    }
    else if (m_nRoleType == RT_BOSS || m_nRoleType == RT_MONSTER || m_nRoleType == RT_SUMMON)
    {
        limitValue(v, min2, max2);
    }
    else
    {
        KXLOGERROR("error role type %d ", m_nRoleType);
#ifndef RunningInServer
        if (CGame::getInstance()->isDebug())
        {
            cocos2d::MessageBox(String::createWithFormat("role type %d", m_nRoleType)->getCString(), "error roleType");
        }
#endif
    }
}

void CRoleModel::roleTypeLimit(float v, float min1, float max1, float min2, float max2)
{
    if (m_nRoleType == RT_HERO || m_nRoleType == RT_SOLDIER)
    {
        limitValue(v, min1, max1);
    }
    else if (m_nRoleType == RT_BOSS || m_nRoleType == RT_MONSTER || m_nRoleType == RT_SUMMON)
    {
        limitValue(v, min2, max2);
    }
    else
    {
        KXLOGERROR("error role type %d ", m_nRoleType);
#ifndef RunningInServer
        if (CGame::getInstance()->isDebug())
        {
            cocos2d::MessageBox(String::createWithFormat("role type %d", m_nRoleType)->getCString(), "error roleType");
        }
#endif
    }
}

int CSoldierModel::init(SoldierCardData* soldierData, int battleType, bool bMercenary)
{
	// 准备数据
	char* pData = reinterpret_cast<char*>(soldierData);
	int offset = 0;

	m_iId = soldierData->SoldierId;
	m_iStar = soldierData->SoldierStar;
    memcpy(m_Talent, soldierData->SoldierTalent, sizeof(m_Talent));

	m_pConf = queryConfSoldier(m_iId, m_iStar);
    if (NULL == m_pConf || !CRoleModel::init(RT_SOLDIER, soldierData->SoldierLv, &m_pConf->Common, battleType))
	{
        KXLOGBATTLE("CSoldierModel init error soldierId %d star %d", m_iId, m_iStar);
		LOG("CSoldierModel init error id %d star %d", m_iId, m_iStar);
		return -1;
	}
	
	// 装备
	offset += sizeof(SoldierCardData);
	for (int i = 0; i < soldierData->EquipCnt; ++i)
	{
		SoldierEquip* equip = reinterpret_cast<SoldierEquip*>(pData + offset);
        m_vecEquips.push_back(*equip);
		offset += sizeof(SoldierEquip);
	}

	m_nAI = m_pConf->Common.AIID;
	CurCost = m_pConf->Cost;
	MaxCD = m_pConf->CD;
	CurCD = MaxCD;
	IsLock = false;
	IsSingo = m_pConf->IsSingo != 0;

	// 装备加成（失败返回-1）
    if (!calcEquipAddition(m_vecEquips))
	{
        KXLOGBATTLE("CSoldierModel init calcEquipAddition error soldierId %d star %d", m_iId, m_iStar);
		return -1;
	}
    // 天赋加成（失败返回-1）
    if (!calcTalentAddition())
    {
        KXLOGBATTLE("CSoldierModel init calcTalentAddition error soldierId %d star %d", m_iId, m_iStar);
        return -1;
    }

    IsMercenary = bMercenary;

	return offset;
}

bool CSoldierModel::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);
	//序列化士兵卡片数据
	data.writeData(CurCost);
	data.writeData(CurCD);
	data.writeData(IsLock);
	data.writeData(IsSingo);
	SERIALIZE_MATCH(data);
	return true;
}

bool CSoldierModel::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);
	//反序列化士兵卡片数据
	data.readData(CurCost);
	data.readData(CurCD);
	data.readData(IsLock);
	data.readData(IsSingo);
	UN_SERIALIZE_MATCH(data);
	return true;
}

bool CSoldierModel::calcEquipAddition(std::vector<SoldierEquip>& effects)
{
	// 统计套装加成<套装ID, 件数>
	std::map<int, int> mapSuit;
	const EquipmentItem *pEquipmentItem = NULL;
	std::vector<SoldierEquip>::iterator iterEquip = effects.begin();
	for (; iterEquip != effects.end(); iterEquip++)
	{
		pEquipmentItem = queryConfEquipment((*iterEquip).confId);
        if (NULL == pEquipmentItem)
        {
            LOG("CSoldierModel::calcEquipAddition queryConfEquipment %d faile", (*iterEquip).confId);
            return false;
        }

		for (int i = 0; i < 8; i++)
		{
			if ((*iterEquip).cEffectID[i] != 0)
			{
				addEffect((*iterEquip).cEffectID[i], (*iterEquip).sEffectValue[i]);
			}
		}

		for (map<int, int>::const_iterator iter = pEquipmentItem->ExtEffect.Buff.begin();
			iter != pEquipmentItem->ExtEffect.Buff.end(); ++iter)
		{
			m_mapAddBuff[iter->first] += iter->second;
		}

		// 套装加成
		if (pEquipmentItem->Suit > 0)
		{
			mapSuit[pEquipmentItem->Suit] += 1;
		}
	}

	std::map<int, int>::iterator iterSuit = mapSuit.begin();
	for (; iterSuit != mapSuit.end(); iterSuit++)
	{
		const SuitItem *pSuitItem = NULL;
		// 两件以上才有套装加成
		if (iterSuit->second >= 2)
		{
			pSuitItem = queryConfSuit(iterSuit->first);
            if (NULL == pSuitItem)
            {
                LOG("CSoldierModel::calcEquipAddition queryConfSuit %d faile", iterSuit->first);
                return false;
            }

			std::map<int, Ability>::const_iterator iterSuitItem = pSuitItem->SuitAbility.find(iterSuit->first);
			if (iterSuitItem != pSuitItem->SuitAbility.end())
			{
				addEffect(iterSuitItem->second.AbilityID, iterSuitItem->second.AbilityParam);
			}
			std::map<int, EquipmentEffect>::const_iterator iterExtEffect = pSuitItem->SuitExtEffect.find(iterSuit->first);
			if (iterExtEffect != pSuitItem->SuitExtEffect.end())
			{
				for (map<int, int>::const_iterator iter = iterExtEffect->second.Buff.begin();
					iter != iterExtEffect->second.Buff.end(); ++iter)
				{
					m_mapAddBuff[iter->first] += iter->second;
				}
			}
		}
	}
	return true;
}

bool CSoldierModel::calcTalentAddition()
{
    for (int i = 0; i < 8; i++)
    {
        const STalentData *pTalentData = queryTalentData(m_Talent[i]);
        if (NULL == pTalentData)
        {
            continue;
        }

        std::map<int, int>::const_iterator iterOutterBonus = pTalentData->OutterBonus.begin();
        for (; iterOutterBonus != pTalentData->OutterBonus.end(); ++iterOutterBonus)
        {
            addEffect(iterOutterBonus->first, iterOutterBonus->second);
        }

        for (unsigned int i = 0; i < pTalentData->BuffId.size(); ++i)
        {
            m_mapAddBuff[pTalentData->BuffId[i].ID] += pTalentData->BuffId[i].num;
        }
    }

    return true;
}

CBattlePlayerModel::CBattlePlayerModel() :m_nUserId(0)
, m_nUserLv(0)
, m_Camp(0)
, m_nIdentity(0)
, m_UserName("")
, m_RoleModel(NULL)
{

}

CBattlePlayerModel::~CBattlePlayerModel()
{
	SAFE_DELETE(m_RoleModel);
}

CPlayerModel::CPlayerModel()
{
}

CPlayerModel::~CPlayerModel()
{
	// 析构释放内存
	deleteAndClearVec(m_SoldierCards);
}

int CPlayerModel::initByUserData(PlayerData* data, int battleType)
{
	char* pData = reinterpret_cast<char*>(data);
	int offset = sizeof(PlayerData);

	m_nUserId = data->UserId;
	m_nUserLv = data->UserLv;
	m_Camp = data->Camp;
	m_UserName = data->UserName;
    m_nIdentity = data->Identity;

	for (int i = 0; i < data->OuterBonusCount; ++i)
	{
		int addId = *(reinterpret_cast<int*>(pData + offset));
		m_Additions.push_back(addId);
		offset += sizeof(int);
	}

	HeroCardData* heroData = reinterpret_cast<HeroCardData*>(pData + offset);
	m_nMainRoleId = heroData->HeroId;
	m_nMainRoleLv = m_nUserLv;
	offset += sizeof(HeroCardData);

	const HeroConfItem* hero = queryConfHero(m_nMainRoleId);
	CHECK_RETURN_INT(hero);
    KXLOGBATTLE("^^^^^^^^^^userId %d userLv %d heroId %d^^^^^^^^^^", 
        m_nUserId, m_nUserLv, m_nMainRoleId);
	m_RoleModel = new CRoleModel();
	CHECK_RETURN_INT(m_RoleModel->init(RT_HERO, m_nMainRoleLv, &hero->Common, battleType));
	m_RoleModel->applyEffect();

	// deleteAndClearVec(m_SoldierCards);
	for (int i = 0; i < data->SoldierCount; ++i)
	{
		SoldierCardData* soldierData = reinterpret_cast<SoldierCardData*>(pData + offset);
		CSoldierModel* soldier = new CSoldierModel();
		m_SoldierCards.push_back(soldier);
        int ret = soldier->init(soldierData, battleType, false);
		CHECK_RETURN_INT(-1 != ret);
		calcAddition(soldier);
		// 附加额外效果
		soldier->applyEffect();
		offset += ret;
        KXLOGBATTLE("^^^^^^^^^^userId %d index %d soldierId %d star %d", 
            m_nUserId, i, soldier->getSoldId(), soldier->getStar());
	}

    for (int i = 0; i < data->MecenaryCount; ++i)
    {
        SoldierCardData* soldierData = reinterpret_cast<SoldierCardData*>(pData + offset);
        CSoldierModel* soldier = new CSoldierModel();
        //m_SoldierCards.push_back(soldier);
        m_SoldierCards.insert(m_SoldierCards.begin(), soldier);
        int ret = soldier->init(soldierData, battleType, true);
        CHECK_RETURN_INT(-1 != ret);
        calcAddition(soldier);
        // 附加额外效果
        soldier->applyEffect();
        offset += ret;
        KXLOGBATTLE("^^^^^^^^^^userId %d index %d soldierId %d star %d",
            m_nUserId, i, soldier->getSoldId(), soldier->getStar());
    }

	return offset;
}

void CPlayerModel::calcAddition(CRoleModel* solider)
{
	for (size_t i = 0; i < m_Additions.size(); ++i)
	{
		addBonusEffect(solider, m_Additions[i]);
	}
}

CSoldierModel* CPlayerModel::getSoldierCard(int index)
{
	if (index < (int)m_SoldierCards.size())
	{
		return m_SoldierCards[index];
	}
	return NULL;
}

bool CPlayerModel::fillBufferData(CBufferData& buff)
{
    buff.writeData(m_nUserId);
    buff.writeData(m_nUserLv);
    buff.writeData(m_Camp);
    buff.writeData((int)m_Additions.size());
    buff.writeData((int)m_SoldierCards.size());
    buff.writeData(0);
    buff.writeData(m_nIdentity);
    buff.writeData(m_UserName.c_str(), 32);

    std::vector<int>::iterator additionIter = m_Additions.begin();
    for (; additionIter != m_Additions.end(); ++additionIter)
    {
        buff.writeData(*additionIter);
    }

    buff.writeData(m_nMainRoleId);

    std::vector<CSoldierModel*>::iterator soldierIter = m_SoldierCards.begin();
    for (; soldierIter != m_SoldierCards.end(); ++soldierIter)
    {
        buff.writeData((*soldierIter)->getSoldId());
        buff.writeData((*soldierIter)->getLevel());
        buff.writeData((*soldierIter)->getStar());
        buff.writeData(0);

        buff.writeData((*soldierIter)->getTalent(), sizeof(((struct SoldierCardData *)0)->SoldierTalent));

        const vector<SoldierEquip> eqs = (*soldierIter)->getEquips();
        buff.writeData((int)eqs.size());
        vector<SoldierEquip>::const_iterator iter = eqs.begin();
        for (; iter != eqs.end(); ++iter)
        {
            buff.writeData((*iter).confId);
            buff.writeData((*iter).cEffectID, sizeof(((struct SoldierEquip *)0)->cEffectID));
            buff.writeData((*iter).sEffectValue, sizeof(((struct SoldierEquip *)0)->sEffectValue));
        }
    }

    return true;
}

CComputerModel::CComputerModel()
{
}

CComputerModel::~CComputerModel()
{
	// 析构释放内存
	deleteAndClearVec(m_MonsterModels);
}

int CComputerModel::init(int stageId, int level, int battleType)
{
	m_nUserId = EDefaultNpc;
	m_nUserLv = level;
	m_Camp = ECamp_Red;

	const StageConfItem *pStageConf = queryConfStage(stageId);
	CHECK_RETURN_INT(NULL != pStageConf);

	const BossConfItem *pBoss = queryConfBoss(pStageConf->Boss);
	CHECK_RETURN_INT(pBoss);
	m_RoleModel = new CRoleModel();
	CHECK_RETURN_INT(m_RoleModel->init(RT_BOSS, level, &pBoss->Common, battleType));
	m_UserName = getLanguageString(CONF_BMC_LAN, pBoss->Common.Name);
	m_RoleModel->applyEffect();

	m_nMainRoleId = pBoss->Common.ClassID;
	m_nMainRoleLv = level;

	for (size_t i = 0; i < pStageConf->Monsters.size(); ++i)
	{
		const MonsterConfItem* pMonster = queryConfMonster(pStageConf->Monsters[i]);
		CHECK_RETURN_INT(pMonster);
		CRoleModel* monster = new CRoleModel();
		m_MonsterModels.push_back(monster);
		CHECK_RETURN_INT(monster->init(RT_MONSTER, level, &pMonster->Common, battleType));
		monster->applyEffect();
	}
	return 0;
}

CRoleModel* CComputerModel::getMonsterModel(int index)
{
	if (index < (int)m_MonsterModels.size())
	{
		return m_MonsterModels[index];
	}
	return NULL;
}

CRoomModel::CRoomModel()
{
	m_pSettleAccount = new CSettleAccountModel;
}

CRoomModel::~CRoomModel()
{
	deleteAndClearMap(m_Players);
	SAFE_DELETE(m_pSettleAccount);
}

bool CRoomModel::initByRoomData(RoomData* data)
{
	CHECK_RETURN(NULL != data);
	m_nMaster = -1;
	m_nOther = -1;
	m_nStageId = data->StageId;
	m_nMonsterLevel = data->StageLevel;
	m_nBattleType = data->BattleType;
	m_nExt1 = data->Ext1;
	m_nExt2 = data->Ext2;

	char* pData = reinterpret_cast<char*>(data + 1);
	int offset = 0;
	for (int i = 0; i < data->BuffCount; ++i)
	{
		BuffData *pBuffData = reinterpret_cast<BuffData *>(pData + offset);
		m_StageBuffs.push_back(*pBuffData);
		offset += sizeof(BuffData);
	}

	for (int i = 0; i < data->PlayerCount; ++i)
	{
		PlayerData* playerData = reinterpret_cast<PlayerData*>(pData + offset);
		if (playerData->Camp == ECamp_Blue)
		{
			m_nMaster = playerData->UserId;
		}

		CBattlePlayerModel* player = NULL;
		if (m_Players.find(playerData->UserId) == m_Players.end())
		{
			player = new CPlayerModel();
		}
		else
		{
			player = m_Players[playerData->UserId];
		}

		CPlayerModel *pPlayer = dynamic_cast<CPlayerModel*>(player);
		int ret = -1;
		if (pPlayer)
		{
			ret = pPlayer->initByUserData(playerData, m_nBattleType);
		}

		CHECK_RETURN(-1 != ret);
		if (m_Players.find(playerData->UserId) == m_Players.end())
		{
			addPlayerModel(player);
		}

		offset += ret;
	}

	if (EBATTLE_PVP != m_nBattleType)
	{
		CComputerModel *pComputerModel = new CComputerModel();
		CHECK_RETURN(0 == pComputerModel->init(m_nStageId, m_nMonsterLevel, m_nBattleType));
		addPlayerModel(pComputerModel);
	}

	return true;
}

bool CRoomModel::initPVPRoomBylayer(int stageId, CBattlePlayerModel* master, CBattlePlayerModel* other)
{
    CHECK_RETURN(NULL != master);
    CHECK_RETURN(NULL != other);
    m_nMaster = master->getUserId();
    m_nOther = other->getUserId();
    m_nStageId = stageId;
    m_nBattleType = EBATTLE_PVP;
    addPlayerModel(master);
    addPlayerModel(other);
    return true;
}

CSettleAccountModel::CSettleAccountModel()
: m_nChallengeResult(0)
, m_nTick(0)
, m_nHeroHpPercent(0)
, m_nHitBossHP(0)
, m_nCostCrystal(0)
, m_nCrystal(0)
{
}

CSettleAccountModel::~CSettleAccountModel()
{
}

void CSettleAccountModel::resetSettle()
{
	m_nChallengeResult = CHALLENGE_FAILED;
	m_nTick = 0;
	m_nHeroHpPercent = 0;
	m_nHitBossHP = 0;
	m_nCostCrystal = 0;
	m_nCrystal = 0;
}
