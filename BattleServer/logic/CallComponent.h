#ifndef __CALLCOMPONENT_H__
#define __CALLCOMPONENT_H__

#include "RoleComponent.h"
#include "BufferData.h"

class CallConfItem;
class CCallComponent : public CRoleComponent
{
public:
    CCallComponent();
    ~CCallComponent();

public:

    bool init(int roleid, const CallConfItem* callConf);

    virtual void onExit();

    void logicUpdate(float delta);

	virtual bool serialize(CBufferData& data);
	virtual bool unserialize(CBufferData& data);

private:

    float                m_fCallTime;
    const CallConfItem*  m_pCallConf;
};

#endif //__CALLCOMPONENT_H__
