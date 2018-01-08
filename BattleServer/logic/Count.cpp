#include "Count.h"
#include "Role.h"
#include "Hero.h"
#include "State.h"
#include "ConfManager.h"
#include "RoleComponent.h"

#ifndef RunningInServer
#include "HPBar.h"
#include "RoleDisplayComponent.h"
#endif

using namespace std;


float getRoleAttribute(CRole* target, int key)
{
    assert(target);
    int ntemp = 0;
    float ftemp = 0.0f;
    if (target->checkAttribute((EAttributeTypes)key, ntemp))
    {
        return static_cast<float>(ntemp);
    }
    else if (target->checkAttribute((EAttributeTypes)key, ftemp))
    {
        return ftemp;
    }
    else
    {
        LOG("Get Attribute Faile %d ", key);
    }
    return ftemp;
}

void setRoleAttribute(CRole* target, int key, float value)
{
    int ntemp = 0;
    float ftemp = 0.0f;
    if (target->checkAttribute((EAttributeTypes)key, ntemp))
    {
        target->setAttribute((EAttributeTypes)key, floatToInt(value));
    }
    else if (target->checkAttribute((EAttributeTypes)key, ftemp))
    {
        target->setAttribute((EAttributeTypes)key, value);
    }
    else
    {
        LOG("Set Attribute Faile %d ", key);
    }
}

float getExpressionValue(const CountExpressionConfItem* exp, CRole* src, CRole* dst, MapStrFloat& varMap)
{
    float ret = 0.0f;
    switch (exp->Target)
    {
    case TargetTypeSource:
        ret = getRoleAttribute(src, exp->AttributeId);
        break;
    case TargetTypeTarget:
        ret = getRoleAttribute(dst, exp->AttributeId);
        break;
    case TargetTypeValue:
        ret = exp->Value;
        break;
    case TargetTypeVar:
        ret = varMap[exp->VarName];
        break;
    default:
        break;
    }
    return ret;
}

void setExpressionValue(const CountExpressionConfItem* exp, CRole* src, CRole* dst, MapStrFloat& varMap, float var)
{
    switch (exp->Target)
    {
    case TargetTypeSource:
        setRoleAttribute(src, exp->AttributeId, var);
        break;
    case TargetTypeTarget:
        setRoleAttribute(dst, exp->AttributeId, var);
        break;
    case TargetTypeVar:
        varMap[exp->VarName] = var;
        break;
    default:
        break;
    }
}

float executeCountExpression(const CountExpressionConfItem* exp, CRole* src, CRole* dst, MapStrFloat& varMap)
{
    assert(exp);
    float ret = 0.0f;
    switch (exp->Operator)
    {
    case OperatorUnvalid:
        ret = getExpressionValue(exp, src, dst, varMap);
        break;
    case OperatorMulti:
        ret = executeCountExpression(exp->LeftExpression, src, dst, varMap)
            * executeCountExpression(exp->RightExpression, src, dst, varMap);
        break;
    case OperatorDiv:
        ret = executeCountExpression(exp->LeftExpression, src, dst, varMap)
            / executeCountExpression(exp->RightExpression, src, dst, varMap);
        break;
    case OperatorAdd:
        ret = executeCountExpression(exp->LeftExpression, src, dst, varMap)
            + executeCountExpression(exp->RightExpression, src, dst, varMap);
        break;
    case OperatorMinus:
        ret = executeCountExpression(exp->LeftExpression, src, dst, varMap)
            - executeCountExpression(exp->RightExpression, src, dst, varMap);
        break;
    case OperatorEqual:
        setExpressionValue(exp->LeftExpression, src, dst, varMap,
            executeCountExpression(exp->RightExpression, src, dst, varMap));
    default:
        break;
    }
    return ret;
}

