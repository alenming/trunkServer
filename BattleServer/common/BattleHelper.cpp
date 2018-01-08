#include "BattleHelper.h"
#include "Hero.h"
#include "Soldier.h"
#include "CallSoldier.h"
#include "Bullet.h"
#include "Buff.h"
#include "KxLog.h"
#include "Monster.h"
#include "Boss.h"
#include "ConfStage.h"
#include "ComponentCache.h"

#include "TargetSearcher.h"

using namespace std;

CBattleHelper::CBattleHelper()
: CurTick(0)
, GameTick(0)
, MaxTick(0)
, TickPerSecond(0)
, MinX(0)
, MaxX(0)
, Dispatch(NULL)
, pEventManager(NULL)
, pMyUserModel(NULL)
, pEnemyUserModel(NULL)
, IsBattleOver(false)
, m_nUserId(0)
, m_nMasterId(0)
, m_nEnmeyUserId(0)
, m_nStageId(0)
, m_nStageLevel(0)
, m_nBattleType(0)
, m_nObjectId(0)
, m_nCurLine(0)
, m_nCurExecutCommandCount(0)
, m_nUserDeadSoldierCount(0)
, m_nEnemyDeadSoldierCount(0)
, m_nCurDispatchCount(0)
, m_pBattleScene(NULL)
, m_pRoom(NULL)
, m_BlueRole(NULL)
, m_RedRole(NULL)
, m_MyRole(NULL)
, m_EnmeyRole(NULL)
, m_pStageConf(NULL)
{
	RandNum.setMode(RNT_PSEUDO);
}

CBattleHelper::~CBattleHelper()
{
    uninit();
}

bool CBattleHelper::init(CRoomModel* room, CBattlePlayerModel* myModel
    , CBattlePlayerModel* enmeyModel,CEventManager<int>* eventMgr, Node* scene)
{
    CHECK_RETURN(myModel && eventMgr && room);
    m_pBattleScene = dynamic_cast<CBattleLayer*>(scene);
    CHECK_RETURN(NULL != m_pBattleScene);

    pEnemyUserModel = enmeyModel;
    pMyUserModel = myModel;
    pEventManager = eventMgr;
    m_nUserId = myModel->getUserId();
    m_pRoom = room;
    m_nMasterId = room->getMaster();
    m_nStageId = room->getStageId();
    m_nStageLevel = room->getStageLevel();
    m_nBattleType = room->getBattleType();
    m_nCurLine = 0;
    m_nObjectId = 0;
    CurTick = 0;
    GameTick = 0;
    TickPerSecond = 10;
    IsBattleOver = false;

    // 房主在左而非房主在右
    m_pStageConf = queryConfStage(m_nStageId);
    CHECK_RETURN(NULL != m_pStageConf);
    const StageSceneConfItem *stageSceneConf = queryConfStageScene(m_pStageConf->StageSenceID);
    CHECK_RETURN(NULL != stageSceneConf);
    // 场景的最左和最右
    MaxX = stageSceneConf->FightScene_Size * 960.0f + 100.0f;
    MinX = -100.0f;
    MaxTick = m_pStageConf->TimeLimit * TickPerSecond;
    // 上中下3条线路的Y值
    m_LineY.resize(3);
    if (stageSceneConf->Walkline.size() == 3)
    {
        m_BlueHeroOrigin = Vec2(static_cast<float>(stageSceneConf->BlueHeroPos), stageSceneConf->Walkline[1]);
        m_RedHeroOrigin = Vec2(static_cast<float>(stageSceneConf->RedHeroPos), stageSceneConf->Walkline[1]);
        m_LineY = stageSceneConf->Walkline;
    }
    else
    {
        m_BlueHeroOrigin = Vec2(static_cast<float>(stageSceneConf->BlueHeroPos), 520.0f);
        m_RedHeroOrigin = Vec2(static_cast<float>(stageSceneConf->RedHeroPos), 520.0f);
        m_LineY[0] = 535;
        m_LineY[1] = 520;
        m_LineY[2] = 505;
    }
    m_BlueLineX = stageSceneConf->BlueSoldierPos;
    m_RedLineX = stageSceneConf->RedSoldierPos;

    // 先创建左边的英雄，再创建右边的英雄
    CPlayerModel* player = dynamic_cast<CPlayerModel*>(getUserModel(getMasterId()));
    CHECK_RETURN(NULL != player && NULL != createHero(player));
    int nUserId = getEnmeyUserId(getMasterId());
    // 根据挑战类型初始化
    switch (m_nBattleType)
    {
    case EBATTLE_PVP:
        m_nEnmeyUserId = enmeyModel->getUserId();
        player = dynamic_cast<CPlayerModel*>(getUserModel(nUserId));
        CHECK_RETURN(NULL != player && NULL != createHero(player));
        break;
	//case EBATTLE_CHAPTER:
    default:
        m_nEnmeyUserId = enmeyModel->getUserId();
        CHECK_RETURN(NULL != createBoss(m_pStageConf->Boss));
        Dispatch = new CDispatch();
        CHECK_RETURN(Dispatch->init(m_nStageId, m_nUserId, ECamp_Red, this));
        break;
    }

    m_MyRole = getMainRole(getCampWithUid(m_nUserId));
    m_EnmeyRole = getMainRole(getEnmeyCampWithUid(m_nUserId));
    return true;
}

