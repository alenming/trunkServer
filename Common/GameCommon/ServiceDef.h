#ifndef __SERVICE_DEF_H__
#define __SERVICE_DEF_H__

#include "KxPlatform.h"
#include <vector>
#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
#include <ctime>
#else
#include<time.h>
#include<stdlib.h>
#endif

// 计算区间概率,返回第几区间(0开始)
inline int randRateIndex(const std::vector<int> &vec)
{
    int nRegion = -1;
    if (!vec.empty())
    {
        int nTotal = 0;
        // [a, b, c...]->[a, a+b, a+b+c,...]
        std::vector<int> vecPro;
        std::vector<int>::const_iterator iter = vec.begin();
        for (; iter != vec.end(); ++iter)
        {
            nTotal += *iter;
            vecPro.push_back(nTotal);
        }

        if (nTotal > 0)
        {
            int nRand = rand() % nTotal;

            for (nRegion = 0; nRegion < int(vecPro.size()); ++nRegion)
            {
                if (nRand < vecPro[nRegion])
                {
                    break;
                }
            }
        }
    }

    return nRegion;
}

// 返回到星期几多少时的时间戳, 星期0则是下一次多少时的时间戳
inline int calcWDayTimeStamp(int hour, int wday = 0)
{
    if (wday < 0 || hour < 0)
    {
        return 0;
    }

    time_t cur = time(NULL);
    tm curTm;
    
    #if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
    gmtime_s(&curTm, &cur);
		#else
    gmtime_r(&cur,&curTm);
    #endif

    if (wday > 7)
        wday %= 7;

    if (hour > 24)
        hour %= 24;

    int nVal = (int)cur;
    int nHour = (hour - (curTm.tm_hour + 8) % 24) * 3600 - (curTm.tm_min * 60 + curTm.tm_sec);
    if (0 == wday)
    {
        nVal += (nHour > 0 ? nHour : nHour + 24 * 3600);
    }
    else if (wday >= curTm.tm_wday)
    {
        int nday = (wday - curTm.tm_wday) * 24 * 3600 + nHour;
        nVal += (nday < 0 ? nday + 7 * 24 * 3600 : nday);
    }
    else
    {
        nVal += (wday - curTm.tm_wday + 7) * 24 * 3600 + nHour;
    }

    return nVal;
}

enum ESkillIndex
{
    SKILL_0,                              // 
    SKILL_1,                              // 技能1
    SKILL_2,                              // 技能2
    SKILL_3,                              // 技能3
    SKILL_4,                              // 技能4
    SKILL_5,                              // 技能5
    SKILL_6,                              // 技能6
    SKILL_7,                              // 技能7
    SKILL_8,                              // 技能8
};

enum EItemType
{
    ITEM_NON,                             // 
    ITEM_EQUIP,                           // 装备
    ITEM_EQUIPMATERIAL,                   // 装备材料
    ITEM_HERO,                            // 英雄卡
    ITEM_SUMMONER,                        // 召唤师
    ITEM_EXPCARD,                         // 经验卡
    ITEM_SKILLBOOK,                       // 技能书
    ITEM_GOLDBAG,                         // 金币袋子
    ITEM_ENERGYGBA,                       // 体力袋子
    ITEM_EXPBAG,                          // 经验袋子
    ITEM_DIAMONDBAG,                      // 钻石袋子
    ITEM_BOX,                             // 宝箱
    ITEM_UPSTARITEM,                      // 升星道具

    ITEM_CONSUMABLE,                      // 消耗品
};

enum EItemEffectType
{
    ITEM_EFFECT_NON,                      // 
    ITEM_EFFECT_ADDGOLD,                  // 加金币
    ITEM_EFFECT_ADDRANDGOLD,              // 随机加金币
    ITEM_EFFECT_ADDEXP,                   // 加经验
    ITEM_EFFECT_ADDENERGY,                // 加体力
    ITEM_EFFECT_ADDDIAMOND,               // 加钻石
    ITEM_EFFECT_ADDITEM,                  // 加道具
    ITEM_EFFECT_ADDHEROCARD,              // 添加召唤师卡片,注:英雄升星时为消耗的道具
    ITEM_EFFECT_ADDSOLDIERCARD,           // 添加英雄卡片,注:英雄升星时为消耗的道具
    ITEM_EFFECT_ADDEXPTOCARD = 100,       // 给卡片加经验
    ITEM_EFFECT_SKILLLVUP,                // 技能升级
};

enum EItemUseType
{
    ITEM_USE_NON,                         // 不可使用
    ITEM_USE_TOBAG,                       // 背包使用
    ITEM_USE_IMMEDIDATE,                  // 立即使用
    ITEM_USE_TOCARD,                      // 卡片吞噬使用
    ITEM_USE_TOEQUIP,                     // 装备合成使用
};

