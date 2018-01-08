/*
 * 战斗相关数据模型
 * 房间模型
 * 玩家模型（用户模型精简版）
 * 英雄模型 
 *
 * 2015-1-6 by 宝爷
 */
#ifndef __BATTLE_MODELS_H__
#define __BATTLE_MODELS_H__

#include <vector>
#include <map>

#include "CommStructs.h"
#include "ConfManager.h"
#include "ConfHall.h"
#include "CallComponent.h"
#include "RoleComm.h"

class CRoleModel
{
public:
    CRoleModel();
    ~CRoleModel();

    //1. 前后交换 out = in << 16 + int >> 16
    //2. 其中一位异或 out char[key%sizeof(in)] ^ key % 128
    template<typename T>
    T encryptNumber(const EAttributeTypes& key, T num)
    {
        if (m_bEncrypt)
        {
            char* p = reinterpret_cast<char*>(&num);

            for (int i = 0; i < sizeof(T)/2; ++i)
            {
                p[i] = p[i] ^ p[sizeof(T) - i - 1];
                p[sizeof(T) - i - 1] = p[i] ^ p[sizeof(T) - i - 1];
                p[i] = p[i] ^ p[sizeof(T) - i - 1];
            }
            p[key % sizeof(T)] ^= (key % 128);
        }
        return num;
    }

    template<typename T>
    T decodeNumber(const EAttributeTypes& key, T num)
    {
        if (m_bEncrypt)
        {
            char* p = reinterpret_cast<char*>(&num);

            p[key % sizeof(T)] ^= (key % 128);
            for (int i = 0; i < sizeof(T) / 2; ++i)
            {
                p[i] = p[i] ^ p[sizeof(T) - i - 1];
                p[sizeof(T) - i - 1] = p[i] ^ p[sizeof(T) - i - 1];
                p[i] = p[i] ^ p[sizeof(T) - i - 1];
            }
        }
        return num;
    }

    bool init(int level, int roleType, const Role* role, int battleType);
    void addEffect(int effId, int effValue);
    // 应用附加效果 - 先计算百分比对基础属性的加成数值，再计算固定加成的总和
    void applyEffect();

    bool classIntKeyExist(const EAttributeTypes& key) const;
    bool classFloatKeyExist(const EAttributeTypes& key) const;
    int getClassInt(EAttributeTypes type);
    float getClassFloat(EAttributeTypes type);
    void setClassInt(const EAttributeTypes& key, int value);
    void setClassFloat(const EAttributeTypes& key, float value);
    // 获取解密后的数据
    std::map<EAttributeTypes, int> getClassIntMap();
    std::map<EAttributeTypes, float> getClassFloatMap();

    // 将对象类型分为2类处理 1: Hero, Soldier 2: Boss, Call, Monster
    void roleTypeLimit(int v, int min1, int max1, int min2, int max2);
    void roleTypeLimit(float v, float min1, float max1, float min2, float max2);
    
    inline int getLevel() { return m_nLevel; }
    inline int getRoleType() { return m_nRoleType; }
    inline const Role* getRoleComm() const { return m_pRole; }

protected:
    int m_nLevel;
    int m_nRoleType;
    const Role* m_pRole;
    std::map<EAttributeTypes, int> m_IntAddMap;
    std::map<EAttributeTypes, float> m_IntPercentMap;
    std::map<EAttributeTypes, float> m_FloatAddMap;
    std::map<EAttributeTypes, float> m_FloatPercentMap;

private:
    // 是否加密了
    bool m_bEncrypt;
    // 保存加密后的值
    std::map<EAttributeTypes, int> m_ClassIntMap;
    std::map<EAttributeTypes, float> m_ClassFloatMap;
};

// 士兵卡片模型
class CSoldierModel : public CRoleModel, public ISerializable
{
public:
    // 成功返回offset值，失败返回-1
    int init(SoldierCardData* soldierData, int battleType, bool bMercenary);

	//序列化,反序列化 士兵卡片模型
	bool serialize(CBufferData& data);
	bool unserialize(CBufferData& data);

