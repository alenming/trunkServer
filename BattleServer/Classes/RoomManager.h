#ifndef __ROOMMANAGER_H__
#define __ROOMMANAGER_H__

#include <map>
#include <vector>

class IRoom;
class CPvpRoom;
class CPvpRobotRoom;
class CRoomManager
{
private:
    CRoomManager(void);
    ~CRoomManager(void);

public:

    static CRoomManager *getInstance();
    static void destroy();

	//指定战斗id是否有房间
	bool haveRoomWithUid(int uid);
	//指定战斗id
	bool haveRoomWithBattleId(int battleId);
	//进入房间
	CPvpRoom *enterRoom(int uid, int battleId, int roomType);
	//通过战斗id获得房间
	IRoom* getRoomWithBattleId(int battleId);
	//通过uid获得房间
	IRoom* getRoomWithUid(int uid);
	//移除房间
	void removeWithBattleId(int battleId);

private:

    int                      m_nRoomId;			//全局房间id
    static CRoomManager*     m_pInstance;
	std::map<int, IRoom*>    m_PVPRoomMap;		//<battleId, IRoom *>
	std::map<int, int>		 m_UsersBattleId;	//<userId, battleId>
};

#endif 
