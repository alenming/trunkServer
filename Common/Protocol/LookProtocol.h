#ifndef __LOOK_PROTOCOL_H__
#define __LOOK_PROTOCOL_H__

#pragma pack(1)

enum LOOKPROTOCOL
{
	CMD_LOOK_CSBEGIN,
    CMD_LOOK_HERO_CS,             // 请求查看英雄信息
    CMD_LOOK_EQUIP_CS,            // 请求查看装备信息
	CMD_LOOK_CSEND,

    CMD_LOOK_SCBEGIN = 100,
    CMD_LOOK_HERO_SC,             // 返回查看英雄信息
    CMD_LOOK_EQUIP_SC,            // 返回查看装备信息
	CMD_LOOK_SCEND,
};

//CMD_LOOK_HERO_CS
struct LookHeroCS
{
    int uid;                     // 玩家id
    int heroId;                  // 英雄id
};

//CMD_LOOK_HERO_SC
struct LookHeroSC
{
    int heroId;                   // 英雄id
    int heroExp;                  // 经验
    unsigned char heroLv;         // 等级
    unsigned char heroStar;       // 星级
    unsigned char equipCount;     // 装备个数
    unsigned char heroTalent[8];  // 天赋
    // equipCount EquipItemInfo
};

//CMD_LOOK_EQUIP_CS
struct LookEquipCS
{
    int uid;                      // 玩家id
    int equipDynId;               // 装备动态id
};

//CMD_LOOK_EQUIP_SC
struct LookEquipSC
{
    int uid;                      // 玩家id
    //EquipItemInfo
};

#pragma pack()

#endif //__LOOK_PROTOCOL_H__
