/*
* Monster
*
* 2015-7-24 L
*/
#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "Role.h"
class BossConfItem;
class CBattleHelper;
class CMonster : public CRole
{
public:
	CMonster();
	~CMonster();

public:
	bool init(int monsterid, int ownerId, int objId, CBattleHelper *battle);
};

#endif