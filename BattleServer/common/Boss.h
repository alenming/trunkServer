/*
* BOSS
*
* 2015-7-24 L
*/
#ifndef __BOSS_H__
#define __BOSS_H__

#include "Role.h"

class CBoss : public CRole
{
public:
	CBoss();
	~CBoss();

    virtual bool init(int typeId, int ownerId, int objId, CBattleHelper* battle);    
    // 设置int值，自动进行上下限过滤
    virtual void modifyAttribute(EAttributeTypes key, int v);
};

#endif