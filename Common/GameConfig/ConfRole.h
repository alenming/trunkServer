#ifndef __CONF_ROLE_H__
#define __CONF_ROLE_H__

#include "ConfManager.h"
#include "CommTools.h"

// 特效播放点
enum StatusEffPlayType
{
    StatusEff_None,                                     // 无特效播放点
    StatusEff_Head,                                     // 头部特效点
    StatusEff_Body,                                     // 身体特效点
    StatusEff_Leg,                                      // 腿部特效点（移动中心点）
};

enum StatusSpeedAffect
{
    StatusSpeed_None,                                   // 无速度联动
    StatusSpeed_Attack,                                 // 攻速联动
    StatusSpeed_Move,                                   // 移速联动
    StatusSpeed_MPRecover,                              // MP恢复联动
};

// 状态属性
class StatusConfItem
{
public:
    StatusConfItem() : Action(NULL)
    {
    }

    ~StatusConfItem()
    {
        SAFE_DELETE(Action);
    }

    bool				Lock;						    // 状态是否加锁
    bool				AnimationAsh;				    // 灰化
    bool				IsFollow;					    // 俯角效果是否跟随移动
    int 				AnimationLoop;				    // 是否循环
    int     			StateId;					    // 状态标签
    int			        NextStateId;				    // 状态自动跳转
    int					CanBreakParam;				    // Status_CanBreakParam
    int					UIEffectID;					    // 附加UI特效（UI特效只能对应1个）
    int					LifeTimePrarm;					// 周期影响参数(0无,1攻击速率,2移动速度影响)
    float				LifeTime;					    // 状态生命周期
    float				AnimationSpeed;				    // 动画播放速度的倍数
    float				AnimationTransparency;		    // 透明度
    float				AnimationScale;				    // 缩放
    float				AnimationFadeOut;			    // 最后1帧淡出
	float				hue;				            // 色相
    StatusSpeedAffect   SpeedAffect;				    // 播放速度影响因素
    StatusEffPlayType   EffectPlayType;				    // 附加效果播放点：（0不播放1头2身3脚）                                                   
    VecInt				EffectIds;					    // 附加表现ID
    VecMusicInfos       MusicInfos;                     // 要播放的音效列表
    std::string			AnimationTag;				    // 状态自动跳转
    CDataFunction*		Action;						    // 角色行为
#ifndef RunningInServer                                    
    Color3B				AnimationRGB;				    // 偏色[R+G+B]
#endif  
};

//AI属性
class AIConfItem : public CConfBase
{
public:
    ~AIConfItem()
    {
        deleteAndClearVec<CDataFunction*>(Condition);
        deleteAndClearVec<CDataFunction*>(Action);
    }

    int					Order;							// AI顺序值
    int					NeedRoleStatus;					// 所需状态
    std::vector<CDataFunction*>	Condition;				// AI条件组
    std::vector<CDataFunction*>	Action;					// AI动作组
};

//角色共有属性
class Role
{
public:
    int					ClassID;					//ID
    int					AnimationID;				//表现ID
    int					StatusID;					//状态表ID
    int					AIID;						//AI表ID
    int 				Speed;						//移动速度
    int 				FireRange;					//近战攻击范围
    int 				FarFireRange;				//远程攻击范围
    int					PAttack;					//物理攻击力
    int					PAttackGrowUp;				//物理攻击力成长系数
    int					MAttack;					//魔法攻击力
    int					MAttackGrowUp;				//魔法攻击力成长值
    int					HP;							//生命值
    int					HPGrowUp;					//血量成长系数
    int					PGuard;						//物理护甲
    int					PGuardGrowUp;				//物理护甲成长
    int					MGuard;						//魔法护甲
    int					MGuardGrowUp;				//魔法护甲成长
    int					PPenetrate;					//物理穿透
    int					MPenetrate;					//魔法穿透
    int					AttackSpeed;				//攻击速度
    int					Rage;						//怒气上限值
    int					RageRecover;				//怒气回复值
    int					MP;							//魔法上限值
    int					MPRecover;					//魔法回复
    int					Strong;						//霸体值
    int					StrongRecover;				//霸体值恢复速度
    int					Haterd;						//仇恨值
    float				Defend;						//物理减伤
    float				Resustance;					//魔法减伤
    float				Mass;						//质量
    float				Scale;						//大小缩放
    float				EffectScale;				//特效大小缩放值
    float				Vampire;					//吸血值
    float				Rebound;					//角色反弹值
    float				Miss;						//闪避值
    float				Crit;						//暴击值
    float				CritGrowUp;					//暴击成长
    float				CritDamage;					//暴击伤害
    Vec2				FireOffset;					//子弹发射点
    Vec2				HeadOffset;					//头部特效播放点
    Vec2				HitOffset;					//身上特效播放点
    VecInt				Skill;						//技能 (前面两个技能可以升级的, 所以即使是空值也要保存0, 否则不确定可以升级的技能是那几个)
    VecInt				HPLine;						//血条类型, 和坐标
	VecFloat			AnimationHSV;               //骨骼HSV

