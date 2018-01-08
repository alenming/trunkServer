#include "ConfFight.h"
using namespace std;

#define MSTOS(ms)       (ms) / 1000.0f       //毫秒转成秒

// 填充结算容器
void fillCountVec(int times, CCsvLoader& loader, std::vector<CountExpressionConfItem*>& out)
{
	for (int i = 0; i < times; ++i)
	{
        CountExpressionConfItem* value = new CountExpressionConfItem();
		if (value->init(loader.NextStr()))
		{
			out.push_back(value);
		}
		else
		{
			delete value;
		}
	}
}

CountExpressionConfItem::CountExpressionConfItem()
: Value(0.0f)
, AttributeId(0)
, Operator(OperatorUnvalid)
, LeftExpression(NULL)
, RightExpression(NULL)
{
}

CountExpressionConfItem::~CountExpressionConfItem()
{
    SAFE_DELETE(LeftExpression);
    SAFE_DELETE(RightExpression);
}

bool CountExpressionConfItem::init(const std::string& str)
{
    if (str.empty())
    {
        return false;
    }

    // 赋值运算符优先级最低
    size_t pos;
    if (std::string::npos != (pos = str.find('=')))
    {
        Operator = OperatorEqual;
        return initExpression(str, pos);
    }
    // +、- 运算符优先级高于 =，且为从左到右结合，应该找到最后的
    if (checkExpression(str, '+', '-', OperatorAdd, OperatorMinus, pos))
    {
        return initExpression(str, pos);
    }
    // *、/ 运算符优先级高于 +、-，且为从左到右结合，应该找到最后的
    if (checkExpression(str, '*', '/', OperatorMulti, OperatorDiv, pos))
    {
        return initExpression(str, pos);
    }

    // 没有找到上述操作符运算符，则为数值或熟悉表达式
    return initValue(str);
}

bool CountExpressionConfItem::initValue(const std::string& str)
{
    size_t pointPos = str.find('.');
    if (pointPos != std::string::npos)
    {
        std::string owner = str.substr(0, pointPos);
        if (owner == "S")
        {
            Target = TargetTypeSource;
            AttributeId = CConfManager::getInstance()->getRoleAttributeID(
                str.substr(pointPos + 1, str.size() - pointPos - 1));
            return true;
        }
        else if (owner == "T" || owner == "BT")
        {
            Target = TargetTypeTarget;
            AttributeId = CConfManager::getInstance()->getRoleAttributeID(
                str.substr(pointPos + 1, str.size() - pointPos - 1));
            return true;
        }
    }
    else if (0 == strncmp(str.c_str(), "Var", 3))
    {
        Target = TargetTypeVar;
        VarName = str;
        return true;
    }

    Target = TargetTypeValue;
    Value = (float)atof(str.c_str());
    return true;
}

bool CountExpressionConfItem::initExpression(const std::string& str, size_t operatorPos)
{
    LeftExpression = new CountExpressionConfItem();
    RightExpression = new CountExpressionConfItem();
    if (LeftExpression->init(str.substr(0, operatorPos))
        && RightExpression->init(str.substr(operatorPos + 1, str.size() - operatorPos - 1)))
    {
        return true;
    }
    return false;
}

// 传入字符串、同级操作符的符号，找出优先级最低的操作符，并将其位置输出的posOut参数中
bool CountExpressionConfItem::checkExpression(const std::string& str, char op1, char op2,
    CountExpressionOperatorType opType1, CountExpressionOperatorType opType2, size_t& posOut)
{
    size_t pos1 = str.find_last_of(op1);
    size_t pos2 = str.find_last_of(op2);
    if (pos1 != std::string::npos)
    {
        if (pos2 != std::string::npos && pos2 > pos1)
        {
            Operator = opType2;
            posOut = pos2;
        }
        else
        {
            Operator = opType1;
            posOut = pos1;
        }
        return true;
    }
    else if (pos2 != std::string::npos)
    {
        Operator = opType2;
        posOut = pos2;
        return true;
    }
    return false;
}

