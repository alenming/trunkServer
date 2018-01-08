/*
挑战(关卡、三大试炼)辅助类
1、判断是否能够挑战
*/

#ifndef __CHALLENGE_HELPER_H__
#define __CHALLENGE_HELPER_H__

#include <vector>
#include "CommStructs.h"
#include "UnionMercenaryModel.h"


// 挑战房间信息
struct BattleRoomInfo
{
	int battleType;
	int stageId;
	int stageLv;
	int ext1;
	int ext2;
	std::vector<int> outerBuffs;
	std::vector<BuffData> buffs;
};

class CChallengeHelper
{
public:
	// 挑战所有类型关卡公共方法
	// 检查召唤师和英雄卡片
	static bool checkChallengeTeam(int uid, int summonerId, const std::vector<int> &heroList);
	// 获得房间数据
	static bool getBattleData(int uid, int summonerId, std::vector<int>& heroList, UnionMercenaryInfo *pMerInfo, BattleRoomInfo &roomInfo, char *data, int &length);
	// 获得房间长度, 通过召唤师, 英雄列表, 佣兵信息, 战斗内buff, 战斗外buff.
	static int getBattleDataLength(int uid, int summonerId, std::vector<int> &heroList, UnionMercenaryInfo *pMerInfo, int buffSize, int outerBonusSize);
	// 根据战斗信息获得关卡星级
	static int getStageStar(int uid, int stageId, ChallengeBattleInfo &finishData, std::vector<int> &starReason);
	// 获得佣兵数据, 如果玩家没有公会或者公会没有佣兵, 返回false
	static bool getMercenaryInfo(int uid, int mercenary, UnionMercenaryInfo &merInfo);

};

#endif