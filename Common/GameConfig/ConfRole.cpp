#include "ConfRole.h"

#ifndef RunningInServer
#include "Game.h"
#endif

#define MSTOS(ms)       (ms) / 1000.0f       //毫秒转成秒

bool StatusConfMap::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

    //如果有数据 
    while(csvLoader.NextLine())
    {
        StatusConfItem* cfg = new StatusConfItem();
        cfg->CanBreakParam = csvLoader.NextInt();
        csvLoader.NextStr();	//这一列不需要解析
        cfg->StateId = CConfManager::getInstance()->getStateID(csvLoader.NextStr());
        cfg->LifeTime = csvLoader.NextInt() * 1.0f / 1000;
        cfg->LifeTimePrarm = csvLoader.NextInt();
        cfg->Lock = csvLoader.NextInt() == 1;
        cfg->NextStateId = CConfManager::getInstance()->getStateID(csvLoader.NextStr());
        CConfAnalytic::ToFunction(csvLoader.NextStr(), &cfg->Action);
        cfg->AnimationTag = csvLoader.NextStr();
        cfg->AnimationLoop = csvLoader.NextInt();
        cfg->AnimationSpeed = csvLoader.NextFloat();
        cfg->SpeedAffect = static_cast<StatusSpeedAffect>(csvLoader.NextInt());
        cfg->AnimationAsh = csvLoader.NextInt() == 1;
        cfg->AnimationTransparency = csvLoader.NextFloat();
        cfg->AnimationFadeOut = csvLoader.NextInt() * 1.0f / 1000;
		cfg->hue = csvLoader.NextFloat();

        CConfAnalytic::ToJsonInt(csvLoader.NextStr(), cfg->EffectIds);
        cfg->EffectPlayType = static_cast<StatusEffPlayType>(csvLoader.NextInt());
        cfg->IsFollow = CConfAnalytic::ToBool(csvLoader.NextStr());
        cfg->UIEffectID = csvLoader.NextInt();
        CConfAnalytic::ToVecMusicInfos(csvLoader.NextStr(), cfg->MusicInfos);

        m_Datas[cfg->StateId] = cfg;
    }
    return true;
}

const StatusConfItem* StatusConfMap::getStateItem(int stateId) const
{
    std::map<int, void*>::const_iterator iter = m_Datas.find(stateId);
    if(iter == m_Datas.end())
    {
        return NULL;
    }
    return reinterpret_cast<const StatusConfItem*>(iter->second);
}

bool CConfStatus::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

    while (csvLoader.NextLine())
    {
        int stateId = csvLoader.NextInt();
        csvLoader.NextStr();
#ifdef RunningInServer
        std::string stateConf = "../config/Role/Status/" + csvLoader.NextStr();
		if (!LoadCSV(stateId, stateConf))
		{
			printf("load csv stateConf=%s error!\n", stateConf.c_str());
			return false;
		}
#else
        m_StatusFileMap[stateId] = csvLoader.NextStr();
#endif

    }

    return true;
}

bool CConfStatus::LoadCSV(int roleId, const std::string& str)
{
    StatusConfMap* map = new StatusConfMap();
    if (!map->LoadCSV(str))
    {
#ifndef RunningInServer
        cocos2d::MessageBox(("Load " + str + " Error!!").c_str(), "Load CSV Error");
#endif 
        delete map;
        return false;
    }

    m_Datas[roleId] = map;
    return true;
}

const StatusConfItem* CConfStatus::getStateItem(int roleId, int stateId)
{
    const StatusConfMap* map = getStateMap(roleId);
    if(!map)
    {
        return NULL; 
    }
    return map->getStateItem(stateId);
}

