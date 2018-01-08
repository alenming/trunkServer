/*
 * 角色的基类
 * 封装了英雄和士兵的公共属性和方法
 * 公共方法属性的初始化
 *
 * 2014-12-23 by 宝爷
 */
#ifndef __ROLE_H__
#define __ROLE_H__

#include "GameObject.h"
#include "Skill.h"

#include "AIComponent.h"
#include "RoleComponent.h"
#include "BuffComponent.h"
#include "SkillComponent.h"
#include "CallComponent.h"
#include "RoleComm.h"

class CRole;

//黑板，用来记录数据
class CBlackBoard : public ISerializable
{
public:
	CBlackBoard();
    ~CBlackBoard();

	void setBattleHelper(CBattleHelper* battleHelper) { m_BattleHelper = battleHelper; }
    void clearTargetList();
    void clearBlackBoard();

    // 将Role添加到TargetList
    void addToTargetList(CRole* role);
    // 将TargetList清空并将Role添加到TargetList
    void clearAndAddToTargetList(CRole* role);
    void removeFormTargetList(CRole* role);

    inline void setTargetPoint(const Vec2& pt) { m_TargetPoint = pt; }
    inline Vec2& getTargetPoint() { return m_TargetPoint; }
    inline std::vector<CRole*>& getTargetList() { return m_TargetList; }

	virtual bool serialize(CBufferData& data);
	virtual bool unserialize(CBufferData& data);
private:
    Vec2                m_TargetPoint;  // 目标点
    std::vector<CRole*> m_TargetList;   // 临时目标角色
	CBattleHelper*		m_BattleHelper;	
};

class CRole : public CGameObject
{
public:
    CRole();
    virtual ~CRole();

    // 传入对象类型ID，所有者ID，对象唯一ID，战斗单例
    virtual bool init(int typeId, int ownerId, int objId, CBattleHelper* battle);
	virtual void onExit();

    // 初始化角色的通用配置属性
    void initRoleAttribute(CRoleModel *model);
	// 初始化角色通用的逻辑组件
    bool initRoleComponents(const Role* role);
	// 初始化显示相关的组件（服务器为空实现）
    bool initDisplayComponents(const Role* role);

	//////////////////////////////////SERIALIZE///////////////////////////////////
	// 将游戏场景打包到data中
	virtual bool serialize(CBufferData& data);
	// 从data中解压出游戏场景
	virtual bool unserialize(CBufferData& data);

    /////////////////////////////////BUFF////////////////////////////////////////
    //是否拥有指定buff
    bool haveBuff(int buffId);
    //添加buff
    bool addBuff(CRole *maker, int buffId, int addstack);
	//删除buff
	void delBuff(int buffId);

    //////////////////////////////////技能////////////////////////////////////////
    CSkill *getSkillWithID(int skillId);
    //能否执行第几个技能
    CSkill *getSkillWithIndex(int skillIndex);
    bool canExecuteSkillIndex(int skillIndex);
    bool executeSkillIndex(int skillIndex, Vec2 &pos);

    //使用指定技能
    bool canExecuteSkill(int skillId);
    bool executeSkill(int skillId, Vec2 &pos);
    bool executeSkill(int skillId, std::vector<CRole*> &targets, Vec2 &pos); 

    //////////////////////////////////状态////////////////////////////////////////
    CState* changeState(int stateId);
    int currentState();