void CBattleHelper::uninit()
{
    IsBattleOver = true;
    m_pBattleScene->removeAllChildrenWithCleanup(true);
    m_pBattleScene = NULL;

    SAFE_DELETE(Dispatch);
    LOG("CBattleHelper::uninit() release roles");

    SAFE_RELEASE(m_BlueRole);
    SAFE_RELEASE(m_RedRole);

    releaseAndClearVec(m_BlueRoles);
    releaseAndClearVec(m_RedRoles);
    releaseAndClearVec(m_NeutralRoles);
    releaseAndClearVec(m_AutoReleaseRoles);
    releaseAndClearList(m_RoleCache);
    releaseAndClearList(m_MonsterCache);
    releaseAndClearList(m_Bullets);
    releaseAndClearList(m_BulletCache);
    releaseAndClearList(m_BuffCache);

    m_ObjectMap.clear();
    m_CommandQueue.clear();

#ifndef RunningInServer
    CComponentCache::destory();
#endif
}

/////////////////////////////////创建游戏对象/////////////////////////////////////////

CSoldier* CBattleHelper::createSoldier(int uid, int soldierId, int line /*= -1*/)
{
    KXLOGBATTLE("createSoldier ObjId %d uid %d index %d line %d", m_nObjectId+1, uid, soldierId, line);
    CSoldier* soldier = newSoldier();
    if (NULL != soldier)
    {
        if (!soldier->init(soldierId, uid, ++m_nObjectId, this))
        {
            KXLOGBATTLE("createSoldier failed");
            destorySoidler(soldier);
            return NULL;
        }
        m_pBattleScene->addChild(soldier);
        soldier->initPosition(getSoliderOrigin(getCampWithUid(uid), line));
#ifndef RunningInServer
        soldier->setLocalZOrder(line > 0 ? line : m_nCurLine);
#endif 
        m_ObjectMap[soldier->getObjectId()] = soldier;
        getRoleWithCamp(getCampWithUid(uid)).push_back(soldier);
        
        if (uid == m_nUserId)
        {
            ++m_nCurDispatchCount;
        }
    }
    return soldier;
}

CMonster* CBattleHelper::createMonster(int monsterIndex, int line /*= -1*/)
{
    KXLOGBATTLE("createMonster ObjId %d index %d line %d", m_nObjectId+1, monsterIndex, line);
    CMonster *pMonster = newMonster();
    if (NULL != pMonster)
    {
        if (!pMonster->init(monsterIndex, EDefaultNpc, ++m_nObjectId, this))
        {
            KXLOGBATTLE("createMonster failed");
            destoryMonster(pMonster);
            return NULL;
        }
        m_pBattleScene->addChild(pMonster);
        pMonster->initPosition(getSoliderOrigin(pMonster->getIntAttribute(EAttributeCamp), line));
#ifndef RunningInServer
        pMonster->setLocalZOrder(line > 0 ? line : m_nCurLine);
#endif 
		m_ObjectMap[pMonster->getObjectId()] = pMonster;
		getRoleWithCamp(ECamp_Red).push_back(pMonster);
    }
    return pMonster;
}