const StatusConfMap* CConfStatus::getStateMap(int roleId)
{
    std::map<int,void*>::iterator iter = m_Datas.find(roleId);
    if(iter == m_Datas.end())
    {
#ifndef RunningInServer
		std::string stateConf = "config/Role/Status/" + m_StatusFileMap[roleId];
#else
		std::string stateConf = "../config/Role/Status/" + m_StatusFileMap[roleId];
#endif
		// std::string stateConf = "config/Role/Status/" + toolToStr(roleId) + "_Status.csv";
		if (LoadCSV(roleId, stateConf))
		{
			return reinterpret_cast<StatusConfMap*>(m_Datas[roleId]);
		}
		return NULL;
    }
    return reinterpret_cast<StatusConfMap*>(iter->second);
}

AIConfMap::~AIConfMap()
{
    for (std::map<int, std::vector<AIConfItem*> >::iterator iter = m_AIDatas.begin();
        iter != m_AIDatas.end(); ++iter)
    {
        deleteAndClearVec<AIConfItem*>(iter->second);
    }
}

bool AIConfMap::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

    //如果有数据 
    while(csvLoader.NextLine())
    {
        AIConfItem* cfg = new AIConfItem();
        //cfg->Order = csvLoader.NextInt();
        csvLoader.NextStr();	//这一列不需要解析
        csvLoader.NextStr();	//这一列不需要解析
        cfg->NeedRoleStatus = CConfManager::getInstance()->getStateID(csvLoader.NextStr());

        for (int i = 0; i < 4; ++i)
        {
            CDataFunction* fun = NULL;
            CConfAnalytic::ToFunction(csvLoader.NextStr(), &fun);
            if (NULL != fun)
            {
                cfg->Condition.push_back(fun);
            }
        }

        for (int i = 0; i < 4; ++i)
        {
            CDataFunction* fun = NULL;
            CConfAnalytic::ToFunction(csvLoader.NextStr(), &fun);
            if (NULL != fun)
            {
                cfg->Action.push_back(fun);
            }
        }

        m_AIDatas[cfg->NeedRoleStatus].push_back(cfg);	
    }
    return true;
}

const std::vector<AIConfItem*>* AIConfMap::getAIItems(int stateid)
{
    std::map<int, std::vector<AIConfItem*> >::iterator iter = m_AIDatas.find(stateid);
    if (iter == m_AIDatas.end())
    {
        return NULL;
    }
    return &(m_AIDatas[stateid]);
}

const std::map<int, std::vector<AIConfItem*> >& AIConfMap::getAIMap()
{
    return m_AIDatas;
}

bool CConfAI::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

    while (csvLoader.NextLine())
    {
        int aiId = csvLoader.NextInt();
#ifdef RunningInServer
        std::string aiConf = "../config/Role/AI/" + csvLoader.NextStr();
		if (!LoadCSV(aiId, aiConf))
		{
			return false;
		}
#else
        m_AIFileMap[aiId] = csvLoader.NextStr();
        //std::string aiConf = "config/Role/AI/" + csvLoader.NextStr();
#endif
    }

    return true;
}

bool CConfAI::LoadCSV(int roleid, const std::string& str)
{
    AIConfMap* map = new AIConfMap();
    if (!map->LoadCSV(str))
    {
#ifndef RunningInServer
        cocos2d::MessageBox(("Load " + str + " Error!!").c_str(), "Load CSV Error");
#endif 
        delete map;
        return false;
    }

    m_Datas[roleid] = map;
    return true;
}

const std::vector<AIConfItem*>* CConfAI::getAIItems(int roleId, int stateid)
{
    AIConfMap* map = (AIConfMap*)getAIMap(roleId);
    if(!map)
    {
        return NULL; 
    }

    return map->getAIItems(stateid);
}

const AIConfMap* CConfAI::getAIMap(int roleId)
{
    std::map<int,void*>::iterator iter = m_Datas.find(roleId);
    if (iter == m_Datas.end())
    {
#ifndef RunningInServer
		std::string aiConf = "config/Role/AI/" + m_AIFileMap[roleId];
#else
		std::string aiConf = "../config/Role/AI/" + m_AIFileMap[roleId];
#endif
        if (LoadCSV(roleId, aiConf))
        {
            return reinterpret_cast<AIConfMap*>(m_Datas[roleId]);
        }
        return NULL;
    }
    return reinterpret_cast<AIConfMap*>(iter->second);
}