    int					Name;						// 卡片名字ID
    int					Desc;						// 卡片描述ID
    int					Race;						// 种族
    int					Sex;						// 性别
    int					AttackType;					// 攻击类型（近战 远程）
    int					Vocation;					// 职业
    int					AttackDistance;				// 角色攻击距离(0=无,1=近,2=中，3=远，4=很远)
    std::string			Picture;					// 对应图片资源
    std::string         HeadIcon;					// 头像
};

// 卡片强化
struct CardEnhance
{
    int EnhanceType;                                //加强类型
    float CDParam;                                  //卡片CD加强
    float ConsumeParam;                             //卡片消耗加强
};

//英雄属性
class HeroConfItem
{
public:
    Role				Common;						//角色共有属性
    float				CrystalSpeedPrarm;			//水晶提升速率
    int					BasicExp;					//升级提高基础经验值
    int					ExpRatio;					//升级提供经验成长系数
    int					RacialRatio;				//同种族经验加成系数
    VecInt				PlayerSkill;				//玩家技能
    CardEnhance         RaceEnhance;                //卡片种族加强
    CardEnhance         VocationEnhance;            //职业加强
    CardEnhance         SexEnhance;                 //性别加强
    CardEnhance         AttackTypeEnhance;          //攻击类型	
};

//BOSS属性
class BossConfItem
{
public:
    Role				Common;						//角色共有属性
};

//MONSTER属性
class MonsterConfItem
{
public:
    Role				Common;						//角色共有属性
};

//战士属性
class SoldierConfItem
{
public:
    Role				            Common;		//角色共有属性
    int					            Star;		//士兵星级
	int								Rare;		//士兵稀有度
    int					            Cost;		//卡片派发消耗
    float				            CD;			//派发CD(秒)
    int					            IsSingo;	//是否唯一派发
};

// 召唤士兵属性
class CallConfItem
{
public:
    int                 CardCurrentLevel;           //卡牌当前等级
    float				RoleLifeTime;				//角色生命周期
    int					RoleType;					//角色阵营
    int                 RoleIdentity;               //角色身份
    int					RoleMoveType;				//角色线路归属
    int					RoleMoveDirection;			//移动方向(0自动获取,1->  2<-)

    Role				Common;						//角色共有属性
};

// 展示界面角色缩放
class ZoomItem
{
public:
    int                 RoleID;     // 角色ID
    float               ZoomNumber; // 展示界面大小缩放值
	float				HallZoom;	// 大厅展示界面大小缩放值
    int                 Priority;   // 优先级(越小越前)
    Vec2                StandOffSet;// 站位偏移量
};

// 一个角色所有的状态Map
class StatusConfMap : public CConfBase
{
public:
    bool LoadCSV(const std::string& str);
    const StatusConfItem* getStateItem(int stateId) const;
    //const std::map<int, StatusConfItem>& getStateMap() const;
};

// 所有角色的状态Map
class CConfStatus : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
    bool LoadCSV(int roleId, const std::string& str);
    const StatusConfItem* getStateItem(int roleId, int stateId);
    const StatusConfMap* getStateMap(int roleId);
    std::map<int, std::string>& getStatusFileMap()
    {
        return m_StatusFileMap;
    }
private:
    std::map<int, std::string> m_StatusFileMap;
};

class AIConfMap : public CConfBase
{
public:
    virtual ~AIConfMap();
    bool LoadCSV(const std::string& str);
    const std::vector<AIConfItem*>* getAIItems(int stateid);
    const std::map<int, std::vector<AIConfItem*> >& getAIMap();

private:
    std::map<int, std::vector<AIConfItem*> >  m_AIDatas;
};

class CConfAI: public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
    bool LoadCSV(int roleid, const std::string& str);
    const std::vector<AIConfItem*>* getAIItems(int roleId, int stateid);
    const AIConfMap* getAIMap(int roleId);
    std::map<int, std::string>& getAIMap()
    {
        return m_AIFileMap;
    }
private:
    std::map<int, std::string> m_AIFileMap;
};

class CConfStrToID : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
    int getIDByName(const std::string& str);

    std::map<std::string, int>& getConvertData()
    {
        return m_mapConvert;
    }
private:
    std::map<std::string, int> m_mapConvert;
};