CCallSoldier* CBattleHelper::createCallSoldier(int uid, int callId, int ownerLv, float x, float y)
{
    KXLOGBATTLE("createCallSoldier ObjId %d uid %d callId %d ownerLv %d x %f y %f", 
        m_nObjectId+1, uid, callId, ownerLv, x, y);
    CCallSoldier* call = new CCallSoldier();
    if (!call->init(callId, ownerLv, uid, ++m_nObjectId, this))
    {
        KXLOGBATTLE("createCallSoldier failed");
        call->onExit();
        delete call;
        return NULL;
    }
    m_pBattleScene->addChild(call);
    int line = call->getCallConf()->RoleMoveType;
    if (-2 == line)
    {
        // -2为使用指定Y值, 获得正确的zorder
        call->initPosition(Vec2(x, y));
#ifndef RunningInServer
        unsigned int i = 0;
        for (; i < m_LineY.size(); ++i)
        {
            if (y >= m_LineY[i])
            {
                break;
            }
        }
        call->setLocalZOrder(i);
#endif 
    }
    else
    {        
        Vec2 pos = getSoliderOrigin(getCampWithUid(uid), line);
        pos.x = x;
        call->initPosition(pos);

#ifndef RunningInServer
        call->setLocalZOrder(line);
#endif 
    }
    m_ObjectMap[call->getObjectId()] = call;
    switch (call->getCallConf()->RoleType)
    {
        case CAMP_FRIEND:
            getRoleWithCamp(getCampWithUid(uid)).push_back(call);
            break;
        case CAMP_ENEMY:
            getRoleWithCamp(getEnmeyCampWithUid(uid)).push_back(call);
            break;
        case CAMP_NEUTRAL:
            getRoleWithCamp(ECamp_Neutral).push_back(call);
            break;
    }
    return call;
}

CBullet* CBattleHelper::createBullet(int uid, int bulletId, CRole* attacker, CRole* target, const Vec2& pos)
{
    KXLOGBATTLE("createBullet ObjId %d uid %d bulletId %d x %f y %f",
        m_nObjectId+1, uid, bulletId, pos.x, pos.y);
    CBullet* bullet = newBullet();
    if (NULL != bullet)
    {
        if (!bullet->init(bulletId, uid, ++m_nObjectId, attacker, this))
        {
            KXLOGBATTLE("createBullet failed");
            freeBullet(bullet);
            return NULL;
        }
		bullet->setTarget(target);
		bullet->setTargetPos(pos);
        m_pBattleScene->addChild(bullet);
        m_ObjectMap[bullet->getObjectId()] = bullet;
        m_Bullets.push_back(bullet);
    }
    return bullet;
}

CBuff* CBattleHelper::createBuff(CBuffComponent *cpnt, CRole *owner, CRole *maker, int buffid, int stack)
{
    KXLOGBATTLE("createBuff buffid %d stack %d", buffid, stack);
    CBuff* buff = newBuff();
    if (NULL != buff)
    {
        if (!buff->init(cpnt, owner, maker, buffid, stack))
        {
            KXLOGBATTLE("createBuff failed");
            freeBuff(buff);
            return NULL;
        }
    }
    return buff;
}

/////////////////////////////////获取与操作游戏对象/////////////////////////////////////////

CRole* CBattleHelper::getMainRole(int camp)
{
    switch (camp)
    {
        case ECamp_Blue:
            return m_BlueRole;

        case ECamp_Red:
            return m_RedRole;

        default:
            return NULL;
    }
}

CRole* CBattleHelper::getEnmeyMainRole(int camp)
{
    return getMainRole(getEnmeyCamp(camp));
}

std::vector<CRole*>& CBattleHelper::getRoleWithCamp(int camp)
{
    switch (camp)
    {
        case ECamp_Blue:
            return m_BlueRoles;

        case ECamp_Red:
            return m_RedRoles;

        case ECamp_Neutral:
            return m_NeutralRoles;

        default:
            return m_NeutralRoles;
    }
}

std::vector<CRole*>& CBattleHelper::getEnmeyRoleWithCamp(int camp)
{
    return getRoleWithCamp(getEnmeyCamp(camp));
}

void CBattleHelper::changeCampType(CRole* role, CampType camp)
{
    m_ChangeCampMap[role] = camp;
}

////////////////////////////////回收释放游戏对象//////////////////////////////////////////

void cleanupRole(CRole* role)
{
    if (role == NULL)
    {
        return;
    }

    role->clearLogicComponents();
    role->removeAllChildrenWithCleanup(true);
#ifndef RunningInServer
    role->removeAllComponents();
#endif 
}