bool CConfStrToID::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    if(!csvLoader.LoadCSV(str.c_str()))
    {
        return false;
    }

    //如果有数据 
    while(csvLoader.NextLine())
    {
        std::string key = csvLoader.NextStr();
        int val = csvLoader.NextInt();
        m_mapConvert[key] = val;
    }
    return true;
}

int CConfStrToID::getIDByName(const std::string& str)
{
    std::map<std::string,int>::iterator iter = m_mapConvert.find(str);
    if (iter != m_mapConvert.end())
    {
        return iter->second;
    }
    else
    {
        if (!str.empty() && str != "0")
        {
            LOG("can't not find %s in NameToIdMap", str.c_str());
        }
        return -1;
    }
}

void CConfRole::LoadRoleLogic(CCsvLoader& csvLoader, Role &common, bool noCall)
{
    // 角色表现ID
    common.AnimationID = csvLoader.NextInt();
	// 骨骼HSV
	CConfAnalytic::ToJsonFloat(csvLoader.NextStr(), common.AnimationHSV);
    // 角色大小缩放
    common.Scale = csvLoader.NextFloat();
    // 角色特效大小缩放值
    common.EffectScale = csvLoader.NextFloat();
    // 角色子弹发射点
    CConfAnalytic::ToVec2(csvLoader.NextStr(), common.FireOffset);
    // 角色头部特效播放点
    CConfAnalytic::ToVec2(csvLoader.NextStr(), common.HeadOffset);
    // 角色身上特效播放点
    CConfAnalytic::ToVec2(csvLoader.NextStr(), common.HitOffset);
    // 角色血条类型和位置
    CConfAnalytic::ToJsonInt(csvLoader.NextStr(), common.HPLine);
    for (int i = (int)common.HPLine.size(); i < 3; ++i)
    {
        common.HPLine.push_back(1);
    }
    // 角色状态表ID
    common.StatusID = csvLoader.NextInt();
    // 角色AI表ID
    common.AIID = csvLoader.NextInt();
    // 技能8个 (前面四个技能需要显示, 所以即使是空值也要保存, 否则不确定那几个技能)
    for (int i = 0; i < 4; ++i)
    {
        common.Skill.push_back(csvLoader.NextInt());
    }
    for (int i = 4; i < 12; ++i)
    {
        int value = csvLoader.NextInt();
        if (value != 0)
        {
            common.Skill.push_back(value);
        }
    }

    // 卡片名字
    common.Name = csvLoader.NextInt();
    // 卡片描述
    common.Desc = csvLoader.NextInt();

    if (noCall)
    {
        // 卡片表现
        common.Picture = csvLoader.NextStr();
        // 卡片头像
        common.HeadIcon = csvLoader.NextStr();

    }
    // 种族
    common.Race = csvLoader.NextInt();
    // 职业
    common.Vocation = csvLoader.NextInt();
    // 性别
    common.Sex = csvLoader.NextInt();
    // 攻击方式
    common.AttackType = csvLoader.NextInt();
    if (noCall)
    {
        // 角色攻击距离
        common.AttackDistance = csvLoader.NextInt();
    }
}