#ifndef RunningInServer
bool CConfEffect::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));

	vector<string> effAudios;
	while (pLoader.NextLine())
	{
		EffectConfItem* eff = new EffectConfItem();
		eff->EffectId = pLoader.NextInt();
		pLoader.NextStr();
		eff->ResID = pLoader.NextInt();
		eff->AnimationName = pLoader.NextStr();
        eff->LifeTime = pLoader.NextInt() * 1.0f / 1000;
		eff->AnimationSpeed = pLoader.NextFloat();
		eff->Loop = pLoader.NextInt();
		CConfAnalytic::ToVec2(pLoader.NextStr(), eff->Scale);
		eff->ZOrderType = pLoader.NextInt();
		eff->ZOrder = pLoader.NextInt();
		CConfAnalytic::ToVec2(pLoader.NextStr(), eff->Offset);
		CConfAnalytic::ToJsonFloat(pLoader.NextStr(), eff->AddColor);
		eff->FadeInTime = pLoader.NextInt() * 1.0f / 1000;
		eff->FadeOutTime = pLoader.NextInt() * 1.0f / 1000;
		CConfAnalytic::ToVecMusicInfos(pLoader.NextStr(), eff->MusicInfos);
        eff->SpeedAffect = pLoader.NextInt();
		m_Datas[eff->EffectId] = eff;
	}
	return true;
}

bool CConfUIEffect::LoadCSV(const std::string& str)
{
    CCsvLoader pLoader;
    CHECK_RETURN(loadCsv(pLoader, str, 3));
    //如果有数据 
    while (pLoader.NextLine())
    {
        UIEffectConfItem* ui = new UIEffectConfItem;
        ui->ID = pLoader.NextInt();
        pLoader.NextStr();
        ui->BlinkingResID = pLoader.NextInt();
        ui->BlinkingCsbAniName = pLoader.NextStr();
        ui->BackgroundResID = pLoader.NextInt();
        ui->BackgroundCsbAniName = pLoader.NextStr();
        ui->DarkDelayTime = pLoader.NextInt() * 1.0f / 1000.0f;
        ui->DarkAlpha = static_cast<int>((pLoader.NextInt() * 1.0f / 100.0f) * 255);
        ui->DarkTime = pLoader.NextInt() * 1.0f / 1000.0f;
        ui->DarkFadeInTime = pLoader.NextInt() * 1.0f / 1000.0f;
        ui->DarkFadeOutTime = pLoader.NextInt() * 1.0f / 1000.0f;
        ui->ShakeDelayTime = pLoader.NextInt() * 1.0f / 1000.0f;
        ui->ShakeLevel = pLoader.NextInt();
        ui->ShakeTime = pLoader.NextInt() * 1.0f / 1000.0f;
        ui->DoodadStartTime = pLoader.NextInt() * 1.0f / 1000.0f;
        ui->DoodadName = pLoader.NextStr();
        ui->DoodadContinuedTime = pLoader.NextInt() * 1.0f / 1000.0f;
        ui->DoodadBackName = pLoader.NextStr();
        ui->DoodadDistance = pLoader.NextFloat();
        ui->DoodadLoop = CConfAnalytic::ToBool(pLoader.NextStr());
        ui->DoodadBackLoop = CConfAnalytic::ToBool(pLoader.NextStr());
        ui->once = CConfAnalytic::ToBool(pLoader.NextStr());
        m_Datas[ui->ID] = ui;
    }
    return true;
}
#endif // RunningInServer

CConfBuff::~CConfBuff()
{
    std::map<int, std::map<int, BuffConfItem*> >::iterator iter = m_BuffData.begin();
    for (; iter != m_BuffData.end(); ++iter)
    {
        deleteAndClearMap(iter->second);
    }

    m_BuffData.clear();
}

