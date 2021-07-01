#include "BattleHelper.h"
#include "Role.h"
#include "Hero.h"

#ifndef RunningInServer
// ǰ���¼�
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
	//ƽ�ּ��
    if ((m_nBattleType == EBATTLE_PVP || m_nBattleType == EBATTLE_PVPROBOT) && GameTick >= MaxTick)
	{
        // Ѫ������ռ�� ʤ��
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
	// ս��ʤ������
	for (size_t i = 0; i < m_pStageConf->Win.size(); ++i)
	{
		// ���жϵз��Ƿ�������, isRealDead()
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
	// ս��ʧ������
	for (size_t i = 0; i < m_pStageConf->Fail.size(); ++i)
	{
		// ���ж���ҷ�������, isRealDead()
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
	// ս��ʤ����ʧ����checkBattleOver������
	// ��¼ͨ��ʱ��
	settle->setTick(GameTick / TickPerSecond);
	// ��¼�ҷ�Ӣ��Ѫ���ٷֱ�
	int HpPercent = m_MyRole->getIntAttribute(EAttributeHP) * 100 / m_MyRole->getIntAttribute(EAttributeMaxHP);
	int nCrystalLev = m_MyRole->getIntAttribute(EHeroCrystalLevel);
	settle->setHPPercent(HpPercent);
	settle->setCrystalLv(nCrystalLev);

	// ���������¼BOSSѪ������ ���� ��BOSS��Ѫ�߼��м�¼
	// ����������¼ˮ������ ���� ��ˮ������ʱ��¼
	// ����������¼����ʣ��ˮ��������������ʿ��������ˮ��
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
				// ��ȡ�ǵڼ�����
				int index = soldier->getTypeId();
				CSoldierModel* soldierCard = hero->getSoldierCard(index);
				CHECK_RETURN_VOID(soldierCard);
				crystal += soldierCard->getConf()->Cost;
			}
		}
		settle->setCrystal(crystal);
	}
}


// ���ʤ��
void CBattleHelper::setUserWinRolesState()
{
    CRole* winHero = getMainRole(getCampWithUid(m_nUserId));
    if (winHero && winHero->currentState() != State_Win)
    {
        winHero->changeState(State_Win);
    }
	// ������ҵ�С��, changeState(State_Win), ������setDuration(longtimes)
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
	// �����з���С��, changeState(State_Lose)
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

// ���ʧ��
void CBattleHelper::setUserLoseRolesState()
{
    CRole* loseHero = getMainRole(getCampWithUid(m_nUserId));
    if (loseHero && loseHero->currentState() != State_Lose)
    {
        loseHero->changeState(State_Lose);
    }
	// ������ҵ�С��, changeState(State_Lose)
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
	// �����з���С��, changeState(State_Win), ������setDuration(longtimes)
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
    // ��Ȼ����ȥ������ʣ���Ϊ�˷��㻹����ô���ˣ�ִ��ÿ��ʿ����ʱ���˳�������Ƿ���Ҫ����
    int ret = 0;
	for (typename std::vector<T*>::iterator iter = v.begin(); iter != v.end();)
	{
		CRole* obj = (CRole*)(*iter);
		processObject(obj, delta);
		if (obj->canRemove())
		{
			obj->remove();
			iter = v.erase(iter);
			// ���ڽ�ɫ����������������ã��������ͷųع���
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
		// 1. ���û�йҵ����ҷ�������Ӫ�л��Ž����л�
		if (role->currentState() == State_None
			|| role->currentState() == State_Death
			|| role->getIntAttribute(EAttributeCamp) == iter->second)
		{
			continue;
		}

		// 2. ��ȫ�л���Ӫ
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
	// 1.����ʱ��
	++GameTick;
	KXLOGBATTLE("+++++++++++++++++game tick ++++++++++++++ %d", GameTick);
	// �ڴ����߼�֮ǰ��������ʿ��
	sortSoldiers();

	// ����Ӣ��
	processObject(getMainRole(ECamp_Blue), delta);
	processObject(getMainRole(ECamp_Red), delta);

	// ����ʿ���б�
    map<int, int> treatCount;
	treatCount[ECamp_Blue] = processObjects(getRoleWithCamp(ECamp_Blue), delta, this);
    treatCount[ECamp_Red] = processObjects(getRoleWithCamp(ECamp_Red), delta, this);
    treatCount[ECamp_Neutral] = processObjects(getRoleWithCamp(ECamp_Neutral), delta, this);

	// ������Ӫ�л�
	processSoliderCamp(this);

	// �����ӵ����ӵ������ӵ���Ҫ���⴦��
	processObjects(getBullets(), delta, this);

	// �����Զ�����
	if (Dispatch != NULL)
	{
		Dispatch->logicUpdate(delta);
	}

	// �Զ��ͷ�û�����õĽ�ɫ
	autoReleaseRoles();
	// ��������
	processCommand();
    // ���´����Ƶ������б�
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
				// ����ˮ����Ԥ��ֵ
				hero->setWithholdValue(0);
#endif
				// �ٻ�ʿ��
				dynamic_cast<CHero*>(getMainRole(camp))->createSoldier(cmd.Ext1);
#ifndef RunningInServer
				pEventManager->raiseEvent(BattleEventEnemyActionTips, &cmd);
#endif
				break;

			case CommandSkill:
				// ���ݼ���ID�ͷ� �ͷż��� - �����Լ�ȥ�۳�����
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
				// ����ˮ����Ԥ��ֵ
				hero->setWithholdValue(0);
#endif
				// ����ˮ�� - �Զ��۳�����
				dynamic_cast<CHero*>(getMainRole(camp))->upgradeCrystal();

#ifndef RunningInServer
                if (cmd.ExecuterId == m_nUserId)
                {
                    pEventManager->raiseEvent(BattleEventCrystalUpgrade);
                }
#endif
				break;

			case CommandCallSolider:
				//�ٻ��ٻ���
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

			// ִ���������Ƴ�
			popBattleCommand();
		}
		else
		{
			break;
		}
	}
}
