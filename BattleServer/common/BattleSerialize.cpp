#include "BattleHelper.h"
#include "Hero.h"
#include "Soldier.h"
#include "CallSoldier.h"
#include "Bullet.h"
#include "Monster.h"
#include "Boss.h"
using namespace std;

// 反序列化后排序子弹
bool bulletListSort(CBullet* obj1, CBullet* obj2)
{
	return obj1->getObjectId() < obj2->getObjectId();
}

bool CBattleHelper::serialize(CBufferData& data)
{
    SERIALIZE_MATCH(data);

    // 保存公共数据，游戏中会改变的变量（CurTick除外）
    data.writeData(GameTick);
    data.writeData(IsBattleOver);
    data.writeData(m_nObjectId);
    data.writeData(m_nCurLine);
	data.writeData(m_nCurExecutCommandCount);
    data.writeData(m_nUserDeadSoldierCount);
    data.writeData(m_nEnemyDeadSoldierCount);
    data.writeData(m_nCurDispatchCount);

    // 保存RnadNum
    data.writeData(RandNum.getGroup());
    data.writeData(RandNum.getIndex());
    data.writeData(RandNum.getMode());

    // 保存待治疗数据
    data.writeData(m_TreatCount[ECamp_Neutral]);
    data.writeData(m_TreatCount[ECamp_Blue]);
    data.writeData(m_TreatCount[ECamp_Red]);

#ifdef RunningInServer
    // 保存队列命令
    int length = m_CommandQueue.size();
    data.writeData(length);
    for (std::vector<BattleCommandInfo>::iterator iter = m_CommandQueue.begin();
        iter != m_CommandQueue.end(); ++iter)
    {
        data.writeData(&(*iter), sizeof(BattleCommandInfo));
    }
#else
    data.writeData(-1);
#endif // RunningInServer

    // 保存士兵目录
    serializeRoleTable(data, getRoleWithCamp(ECamp_Blue));
    serializeRoleTable(data, getRoleWithCamp(ECamp_Red));
    serializeRoleTable(data, getRoleWithCamp(ECamp_Neutral));
    // 保存已经离开场景，但还有引用的角色目录
    serializeRoleTable(data, m_AutoReleaseRoles);

    // 保存子弹目录
    serializBulletTable(data, getBullets());

    // 保存双方英雄
    getMainRole(ECamp_Blue)->serialize(data);
    getMainRole(ECamp_Red)->serialize(data);

    // 保存双方士兵，以及已经离开场景，但还有引用的角色
    serializeRoles(data, getRoleWithCamp(ECamp_Blue));
    serializeRoles(data, getRoleWithCamp(ECamp_Red));
    serializeRoles(data, getRoleWithCamp(ECamp_Neutral));
    serializeRoles(data, m_AutoReleaseRoles);

    // 保存双方子弹
    serializBullets(data, getBullets());

    SERIALIZE_MATCH(data);

    return true;
}