bool CConfBuff::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据 
	while (pLoader.NextLine())
	{
		BuffConfItem* buff = new BuffConfItem;
		buff->ID = pLoader.NextInt();
		pLoader.NextStr();	//这一列不需要解析
		buff->Type = pLoader.NextInt();
		buff->Stack = pLoader.NextInt();
		buff->lifeTime = pLoader.NextInt();
        if (buff->lifeTime > 0)
        {
            buff->lifeTime = MSTOS(buff->lifeTime);
        }
		buff->NextBuffCondition = pLoader.NextInt();
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), buff->nextBuffID);
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), buff->AnimationID);
		buff->UIEffectID = pLoader.NextInt();
		buff->EffectWhere = pLoader.NextInt();
		buff->IsEffectMove = CConfAnalytic::ToBool(pLoader.NextStr());
		buff->TargetType = pLoader.NextInt();
		buff->FirstTime = MSTOS(pLoader.NextInt());
		buff->MaxCount = pLoader.NextInt();
		buff->TriggerType = pLoader.NextInt();
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), buff->TriggerTypeParam);

		for (int i = 0; i < 4; ++i)
		{
			CDataFunction* f = NULL;
			CConfAnalytic::ToFunction(pLoader.NextStr(), &f);
			if (NULL != f)
			{
				buff->Conditions.push_back(f);
			}
		}

        CConfAnalytic::ToJsonInt(pLoader.NextStr(), buff->CountID);
        CConfAnalytic::ToJsonInt(pLoader.NextStr(), buff->SummonerCountID);
        CConfAnalytic::ToJsonInt(pLoader.NextStr(), buff->LapseCountID);
        CConfAnalytic::ToJsonInt(pLoader.NextStr(), buff->SummonerLapseCountID);
		buff->SkillID = pLoader.NextInt();
		m_BuffData[buff->ID][buff->Stack] = buff;
	}
	return true;
}

BuffConfItem * CConfBuff::getData(int buffid, int stack)
{
	if (m_BuffData.find(buffid) != m_BuffData.end())
	{
		if (m_BuffData[buffid].find(stack) != m_BuffData[buffid].end())
		{
			return m_BuffData[buffid][stack];
		}
	}
	return NULL;
}

int CConfBuff::getMaxStack(int buffid)
{
	if (m_BuffData.find(buffid) != m_BuffData.end())
	{
		return m_BuffData[buffid].size();
	}
	return 0;
}

bool CConfBullet::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据 
	while (pLoader.NextLine())
	{
		BulletConfItem* bullet = new BulletConfItem;
		bullet->ID = pLoader.NextInt();
		pLoader.NextStr();	//这一列不需要解析
		bullet->Locus = pLoader.NextInt();
		CConfAnalytic::ToJsonFloat(pLoader.NextStr(), bullet->Locus_Param);
		bullet->FlyTime = MSTOS(pLoader.NextInt());
		bullet->LifeTime = MSTOS(pLoader.NextInt());
		bullet->StartType = pLoader.NextInt();
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), bullet->StartParam);
		bullet->EndType = pLoader.NextInt();
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), bullet->EndParam);
		bullet->HitType = pLoader.NextInt();
		bullet->HitJudgeType = pLoader.NextInt();
		bullet->HitJudgeParam = pLoader.NextInt();
		bullet->HitJudgeFirstTime = MSTOS(pLoader.NextInt());
		bullet->HitJudgeMaxTimes = pLoader.NextInt();
		bullet->HitJudgePerTime = MSTOS(pLoader.NextInt());
		bullet->UnitMaxCountTimes = pLoader.NextInt();
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), bullet->CountId);
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), bullet->SummonerCountId);
		CConfAnalytic::ToJsonID_Num(pLoader.NextStr(), bullet->BuffId);
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), bullet->AnimationId);
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), bullet->EndAnimationId);
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), bullet->HitAnimationId);
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), bullet->HitAllAnimationId);
		bullet->LockDirect = CConfAnalytic::ToBool(pLoader.NextStr());
		bullet->UiEffectTime = pLoader.NextInt();
		bullet->UiEffectId = pLoader.NextInt();
		bullet->ZOrderType = pLoader.NextInt();

		m_Datas[bullet->ID] = bullet;
	}
	return true;
}