bool CCount::roleExecute(int countID, CRole* src, CRole* aim)
{
    CHECK_RETURN(NULL != aim && !aim->canRemove());

    KXLOGBATTLE("roleExecute countId %d aimObjId %d srcObjId %d", countID, aim->getObjectId(), src->getObjectId());

    CBattleHelper* helper = aim->getBattleHelper();
    // 取出结算配置
    const CountConfItem* countItem = queryConfCount(countID);
    if (countItem == NULL)
    {
        KXLOGBATTLE("Count Id Error %d", countID);
        return false;
    }
    
    // 进行公式计算
	MapStrFloat var;
    for (vector<CountExpressionConfItem*>::const_iterator iter = countItem->TargetProperty.begin();
        iter != countItem->TargetProperty.end(); ++iter)
	{
        executeCountExpression(*iter, src, aim, var);
	}

	// v4.0新加的内容
	extra(countItem, aim);

	// 是否走伤害流程
	if (countItem->Damage)
	{
		damage(countItem, src, aim);
	}

    switch (countItem->CampChange)
    {
        case EChangeToBlue:
            helper->changeCampType(aim, ECamp_Blue);
            break;
        case EChangeToRed:
            helper->changeCampType(aim, ECamp_Red);
            break;
        case EChangeToNetral:
            helper->changeCampType(aim, ECamp_Neutral);
            break;
        case EChangeToEnmey:
            helper->changeCampType(aim, (CampType)helper->getEnmeyCamp(
                aim->getIntAttribute(EAttributeCamp)));
            break;
    }

	// 卡片结算
	if (!countItem->CardCountID.empty())
	{
		for (VecInt::const_iterator iter = countItem->CardCountID.begin();
            iter != countItem->CardCountID.end(); ++iter)
		{
			cardExecute(*iter, aim);
		}
	}

	// 硬直处理
	int aim_cur_crick = aim->getIntAttribute(EAttributeStrong);
	int strong = countItem->Strong;
    if (strong > 0)
    {
        aim->updateAttribute(EAttributeRage, aim->getIntAttribute(EAttributeRageRecover));
        if (src != NULL)
        {
            src->updateAttribute(EAttributeRage, src->getIntAttribute(EAttributeRageRecover));
        }
        int aim_crick = aim_cur_crick - strong;

		KXLOGBATTLE("aim_cur_crick %d strong %d", aim_cur_crick, strong);
        if (aim_crick < 0)
        {
            CState* state = aim->changeState(countItem->Status);
            if (state)
            {
                if (countItem->StatusTime > 0.0f)
                {
                    state->setDuration(countItem->StatusTime);
                }
                aim->setAttribute(EAttributeStrong, aim->getIntAttribute(EAttributeMaxStrong));
            }
        }
        else
        {
            aim->setAttribute(EAttributeStrong, aim_crick);
        }
    }

    // 判断是否死亡
    if (aim->getIntAttribute(EAttributeHP) < 1)
    {
        // 只更新血量，不播放动画
        playCountEffect(aim, kUnKnow, 1);
        playCountEffect(src, kUnKnow, 1);

        // 这里认为是返回成功的
        aim->changeState(State_Death);
        return true;
    }

    // 只更新血量，不播放动画
    playCountEffect(aim, kUnKnow, 1);
    playCountEffect(src, kUnKnow, 1);
	return true;
}

void changeCD(CSoldierModel* card, int type, int value)
{
    switch (type)
    {
    case CCT_PERCENT:
        card->MaxCD *= (100.0f + value) / 100.0f;
        card->CurCD *= (100.0f + value) / 100.0f;
        break;
    case CCT_RESET:
        card->MaxCD = card->getConf()->CD;
        break;
    default:
        break;
    }
}

void changeCost(CSoldierModel* card, int type, int value)
{
    switch (type)
    {
    case CCT_RESET:
        card->CurCost = card->getConf()->Cost;
        break;
    case CCT_PERCENT:
        card->CurCost += static_cast<int>(card->getConf()->Cost * value / 100.0f);
        break;
    case CCT_NUMBER:
        card->CurCost += value;
        break;
    case CCT_EQUAL:
        card->CurCost = value;
        break;
    default:
        break;
    }

    if (card->CurCost <= 0)
    {
        card->CurCost = 1;
    }
}

bool CCount::cardExecute(int countID, CRole* aim)
{
	//卡片结算配置
    CHECK_RETURN(NULL != aim);
    const CardCountConfItem* count = queryConfCardCount(countID);
	CHECK_RETURN(NULL != count);

	//转换为英雄，因为只有英雄才拥有卡片
	CHero* hero = dynamic_cast<CHero*>(aim);
	CHECK_RETURN(NULL != hero);

	//遍历所有卡片
	std::vector<CSoldierModel*>* cards = hero->getSoldierCards();
    for (std::vector<CSoldierModel*>::iterator iter = cards->begin(); iter != cards->end(); ++iter)
	{
		CSoldierModel* model = *iter;
		CHECK_CONTINUE(NULL != model);
		const SoldierConfItem* cardItem = model->getConf();
		CHECK_CONTINUE(NULL != cardItem);

		//判断条件
		CHECK_CONTINUE(judge(cardItem->Star, count->StarCondition, count->StarConditionPrarm));
		CHECK_CONTINUE(judge(cardItem->Cost, count->CrystalCondition, count->CrystalConditionPrarm));
		CHECK_CONTINUE(judge(cardItem->Common.Race, count->RaceCondition));
		CHECK_CONTINUE(judge(cardItem->Common.Sex, count->SexCondition));
		CHECK_CONTINUE(judge(cardItem->Common.Vocation, count->VocationCondition));
		CHECK_CONTINUE(judge(cardItem->Common.AttackType, count->AttackTypeCondition));

		//执行变化
		changeCost(model, count->CrystalChangeType, count->CrystalChangePrarm);
        changeCD(model, count->CDChangeType, count->CDChangePrarm);
		change(model->IsLock, count->CardLockChange);
        switch (count->ChangeSingo)
        {
        case 1:
            model->IsSingo = true;
            break;
        case 2:model->IsSingo = false;
            break;
        }
		//change(model->IsSingo, count->SingoChange);
	}

	return true;
}