void CBattleHelper::destorySoidler(CSoldier* role)
{
    if (NULL != role)
    {
        freeSoldier(role);
    }
}

void CBattleHelper::destoryMonster(CMonster* monster)
{
    if (NULL != monster)
	{
        freeMonster(monster);
    }
}

void CBattleHelper::destoryBullet(CBullet* bullet)
{
    if (NULL != bullet)
	{
        freeBullet(bullet);
    }
}

void CBattleHelper::destroyObject(CGameObject* obj)
{
    if (NULL == obj)
    {
        return;
    }

    switch (obj->getObjectType())
    {
            // 英雄(boss)不释放
        case EObjectHero:
        case EObjectBoss:
            break;
            // 士兵回收到池中
        case EObjectSoldier:
			// 统计士兵阵亡数,会重置
			++m_nUserDeadSoldierCount;
            destorySoidler(dynamic_cast<CSoldier*>(obj));
            break;
            // Monster回收
        case EObjectMonster:
            // 统计Monster阵亡数,会重置(关卡才有monster)
            ++m_nEnemyDeadSoldierCount;
            destoryMonster(dynamic_cast<CMonster*>(obj));
            break;

            // 召唤物
        case EObjectCall:
            KXLOGBATTLE("destroyCall callObjId %d", obj->getObjectId());
            obj->release();
            break;
            // 回收子弹
        case EObjectBullet:
            destoryBullet(dynamic_cast<CBullet*>(obj));
            break;
    }
}

void CBattleHelper::destoryBuff(CBuff* buff)
{
    if (NULL != buff)
    {
        freeBuff(buff);
    }
}

void CBattleHelper::autoRleaseRole(CRole* role)
{
    m_AutoReleaseRoles.push_back(role);
}