bool CConfCardCount::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据 
	while (pLoader.NextLine())
	{
		CardCountConfItem* card = new CardCountConfItem;
		card->CardCountID = pLoader.NextInt();
		pLoader.NextStr();	//这一列不需要解析
		card->StarCondition = pLoader.NextInt();
		card->StarConditionPrarm = pLoader.NextInt();
		card->CrystalCondition = pLoader.NextInt();
		card->CrystalConditionPrarm = pLoader.NextInt();
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), card->RaceCondition);
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), card->SexCondition);
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), card->VocationCondition);
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), card->AttackTypeCondition);
        card->ChangeSingo = pLoader.NextInt();
		card->CrystalChangeType = pLoader.NextInt();
		card->CrystalChangePrarm = pLoader.NextInt();
		card->CDChangeType = pLoader.NextInt();
		card->CDChangePrarm = pLoader.NextInt();
		card->CardLockChange = CConfAnalytic::ToBool(pLoader.NextStr());
		m_Datas[card->CardCountID] = card;
	}
	return true;
}

bool CConfCount::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	std::vector<int> powerVec(2);
	//如果有数据 
	while (pLoader.NextLine())
	{
		CountConfItem* count = new CountConfItem;
		count->ID = pLoader.NextInt();
		pLoader.NextStr();	//这一列不需要解析
		count->TargetProperty.reserve(4);
		fillCountVec(4, pLoader, count->TargetProperty);
		count->Damage = CConfAnalytic::ToBool(pLoader.NextStr());

		// 赋值攻击百分比和攻击力
		powerVec.clear();
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), powerVec);
		count->PowerPercent = powerVec.size() > 0 ? powerVec[0] * 1.0f / 100 : 0.0f;
		count->PowerExt = powerVec.size() > 1 ? powerVec[1] : 0.0f;

		//count->PowerPercent = pLoader.NextFloat();
		count->Miss = CConfAnalytic::ToBool(pLoader.NextStr());
		count->DamageType = pLoader.NextInt();
		count->Crit = CConfAnalytic::ToBool(pLoader.NextStr());
		count->DamageReturn = CConfAnalytic::ToBool(pLoader.NextStr());
		count->Strong = pLoader.NextInt();
		count->Status = pLoader.NextInt();
		count->StatusTime = pLoader.NextInt() * 1.0f / 1000;
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), count->CardCountID);
		count->BuffListDel = pLoader.NextInt();
		count->DeBuffListDel = pLoader.NextInt();
		count->BuffDel = pLoader.NextInt();
		count->AIIDChange = pLoader.NextInt();
		count->StatusIDChange = pLoader.NextInt();
		count->CampChange = pLoader.NextInt();
		m_Datas[count->ID] = count;
	}
	return true;
}

bool CConfCrystal::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据 
	while (pLoader.NextLine())
	{
		CrystalConfItem* crystal = new CrystalConfItem;
		crystal->Level = pLoader.NextInt();
		crystal->Speed = pLoader.NextFloat();
		crystal->Max = pLoader.NextInt();
		crystal->Price = pLoader.NextInt();
		m_Datas[crystal->Level] = crystal;
	}
	return true;
}