class CConfRole : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str)
    {
        return true;
    }

    // 加载相同部分的逻辑表,最后一个参数表示是否召唤物,默认不是
    void LoadRoleLogic(CCsvLoader& pLoader, Role &common, bool noCall = true);

    // 加载相同部分的数值表,最后一个参数表示是否士兵,默认不是
    void LoadRoleNumerial(CCsvLoader& pLoader, Role &common);
};

class CConfHero : public CConfRole
{
public:
    virtual bool LoadCSV(const std::string& str);
};

class CConfBoss : public CConfRole
{
public:
    virtual bool LoadCSV(const std::string& str);
};

class CConfMonster : public CConfRole
{
public:
    virtual bool LoadCSV(const std::string& str);
};

class CConfSoldier : public CConfRole
{
public:
    virtual bool LoadCSV(const std::string& str);

    // 获取士兵信息
    SoldierConfItem* getData(int id, int star)
    {
        std::map<int, std::map<int, SoldierConfItem*> >::iterator iter = m_mapSodierItem.find(id);
        if (iter != m_mapSodierItem.end())
        {
            std::map<int, SoldierConfItem*>::iterator iterItem = iter->second.find(star);
            if (iterItem != iter->second.end())
            {
                return iterItem->second;
            }
        }

        return NULL;
    }

    // 获取所有士兵的信息
    std::map<int, std::map<int, SoldierConfItem*> >& getSoldiersConfig()
    {
        return m_mapSodierItem;
    }

private:
    std::map<int, std::map<int, SoldierConfItem*> > m_mapSodierItem;
};

class CConfCall : public CConfRole
{
public:
    virtual bool LoadCSV(const std::string& str);
};

class CConfZoom : public CConfRole
{
public:
	virtual bool LoadCSV(const std::string& str);
};

/////////////////////////////// 查询 ////////////////////////////////////////

inline const StatusConfMap* queryConfStateMap(int roleId)
{
    CConfStatus* conf = dynamic_cast<CConfStatus*>(
        CConfManager::getInstance()->getConf(CONF_STATUS));
    return conf->getStateMap(roleId);
}

inline const StatusConfItem* queryConfStateItem(int roleId, int stateId)
{
    CConfStatus* conf = dynamic_cast<CConfStatus*>(
        CConfManager::getInstance()->getConf(CONF_STATUS));
    return conf->getStateItem(roleId, stateId);
}

// 查询AI配置表的辅助方法
inline const AIConfMap* queryConfAIMap(int roleId)
{
    CConfAI* conf = dynamic_cast<CConfAI*>(
        CConfManager::getInstance()->getConf(CONF_AIDATA));
    return conf->getAIMap(roleId);
}

// 查询英雄配置表的辅助方法
inline const HeroConfItem* queryConfHero(int heroId)
{
    CConfHero* conf = dynamic_cast<CConfHero*>(
        CConfManager::getInstance()->getConf(CONF_HERO));
    return static_cast<HeroConfItem*>(conf->getData(heroId));
}

// 查询士兵配置表的辅助方法
inline const SoldierConfItem* queryConfSoldier(int soldierId, int soldierStar)
{
    CConfSoldier* conf = dynamic_cast<CConfSoldier*>(
        CConfManager::getInstance()->getConf(CONF_SOLDIER));
    return static_cast<SoldierConfItem*>(conf->getData(soldierId, soldierStar));
}

// 查询BOSS配置表的辅助方法
inline const BossConfItem* queryConfBoss(int soldierId)
{
    CConfBoss* conf = dynamic_cast<CConfBoss*>(
        CConfManager::getInstance()->getConf(CONF_BOSS));
    return static_cast<BossConfItem*>(conf->getData(soldierId));
}

// 查询Monster配置表的辅助方法
inline const MonsterConfItem* queryConfMonster(int soldierId)
{
    CConfMonster* conf = dynamic_cast<CConfMonster*>(
        CConfManager::getInstance()->getConf(CONF_MONSTER));
    return static_cast<MonsterConfItem*>(conf->getData(soldierId));
}

//查询召唤物配置
inline const CallConfItem* queryConfCall(int callid)
{
    CConfCall *conf = dynamic_cast<CConfCall*>(
        CConfManager::getInstance()->getConf(CONF_CALL));
    return static_cast<CallConfItem*>(conf->getData(callid));
}

//查询界面角色骨骼缩放大小配置
inline const ZoomItem* queryConfZoom(int roleID)
{
    CConfZoom *conf = dynamic_cast<CConfZoom*>(
        CConfManager::getInstance()->getConf(CONF_ROLE_ZOOM));
    return static_cast<ZoomItem*>(conf->getData(roleID));
}

#endif