void CConfRole::LoadRoleNumerial(CCsvLoader& csvLoader, Role &common)
{
    // 角色近战攻击范围
    common.FireRange = csvLoader.NextInt();
    // 角色远程攻击范围
    common.FarFireRange = csvLoader.NextInt();
    // 角色移动速度
    common.Speed = csvLoader.NextInt();
    // 角色生命值
    common.HP = csvLoader.NextInt();
    // 角色生命成长值
    common.HPGrowUp = csvLoader.NextInt();
    // 角色物理攻击力
    common.PAttack = csvLoader.NextInt();
    // 角色物理攻击力成长值
    common.PAttackGrowUp = csvLoader.NextInt();
    // 魔法攻击力
    common.MAttack = csvLoader.NextInt();
    // 魔法攻击力成长值
    common.MAttackGrowUp = csvLoader.NextInt();
    // 角色物理减伤
    common.Defend = csvLoader.NextFloat();
    // 角色物理护甲
    common.PGuard = csvLoader.NextInt();
    // 角色物理护甲成长
    common.PGuardGrowUp = csvLoader.NextInt();
    // 角色魔法减伤
    common.Resustance = csvLoader.NextFloat();
    // 魔法护甲
    common.MGuard = csvLoader.NextInt();
    // 魔法护甲成长
    common.MGuardGrowUp = csvLoader.NextInt();
    // 角色物理穿透
    common.PPenetrate = csvLoader.NextInt();
    // 角色魔法穿透
    common.MPenetrate = csvLoader.NextInt();
    // 角色攻击速度
    common.AttackSpeed = csvLoader.NextInt();
    // 角色吸血值
    common.Vampire = csvLoader.NextFloat();
    // 角色反弹值
    common.Rebound = csvLoader.NextFloat();
    // 角色闪避值
    common.Miss = csvLoader.NextFloat();
    // 角色暴击值
    common.Crit = csvLoader.NextFloat();
    // 角色暴击成长
    common.CritGrowUp = csvLoader.NextFloat();
    // 角色暴击伤害
    common.CritDamage = csvLoader.NextFloat();
    // 角色怒气上限值
    common.Rage = csvLoader.NextInt();
    // 角色怒气回复值
    common.RageRecover = csvLoader.NextInt();
    // 角色魔法上限值
    common.MP = csvLoader.NextInt();
    // 角色魔法回复
    common.MPRecover = csvLoader.NextInt();
    // 角色质量
    common.Mass = csvLoader.NextFloat();
    // 角色霸体值上限
    common.Strong = csvLoader.NextInt();
    // 角色霸体值恢复速度
    common.StrongRecover = csvLoader.NextInt();
    // 角色仇恨值
    common.Haterd = csvLoader.NextInt();
}

bool CConfHero::LoadCSV(const std::string& str)
{
    std::string strLogicCsv = str;
    std::string strNumerialCsv = str;
    strLogicCsv.append(".csv");
    strNumerialCsv.append("_Num.csv");

    CCsvLoader Loader;
    CHECK_RETURN(loadCsv(Loader, strLogicCsv.c_str(), 3));

    //如果有数据 
    while (Loader.NextLine())
    {
        HeroConfItem* hero = new HeroConfItem;
        // 角色ID
        hero->Common.ClassID = Loader.NextInt();
        Loader.NextStr();//备注
        LoadRoleLogic(Loader, hero->Common);
        // 玩家技能3个
        hero->PlayerSkill.reserve(3);
        fillIntVec(3, Loader, hero->PlayerSkill);
        // 英雄水晶提升速率
        hero->CrystalSpeedPrarm = Loader.NextFloat();
        // 卡片种族加强
        hero->RaceEnhance.EnhanceType = Loader.NextInt();
        hero->RaceEnhance.ConsumeParam = Loader.NextFloat();
        hero->RaceEnhance.CDParam = CConfAnalytic::ToPercent(Loader.NextStr());
        // 职业加强
        hero->VocationEnhance.EnhanceType = Loader.NextInt();
        hero->VocationEnhance.ConsumeParam = Loader.NextFloat();
        hero->VocationEnhance.CDParam = CConfAnalytic::ToPercent(Loader.NextStr());
        // 性别加强
        hero->SexEnhance.EnhanceType = Loader.NextInt();
        hero->SexEnhance.ConsumeParam = Loader.NextFloat();
        hero->SexEnhance.CDParam = CConfAnalytic::ToPercent(Loader.NextStr());
        // 攻击类型加强
        hero->AttackTypeEnhance.EnhanceType = Loader.NextInt();
        hero->AttackTypeEnhance.ConsumeParam = Loader.NextFloat();
        hero->AttackTypeEnhance.CDParam = CConfAnalytic::ToPercent(Loader.NextStr());
        m_Datas[hero->Common.ClassID] = hero;
    }

    // 清空重置
    Loader.Release();
    // 填充数值表部分
    CHECK_RETURN(loadCsv(Loader, strNumerialCsv.c_str(), 3));

    while (Loader.NextLine())
    {
        int nRoleID = Loader.NextInt();
        Loader.NextStr();//备注
        std::map<int, void*>::iterator iter = m_Datas.find(nRoleID);
        if (iter == m_Datas.end())
        {
            return false;
        }

        HeroConfItem* pHeroConfigItem = static_cast<HeroConfItem*>(iter->second);
        LoadRoleNumerial(Loader, pHeroConfigItem->Common);
    }

    return true;
}

