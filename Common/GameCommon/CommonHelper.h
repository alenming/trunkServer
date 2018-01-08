#ifndef __COMMON_HELPER_H__
#define __COMMON_HELPER_H__

#define TIME_SQUARE_INTERVAL			28800	//时区差
#define DAYOFSECOND						86400	//一天秒数

#include <vector>

struct STalentArrangeData;
struct DropItemInfo;
struct ArenaChestItem;
class CCommonHelper
{
public:
	//根据mmr值获得对应段位
	static int getUserMMRDan(int mmr);
	//根据积分获得对应段位
	static int getUserIntegralDan(int integral);
	//获得段位的DanMMR_K
	static float getDanMMR_K(int dan);
	//获得段位的MMR_Kx
	static float getDanMMR_Kx(int dan);
	//获得段位的Arena_K
	static float getDanArena_K(int dan);
	//获得段位机器人匹配时间
	static int getDanRobotMatchTime(int dan);
	//获得章节类型对应的任务类型
	static int getChapterTypeToTask(int chaperId);
    //获取关卡类型
    static int getStageTypeToTask(int stageId);
	//获得关卡需要的体力
	//static int getStageNeedEnergy(int chapterId, int stageId);
	//获得关卡需要的等级
	static int getStageNeedLevel(int chapterId, int stageId);
	//获得关卡的下个解锁Id
	static bool getNextChapterStage(int chapterId, int stageId, int &nextChapter, int &nextStage);
	//获得英雄最高星级
	static int getHeroTopStar(int heroId);

	//根据最小值最大值随机
	//static int RandomMinMax(int min, int max);
	//将dropItems分成4类物品, 背包物品, 英雄物品, 召唤师物品, 资源物品(包括头像, 英雄碎片)
	static void classifyItems(const std::vector<DropItemInfo> &dropItems, std::vector<DropItemInfo> &bagItems,
        std::vector<DropItemInfo> &heroItems, std::vector<DropItemInfo> &summonerItems, std::vector<DropItemInfo> &resourceItems);
    //将dropItems分成2类物品, 背包物品, 非背包物品
    static void classifyItems(const std::vector<DropItemInfo> &dropItems, std::vector<DropItemInfo> &bagItems, std::vector<DropItemInfo> &otherItems);

	//获取当前时间毫秒数
	static int getCurMinTime();
	//获取当前时间字符串
	static void getCurTimeStr(char *pBuf, int nLen);
	//获取当前总天数
	static int getTotalDay(int nTime = 0);

    //加密/解密协议
    static void encryptProtocolBuff(int mainCmd, int subCmd, char *buff, int len);

    // 获取种族对应的种族天赋页
    static const STalentArrangeData *getRaceTalentArrange(int race);
    // 获取职业对应的职业天赋页
    static const STalentArrangeData *getVocationTalentArrange(int vocation);

	// 获得pvp宝箱
	static const ArenaChestItem* getArenaChest(int pvpLv, int userLv, int quality);
	// 根据玩家等级, 日胜场获得掉落id
	static const int getArenaDrop(int userLv, int dayWinTimes);

};

#endif //__COMMON_HELPER_H__

