#include "BattleHelper.h"
#include "Role.h"
#include "Hero.h"

#ifndef RunningInServer
// 前端事件
#include "Events.h"
#endif

using namespace std;

bool isMainRoleDie(CRole* mainRole)
{
	return mainRole->canRemove()
		|| mainRole->getParent() == NULL;
}

bool CBattleHelper::checkBattleOver()
{
	CSettleAccountModel* settle = m_pRoom->getSettleAccountModel();
	//平局检查
    if ((m_nBattleType == EBATTLE_PVP || m_nBattleType == EBATTLE_PVPROBOT) && GameTick >= MaxTick)
	{
        // 血量多者占优 胜利
        int nMyRoleHp = m_MyRole->getIntAttribute(EAttributeHP);
        int nEnemyHp = m_EnmeyRole->getIntAttribute(EAttributeHP);

        if (nMyRoleHp > nEnemyHp)
            settle->setChallengeResult(CHALLENGE_WON);
        else if (nMyRoleHp < nEnemyHp)
            settle->setChallengeResult(CHALLENGE_FAILED);
        else
            settle->setChallengeResult(CHALLENGE_DRAW);

		IsBattleOver = true;
		return true;
	}
	// 战斗胜利条件
	for (size_t i = 0; i < m_pStageConf->Win.size(); ++i)
	{
		// 先判断敌方是否真死了, isRealDead()
		if (m_EnmeyRole->isRealDead())
		{
			setUserWinRolesState();
		}

		int winType = m_pStageConf->Win[i];
		switch (winType)
		{
		case 1:
			if (isMainRoleDie(m_EnmeyRole))
			{
				settle->setChallengeResult(CHALLENGE_WON);
				IsBattleOver = true;
				return true;
			}
			break;
		case 2:
			if (GameTick >= MaxTick)
			{
				settle->setChallengeResult(CHALLENGE_WON);
				IsBattleOver = true;
				return true;
			}
			break;
		default:
			break;
		}
	}
	// 战斗失败条件
	for (size_t i = 0; i < m_pStageConf->Fail.size(); ++i)
	{
		// 先判断玩家否真死了, isRealDead()
		if (m_MyRole->isRealDead())
		{
			setUserLoseRolesState();
		}

		int failType = m_pStageConf->Fail[i];
		switch (failType)
		{
		case 1:
			if (isMainRoleDie(m_MyRole))
			{
				settle->setChallengeResult(CHALLENGE_FAILED);
				IsBattleOver = true;
				return true;
			}
			break;
		case 2:
			if (GameTick >= MaxTick)
			{
				settle->setChallengeResult(CHALLENGE_FAILED);
				IsBattleOver = true;
				return true;
			}
			break;
		default:
			break;
		}
	}

	return false;
}

void CBattleHelper::onBattleOver()
{
	CSettleAccountModel* settle = m_pRoom->getSettleAccountModel();
	// 战斗胜利和失败在checkBattleOver中设置
	// 记录通关时间
	settle->setTick(GameTick / TickPerSecond);
	// 记录我方英雄血量百分比
	int HpPercent = m_MyRole->getIntAttribute(EAttributeHP) * 100 / m_MyRole->getIntAttribute(EAttributeMaxHP);
	int nCrystalLev = m_MyRole->getIntAttribute(EHeroCrystalLevel);
	settle->setHPPercent(HpPercent);
	settle->setCrystalLv(nCrystalLev);

	// 金币试炼记录BOSS血量消耗 —— 在BOSS扣血逻辑中记录
	// 爬塔试炼记录水晶消耗 —— 在水晶消耗时记录
	// 爬塔试炼记录最终剩余水晶，并回收所有士兵的消耗水晶
	if (m_nBattleType == EBATTLE_TOWERTEST)
	{
		CHero* hero = dynamic_cast<CHero*>(m_MyRole);
		int crystal = static_cast<int>(hero->getFloatAttribute(EHeroCrystal));
		CHECK_RETURN_VOID(hero);
		vector<CRole*>& roles = getRoleWithCamp(getCampWithUid(m_nUserId));
		for (vector<CRole*>::iterator iter = roles.begin();
			iter != roles.end(); ++iter)
		{
			CRole* soldier = *iter;
			if (soldier->getRoleType() == RT_SOLDIER)
			{
				// 获取是第几个兵
				int index = soldier->getTypeId();
				CSoldierModel* soldierCard = hero->getSoldierCard(index);
				CHECK_RETURN_VOID(soldierCard);
				crystal += soldierCard->getConf()->Cost;
			}
		}
		settle->setCrystal(crystal);
	}
}


