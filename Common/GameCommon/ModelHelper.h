#ifndef __MODEL_HELPER_H__
#define __MODEL_HELPER_H__

#include <vector>

#include "IDBModel.h"
#include "EquipComm.h"
#include "ConfHall.h"

// 玩家基础信息
struct UserBasicInfo
{
	int userLv;				           // 玩家等级
    int lastLoginTime;                 // 玩家最后登录时间
    int identity;                      // 身份(如蓝钻,蓝钻等级*10+蓝钻类型) 
    std::string name;                  // 玩家名字

    UserBasicInfo()
        : userLv(0)
        , lastLoginTime(0)
        , identity(0)
        , name("")
    {

    }
};  

struct UserUnionBasicInfo
{
    int todayPvpLiveness;              // 今日pvp活跃度
    int todayStageLiveness;            // 今日关卡活跃度
    int totalContribution;             // 累计贡献

    UserUnionBasicInfo()
        : todayPvpLiveness(0)
        , todayStageLiveness(0)
        , totalContribution(0)
    {

    }
};

struct DropItemInfo;
struct SShopGoods;
struct DBEliteStageInfo;
class ShopGoodsConfigData;

class CModelHelper
{
public:
	// 获得模型
	static IDBModel *getModel(int uid, int modelType);

	/////////////////////////////用户数据相关////////////////////////////////////
	//增加金币
	static bool addGold(int uid, int gold);
	//增加经验
    static bool addExp(int uid, int exp);
	static bool addExp(int uid, int exp, int &newLv, int &newExp);
	//增加钻石
	static bool addDiamond(int uid, int diamond,int nParam1 = 0,int nParam2 = 0);
	//操作体力(增加或消耗)调用,体力时间戳的恢复存储计算
	//static bool addEnergy(int uid, int count);
	//获得体力
	//static int getEnergy(int uid);
    //增加竞技币
    static bool addPvpCoin(int uid, int pvpCoin);
    //增加塔币
    static bool addTowerCoin(int uid, int towerCoin);
    //增加公会贡献
    static bool addGuildContrib(int uid, int guildContrib);
	//增加抽卡券
	static bool	addFlashCard(int uid, int flashcard);
	//增加10抽卡卷
	static bool	addFlashCard10(int uid, int flashcard10);

	//获取玩家名字(可能不在线)
	static std::string getUserName(int uid);
	//获取玩家信息(可能不在线)
	static bool getUserInfo(int uid, UserBasicInfo &info);
    //获取玩家公会信息(可能不在线)
    static bool getUserUnionInfo(int uid, UserUnionBasicInfo &info);
	//是否体力满足
	//static bool isEnergyEnough(int uid, int energy, int times = 1);
	//是否等级满足
	static bool isUserLevelEnough(int uid, int needLv);
	//添加头像
	static bool AddHeadID(int uid, int HeadID);
    //添加公会今日活跃度
    static bool addDayLiveness(int uid, int type, int count = 1);

	/////////////////////////////用户数据相关////////////////////////////////////

	//////////////////////////// 背包相关 //////////////////////////////////////
	//增加背包容量
	static bool addBagCapacity(int uid, int capacity);
	//增加英雄容量
	static bool addHeroCapacity(int uid, int capacity);
	//添加物品, 添加时会进行容量判断
	static bool addEquip(int uid,int CreateId, SEquipInfo &equipData);
	//批量添加物品
	static bool addEquips(int uid, std::vector<int>& equipConfIds, std::vector<int>& equipIds);
	//添加普通物品
	static bool addItem(int uid, int itemConfId, int count);
	//如果移除物品后, 个数<=0, 则会从背包里删除物品
	static bool removeItem(int uid, int itemConfId, int count);
	//获得背包剩余容量
	static int getBagStoreCapacity(int uid);
    //检测背包是否能存下道具
	static bool checkBagCapacity(int uid, const std::vector<DropItemInfo> &awards);
	//检测卡包是否能存下道具
	static bool checkHeroCapacity(int uid, const std::vector<DropItemInfo> &awards);
	//检查所有物品(背包, 卡包, 资源物品, 召唤师)是否能够存入
	static bool checkAllCapacity(int uid, const std::vector<DropItemInfo> &awards);

    // 从背包中添加已有的装备
    static bool addExistEquipToBag(int uid, int equipId);
    // 从背包中移除已有的装备
    static bool removeExistEquipFromBag(int uid, int equipId);

    // 获得物品类型
    static int getItemType(int itemId);

	//////////////////////////// 背包相关 //////////////////////////////////////

	////////////////////////// 英雄相关 ////////////////////////////////////////
	//英雄是否为整卡
	static bool isHeroCompletion(int uid, int heroId);
	//添加英雄碎片
	static bool addHeroFragment(int uid, int heroId, int fragment);
	//添加英雄, 返回英雄的唯一id
	static bool addHero(int uid, int heroId, int star, int lv);
	//根据英雄ID删除英雄, 暂无用
	static bool removeHero(int uid, int heroId);
	//检查列表内的英雄是否都拥有
	static bool checkHeros(int uid, std::vector<int> &heroIds);

	////////////////////////// 英雄相关 ////////////////////////////////////////

	////////////////////////// 召唤师相关 ////////////////////////////////////////
	//是否拥有召唤师
	static bool haveSummoner(int uid, int summonerId);
	//添加召唤师
	static bool addSummoner(int uid, int summonerId);

	////////////////////////// 召唤师相关 ////////////////////////////////////////