    inline const SoldierConfItem* getConf() { return m_pConf; }
	inline int getSoldId() { return m_iId; }
	inline int getStar() { return m_iStar; }
	void setConf(SoldierConfItem* item) { m_pConf = item; }
    inline int getAI() { return m_nAI; }
    inline char* getTalent(){ return m_Talent; }
    inline const std::vector<int>& getSkillIds() { return m_pConf->Common.Skill; }
    inline std::map<int, int>& getAddBuff(){ return m_mapAddBuff; }
    inline const std::vector<SoldierEquip>& getEquips() { return m_vecEquips; }

protected:
    // 计算装备属性加成 —— 不应该执行Apply操作，因为外部还会有其他属性加成
    // 属性加成的最终生效会由模型创建处执行
    bool calcEquipAddition(std::vector<SoldierEquip>& effects);
    // 天赋属性加成
    bool calcTalentAddition();

public:
    int CurCost;                    // 当前水晶消耗
    float CurCD;                    // CD时间
    float MaxCD;                    // 最大CD
    bool IsLock;                    // 是否被锁住
    bool IsSingo;			        // 是否唯一派发
    bool IsMercenary;               // 是否为佣兵

private:
    int	m_iId;	                            // 对象ID
    int m_iStar;                            // 星级
    int m_nAI;                              // AI
    char m_Talent[8];                       // 天赋列表
    const SoldierConfItem* m_pConf;         // 对应卡片配置
    std::map<int, int> m_mapAddBuff;        // 添加buff
    std::vector<SoldierEquip> m_vecEquips;  // 装备
};

// 战斗用户基类
class CBattlePlayerModel
{
public:
    CBattlePlayerModel();
    virtual ~CBattlePlayerModel();

    inline int getUserId() { return m_nUserId; }
    inline int getUserLv() { return m_nUserLv; }
    inline int getCamp() { return m_Camp; }
    inline void setCamp(int camp) { m_Camp = camp; }
    inline int getMainRoleId() { return m_nMainRoleId; }
    inline int getMainRoleLv() { return m_nMainRoleLv; }
    inline int getIdentity() { return m_nIdentity; }
    inline const std::string& getUserName() { return m_UserName; }
    inline void setUserName(std::string name) { m_UserName = name; }
    inline CRoleModel* getRoleModel(){ return m_RoleModel; }

protected:
    int m_nUserId;
    int m_nUserLv;
    int m_Camp;
    int m_nMainRoleId;
    int m_nMainRoleLv;
    int m_nIdentity;
    std::string m_UserName;
    CRoleModel* m_RoleModel;
};

// 战斗用户模型（战斗内，精简版用户模型）
class CPlayerModel : public CBattlePlayerModel
{
public:
    CPlayerModel();
    virtual ~CPlayerModel();

    // 根据用户数据结构体来初始化（网络数据包）
    int initByUserData(PlayerData* data, int battleType);

    CSoldierModel* getSoldierCard(int index);
    inline std::vector<CSoldierModel*>& getSoldierCards() { return m_SoldierCards; }
    inline const std::vector<int>& getAdditions() { return m_Additions; }
    // 填充buffdata
    bool fillBufferData(CBufferData& buff);

private:
    void calcAddition(CRoleModel* solider);

private:

    std::vector<int> m_Additions;
    std::vector<CSoldierModel*> m_SoldierCards;
};

// 战斗内电脑用户模型
class CComputerModel : public CBattlePlayerModel
{
public:
    CComputerModel();
    virtual ~CComputerModel();

    int init(int stageId, int level, int battleType);
    
    CRoleModel* getMonsterModel(int index);
    inline std::vector<CRoleModel*>& getMonsterModels() { return m_MonsterModels; }

private:
    std::vector<CRoleModel*> m_MonsterModels;
};

// 所有货币
struct DropCurrency
{
	int exp;				//经验
	int gold;				//金币
	int diamond;			//钻石
	int pvpCoin;			//竞技币
	int towerCoin;			//塔币
	int guildContrib;		//公会贡献
};

// 结算模型
class CSettleAccountModel
{
public:
	CSettleAccountModel();
	~CSettleAccountModel();

public:
	// 设置战斗结果
    void setChallengeResult(int type) { m_nChallengeResult = type; }
	// 设置tick
	void setTick(int tick) { m_nTick = tick; }
	// 0-100
	void setHPPercent(int percent) { m_nHeroHpPercent = percent; }
	// 递增boss被消耗的血量
	void addHitBossHP(int hp) { m_nHitBossHP += hp; }
	// 递增消耗的水晶数
	void addCostCrystal(int crystal) { m_nCostCrystal += crystal; }
    // 剩余水晶数
    void setCrystal(int crystal) { m_nCrystal = crystal; }
	//设置水晶等级
	void setCrystalLv(int crystalLv) { m_nCrystalLv = crystalLv; }
	
