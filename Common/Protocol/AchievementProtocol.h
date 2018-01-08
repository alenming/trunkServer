#ifndef __ACHIEVEMENT_PROTOCOL_H__
#define __ACHIEVEMENT_PROTOCOL_H__

#pragma pack(1)

enum ACHIEVEMETNPROTOCOL
{
    CMD_ACHIEVEMENT_CSBEGIN,
    CMD_ACHIEVEMENT_FINISH_CS,             // 前端发送完成成就
    CMD_ACHIEVEMENT_GAIN_CS,               // 发送领取成就奖励
    CMD_ACHIEVEMENT_CSEND,
    
    CMD_ACHIEVEMENT_SCBEGIN = 100,
    CMD_ACHIEVEMENT_FINISH_SC,             // 前端发送完成成就回发
    CMD_ACHIEVEMENT_GAIN_SC,               // 发送领取成就奖励回发
    CMD_ACHIEVEMENT_SCEND,
};

// CMD_ACHIEVEMENT_FINISH_CS
// CMD_ACHIEVEMENT_FINISH_SC
struct AchieveFinishCSC
{
    unsigned short sachieveID;          // 成就ID
    int				extend;				// 扩展
};

// CMD_ACHIEVEMENT_GAIN_CS
struct AchieveGainCS
{
	unsigned short sachieveID;          // 成就ID
};

// CMD_ACHIEVEMENT_GAIN_SC
struct AchieveGainSC
{
	unsigned short			achieveID;          // 成就ID
    unsigned char			cCount;				// 奖励物品数量
    // awardCount ->DropItemInfo
};

#pragma pack()

#endif