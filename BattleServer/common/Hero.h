/*
* 英雄
*
* 2014-12-23 by 宝爷
*/
#ifndef __HERO_H__
#define __HERO_H__

#include "Role.h"

class CHero : public CRole
{
public:
	CHero();
	virtual ~CHero();

	virtual bool init(int typeId, int ownerId, int objId, CBattleHelper* battle);

	//序列化,反序列化 英雄
	bool serialize(CBufferData& data);
	bool unserialize(CBufferData& data);

	virtual bool executeSkillIndex(int skillIndex, Vec2 pos = Vec2::ZERO);
    virtual bool executeSkill(int skillId, Vec2 pos = Vec2::ZERO);

    // 是否可以升级水晶
    bool canUpgradeCrystal();
    // 升级水晶 - 自动扣除消耗
    bool upgradeCrystal();
    // 是否能使用这么多水晶
    bool canUseCrystal(int crystal);
    // 消耗水晶
    bool useCrystal(int crystal);
	// 水晶是否最大级别
	bool isCrystalMaxLevel();
    // 升级指定级别的水晶所需消耗
	int getCrystalCost(int level);

    // 是否能召唤士兵
    bool canUseSoldierCard(int soldierId);
    // 召唤士兵
    bool createSoldier(int soldierId, int line = -1);

	bool isSoldierCardLock(int id);
	bool isSoldierCardBuff(int id);
	bool isSoldierCardDebuff(int id);
	bool isSoldierCardBinding(int id);
	float getSoldierCardCDPercent(int id);

    // 当前卡片是否唯一
    bool isSoldierCardSingle(int soldierId);
    // 当前是否有唯一士兵在场上
    bool isSoldierSingle(int soldierId);
    // 每帧调用此方法来判断唯一卡片
    void checkSingo();

    CSoldierModel* getSoldierCard(int index)
    {
        if (NULL != m_SoldierCards
            && index >= 0 && index < static_cast<int>(m_SoldierCards->size()))
        {
            return m_SoldierCards->at(index);
        }
        return NULL;
    }

    std::vector<CSoldierModel*>* getSoldierCards()
    {
        return m_SoldierCards; 
    }

#ifndef RunningInServer
    // 设置水晶的预扣值
    void setWithholdValue(int value = 0);
    inline int getWithholdValue(){ return m_nWithholdValue; }

    // 设置公共技能执行时间
    void startSkillExecute(bool flag);
    void setCurSkillExecutingTime(float time);
    void setMaxSkillExecutingTime(float time);
    float getCommonCDPercent();
    bool isCommonCD(){ return m_bStartSkillExecute; }
#endif

private:
    std::map<int, CRole*> m_SingleMap; // 唯一派发
    std::vector<CSoldierModel*>* m_SoldierCards;

#ifndef RunningInServer
    int m_nWithholdValue;                               // 水晶预扣值
    bool m_bStartSkillExecute;                          // 开始执行的标志
    float m_fCurSkillExecutingTime;                     // 当前执行时间
    float m_fMaxSKillExecutingTime;                     // 最大执行时间
#endif
};

#endif