    //////////////////////////////////固定点//////////////////////////////////////
    inline Vec2& getFireOffset() { return m_FireOffset; }
    inline Vec2 getBaseFireOffset()     
    {
		Vec2 ret = m_pModel->getRoleComm()->FireOffset;
        ret.x *= m_nDirection;
        return ret;
    }
    inline Vec2 getFirePoint() { return m_Position + m_FireOffset; }
    inline Vec2& getHeadOffset() { return m_HeadOffset; }
    inline Vec2 getBaseHeadOffset() 
    {
		Vec2 ret = m_pModel->getRoleComm()->HeadOffset;
        ret.x *= m_nDirection;
        return ret;
    }
    inline Vec2 getHeadPoint() { return m_Position + m_HeadOffset; }
    inline Vec2& getHitOffset() { return m_HitOffset; }
    inline Vec2 getBaseHitOffset()
    {
		Vec2 ret = m_pModel->getRoleComm()->HitOffset;
        ret.x *= m_nDirection;
        return ret;
    }
    inline Vec2 getHitPoint() { return m_Position + m_HitOffset; }
    inline void initPosition(const Vec2& pos)
    {
        setAllPosition(pos);
        // 需要根据线路ID进行设置
		initAttribute(EAttributeYmin, pos.y);
    }
    inline void updateDirection()
    {
        int nPreDirection = m_nDirection;
        m_nDirection = getIntAttribute(EAttributeCamp) == ECamp_Blue ? 1 : -1;
        if (nPreDirection != m_nDirection)
        {
            m_FireOffset.x *= m_nDirection;
            m_HitOffset.x *= m_nDirection;
            m_HeadOffset.x *= m_nDirection;
        }
    }

    //////////////////////////////////各类组件////////////////////////////////////////
    inline CAIComponent* getAIComponent() { return m_AIComponent; }
    inline CBuffComponent* getBuffComponent() { return m_BuffComponent; }
    inline CSkillComponent* getSkillComponent() { return m_SkillComponent; }
    inline CRoleComponent* getRoleComponent() { return m_RoleComponent; }
    inline CBlackBoard& getBlackBoard()
    {
        return m_BlackBoard;
    }

	inline Node* getDisplayNode()
	{
		if (NULL != m_DisplayNode)
		{
			return m_DisplayNode;
		}
		else
		{
			return this;
		}
	}

    ///////////////////////////////////属性相关///////////////////////////////////////
    // 获取角色类型
    inline RoleType getRoleType() { return m_RoleType; }
    // 获取模型配置
    inline const CRoleModel* getRoleModel() { return m_pModel; }
    // 自增Stat
    void updateStat(EAttributeTypes stat);

    virtual int getIntAttribute(EAttributeTypes key);
    virtual float getFloatAttribute(EAttributeTypes key);
    virtual bool checkAttribute(EAttributeTypes key, int& outv);
    virtual bool checkAttribute(EAttributeTypes key, float& outv);
    int limitAttribute(EAttributeTypes key, int v);
    float limitAttribute(EAttributeTypes key, float v);
    void setAttribute(EAttributeTypes key, int v);
    void setAttribute(EAttributeTypes key, float v);
    bool updateAttribute(EAttributeTypes key, int v);
    bool updateAttribute(EAttributeTypes key, float v);
	void initAttribute(EAttributeTypes key, int v);
	void initAttribute(EAttributeTypes key, float v);

protected:
    // 设置int值，自动进行上下限过滤
    virtual void modifyAttribute(EAttributeTypes key, int v);
    // 设置float值，自动进行上下限过滤
    virtual void modifyAttribute(EAttributeTypes key, float v);
    // 检测属性是否可在结算修改
    inline bool canModifyAttribute(EAttributeTypes key);
    // 联动修改相应的属性
    inline void changeOtherAttrbute(EAttributeTypes key);

protected:
    RoleType m_RoleType;                    // 角色类型
    Vec2 m_FireOffset;                      // 发射偏移点
    Vec2 m_HeadOffset;                      // 头部偏移点
    Vec2 m_HitOffset;                       // 受击偏移点
    CBlackBoard m_BlackBoard;               // 黑板
	CAIComponent* m_AIComponent;		    // AI组件
	CBuffComponent* m_BuffComponent;	    // BUFF组件
	CSkillComponent* m_SkillComponent;	    // 技能组件
    CRoleComponent* m_RoleComponent;	    // 角色组件
    CRoleModel* m_pModel;

private:
    Node* m_DisplayNode;				    // 挂载显示相关的节点
};

#endif