bool CConfBoss::LoadCSV(const std::string& str)
{
    std::string strLogicCsv = str;
    std::string strNumerialCsv = str;
    strLogicCsv.append(".csv");
    strNumerialCsv.append("_Num.csv");

    CCsvLoader Loader;
    CHECK_RETURN(loadCsv(Loader, strLogicCsv.c_str(), 3));

    //如果有数据 
    while (Loader.NextLine())
    {
        BossConfItem* boss = new BossConfItem;
        boss->Common.ClassID = Loader.NextInt();
        Loader.NextStr();//备注
        LoadRoleLogic(Loader, boss->Common);

        m_Datas[boss->Common.ClassID] = boss;
    }

    // 清空重置
    Loader.Release();
    // 填充数值表部分
    CHECK_RETURN(loadCsv(Loader, strNumerialCsv.c_str(), 3));

    while (Loader.NextLine())
    {
        int nRoleID = Loader.NextInt();
        Loader.NextStr();//备注
        std::map<int, void*>::iterator iter = m_Datas.find(nRoleID);
        if (iter == m_Datas.end())
        {
            return false;
        }

        BossConfItem* pBossConfigItem = static_cast<BossConfItem*>(iter->second);
        LoadRoleNumerial(Loader, pBossConfigItem->Common);
    }

    return true;
}

bool CConfMonster::LoadCSV(const std::string& str)
{
    std::string strLogicCsv = str;
    std::string strNumerialCsv = str;
    strLogicCsv.append(".csv");
    strNumerialCsv.append("_Num.csv");

    CCsvLoader Loader;
    CHECK_RETURN(loadCsv(Loader, strLogicCsv.c_str(), 3));

    //如果有数据 
    while (Loader.NextLine())
    {
        MonsterConfItem* monster = new MonsterConfItem;
        monster->Common.ClassID = Loader.NextInt();
        Loader.NextStr();//备注
        LoadRoleLogic(Loader, monster->Common);

        m_Datas[monster->Common.ClassID] = monster;
    }

    // 清空重置
    Loader.Release();
    // 填充数值表部分
    if (!loadCsv(Loader, strNumerialCsv.c_str(), 3))
    {
        return false;
    }

    while (Loader.NextLine())
    {
        int nRoleID = Loader.NextInt();
        Loader.NextStr();//备注
        std::map<int, void*>::iterator iter = m_Datas.find(nRoleID);
        if (iter == m_Datas.end())
        {
            return false;
        }

        MonsterConfItem* pMonsterConfigItem = static_cast<MonsterConfItem*>(iter->second);
        LoadRoleNumerial(Loader, pMonsterConfigItem->Common);
    }

    return true;
}