enum EExpMultiple
{
    EXP_MULTIPLE_NORMAL,                   // 普通成功(1倍)
    EXP_MULTIPLE_SUCCESS,                  // 成功(1.2倍)
    EXP_MULTIPLE_BIGSUCCESS,               // 大成功(1.5倍)
    EXP_MULTIPLE_HUGESUCCESS,              // 超大成功(2倍)
};

enum ELock
{
    EL_UNLOCK,                             // 解锁
    EL_LOCK,                               // 上锁
};

enum ECostType
{
    COST_FREE,                             // 免费
    COST_OUTLAY,                           // 支出
};

// 配置表参数个数
enum EParamCount
{
    EPARAM_COUNT_NON,          // 无参数
    EPARAM_COUNT_ONE,          // 1个参数
    EPARAM_COUNT_TWO,          // 2个参数
    EPARAM_COUNT_THREE,        // 3个参数
};

// 任务显示(包括任务、成就)
enum ETaskDisplayType
{
    TASK_HIDE,                      // 隐藏
    TASK_DISPLAY,                   // 显示
};

// 公会职位
enum EUnionPosition
{
    UNION_POS_NON = -1,
    UNION_POS_NORMAL,                // 普通会员
    UNION_POS_VICE_CHAIRMAN,         // 副会长
    UNION_POS_CHAIRMAN,              // 会长
};

// 公会功能类型
enum EUnionFuncType
{
    UNION_FUNC_JOIN,                  // 加入
    UNION_FUNC_EXIT,                  // 退出
    UNION_FUNC_KICK,                  // 踢出
    UNION_FUNC_APPOINT,               // 任命
    UNION_FUNC_ASSIGN,                // 分配物品
    UNION_FUNC_TRANSFER,              // 权利移交
    UNION_FUNC_RELIEVE,               // 撤任
    UNION_FUNC_RESIGN,                // 辞职
    UNION_FUNC_DISMISS,               // 解散
};

// 公会相关错误码
enum EUnionErrorCode
{
    UNION_EC_FAILD,
    UNION_EC_SUCCESS,
    UNION_EC_AUDIT_OVERTIME,           // 审核时间已过
    UNION_EC_OWN_UNION,                // 已经拥有公会
    UNION_EC_NOENOUGH_LV,              // 等级不够
    UNION_EC_NOENOUGH_MONEY,           // 费用不够
    UNION_EC_MEMBER_FULL,              // 公会满员
    UNION_EC_APPLY_TIMESTAMP,          // 申请(创建)公会冷却时间没到
    UNION_EC_NAME_LEGAL,               // 公会名不合法(为空)
    UNION_EC_NAME_REPEAT,              // 公会名重复   
    UNION_EC_UNION_NOEXIST,            // 公会不存在
    UNION_EC_APPLY_SAME,               // 重复申请公会
    UNION_EC_NOENOUGH_APPLYCOUNT,      // 申请次数不足
    UNION_EC_NO_AUDIT_POWER,           // 无审核权限
    UNION_EC_AUTO_AUDIT,               // 自动通过审核
    UNION_EC_PASS_AUDIT,               // 通过审核
    UNION_EC_REFUSE_AUDIT,             // 拒绝通过审核
    UNION_EC_VICECHAIR_FULL,           // 副会长上限
    UNION_EC_ALREADY_DO,               // 已经被操作
};

// 审核信息,是否同意申请
enum EUnionAuditApply
{
    UNION_APPLY_REFUSE,                // 拒绝
    UNION_APPLY_AGREE,                 // 同意
};

// 搜索公会类型
enum ESearchUnionType
{
    SEARCH_UNION_RANK,                 // 排名公会(等级>会员数量>ID)
    SEARCH_UNION_NO_FULL,              // 未满员公会 
};

// 公会福利类型
enum EUnionWelfareType
{
    UNION_WELFARE_ACTIVEBOX,         // 活跃宝箱
    UNION_WELFARE_SUPTERACTIVEBOX,   // 超级活跃宝箱
};

// 在线状态类型
enum EUserLineStatus
{
    STATUS_OFFLINE,                    // 离线
    STATUS_ONLINE,                     // 在线
};

struct ConciseHeroInfo
{
    int heroDynID;                     // 英雄ID(唯一)
    int heroConfID;                    // 配置表的ID
    int heroLv;                        // 等级
    int heroStarLv;                    // 星级
};

struct EquipInfo
{
    int equipID;                       // 装备唯一ID
    int equipConfID;                   // 装备配置ID
};

struct ItemInfo
{
    int itemID;                        // 配置表ID
    int itemCount;                     // 道具数量
};

#endif