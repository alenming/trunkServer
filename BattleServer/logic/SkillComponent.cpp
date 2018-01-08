#include "SkillComponent.h"
#include "Skill.h"
#include "Role.h"

using namespace std;

CSkillComponent::CSkillComponent()
: m_nPrevState(0)
, m_pOwner(NULL)
{
    m_eComponentType = ESkillComponent;
}

CSkillComponent::~CSkillComponent()
{
    for (std::map<int, CSkill*>::iterator iter = m_Skills.begin();
        iter != m_Skills.end(); ++iter)
    {
        CSkill *skill = iter->second;
        if (NULL != skill)
        {
            delete skill;
        }
    }

    m_Skills.clear();
}

bool CSkillComponent::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);

	//序列化父类
	CLogicComponent::serialize(data);

    // 技能个数(技能个数不定, 会动态添加)
    int count = m_Skills.size();
    data.writeData(count);
    //每个技能序列化
    for (std::map<int, CSkill*>::iterator iter = m_Skills.begin(); 
        iter != m_Skills.end(); ++iter)
    {
        //每个技能数据之前先写入技能id
        data.writeData(iter->first);
        iter->second->serialize(data);
    }

    // 上一个技能状态
    data.writeData(m_nPrevState);

	SERIALIZE_MATCH(data);

    return true;
}

bool CSkillComponent::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);

	//反序列化父类
	CLogicComponent::unserialize(data);

    int count = 0;
    int skillid = 0;
    data.readData(count);
    // 移除所有技能, 里面可能有buff添加的技能, 而服务器没有该技能
    // removeAllSkill();
    std::set<int> validSkillSet;
    // 反序列化技能列表
    for (int i = 0; i < count; ++i)
    {
        data.readData(skillid);
        validSkillSet.insert(skillid);
        map<int, CSkill*>::iterator iter = m_Skills.find(skillid);
        if (iter == m_Skills.end())
        {
            // 有新的技能被挂载上
            addNewSkill(skillid);
            m_Skills[skillid]->unserialize(data);
        }
        else
        {
            // 对旧的技能反序列化
            iter->second->unserialize(data);
        }
    }

    // 统计失效的技能
    if (m_Skills.size() > validSkillSet.size())
    {
        std::set<int> invalidSkillSet;
        for (map<int, CSkill*>::iterator iter = m_Skills.begin();
            iter != m_Skills.end(); ++iter)
        {
            if (validSkillSet.find(iter->first) == validSkillSet.end())
            {
                invalidSkillSet.insert(iter->first);
            }
        }
        // 移除失效的技能
        for (std::set<int>::iterator iterInvalidSkill = invalidSkillSet.begin();
            iterInvalidSkill != invalidSkillSet.end(); ++iterInvalidSkill)
        {
            removeSkill(*iterInvalidSkill);
        }
    }

    data.readData(m_nPrevState);

	UN_SERIALIZE_MATCH(data);

    return true;
}

bool CSkillComponent::init(CRole *owner, const std::vector<int> &skillids)
{
    if (NULL == owner || skillids.empty())
    {
        return false;
    }

    m_pOwner = owner;
    m_nPrevState = owner->currentState();
    
    for (std::vector<int>::const_iterator iter = skillids.begin();
        iter != skillids.end(); ++iter)
    {
        addNewSkill(*iter);
    }

    return true;
}

bool CSkillComponent::init(CRole *owner, const std::vector<int> &skillids, const std::vector<int> &skilllvs)
{
    if (NULL == owner || skillids.size() != skilllvs.size())
    {
        return false;
    }

    m_pOwner = owner;
    m_nPrevState = owner->currentState();

    for (size_t i = 0; i < skillids.size(); ++i)
    {
        addNewSkill(skillids[i], skilllvs[i]);
    }

    return true;
}

void CSkillComponent::onExit()
{
    CLogicComponent::onExit();
    
    removeAllSkill();

    m_CanBreakSkills.clear();
}

void CSkillComponent::logicUpdate(float delta)
{
    //检测负面状态并打断技能
    checkBreak();

    for (std::map<int, CSkill*>::iterator iter = m_Skills.begin();
        iter != m_Skills.end(); ++iter)
    {
        CSkill *skill = iter->second;
        skill->update(delta);
    }
}

void CSkillComponent::regSkillBreak(CSkill *skill)
{
    if (m_CanBreakSkills.find(skill->skillId()) == m_CanBreakSkills.end())
    {
        m_CanBreakSkills[skill->skillId()] = skill;
    }
}

bool CSkillComponent::hasSkillID(int skillid)
{
    return m_Skills.find(skillid) != m_Skills.end();
}

void CSkillComponent::addNewSkill(int skillid, int skillLv)
{
    if (m_Skills.find(skillid) == m_Skills.end())
    {
        CSkill *skill = new CSkill();
        if (!skill->init(this, m_pOwner, skillid, m_Skills.size() + 1, skillLv))
        {
            delete skill;
            return;
        }

        m_Skills[skillid] = skill;
    }
}

