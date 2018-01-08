#ifndef __GAME_USER_H__
#define __GAME_USER_H__

#include "UserModel.h"
#include "BagModel.h"
#include "EquipModel.h"
#include "HeroModel.h"
#include "SummonModel.h"
#include "StageModel.h"
#include "TeamModel.h"
#include "AchievementModel.h"
#include "TaskModel.h"
#include "GuideModel.h"
#include "MailModel.h"
#include "ActivityInstanceModel.h"
#include "UserActionManager.h"
#include "GoldTestModel.h"
#include "HeroTestModel.h"
#include "TowerTestModel.h"
#include "PvpModel.h"
#include "PvpTaskModel.h"
#include "ShopModel.h"
#include "UserActiveModel.h"
#include "HeadModel.h"
#include "UserUnionModel.h"
#include "PersonMercenaryModel.h"
#include "BlueDiamondModel.h"

#include <vector>
#include <map>

class CGameUser
{
public:
	CGameUser();
	~CGameUser();

public:

	bool initModels(int uid);
	
	void refreshModels();

	void refreshModel(int modelType);

	void setModel(int modelType, IDBModel *model);

	int	getUid() { return m_nUid; }
	void setUid(int uid) { m_nUid = uid; }

	IDBModel* getModel(int modelType);
	
	CUserActionManager *getUserActionManager() { return &m_UserActionManager; }

private:

	int							m_nUid;
    CUserActionManager  		m_UserActionManager;
	std::map<int, IDBModel*>	m_mapModels;
};

#endif //__GAME_USER_H__
