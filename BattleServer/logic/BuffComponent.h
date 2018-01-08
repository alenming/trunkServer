#ifndef __BUFF_COMPONENT_H__
#define __BUFF_COMPONENT_H__

#include "LogicComponent.h"
#include "BufferData.h"

class CRole;
class CBuff;

struct NewBuffData
{
    CRole *maker;           // buff的产生者, 与携带者相对, 可能产生者就是携带者本对象
    int buffid;             // 产生的buffid
    int addstack;           // 变化的层数, 可能为负数, 意为减几层
};

class CBuffComponent : public CLogicComponent
{
public:
    CBuffComponent();
    ~CBuffComponent();

public:

    // 序列化反序列化
    virtual bool serialize(CBufferData& data);
    virtual bool unserialize(CBufferData& data);

    // 组件初始化
    bool init(CRole *owner);
    virtual void onExit();
    // 逻辑轮询
    void logicUpdate(float delta);
    // 添加一个buff 如果配表没有默认一层, addstack为层数, 根据配表中某buffid配几行决定最多几层.
    // n代表增加n层, -n减少n层
    bool addBuff(CRole *maker, int buffid, int addstack = 1);
    // 是否拥有buff
    bool hasBuff(int buffid);
    // 移除buff
    void removeBuff(int buffid);
    // 驱散 干掉正面buff
    void disperse();
    // 净化 干掉负面buff(debuff)
    void purification();
    // 清除所有buff
    void clear();
    // 创建buff
    CBuff* createBuff(CBuffComponent *cpnt, CRole *owner, CRole *maker, int buffid, int stack);
    // 回收buff
    void destoryBuff(CBuff* buff);

private:

    void addBuffToRole(CRole *maker, int buffid, int addstack);

private:

    CRole *                 m_pOwner;
    std::map<int, CBuff*>   m_BuffList;
    std::vector<CBuff*>     m_ExitBuffList;
    std::vector<NewBuffData> m_AddBuffList;
};

#endif //__BUFF_COMPONENT_H__