void CBattleHelper::autoReleaseRoles()
{
    for (vector<CRole*>::iterator iter = m_AutoReleaseRoles.begin();
        iter != m_AutoReleaseRoles.end();)
    {
        CRole* role = *iter;
        if (role->getReferenceCount() == 1)
        {
            m_ObjectMap.erase(role->getObjectId());
            destroyObject(role);
            iter = m_AutoReleaseRoles.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

///////////////////////////////搜索获得目标列表接口///////////////////////////////////////////

bool CBattleHelper::getTargetsWithId(int searchid, CGameObject* searcher, std::vector<CRole *> *targets)
{
    return CTargetSearcher::searchTargetsWithId(searchid, searcher, targets);
}

bool CBattleHelper::isHeroInRange(int uid, int camp, Vec2 &curPos, int range)
{
    int heroCamp;
    if (camp == CAMP_FRIEND)
    {
        heroCamp = getCampWithUid(uid);
    }
    else if (camp == CAMP_ENEMY)
    {
        heroCamp = getEnmeyCamp(uid);
    }
    else
    {
        return false;
    }

    return CTargetSearcher::isTargetInRange(getMainRole(heroCamp), curPos.x - range, curPos.x + range);
}

void CBattleHelper::sortSoldiers()
{
    CTargetSearcher::sortList(getRoleWithCamp(ECamp_Blue), Positive_Sequence);
    CTargetSearcher::sortList(getRoleWithCamp(ECamp_Red), Inverted_Sequence);
    CTargetSearcher::sortList(getRoleWithCamp(ECamp_Neutral), Positive_Sequence);
}

////////////////////////////////操作游戏命令//////////////////////////////////////////

bool CBattleHelper::insertBattleCommand(BattleCommandInfo cmd)
{
    if (cmd.Tick < GameTick)
	{
        KXLOGERROR("Tick %d GameTick %d connandId %d executerId %d", cmd.Tick, GameTick, cmd.CommandId, cmd.ExecuterId);
	}
    //KXLOGBATTLE("insertBattleCommand commandId %d executer %d tick %d ext1 %d ext2 %d",
    //    cmd.CommandId, cmd.ExecuterId, cmd.Tick, cmd.Ext1, cmd.Ext2);
    for (vector<BattleCommandInfo>::iterator iter = m_CommandQueue.begin();
        iter != m_CommandQueue.end(); ++iter)
    {
        if (cmd.Tick < iter->Tick)
        {
            m_CommandQueue.insert(iter, cmd);
            return true;
        }
    }
    m_CommandQueue.push_back(cmd);
    
    return true;
}

BattleCommandInfo& CBattleHelper::topBattleCommand()
{
	if (m_nCurExecutCommandCount < static_cast<int>(m_CommandQueue.size()))
	{
		return m_CommandQueue[m_nCurExecutCommandCount];
	}
	return *(m_CommandQueue.end());
}

void CBattleHelper::popBattleCommand()
{
	if (m_CommandQueue[m_nCurExecutCommandCount].CommandId == CommandCallSolider)
	{
		// 执行完便立即移除的命令
		m_CommandQueue.erase(m_CommandQueue.begin() + m_nCurExecutCommandCount);
	}
	else
	{
		++m_nCurExecutCommandCount;
	}
}

void CBattleHelper::resetBattleCommandQueue(std::vector<BattleCommandInfo>& newCmdQueue)
{
    m_CommandQueue = newCmdQueue;
}

void CBattleHelper::clearBattleCommandQueue()
{
    m_CommandQueue.clear();
	m_nCurExecutCommandCount = 0;
}

////////////////////////////////getter and setter////////////////////////////////////

int CBattleHelper::getCampWithUid(int uid)
{
    //uid是BOSS的时候, getUserModel为空, 先判断避免返回中立(boss没有model, BOSS的阵营为ECamp_Red)
    if (EDefaultNpc == uid)
    {
        return ECamp_Red;
    }
    CBattlePlayerModel* player = getUserModel(uid);
    if (NULL != player)
    {
        return player->getCamp();
    }
    return ECamp_Neutral;
}

int CBattleHelper::getEnmeyCampWithUid(int uid)
{
    //uid是BOSS的时候, getUserModel为空, 先判断避免返回中立(boss没有model, BOSS的阵营为ECamp_Red)
    if (EDefaultNpc == uid)
    {
        return ECamp_Blue;
    }
    CBattlePlayerModel* player = getUserModel(uid);
    if (NULL != player)
    {
        return player->getCamp() == ECamp_Blue ? ECamp_Red : ECamp_Blue;
    }

    return ECamp_Neutral;
}

int CBattleHelper::getEnmeyCamp(int camp)
{
    if (camp == ECamp_Blue)
    {
        return ECamp_Red;
    }
    else if (camp == ECamp_Red || camp == ECamp_Neutral)
    {
        return ECamp_Blue;
    }
    return ECamp_Neutral;
}

const Vec2& CBattleHelper::getOrigin(int camp)
{
    return (camp == ECamp_Blue) ? m_BlueHeroOrigin : m_RedHeroOrigin;
}

Vec2 CBattleHelper::getSoliderOrigin(int camp, int line/* = -1*/)
{
    if (line < 0)
    {
        m_nCurLine = ++m_nCurLine % m_LineY.size();
        line = m_nCurLine;
    }
    else
    {
        // line 0为第一条线
        line = line % m_LineY.size();
    }

    if (camp == ECamp_Blue)
    {
        return Vec2(m_BlueLineX[line], m_LineY[line]);
    }
    else
    {
        return Vec2(m_RedLineX[line], m_LineY[line]);
    }
}

float CBattleHelper::getSoldierLineY(int line)
{
    if (line < static_cast<int>(m_LineY.size()))
    {
        return m_LineY[line];
    }
    return 0;
}

// 获取已经阵亡的士兵数量
bool CBattleHelper::isEnoughDeadSoldierCount(int uid, int compareNum)
{
    if (uid == m_nUserId)
    {
        if (m_nUserDeadSoldierCount >= compareNum)
        {
            // 重置0
            m_nUserDeadSoldierCount = 0;
            return true;
        }
    }
    else
    {
        if (m_nEnemyDeadSoldierCount >= compareNum)
        {
            // 重置0
            m_nEnemyDeadSoldierCount = 0;
            return true;
        }
    }
    return false;
}

// 获取派发的数量
bool CBattleHelper::isEnoughDispatchCount(int compareNum)
{
    if (m_nCurDispatchCount >= compareNum)
    {
        // 重置0
        m_nCurDispatchCount = 0;
        return true;
    }
    return false;
}

//////////////////////////////// 游戏对象相关私有方法 ////////////////////////////////////

CHero* CBattleHelper::createHero(CPlayerModel* player)
{
    KXLOGBATTLE("createHero ObjId %d heroId %d userId %d camp %d", m_nObjectId+1,
        player->getMainRoleId(), player->getUserId(), player->getCamp());
    CHero* hero = new CHero();
    if (!hero->init(player->getMainRoleId(), player->getUserId(), ++m_nObjectId, this))
    {
        KXLOGBATTLE("createHero failed");
        hero->onExit();
        delete hero;
        return NULL;
    }
    m_pBattleScene->addChild(hero);
    // 设置逻辑位置与显示位置
    hero->initPosition(getOrigin(player->getCamp()));
#ifndef RunningInServer
    hero->setLocalZOrder(1);
#endif 
    m_ObjectMap[hero->getObjectId()] = hero;
    switch (player->getCamp())
    {
    case ECamp_Blue:
        m_BlueRole = hero;
        break;
    case ECamp_Red:
        m_RedRole = hero;
        break;
    }
    return hero;
}

CBoss* CBattleHelper::createBoss(int bossID)
{
    KXLOGBATTLE("createBoss bossId %d", bossID);
    CBoss* boss = new CBoss();
    if (!boss->init(bossID, EDefaultNpc, ++m_nObjectId, this))
    {
        KXLOGBATTLE("createBoss failed");
        boss->onExit();
        delete boss;
        return NULL;
    }
    m_pBattleScene->addChild(boss);
    // 设置逻辑位置与显示位置
    boss->initPosition(getOrigin(ECamp_Red));
#ifndef RunningInServer
    boss->setLocalZOrder(1);
#endif 
    m_ObjectMap[boss->getObjectId()] = boss;
    m_RedRole = boss;
    return boss;
}

CSoldier* CBattleHelper::newSoldier()
{
    if (m_RoleCache.size() != 0)
    {
        CSoldier* ret = dynamic_cast<CSoldier*>(*m_RoleCache.begin());
        m_RoleCache.erase(m_RoleCache.begin());
        return ret;
    }
    else
    {
        return new CSoldier();
    }
}

void CBattleHelper::freeSoldier(CSoldier* soldier)
{
    KXLOGBATTLE("freeSoldier soldierObjId %d", soldier->getObjectId());
    soldier->reset();
    m_RoleCache.push_back(soldier);
}

CMonster* CBattleHelper::newMonster()
{
	if (m_MonsterCache.size() != 0)
	{
		CMonster* ret = dynamic_cast<CMonster*>(*m_MonsterCache.begin());
        m_MonsterCache.erase(m_MonsterCache.begin());
		return ret;
	}
	else
	{
		return new CMonster();
	}
}

void CBattleHelper::freeMonster(CMonster* monster)
{
    KXLOGBATTLE("freeMonster monsterObjId %d", monster->getObjectId());
    monster->reset();
	m_MonsterCache.push_back(monster);
}

CBullet* CBattleHelper::newBullet()
{
    if (m_BulletCache.size() != 0)
    {
        CBullet* ret = *m_BulletCache.begin();
        m_BulletCache.erase(m_BulletCache.begin());
#ifndef RunningInServer
		ret->setRotation(0.0f);
#endif // !RunningInServer
        return ret;
    }
    else
    {
        return new CBullet();
    }
}

void CBattleHelper::freeBullet(CBullet* bullet)
{
    KXLOGBATTLE("freeBullet bulletObjId %d", bullet->getObjectId());
    bullet->reset();
    m_BulletCache.push_back(bullet);
}

CBuff* CBattleHelper::newBuff()
{
    if (!m_BuffCache.empty())
    {
        CBuff* ret = m_BuffCache.front();
        m_BuffCache.pop_front();
        return ret;
    }
    else
    {
        return new CBuff();
    }
}

void CBattleHelper::freeBuff(CBuff* buff)
{
    KXLOGBATTLE("freeBuff");
    m_BuffCache.push_back(buff);
}

int CBattleHelper::getTreatCount(int camp)
{
    // -1 表示查询全部阵营
    map<int, int>::iterator iter;
    if (camp == -1)
    {
        int ret = 0;
        for (iter = m_TreatCount.begin();
            iter != m_TreatCount.end(); ++iter)
        {
            ret += iter->second;
        }
        return ret;
    }
    else
    {
        iter = m_TreatCount.find(camp);
        if (iter == m_TreatCount.end())
        {
            return 0;
        }
        return iter->second;
    }
    return 0;
}