#ifndef __SKILL_COMPONENT_H__
#define __SKILL_COMPONENT_H__

#include "LogicComponent.h"
#include "BufferData.h"

struct SkillItem
{
    int SkillID;                 //技能ID
    int SKillLv;                 //技能等级
};

class CRole;
class CSkill;
class CSkillComponent : public CLogicComponent
{
public:
    CSkillComponent();
    ~CSkillComponent();

public:

    // 序列化反序列化
    virtual bool serialize(CBufferData& data);
    virtual bool unserialize(CBufferData& data);

    // 初始化技能逻辑组件
    virtual bool init(CRole *owner, const std::vector<int> &skillids);
    virtual bool init(CRole *owner, const std::vector<int> &skillids, const std::vector<int> &skillidlvs);
    virtual void onExit();
    // 逻辑帧的回调
    virtual void logicUpdate(float delta);
    // 技能注册打断事件
    void regSkillBreak(CSkill *skill);
    // 是否拥有技能
    bool hasSkillID(int skillid);
    // 添加一个新技能
    void addNewSkill(int skillid, int skillLv = 1);
    // 移除指定技能 (应该只移除buff添加的技能)
    void removeSkill(int skillid);
    // 移除所有技能
    void removeAllSkill();
    // cd百分比
    float getCDPercent(int index);

    CSkill *getSkillWithID(int id);
    CSkill *getSkillWithIndex(int index);
    // 是否可以使用第n个技能
    bool canExecuteSkillIndex(int index);
    // 是否可以使用指定技能
    bool canExecuteSkill(int skillid);
    // 对指定目标使用第n个技能
    void executeSkillIndex(int index, std::vector<CRole*> &targets);
    // 对指定位置使用第n个技能
    void executeSkillIndex(int index, Vec2 &pos);

    // 同时传目标和目标点对, 所有目标使用指定技能
    void executeSkill(int skillid, std::vector<CRole*> &targets, Vec2 &pos);
    // 对所有目标使用指定技能
    void executeSkill(int skillid, std::vector<CRole*> &targets);
    // 对指定位置使用指定技能
    void executeSkill(int skillid, Vec2 &pos);

private:
    // 检查负面状态发生
    void checkBreak();
private:

    int                     m_nPrevState;      // 技能所属者的上一个状态
    CRole *                 m_pOwner;
    std::map<int, CSkill*> m_Skills;           // <skillid, skill> 
    std::map<int, CSkill*> m_CanBreakSkills;   // 可以打断的技能
};

#endif //__SKILL_COMPONENT_H__