// 玩家胜利
void CBattleHelper::setUserWinRolesState()
{
    CRole* winHero = getMainRole(getCampWithUid(m_nUserId));
    if (winHero && winHero->currentState() != State_Win)
    {
        winHero->changeState(State_Win);
    }
	// 遍历玩家的小兵, changeState(State_Win), 并调用setDuration(longtimes)
	vector<CRole*>& winner = getRoleWithCamp(getCampWithUid(m_nUserId));
	for (vector<CRole*>::iterator iter = winner.begin(); iter != winner.end(); ++iter)
	{
		CRole* soldier = *iter;
		if (soldier->currentState() != State_Win)
		{
			CState* state = soldier->changeState(State_Win);
			if (state)
			{
				state->setDuration(1200);
			}
		}
	}

    CRole* loseHero = getMainRole(getCampWithUid(m_nEnmeyUserId));
    if (loseHero && loseHero->currentState() != State_Lose)
    {
        loseHero->changeState(State_Lose);
    }
	// 遍历敌方的小兵, changeState(State_Lose)
	vector<CRole*>& loser = getRoleWithCamp(getCampWithUid(m_nEnmeyUserId));
	for (vector<CRole*>::iterator iter = loser.begin(); iter != loser.end(); ++iter)
	{
		CRole* soldier = *iter;
		if (soldier->currentState() != State_Lose)
		{
			soldier->changeState(State_Lose);
		}
	}
}

// 玩家失败
void CBattleHelper::setUserLoseRolesState()
{
    CRole* loseHero = getMainRole(getCampWithUid(m_nUserId));
    if (loseHero && loseHero->currentState() != State_Lose)
    {
        loseHero->changeState(State_Lose);
    }
	// 遍历玩家的小兵, changeState(State_Lose)
	vector<CRole*>& loser = getRoleWithCamp(getCampWithUid(m_nUserId));
	for (vector<CRole*>::iterator iter = loser.begin(); iter != loser.end(); ++iter)
	{
		CRole* soldier = *iter;
		if (soldier->currentState() != State_Lose)
		{
			soldier->changeState(State_Lose);
		}
	}

    CRole* winHero = getMainRole(getCampWithUid(m_nEnmeyUserId));
    if (winHero && winHero->currentState() != State_Win)
    {
        winHero->changeState(State_Win);
    }
	// 遍历敌方的小兵, changeState(State_Win), 并调用setDuration(longtimes)
	vector<CRole*>& winner = getRoleWithCamp(getCampWithUid(m_nEnmeyUserId));
	for (vector<CRole*>::iterator iter = winner.begin(); iter != winner.end(); ++iter)
	{
		CRole* soldier = *iter;
		if (soldier->currentState() != State_Win)
		{
			CState* state = soldier->changeState(State_Win);
			if (state)
			{
				state->setDuration(1200);
			}
		}
	}
}

void processObject(CGameObject* obj, float delta)
{
	if (NULL != obj)
	{
		obj->updateLogic(delta);
	}
}

template <class T>
void processObjects(std::list<T*>& l, float delta, CBattleHelper* helper)
{
	for (typename std::list<T*>::iterator iter = l.begin();
		iter != l.end();)
	{
		CGameObject* obj = (CGameObject*)(*iter);
		processObject(obj, delta);
		if (obj->canRemove())
		{
			obj->remove();
			iter = l.erase(iter);
			helper->destroyObject(obj);
		}
		else
		{
			++iter;
		}
	}
}

template <class T>
int processObjects(std::vector<T*>& v, float delta, CBattleHelper* helper)
{
    // 虽然看上去不大合适，但为了方便还是这么做了，执行每个士兵的时候会顺便检查他是否需要治疗
    int ret = 0;
	for (typename std::vector<T*>::iterator iter = v.begin(); iter != v.end();)
	{
		CRole* obj = (CRole*)(*iter);
		processObject(obj, delta);
		if (obj->canRemove())
		{
			obj->remove();
			iter = v.erase(iter);
			// 对于角色，如果还有其他引用，则交由自释放池管理
			if (obj->getReferenceCount() > 1)
			{
				helper->autoRleaseRole(obj);
			}
			else
			{
				helper->removeObjectMapByObjectId(obj->getObjectId());
				helper->destroyObject(obj);
			}
		}
		else
		{
            int hp = obj->getIntAttribute(EAttributeHP);
            if (hp > 1 && hp < obj->getIntAttribute(EAttributeMaxHP))
            {
                ++ret;
            }
			++iter;
		}
	}
    return ret;
}