bool CBattleHelper::unserialize(CBufferData& data)
{
    UN_SERIALIZE_MATCH(data);

    // 读取公共数据，游戏中会改变的变量（CurTick除外）
    data.readData(GameTick);
    data.readData(IsBattleOver);
    data.readData(m_nObjectId);
    data.readData(m_nCurLine);
	data.readData(m_nCurExecutCommandCount);
    data.readData(m_nUserDeadSoldierCount);
    data.readData(m_nEnemyDeadSoldierCount);
    data.readData(m_nCurDispatchCount);

    // 读取RnadNum
    int randGroup = 0;
    int randIndex = 0;
    int randModel = 0;
    data.readData(randGroup);
    RandNum.setGroup(randGroup);
    data.readData(randIndex);
    RandNum.setIndex(randIndex);
    data.readData(randModel);
    RandNum.setMode(randModel);

    // 读取待治疗数据
    data.readData(m_TreatCount[ECamp_Neutral]);
    data.readData(m_TreatCount[ECamp_Blue]);
    data.readData(m_TreatCount[ECamp_Red]);

    // 读取队列命令
    int cmdCounts = 0;
    data.readData(cmdCounts);
    if (cmdCounts != -1)
    {
        m_CommandQueue.clear();
        for (int i = 0; i < cmdCounts; ++i)
        {
            BattleCommandInfo info;
            data.readData(&info, sizeof(info));
            m_CommandQueue.push_back(info);
        }
    }

    // 直接清除ObjectMap，在反序列化的过程中动态生成
    m_ObjectMap.clear();

    CRole* hero = getMainRole(ECamp_Blue);
    m_ObjectMap[hero->getObjectId()] = hero;
    hero = getMainRole(ECamp_Red);
    m_ObjectMap[hero->getObjectId()] = hero;

    // 必须先将所有的士兵和子弹信息准备好，才能执行反序列化
    // 因为士兵，子弹，英雄之间可能会有互相关联
    // 解析士兵目录
    vector<int> blueRoles;
    vector<int> redRoles;
    vector<int> neutralRoles;
    vector<int> cacheRoles;
    vector<CRole*> vecRemoveRoles;
    unserializeRoleTable(data, getRoleWithCamp(ECamp_Blue), vecRemoveRoles, blueRoles);
    unserializeRoleTable(data, getRoleWithCamp(ECamp_Red), vecRemoveRoles, redRoles);
    unserializeRoleTable(data, getRoleWithCamp(ECamp_Neutral), vecRemoveRoles, neutralRoles);
    unserializeRoleTable(data, m_AutoReleaseRoles, vecRemoveRoles, cacheRoles);

    // 解析子弹目录
    vector<int> masterBullets;
    vector<CBullet*> vecRemoveBullets;
    unserializeBulletTable(data, getBullets(), vecRemoveBullets, masterBullets);

    // 读取双方英雄
    getMainRole(ECamp_Blue)->unserialize(data);
    getMainRole(ECamp_Red)->unserialize(data);

    // 读取双方士兵
    unserializeRoles(data, blueRoles);
    unserializeRoles(data, redRoles);
    unserializeRoles(data, neutralRoles);
    unserializeRoles(data, cacheRoles);

    // 读取双方子弹
    unserializeBullets(data, masterBullets);

    // 清除已经不在场景中的士兵
    removeRoles(vecRemoveRoles);
    // 清除已经不在场景中的子弹
    removeBullets(vecRemoveBullets);
    // 对被移除出场景的对象再执行一次Remove
    for (vector<CRole*>::iterator iter = m_AutoReleaseRoles.begin();
        iter != m_AutoReleaseRoles.end(); ++iter)
    {
        (*iter)->remove();
    }

    UN_SERIALIZE_MATCH(data);

    RandNum.setGroup(randGroup);
    RandNum.setIndex(randIndex);
    RandNum.setMode(randModel);

    return true;
}

CRole* CBattleHelper::getRoleByObjectId(int objectId)
{
    map<int, CGameObject*>::iterator iter = m_ObjectMap.find(objectId);
    if (iter != m_ObjectMap.end())
    {
        return dynamic_cast<CRole*>(iter->second);
    }
    return NULL;
}

void CBattleHelper::serializeRoleTable(CBufferData& data, vector<CRole*>& vec)
{
    SERIALIZE_MATCH(data);

	int length = vec.size();
	data.writeData(length);
    // 先序列化ID，用于辅助恢复
    for (vector<CRole*>::iterator iter = vec.begin();
        iter != vec.end(); ++iter)
    {
        CRole* role = *iter;
        data.writeData(role->getObjectId());
        data.writeData(role->getRoleType());
        data.writeData(role->getTypeId());
        data.writeData(role->getOwnerId());
        data.writeData(role->getIntAttribute(ECardLevel));
    }

    SERIALIZE_MATCH(data);
}

void CBattleHelper::serializeRoles(CBufferData& data, vector<CRole*>& vec)
{
    SERIALIZE_MATCH(data);

    for (vector<CRole*>::iterator iter = vec.begin();
        iter != vec.end(); ++iter)
    {
        (*iter)->serialize(data);
    }

    SERIALIZE_MATCH(data);
}

void CBattleHelper::serializBulletTable(CBufferData& data, list<CBullet*>& l)
{
    SERIALIZE_MATCH(data);

	int length = l.size();
	data.writeData(length);
    // 先序列化ID，用于辅助恢复
    for (list<CBullet*>::iterator iter = l.begin();
        iter != l.end(); ++iter)
    {
        data.writeData((*iter)->getObjectId());
        data.writeData((*iter)->getTypeId());
        data.writeData((*iter)->getOwner()->getObjectId());
        data.writeData((*iter)->getOwnerId());
    }

    SERIALIZE_MATCH(data);
}