void CCount::extra(const CountConfItem* item, CRole* aim)
{
	if (item->BuffListDel > 0)
	{
        // 驱散正面Buff
        aim->getBuffComponent()->disperse();
	}

	if (item->DeBuffListDel > 0)
	{
        // 净化负面Buff
        aim->getBuffComponent()->purification();
	}

    if (item->BuffDel > 0)
    {
        // 移除指定BUFF
        aim->getBuffComponent()->removeBuff(item->BuffDel);
    }

	if (0 != item->AIIDChange)
	{
        aim->getAIComponent()->reload(item->AIIDChange);
	}

	if (0 != item->StatusIDChange)
	{
        aim->getRoleComponent()->reload(item->StatusIDChange);
	}
}

void CCount::damage(const CountConfItem* item, CRole* src, CRole* aim)
{
    KXLOGBATTLE("damage countId %d", item->ID);
	eHurtType hurtType = kUnKnow;
	int hurtValue = 0;
    CHECK_RETURN_VOID(NULL != src);
    KXLOGBATTLE("damage src_canRemove %d", src->canRemove());
	// 闪避处理
	if (item->Miss)
	{
        float miss_rand = static_cast<float>(src->getBattleHelper()->RandNum.random());
		float miss_rate = aim->getFloatAttribute(EAttributeMiss);
        KXLOGBATTLE("damage miss_rand %f miss_rate %f", miss_rand, miss_rate);
		if (miss_rand < miss_rate)
		{
            // 闪避和被闪避
            aim->updateStat(EStatDodge);
            src->updateStat(EStatBeDodged);
            playCountEffect(aim, kMiss, hurtValue);
			return;
		}
	}

	// 计算伤害
	int finalDamage = 0;
	int type = item->DamageType;
	switch (type)
	{
	case CDT_PHYSICAL:
		{
            int atk_physical = floatToInt(src->getIntAttribute(EAttributePAttack) * item->PowerPercent + item->PowerExt);
			int armor_physical = aim->getIntAttribute(EAttributePGuard);
			int penetrate_physical = src->getIntAttribute(EAttributePPenetrate);
			float rate = static_cast<float>(armor_physical) / (armor_physical + penetrate_physical * 33 + 750) * 1.0f 
                + aim->getFloatAttribute(EAttributeDefend);
            limitValue(rate, -1.0f, 1.0f);
            finalDamage = floatToInt(atk_physical * (1 - rate)) + 1;
			break;
		}
	case CDT_MAGIC:
		{
            int atk_magic = floatToInt(src->getIntAttribute(EAttributeMAttack) * item->PowerPercent + item->PowerExt);
			int armor_magic = aim->getIntAttribute(EAttributeMGuard);
            int penetrate_magic = src->getIntAttribute(EAttributeMPenetrate);
            float rate = static_cast<float>(armor_magic) / (armor_magic + penetrate_magic * 33 + 750) * 1.0f
                + aim->getFloatAttribute(EAttributeResustance);
            limitValue(rate, -1.0f, 1.0f);
            finalDamage = floatToInt(atk_magic * (1 - rate)) + 1;
			break;
		}
	case CDT_REAL:
		{
            int atk_magic = floatToInt(src->getIntAttribute(EAttributeMAttack) * item->PowerPercent + item->PowerExt);
            int atk_physical = static_cast<int>(src->getIntAttribute(EAttributePAttack) * item->PowerPercent + item->PowerExt);
			finalDamage = atk_magic + atk_physical;
			break;
		}
	case CDT_TREAT:
		{
            int atk_magic = floatToInt(src->getIntAttribute(EAttributeMAttack) * item->PowerPercent + item->PowerExt);
            int atk_physical = static_cast<int>(src->getIntAttribute(EAttributePAttack) * item->PowerPercent + item->PowerExt);
			finalDamage = -std::max(atk_magic, atk_physical);
			break;
		}
    case CDT_PERCENT:
        // 百分比伤害 = (目标的最大生命值 * 表格中的第一参数 / 100)  +  (目标的当前生命值 * 表格中的第二参数 / 100)
        finalDamage = static_cast<int>((aim->getIntAttribute(EAttributeMaxHP) * item->PowerPercent)
            + (aim->getIntAttribute(EAttributeHP) * item->PowerExt / 100.0f));
        break;
	default:
		break;
	}
    KXLOGBATTLE("damage finalDamage %d", finalDamage);

	// 暴击处理
	if (item->Crit)
	{
		float crit_rand = static_cast<float>(src->getBattleHelper()->RandNum.random());
		float crit_rate = src->getFloatAttribute(EAttributeCrit);
        KXLOGBATTLE("damage crit_rand %f crit_rate %f crit_dag %f", 
            crit_rand, crit_rate, src->getFloatAttribute(EAttributeCritDamage));
		if (crit_rand < crit_rate)
		{
			float crit_dag = src->getFloatAttribute(EAttributeCritDamage);
            finalDamage = floatToInt(finalDamage * (2 + crit_dag * 0.01f));
            // 暴击和被暴击
            aim->updateStat(EStatCrit);
            src->updateStat(EStatBeCrited);
			hurtType = kCrit;
			hurtValue = finalDamage;
		}
	}

	// 反弹和吸血处理（策划说是合在一起的）
	int hp_aim = aim->getIntAttribute(EAttributeHP);
	if (item->DamageReturn)
	{
        // 计算伤害
		int damage_final = hp_aim < finalDamage ? hp_aim : finalDamage;
        // 根据伤害计算反弹
		float rebound_per = aim->getFloatAttribute(EAttributeRebound);
        int rebound = floatToInt(damage_final * rebound_per * 0.01f);
        // 根据伤害计算吸血
		float vampire_per = src->getFloatAttribute(EAttributeVampire);
        int vampire = floatToInt(damage_final * vampire_per * 0.01f);
        // 结合吸血和反弹计算最终值
		int hp_src = src->getIntAttribute(EAttributeHP);
        int result = 
		hp_src += (vampire - rebound);
		hp_src = std::max(hp_src, 1);   //保证自身不会被弹死
		src->setAttribute(EAttributeHP, hp_src);

        if (rebound > 0)
        {
            playCountEffect(src, kUnCrit, rebound);
        }
        if (vampire > 0)
        {
            playCountEffect(src, kAddBlood, vampire);
        }
	}

    if (finalDamage > 0)
    {
        // 攻击和被攻击
        src->updateStat(EStatAttack);
        aim->updateStat(EStatBeAttacked);
        if (kUnKnow == hurtType)
        {
            hurtType = kUnCrit;
			hurtValue = finalDamage;
		}
        // 检查额外血量
        int extraHp = aim->getIntAttribute(EAttributeExtraHP);
        if (extraHp > 0)
        {
            if (extraHp > finalDamage)
            {
                aim->setAttribute(EAttributeExtraHP, extraHp - finalDamage);
                finalDamage = 0;
            }
            else
            {
                aim->setAttribute(EAttributeExtraHP, 0);
                finalDamage = -extraHp;

                // 破盾和被破盾
                aim->updateStat(EStatBrokenShield);
            }
        }
    }
    else if (finalDamage < 0)
    {
        // 被治疗
        aim->updateStat(EStatBeTreated);
		hurtType = kAddBlood;
		hurtValue = -finalDamage;
    }

	//更新血量
	hp_aim -= finalDamage;
    aim->setAttribute(EAttributeHP, hp_aim);
    playCountEffect(aim, hurtType, hurtValue);

	KXLOGBATTLE("srcObjId %d aimObjId %d srcHp %d aimHp %d Damage %d", src->getObjectId(), aim->getObjectId(),
        src->getIntAttribute(EAttributeHP), aim->getIntAttribute(EAttributeHP), finalDamage);
}