bool CConfSoldier::LoadCSV(const std::string& str)
{
    std::string strLogicCsv = str;
    std::string strNumerialCsv = str;
    strLogicCsv.append(".csv");
    strNumerialCsv.append("_Num.csv");

    CCsvLoader Loader;
    CHECK_RETURN(loadCsv(Loader, strLogicCsv.c_str(), 3));

    //如果有数据 
    while (Loader.NextLine())
    {
        SoldierConfItem* soldier = new SoldierConfItem;
        soldier->Common.ClassID = Loader.NextInt();
        Loader.NextStr();//备注
        soldier->Star = Loader.NextInt();
		soldier->Rare = Loader.NextInt();
        LoadRoleLogic(Loader, soldier->Common);
        // 是否唯一派发
        soldier->IsSingo = Loader.NextInt();

        // SoilderID + Star
        m_mapSodierItem[soldier->Common.ClassID][soldier->Star] = soldier;
    }

    // 清空重置
    Loader.Release();
    // 填充数值表部分
    CHECK_RETURN(loadCsv(Loader, strNumerialCsv.c_str(), 3));

    while (Loader.NextLine())
    {
        int nRoleID = Loader.NextInt();
        Loader.NextStr();//备注
        int nRoleStar = Loader.NextInt();
        std::map<int, std::map<int, SoldierConfItem*> >::iterator iter = m_mapSodierItem.find(nRoleID);
        // 
        if (iter == m_mapSodierItem.end())
        {
            return false;
        }

        std::map<int, SoldierConfItem*>::iterator iterItem = iter->second.find(nRoleStar);
        if (iterItem == iter->second.end())
        {
            return false;
        }

        SoldierConfItem* pSoldierConfigItem = iterItem->second;
        LoadRoleNumerial(Loader, pSoldierConfigItem->Common);
        // 派发水晶消耗
        pSoldierConfigItem->Cost = Loader.NextInt();
        // 派发CD
        pSoldierConfigItem->CD = MSTOS(Loader.NextInt());
    }

    return true;
}

bool CConfCall::LoadCSV(const std::string& str)
{
    std::string strLogicCsv = str;
    std::string strNumerialCsv = str;
    strLogicCsv.append(".csv");
    strNumerialCsv.append("_Num.csv");

    CCsvLoader Loader;
    CHECK_RETURN(loadCsv(Loader, strLogicCsv.c_str(), 3));

    //如果有数据 
    while (Loader.NextLine())
    {
        CallConfItem* call = new CallConfItem;
        call->Common.ClassID = Loader.NextInt();
        Loader.NextStr();//备注
        LoadRoleLogic(Loader, call->Common, false);

        call->CardCurrentLevel = Loader.NextInt();
        call->RoleLifeTime = Loader.NextInt() * 1.0f / 1000.0f;
        call->RoleType = Loader.NextInt();
        call->RoleIdentity = Loader.NextInt();
        call->RoleMoveType = Loader.NextInt();
        call->RoleMoveDirection = Loader.NextInt();

        m_Datas[call->Common.ClassID] = call;
    }

    // 清空重置
    Loader.Release();
    // 填充数值表部分
    CHECK_RETURN(loadCsv(Loader, strNumerialCsv.c_str(), 3));

    while (Loader.NextLine())
    {
        int nRoleID = Loader.NextInt();
        Loader.NextStr();//备注
        std::map<int, void*>::iterator iter = m_Datas.find(nRoleID);
        if (iter == m_Datas.end())
        {
            return false;
        }

        CallConfItem* pCallConfigItem = static_cast<CallConfItem*>(iter->second);
        LoadRoleNumerial(Loader, pCallConfigItem->Common);
    }

    return true;
}

bool CConfZoom::LoadCSV(const std::string& str)
{
    CCsvLoader Loader;
    CHECK_RETURN(loadCsv(Loader, str.c_str(), 3));

    while (Loader.NextLine())
    {
        ZoomItem *item = new ZoomItem;
        item->RoleID = Loader.NextInt();
        Loader.NextStr(); // 备注

        item->ZoomNumber = Loader.NextFloat();
        CConfAnalytic::ToVec2(Loader.NextStr(), item->StandOffSet);
		item->HallZoom = Loader.NextFloat();
        item->Priority = Loader.NextInt();

        m_Datas[item->RoleID] = item;
    }

    return true;
}