	////////////////////////// 任务相关 ////////////////////////////////////////

	////////////////////////// 任务相关 ////////////////////////////////////////

	////////////////////////// 关卡&试练相关 ////////////////////////////////////
	//是否首次挑战
	static bool isFirstChllange(int uid, int chapterId, int stageId);
	//能否挑战指定关卡
	static bool canChallengeStage(int uid, int chapterId, int stageId, int times = 1);
	//该接口与finishChapter不同的是, 本接口只改变章节状态, 不改变关节状态
	static void unlockChapter(int uid, const std::vector<int> &chapters);
	//章节是否解锁
	static bool isChapterUnlock(int uid, int chapterId);
	//检查是否可以解锁关卡, 传入现在的积分
	static bool checkChapterUnlock(int uid, int integral);

	//解锁下个关卡, stageId为当前关卡
	static void finishChapterStage(int uid, int chapterId, int stageId, int status, std::vector<int> &unlockVec);
	//完成副本副本
	static void finishInstance(int uid, int instance, int difficulty, int star);
	//获得精英副本信息
	static bool getEliteInfo(int uid, int chapterId, int stageId, DBEliteStageInfo &eliteInfo);
	//精英关卡使用次数
	static bool addEliteUseTimes(int uid, int chapterId, int stageId, int times);
    //获取章节所有星星数
    static int getStageChapterStar(int uid, int chapterId);
	//获取为3整除的关卡星星数
	static int getChapterRewardStar(int uid, int chapterId);
	//获得关卡状态
	static int getChapterStageState(int uid, int chapterId, int stageId);
    //设置金币试炼伤害和次数
    static void setGoldTestData(int uid, int damage, int count = 1);
	//标记宝箱状态
	static int markGoldTestChest(int uid, std::vector<int> &chests);

	///////////////////////// 关卡&试练相关 //////////////////////////////////////

	////////////////////////// 佣兵 /////////////////////////////////////////
	// 佣兵是否可以使用
	static bool canMercenaryUse(int uid, int mercenary);
	// 添加佣兵至使用列表
	static void addMercenaryUseList(int uid, int mercenary);

	////////////////////////// 佣兵 /////////////////////////////////////////


	//////////////////////////PVP相关//////////////////////////////////////////
	//更新pvp时间
	static void updatePvpInfo(int uid);
	//持久化战斗idkey
	static bool persistPvpBattleKey(int uid);
	//移除玩家pvp战斗idkey
	static bool deletePvpBattleKey(int uid);
	//设置战斗id
	static bool setPvpBattleId(int uid, int battleId, int robotId);
	//获取角色上一届锦标赛排名
	static bool getUserPreChampionRank(int uid, int &Rank);
	//重置pvp每日任务
	static bool resetPvpDayInfo(int uid);
	//重置锦标赛
	static bool resetPvpCPN(int uid);

    // 添加pvp宝箱(规则),成功返回宝箱id>0
    static int addPvpChest(int uid);
	// 添加指定的pvp宝箱
	static bool addPvpChest(int uid, int chestId);
	// 获得玩家pvp胜利后掉落id
	static int getWinDropId(int uid);
	// 删除第一个宝箱, 并返回宝箱id, 如果宝箱id返回0, 则没有宝箱
	static int popPvpChest(int uid);
	// pvp宝箱是否达到上限, 最大上限为5个, 固定且不配表by策划
	static bool isPvpChestFull(int uid);
	// pvp宝箱列表是否为空
	static bool isPvpChestEmpty(int uid);
    // 刷新pvp宝箱到前端
    static void refreshPvpChest(int uid);

	//////////////////////////PVP相关//////////////////////////////////////////

	///////////////////////////商店相关/////////////////////////////////////////

    static bool FreshShop(int uid, int nShopID, int usrLv, std::map<int, SShopGoods> &ShopMap);
    // 发送监听动作事件
    static void DispatchActionEvent(int uid, int actID, void *data, int len);

	///////////////////////////商店相关/////////////////////////////////////////

	/////////////////////////// 邮件相关 ////////////////////////////////////

	static bool removeExpireMail(int uid);

	static bool removeExpireGlobalMail();

	/////////////////////////// 邮件相关 ////////////////////////////////////

	//////////////////////////时间重置相关//////////////////////////////////////////

	//检查重置的玩家信息
	static void updateUserModelReset(int uid, bool bLogin = false);
	//更新活动副本信息
	static void updateInstanceInfo(int uid, int activityId);
	//更新爬塔试练信息
	static void updateTowerTestInfo(int uid);
	//更新金币试练信息
	static void updateGoldTestInfo(int uid);
	//更新英雄试练信息
	static void updateHeroTestInfo(int uid);
	//更新所有精英关卡信息
	static void updateAllEliteInfo(int uid);
	//更新指定精英关卡信息
	static void updateEliteInfo(int uid, int chapterId, int stageId);
    //更新任务信息
    static void updateTaskInfo(int uid);
    //更新个人公会信息
    static void updateUserUnionInfo(int uid, bool bRefresh = true);
	//重置商店数据
	static bool updateShopData(int uid);
	//检测商店刷新次数
	static bool checkShopFreshTimes(int uid);

	//////////////////////////时间重置相关//////////////////////////////////////////
	static bool PvpMailSend(int uid);

	static bool TowerMailSend(int uid);

	//////////////////////////充值相关//////////////////////////////////////////
	static void checkPayData(int uid);
};

#endif //__MODEL_HELPER_H__
