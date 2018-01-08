#ifndef __TOWERTESTMODEL_H__
#define __TOWERTESTMODEL_H__

#include "IDBModel.h"
#include "StorageManager.h"
#include <map>
#include <vector>

enum ETowerTestField
{
	TOWER_FD_NONE = 0,
	TOWER_FD_TIMES,					//挑战次数
	TOWER_FD_TIMESTAMP,				//恢复时间戳
	TOWER_FD_FLOOR,					//当前楼层
	TOWER_FD_STAGEID,				//楼层选定的关卡ID
	TOWER_FD_FLOORSTATE,			//楼层状态
	TOWER_FD_EVENTPARAM,			//事件参数
	TOWER_FD_INTEGRAL,				//积分
	TOWER_FD_CRYSTAL,				//挑战水晶数
	TOWER_FD_STARS,					//星星数量
	TOWER_FD_OUTERBONUSLIST,		//外部buff列表
	TOWER_FD_END,					//结束标识
};								

enum ETowerFloorState
{
	FLOORSTATE_FIGHTING = 0,		//挑战状态
	FLOORSTATE_OUTERBONUS,			//外部buff事件状态
	FLOORSTATE_TREASURE,			//宝箱事件状态
};

class CTowerTestModel : public IDBModel
{
public:
	CTowerTestModel();
	~CTowerTestModel();

public:

	bool init(int uid);

	bool Refresh();

	bool SetTowerTestField(std::map<int, int> &values);

	bool SetTowerTestField(int field, int value);

	bool GetTowerTestField(std::map<int, int> &values);

	int GetTowerTestField(int field);

	bool AddOuterBonus(int outerId);

	bool AddOuterBonus(std::vector<int> &outerIds);

	bool ResetTowerTest();

	std::vector<int> & GetOuterBonusList();

private:

	int					 m_nUid;
	Storage *			 m_pStorage;
	std::string			 m_strTowerkey;
	std::vector<int>	 m_mapOuterBonusList;
	std::map<int, int>   m_mapTowerData;
};

#endif //__TOWERTESTMODEL_H__