void CSkillComponent::removeSkill(int skillid)
{
    std::map<int, CSkill*>::iterator iter = m_Skills.find(skillid);
    if (iter != m_Skills.end())
    {
        CSkill *skill = m_Skills[skillid];
        delete skill;
        m_Skills.erase(iter);
    }

    iter = m_CanBreakSkills.find(skillid);
    if (iter != m_CanBreakSkills.end())
    {
        m_CanBreakSkills.erase(iter);
    }
}

void CSkillComponent::removeAllSkill()
{
    for (std::map<int, CSkill*>::iterator iter = m_Skills.begin();
        iter != m_Skills.end();++iter)
    {
        delete iter->second;
    }

    m_Skills.clear();
    m_CanBreakSkills.clear();
}

float CSkillComponent::getCDPercent(int index)
{
    for (std::map<int, CSkill*>::iterator iter = m_Skills.begin();
        iter != m_Skills.end(); ++iter)
    {
        if (iter->second->skillIndex() == index)
        {
            return iter->second->getCDPercent();
        }
    }

    return 0.0f;
}

CSkill *CSkillComponent::getSkillWithID(int id)
{
    std::map<int, CSkill*>::iterator iter = m_Skills.find(id);
    if (iter != m_Skills.end())
    {
        return iter->second;
    }
    return NULL;
}

CSkill *CSkillComponent::getSkillWithIndex(int index)
{
    for (std::map<int, CSkill*>::iterator iter = m_Skills.begin();
        iter != m_Skills.end(); ++iter)
    {
        if (iter->second->skillIndex() == index)
        {
            return iter->second;
        }
    }

    return NULL;
}

bool CSkillComponent::canExecuteSkillIndex(int index)
{
    for (std::map<int, CSkill*>::iterator iter = m_Skills.begin(); 
        iter != m_Skills.end(); ++iter)
    {
        if (iter->second->skillIndex() == index)
        {
            return iter->second->canExecute();
        }
    }

    return false;
}

bool CSkillComponent::canExecuteSkill(int skillid)
{
    if (m_Skills.find(skillid) != m_Skills.end())
    {
        return m_Skills[skillid]->canExecute();
    }

    return false;
}

void CSkillComponent::executeSkillIndex(int index, std::vector<CRole*> &targets)
{
    KXLOGBATTLE("executeSkillIndex 1 index %d size %d", index, targets.size());
    for (std::map<int, CSkill*>::iterator iter = m_Skills.begin();
        iter != m_Skills.end(); ++iter)
    {
        CSkill* skill = iter->second;
        if (skill->skillIndex() == index)
        {
            if (skill->changeState())
            {
                skill->execute(targets);
            }
            break;
        }
    }
}

void CSkillComponent::executeSkillIndex(int index, Vec2 &pos)
{
    KXLOGBATTLE("executeSkillIndex 2 index %d X %f Y %f", index, pos.x, pos.y);
    for (std::map<int, CSkill*>::iterator iter = m_Skills.begin();
        iter != m_Skills.end(); ++iter)
    {
        CSkill* skill = iter->second;
        if (skill->skillIndex() == index)
        {
            if (skill->changeState())
            {
                skill->execute(pos);
            }
            break;
        }
    }
}

void CSkillComponent::executeSkill(int skillid, std::vector<CRole*> &targets, Vec2 &pos)
{
    KXLOGBATTLE("executeSkillIndex 3 size %d X %f Y %f", targets.size(), pos.x, pos.y);
    if (m_Skills.find(skillid) != m_Skills.end())
    {
        CSkill* skill = m_Skills[skillid];
        if (skill->changeState())
        {
            skill->execute(targets, pos);
        }
    }
}

void CSkillComponent::executeSkill(int skillid, std::vector<CRole*> &targets)
{
    KXLOGBATTLE("executeSkillIndex 4 size %d", targets.size());
    if (m_Skills.find(skillid) != m_Skills.end())
    {
        CSkill* skill = m_Skills[skillid];
        if (skill->changeState())
        {
            skill->execute(targets);
        }
    }
}

void CSkillComponent::executeSkill(int skillid, Vec2 &pos)
{
    KXLOGBATTLE("executeSkillIndex 5 skillid %d X %f Y %f", skillid, pos.x, pos.y);
    if (m_Skills.find(skillid) != m_Skills.end())
    {
        CSkill* skill = m_Skills[skillid];
        if (skill->changeState())
        {
            skill->execute(pos);
        }
    }
}

void CSkillComponent::checkBreak()
{
    int curState = m_pOwner->currentState();
    if (m_nPrevState != curState)
    {   
        if ((curState >= State_Negative_Begin
            && curState <= State_Negative_End)
            || curState == State_Death)
        {
            // 如果正在执行的则进行技能打断
            for (std::map<int, CSkill*>::iterator iter = m_CanBreakSkills.begin();
                iter != m_CanBreakSkills.end(); ++iter)
            {
                CSkill *skill = iter->second;
                if (!skill->isFinish())
                {
                    skill->breakSkill();
                }
            }
        }

        m_nPrevState = curState;
    }  
}