bool CConfSearch::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));

	while (pLoader.NextLine())
	{
		SearchConfItem *pSearch = new SearchConfItem;

		pSearch->ID = pLoader.NextInt();
		pLoader.NextStr();
		pSearch->ListType = pLoader.NextInt();
		pSearch->RangeType = pLoader.NextInt();
		pSearch->RangeParam = pLoader.NextInt();
		pSearch->Death = CConfAnalytic::ToBool(pLoader.NextStr());
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), pSearch->Identity);
        if (pSearch->Identity.size() >= 2)
        {
            pSearch->Conditions.push_back(ConditionIdentity);
        }
		pSearch->Self = CConfAnalytic::ToBool(pLoader.NextStr());
        CConfAnalytic::ToJsonFloat(pLoader.NextStr(), pSearch->HP);
        if (pSearch->HP.size() == 2)
        {
            pSearch->Conditions.push_back(ConditionHP);
        }
        CConfAnalytic::ToJsonInt(pLoader.NextStr(), pSearch->Career);
        if (pSearch->Career.size() >= 2)
        {
            pSearch->Conditions.push_back(ConditionCareer);
        }
        CConfAnalytic::ToJsonInt(pLoader.NextStr(), pSearch->Sex);
        if (pSearch->Sex.size() == 2)
        {
            pSearch->Conditions.push_back(ConditionSex);
        }
        pSearch->AttackType = pLoader.NextInt();
        if (pSearch->AttackType > 0)
        {
            pSearch->Conditions.push_back(ConditionAttackType);
        }
        CConfAnalytic::ToJsonInt(pLoader.NextStr(), pSearch->Race);
        if (pSearch->Race.size() == 2)
        {
            pSearch->Conditions.push_back(ConditionRace);
        }
        CConfAnalytic::ToJsonInt(pLoader.NextStr(), pSearch->Star);
        if (pSearch->Star.size() == 2)
        {
            pSearch->Conditions.push_back(ConditionStar);
        }
        CConfAnalytic::ToJsonInt(pLoader.NextStr(), pSearch->Buff);
        if (pSearch->Buff.size() >= 2)
        {
            pSearch->Conditions.push_back(ConditionBuff);
        }
        CConfAnalytic::ToJsonInt(pLoader.NextStr(), pSearch->RoleID);
        if (pSearch->RoleID.size() >= 2)
        {
            pSearch->Conditions.push_back(ConditionRoleId);
        }
        CConfAnalytic::ToJsonInt(pLoader.NextStr(), pSearch->State);
        if (pSearch->State.size() == 2)
        {
            pSearch->Conditions.push_back(ConditionState);
        }
        pSearch->Type = pLoader.NextInt();
        if (pSearch->Type > 0)
        {
            pSearch->Conditions.push_back(ConditionType);
        }
        pSearch->Reorder = pLoader.NextInt();
		pSearch->Num = pLoader.NextInt();

		m_Datas[pSearch->ID] = pSearch;
	}

	return true;
}

CConfSkill::~CConfSkill()
{
}

bool CConfSkill::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	//如果有数据 
	while (pLoader.NextLine())
	{
		SkillConfItem* skill = new SkillConfItem;
		skill->ID = pLoader.NextInt();
		pLoader.NextStr();	//这一列不需要解析
		skill->CastType = pLoader.NextInt();
        skill->CastRange = pLoader.NextInt();
		skill->LockType = pLoader.NextInt();
		skill->LockTypePrarm = pLoader.NextInt();
		skill->CanBreak = CConfAnalytic::ToBool(pLoader.NextStr());
		skill->CD = MSTOS(pLoader.NextInt());
		skill->CDParam = pLoader.NextFloat();
		skill->CostType = pLoader.NextInt();
		skill->CostTypeParam = pLoader.NextInt();
		skill->MaxCast = pLoader.NextInt();
		skill->CastTime = pLoader.NextFloat() / 1000.0f;
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), skill->TargetBullet);
		skill->TargetBulletDelay = MSTOS(pLoader.NextInt());
		skill->TargetBulletInterval = MSTOS(pLoader.NextInt());
		CConfAnalytic::ToJsonID_Num(pLoader.NextStr(), skill->Buff);
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), skill->PointBullet);
		skill->PointBulletDelay = MSTOS(pLoader.NextInt());
		skill->PointPointBulletInterval = MSTOS(pLoader.NextInt());
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), skill->Call);
		skill->BulletParam = pLoader.NextInt();
		skill->Name = pLoader.NextInt();
		skill->CostDesc1 = pLoader.NextInt();
		skill->Desc = pLoader.NextInt();
		skill->IconName = pLoader.NextStr();
		skill->StateID = CConfManager::getInstance()->getStateID(pLoader.NextStr());
		skill->CostDesc2 = pLoader.NextInt();
        skill->BattleSkillIcon = pLoader.NextStr();
		
        m_Datas[skill->ID] = skill;
	}
	return true;
}