	// 战斗结果
    int getChallengeResult() { return m_nChallengeResult; }
	// 获得tick
	int getTick() { return m_nTick; }
	// 获得血量百分比
	int getHPPercent() { return m_nHeroHpPercent; }
	// 获得boss被消耗血量
	int getHitBossHP() { return m_nHitBossHP; }
	// 获得消耗的水晶数量
    int getCostCrystal() { return m_nCostCrystal; }
    // 获得剩余水晶数
    int getCrystal() { return m_nCrystal; }
	// 获得水晶等级
	int getCrystalLv() { return m_nCrystalLv; }
	// 重置
	void resetSettle();

private:

    int	    m_nChallengeResult;     // 挑战结果
    int		m_nTick;				// 战斗所消耗的时间tick(100ms)
    int		m_nHeroHpPercent;		// 英雄剩余血量与最大血量比值
    int		m_nHitBossHP;			// boss被消耗的血量
    int		m_nCostCrystal;			// 玩家消耗的水晶数量
    int     m_nCrystal;             // 剩余水晶数
	int		m_nCrystalLv;			// 当前水晶等级
};

// 房间通用接口 —— 前后端通用
class CBattleRoom
{
public:
    CBattleRoom()
        : m_nMaster(0)
        , m_nOther(0)
        , m_nStageId(0)
        , m_nMonsterLevel(1)
        , m_nBattleType(0)
        , m_nExt1(0)
        , m_nExt2(1)
    {
    }
    inline void setMaster(int master) { m_nMaster = master; }
    inline int getMaster() { return m_nMaster; }
    inline int getStageId() { return m_nStageId; }
    inline void setStageId(int map) { m_nStageId = map; }
    inline int getStageLevel() { return m_nMonsterLevel; }
    inline int getBattleType() { return m_nBattleType; }
    inline void setBattleType(int battle) { m_nBattleType = battle; }
    inline int getExt1() { return m_nExt1; }
    inline int getExt2() { return m_nExt2; }

protected:
    int m_nMaster;
    int m_nOther;
    int m_nStageId;
    int m_nMonsterLevel;
    int m_nBattleType;
    int m_nExt1;
    int m_nExt2;
};

// 房间数据模型
class CRoomModel : public CBattleRoom
{
public:
    CRoomModel();
    virtual ~CRoomModel();

    // 根据用户数据结构体来初始化（网络数据包）
    bool initByRoomData(RoomData* data);
    // 根据RoomData和两个Player来初始化
    bool initPVPRoomBylayer(int stageId, CBattlePlayerModel* master, CBattlePlayerModel* other);

    inline void addPlayerModel(CBattlePlayerModel* player)
    {
        if (NULL != player
            && m_Players.find(player->getUserId()) == m_Players.end())
        {
            m_Players[player->getUserId()] = player;
            if (m_nMaster != player->getUserId())
            {
                m_nOther = player->getUserId();
            }
        }
    }
    inline CBattlePlayerModel* getMasterModel()
    {
        std::map<int, CBattlePlayerModel*>::iterator iter = m_Players.find(m_nMaster);
        if (iter != m_Players.end())
        {
            return iter->second;
        }
        return NULL;
    }
    inline CBattlePlayerModel* getOtherModel()
    {
        std::map<int, CBattlePlayerModel*>::iterator iter = m_Players.find(m_nOther);
        if (iter != m_Players.end())
        {
            return iter->second;
        }
        return NULL;
    }
    inline CBattlePlayerModel* getPlayer(int i) 
    { 
        std::map<int, CBattlePlayerModel*>::iterator iter = m_Players.find(i);
        if (iter != m_Players.end())
        {
            return iter->second;
        }
        return NULL;
    }
    
	inline std::vector<BuffData>& getStageBuffs() { return m_StageBuffs; }
	inline std::map<int, CBattlePlayerModel*>& getPlayers(){ return m_Players; }
    inline CSettleAccountModel *getSettleAccountModel() { return m_pSettleAccount; }

private:
	CSettleAccountModel *m_pSettleAccount;
	std::vector<BuffData> m_StageBuffs;
    std::map<int, CBattlePlayerModel*> m_Players;
};

#endif