bool CCount::judge(int cardAttr, int countCondition, int countParam)
{
	switch (countCondition)
	{
	case CJT_BIGGER:
		{
			return cardAttr > countParam;
		}
	case CJT_EQUAL:
		{
			return cardAttr == countParam;
		}
	case CJT_SMALLER:
		{
			return cardAttr < countParam;
		}
	default:
		break;
	}
	return true;  //默认0
}

bool CCount::judge(int cardAttr, const VecInt& countCondition)
{
	if ((int)countCondition.size() == 0)
	{
		return true;  //默认空
	}
	for(VecInt::const_iterator iter = countCondition.begin();iter != countCondition.end(); ++iter)
	{
		if(*iter == cardAttr)
		{
			return true;
		}
	}
	return false;
}

bool CCount::judge(int cardAttr, int countCondition)
{
	return cardAttr == countCondition ? true : false;
}

void CCount::change(bool& cardAttr, bool countValue)
{
	if(countValue)
	{
		cardAttr = !cardAttr;
	}
}

void CCount::playCountEffect(CRole* role, eHurtType hurtType, int hurtValue)
{
#ifndef RunningInServer
    if (role)
    {
        CRoleDisplayComponent* roleCom = dynamic_cast<CRoleDisplayComponent*>(
            role->getComponent("CRoleDisplayComponent"));
        if (roleCom)
        {
            roleCom->playCountEffect(hurtType, hurtValue);
        }
    }
#endif
}