void processSoliderCamp(CBattleHelper* helper)
{
	map<CRole*, CampType>& tmpmap = helper->getChangeCamp();
	if (tmpmap.size() == 0)
	{
		return;
	}

	for (map<CRole*, CampType>::iterator iter = tmpmap.begin();
		iter != tmpmap.end(); ++iter)
	{
		CRole* role = iter->first;
		// 1. 如果没有挂掉，且发生了阵营切换才进行切换
		if (role->currentState() == State_None
			|| role->currentState() == State_Death
			|| role->getIntAttribute(EAttributeCamp) == iter->second)
		{
			continue;
		}

		// 2. 安全切换阵营
		if (eraseObjectInVec(helper->getRoleWithCamp(
			role->getIntAttribute(EAttributeCamp)), role))
		{
			helper->getRoleWithCamp(iter->second).push_back(role);
			role->setAttribute(EAttributeCamp, static_cast<int>(iter->second));
			role->updateDirection();
		}
	}
	tmpmap.clear();
}

void CBattleHelper::processBattle(float delta)
{
	// 1.自增时间
	++GameTick;
	KXLOGBATTLE("+++++++++++++++++game tick ++++++++++++++ %d", GameTick);
	// 在处理逻辑之前排序所有士兵
	sortSoldiers();

	// 更新英雄
	processObject(getMainRole(ECamp_Blue), delta);
	processObject(getMainRole(ECamp_Red), delta);

	// 更新士兵列表
    map<int, int> treatCount;
	treatCount[ECamp_Blue] = processObjects(getRoleWithCamp(ECamp_Blue), delta, this);
    treatCount[ECamp_Red] = processObjects(getRoleWithCamp(ECamp_Red), delta, this);
    treatCount[ECamp_Neutral] = processObjects(getRoleWithCamp(ECamp_Neutral), delta, this);

	// 处理阵营切换
	processSoliderCamp(this);

	// 更新子弹（子弹创建子弹需要特殊处理）
	processObjects(getBullets(), delta, this);

	// 处理自动发兵
	if (Dispatch != NULL)
	{
		Dispatch->logicUpdate(delta);
	}

	// 自动释放没有引用的角色
	autoReleaseRoles();
	// 处理命令
	processCommand();
    // 更新待治疗的数量列表
    m_TreatCount = treatCount;
}

void CBattleHelper::processCommand()
{

	while (commandCount() > 0)
	{
		BattleCommandInfo& cmd = topBattleCommand();
		if (cmd.Tick <= GameTick)
		{
#ifndef RunningInServer
			CHero* hero = dynamic_cast<CHero*>(m_MyRole);
#endif
			int camp = getCampWithUid(cmd.ExecuterId);
            KXLOGBATTLE("processCommand commandId %d executer %d tick %d ext1 %d ext2 %d",
                cmd.CommandId, cmd.ExecuterId, cmd.Tick, cmd.Ext1, cmd.Ext2);
			switch (cmd.CommandId)
			{
			case CommandSummoner:
#ifndef RunningInServer
				// 设置水晶的预扣值
				hero->setWithholdValue(0);
#endif
				// 召唤士兵
				dynamic_cast<CHero*>(getMainRole(camp))->createSoldier(cmd.Ext1);
#ifndef RunningInServer
				pEventManager->raiseEvent(BattleEventEnemyActionTips, &cmd);
#endif
				break;

			case CommandSkill:
				// 根据技能ID释放 释放技能 - 技能自己去扣除消耗
				if (cmd.Ext2 >= 0)
				{
					Vec2 temp = Vec2(static_cast<float>(cmd.Ext2), getSoldierLineY(1));
					getMainRole(camp)->executeSkillIndex(cmd.Ext1, temp);
				}
				else
				{
					dynamic_cast<CHero*>(getMainRole(camp))->executeSkillIndex(cmd.Ext1);
				}
#ifndef RunningInServer
				hero->startSkillExecute(true);
				pEventManager->raiseEvent(BattleEventEnemyActionTips, &cmd);
#endif
				break;

			case CommandCrystal:
#ifndef RunningInServer
				// 设置水晶的预扣值
				hero->setWithholdValue(0);
#endif
				// 升级水晶 - 自动扣除消耗
				dynamic_cast<CHero*>(getMainRole(camp))->upgradeCrystal();

#ifndef RunningInServer
                if (cmd.ExecuterId == m_nUserId)
                {
                    pEventManager->raiseEvent(BattleEventCrystalUpgrade);
                }
#endif
				break;

			case CommandCallSolider:
				//召唤召唤物
				createCallSoldier(cmd.ExecuterId, cmd.Ext1, cmd.Ext2, (float)cmd.Ext3, (float)cmd.Ext4);
				break;
#ifndef RunningInServer
			case CommandTalk:
				pEventManager->raiseEvent(BattleEventTalkCommand, &cmd);
				break;
#endif // !RunningInServer

			default:
				break;
			}

			// 执行完命令移除
			popBattleCommand();
		}
		else
		{
			break;
		}
	}
}
