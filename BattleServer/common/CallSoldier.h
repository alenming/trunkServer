#ifndef __CALLSOLDIER_H__
#define __CALLSOLDIER_H__

#include "Role.h"

class CBattleHelper;
class CallConfItem;
class CCallSoldier : public CRole
{
public:
    CCallSoldier();
    ~CCallSoldier();

public:
    bool init(int callid, int level, int ownerId, int objId, CBattleHelper *battle);
    inline const CallConfItem * getCallConf() { return m_pCallConf; }

private:
    CRoleModel* m_pCallModel;
    const CallConfItem* m_pCallConf;
};

#endif //__CALLSOLDIER_H__
