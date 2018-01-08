#ifndef __PVPROBOTROOM_H__
#define __PVPROBOTROOM_H__

#include <vector>
#include "GameUserManager.h"

class CRobotHelper
{
public:
	// 是否有匹配信息
	static bool canChallengeRobot(int uid, int &robotId);
	// 能否挑战机器人
	static bool canChallengeRobot(int uid, int summonerId, std::vector<int>& heroList);
	// 获得房间长度, 用来分配内存
	static int getRoomDataLength(int uid, int summonerId, std::vector<int>& heroList);
	// 获得战斗数据
	static bool getRobotBattleData(int uid, int robotId, int summonerId, std::vector<int>& heroList, char *data, int &length);
	// 战胜机器人设置机器人模型
	static void setModelWinRobot(int uid, int robotId);
	// 战败机器人设置机器人模型
	static void setModelLoseRobot(int uid, int robotId);

};

#endif //__PVPROBOTROOM_H__