void CBattleHelper::serializBullets(CBufferData& data, list<CBullet*>& l)
{
    SERIALIZE_MATCH(data);

    for (list<CBullet*>::iterator iter = l.begin();
        iter != l.end(); ++iter)
    {
        (*iter)->serialize(data);
    }

    SERIALIZE_MATCH(data);
}

bool CBattleHelper::createRole(int ownerId, int objId, int objType, int typeId, int lv, CBufferData& data, vector<CRole*>& vec)
{
    CRole* role = NULL;
    switch (objType)
    {
        // 士兵
    case RT_SOLDIER:
        {
            CSoldier* soldier = newSoldier();
            if (NULL == soldier || !soldier->init(typeId, ownerId, objId, this))
			{
				LOG("createRole RT_SOLDIER faile");
                // 回收到缓存池
                freeSoldier(soldier);
                return false;
            }
            role = soldier;
        }
        break;

        // 召唤物
    case RT_SUMMON:
        {
            CCallSoldier* call = new CCallSoldier();
            if (NULL == call || !call->init(typeId, lv, ownerId, objId, this))
			{
				LOG("createRole RT_SUMMON faile");
                delete call;
                return false;
            }
            role = call;
        }
        break;

        //怪物
    case RT_MONSTER:
        {
            CMonster* monster = newMonster();
            if (NULL == monster || !monster->init(typeId, ownerId, objId, this))
			{
				LOG("createRole RT_MONSTER faile");
                // 回收到缓存池
                freeMonster(monster);
                return false;
            }
            role = monster;
        }
        break;
    }

    m_ObjectMap[objId] = role;
    m_pBattleScene->addChild(role);
    vec.push_back(role);
    return true;
}

void CBattleHelper::unserializeRoleTable(CBufferData& data, std::vector<CRole*>& vec, std::vector<CRole*>& vecRemove, std::vector<int>& idVec)
{
    UN_SERIALIZE_MATCH(data);

    // 1.data中没有，本地有的删掉
    // 2.data中有，本地有的更新
    // 3.data中有，本地没有的创建
    // 4.必须先将所有对象创建好，才能顺序进行反序列化（中间对象之间可能有相互引用）
    // 取出角色数量
    int roleCounts = 0;
    data.readData(roleCounts);

    // 先解析该列表的目录数据
    set<int> roleIds;           // 角色的Id集合，用于快速查找
    vector<int> roleTypeVec;    // 角色的类型数组，如士兵，中立，召唤物
    vector<int> roleTypeIdVec;  // 角色的类型ID数组，如某种类型的具体ID
    vector<int> roleOwnerIdVec; // 角色的OwnerId数组
    vector<int> roleExtVec;     // 角色的扩展数据(等级)
    for (int i = 0; i < roleCounts; ++i)
    {
        int var = 0;
        data.readData(var);
        roleIds.insert(var);
        idVec.push_back(var);
        data.readData(var);
        roleTypeVec.push_back(var);
        data.readData(var);
        roleTypeIdVec.push_back(var);
        data.readData(var);
        roleOwnerIdVec.push_back(var);
        data.readData(var);
        roleExtVec.push_back(var);
    }

    // 创建一个Map缓存，提高创建和更新的效率，并剔除不存在的对象
    map<int, CRole*> roles;
    for (vector<CRole*>::iterator iter = vec.begin();
        iter != vec.end();)
    {
        CRole* role = *iter;
        if (roleIds.find(role->getObjectId()) == roleIds.end())
        {
            // 直接删掉已经不存在的对象
            // 这里不进行remove，是因为有可能该对象被移动到了另一个列表中
            // role->remove();
            iter = vec.erase(iter);
            vecRemove.push_back(role);
        }
        else
        {
            // 重新建立ObjectMap
            roles[role->getObjectId()] = role;
            m_ObjectMap[role->getObjectId()] = role;
            ++iter;
        }
    }

    // 将需要新创建的角色创建出来
    for (int i = 0; i < roleCounts; ++i)
    {
        int id = idVec[i];
        map<int, CRole*>::iterator roleIter = roles.find(id);
        // 如果本地没有，则创建
        if (roleIter == roles.end())
        {
            //警告: monster等级暂时默认为1,需要解决
            // 创建士兵/中立士兵/召唤物
            createRole(roleOwnerIdVec[i], id, roleTypeVec[i], roleTypeIdVec[i], roleExtVec[i], data, vec /*,等级*/);
        }
    }

    UN_SERIALIZE_MATCH(data);
}

