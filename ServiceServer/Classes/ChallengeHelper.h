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
	// 根据战斗信息获得关卡星级
	static int getStageStar(int uid, int stageId, ChallengeBattleInfo &finishData, std::vector<int> &starReason);
	// 获得佣兵数据, 如果玩家没有公会或者公会没有佣兵, 返回false
	static bool getMercenaryInfo(int uid, int mercenary, UnionMercenaryInfo &merInfo);

	// 关卡挑战
	// 是否可以挑战世界地图, 扫荡times > 1
	static bool canChallengeStage(int uid, int chapterId, int stageId, int times = 1);
	// 获得章节关卡的关卡等级
	static int getChapterStageLevel(int chapterId, int stageId);

	// 金币试练
	// 是否可以挑战
	static bool canChallengeGoldTest(int uid, int &stageId, int &stageLv);
	// 判断能否领取宝箱, 能则设置领取位置
	static bool isEnoughGoldTestChest(int uid, int curChest);

	// 英雄试练
	// 是否可以挑战英雄试练
	static bool canChallengeHeroTest(int uid, int instanceId, int diff);
	// 获得英雄试练关卡等级
	static int getHeroTestStageLevel(int uid, int instanceId, int diff);

	// 爬塔试练
	// 是否可以挑战爬塔试练
	static bool canChallengeTowerTest(int uid, int floor);
	// 获得爬塔对应楼层的关卡等级
	static int getTowerTestStageLevel(int uid, int floor);

	// 副本挑战
	// 是否可以挑战副本
	static bool canChallengeSpecialTest(int uid);

};

#endif