#ifndef __PVPCHEST_PROTOCOL_H__
#define __PVPCHEST_PROTOCOL_H__

#pragma pack(1)

enum PvpChestProtocol
{
	CMD_PVPCHEST_CSBEGIN,
	CMD_PVPCHEST_REFRESH_CS,		//刷新宝箱
	CMD_PVPCHEST_BUY_CS,            //购买宝箱
	CMD_PVPCHEST_OPEN_CS,           //打开宝箱
	CMD_PVPCHEST_OPENATONCE_CS,     //立即打开宝箱
	CMD_PVPCHEST_CSEND,
        
    CMD_PVPCHEST_SCBEGIN = 100,
    CMD_PVPCHEST_REFRESH_SC,		//下发刷新宝箱结果
    CMD_PVPCHEST_BUY_SC,            //下发购买宝箱结果
    CMD_PVPCHEST_OPEN_SC,           //下发打开宝箱结果
    CMD_PVPCHEST_OPENATONCE_SC,     //下发立即打开宝箱结果
    CMD_PVPCHEST_SCEND,
};

//CMD_PVPCHEST_REFRESH_CS
//CMD_PVPCHEST_REFRESH_SC
struct PvpChestRefreshSC
{
    char chestCount;            //宝箱个数
    // int[] chestId            //宝箱列表
};

//CMD_PVPCHEST_BUY_CS
//CMD_PVPCHEST_BUY_SC
struct PvpChestBuySC
{
    int chestId;                //宝箱id >0
    int costDiamond;            //扣除钻石
};

//CMD_PVPCHEST_OPEN_CS
struct PvpChestOpenCS
{
    int chestId;                //宝箱id
};

//CMD_PVPCHEST_OPEN_SC
struct PvpChestOpenSC
{
    int chestId;                //宝箱id
    int rewardCount;		    //奖励个数
    //...DropItemInfo
};

//CMD_PVPCHEST_OPENATONCE_CS
//CMD_PVPCHEST_OPENATONCE_SC

#pragma pack()

#endif 