void CBattleHelper::unserializeRoles(CBufferData& data, vector<int>& vec)
{
    UN_SERIALIZE_MATCH(data);

    // 顺序反序列化所有对象
    for (int i = 0; i < static_cast<int>(vec.size()); ++i)
    {
        int id = vec[i];
        m_ObjectMap[id]->unserialize(data);
    }

    UN_SERIALIZE_MATCH(data);
}

void CBattleHelper::removeRoles(vector<CRole*>& roles)
{
    for (vector<CRole*>::iterator iter = roles.begin();
        iter != roles.end(); ++iter)
    {
        CRole* role = *iter;
        if (m_ObjectMap.find(role->getObjectId()) == m_ObjectMap.end())
        {
            role->remove();
            destroyObject(role);
        }
        else
        {
            // 提示信息，有角色移动到了其他队列
            LOG("Some Role %d Change Queue", role->getObjectId());
        }
    }
}

void CBattleHelper::unserializeBulletTable(CBufferData& data, list<CBullet*>& l, vector<CBullet*>& vecRemove, vector<int>& idVec)
{
    UN_SERIALIZE_MATCH(data);

    // 1.data中没有，本地有的删掉
    // 2.data中有，本地有的更新
    // 3.data中有，本地没有的创建
    // 4.必须先将所有对象创建好，才能顺序进行反序列化（中间对象之间可能有相互引用）

    // 取出子弹数量
    int bulletCount = 0;
    data.readData(bulletCount);

    // 先解析该列表的目录数据
    set<int> bulletIds;
    vector<int> bulletTypeIdVec;
    vector<int> attackerIdVec;
    vector<int> ownerIdVec;
    for (int i = 0; i < bulletCount; ++i)
    {
        int var = 0;
        data.readData(var);
        bulletIds.insert(var);
        idVec.push_back(var);
        data.readData(var);
        bulletTypeIdVec.push_back(var);
        data.readData(var);
        attackerIdVec.push_back(var);
        data.readData(var);
        ownerIdVec.push_back(var);
    }

    map<int, CBullet*> bullets;
    // 先剔除不存在的子弹
    for (list<CBullet*>::iterator iter = l.begin(); iter != l.end();)
    {
        CBullet* bullet = *iter;
        if (bulletIds.find(bullet->getObjectId()) == bulletIds.end())
        {
            bullet->remove();
            iter = l.erase(iter);
        }
        else
        {
            bullets[bullet->getObjectId()] = bullet;
            m_ObjectMap[bullet->getObjectId()] = bullet;
            ++iter;
        }
    }

    // 添加新的子弹
    for (int i = 0; i < bulletCount; ++i)
    {
        int id = idVec[i];
        map<int, CBullet*>::iterator bulletIter = bullets.find(id);
        // 如果本地没有，则创建
        if (bulletIter == bullets.end())
        {
            CBullet* bullet = newBullet();
            CRole* attacker = getRoleByObjectId(attackerIdVec[i]);
            if (bullet->init(bulletTypeIdVec[i], ownerIdVec[i], id, attacker, this))
            {
                m_pBattleScene->addChild(bullet);
                l.push_back(bullet);
                m_ObjectMap[id] = bullet;
            }
            else
			{
				LOG("init Bullet Error! objectId %d bulletTypeId %d ownerId %d attacker %d",
                    id, bulletTypeIdVec[i], ownerIdVec[i], attacker);
                // 报错
                freeBullet(bullet);
            }
        }
    }

	// 反序列化后子弹的顺序与反序列化前的顺序可能会不同
	l.sort(bulletListSort);

    UN_SERIALIZE_MATCH(data);
}

void CBattleHelper::unserializeBullets(CBufferData& data, vector<int>& vec)
{
    UN_SERIALIZE_MATCH(data);

    // 顺序反序列化所有对象
    for (int i = 0; i < static_cast<int>(vec.size()); ++i)
    {
        int id = vec[i];
        m_ObjectMap[id]->unserialize(data);
    }

    UN_SERIALIZE_MATCH(data);
}

void CBattleHelper::removeBullets(std::vector<CBullet*>& bullets)
{
    for (vector<CBullet*>::iterator iter = bullets.begin();
        iter != bullets.end(); ++iter)
    {
        SAFE_RELEASE((*iter));
    }
}
