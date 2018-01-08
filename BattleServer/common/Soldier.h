/*
* Ê¿±ø
*
* 2014-12-23 by ±¦Ò¯
*/
#ifndef __SOLDIER_H__
#define __SOLDIER_H__

#include "Role.h"

class CSoldier : public CRole
{
public:
    CSoldier();
    virtual ~CSoldier();

	virtual bool init(int index, int ownerId, int objId, CBattleHelper* battle);

    int getStar();
private:
    void equipAddition(const std::map<int, int> &mapIntAttribute, const std::map<int, float> &mapFloatAttribute);
};

#endif
