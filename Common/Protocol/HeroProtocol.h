#ifndef __HERO_PROTOCOL_H__
#define __HERO_PROTOCOL_H__

#pragma pack(1)

enum HEROPROTOCOL
{
	CMD_HERO_CSBEGIN,
    CMD_HERO_BUYCARD_CS,			//抽卡
	CMD_HERO_GEN_CS,				//生成英雄
    CMD_HERO_UPGRADE_CS,			//英雄升级
    CMD_HERO_UPSTAR_CS,				//英雄升星
    CMD_HERO_EQUIP_CS,				//英雄装备
	CMD_HERO_ACTIVETALENT_CS,		//装备天赋
	CMD_HERO_CSEND,

    CMD_HERO_SCBEGIN = 100,
    CMD_HERO_BUYCARD_SC,			//抽卡返回
	CMD_HERO_GEN_SC,				//生成英雄
    CMD_HERO_UPGRADE_SC,			//英雄升级返回
    CMD_HERO_UPSTAR_SC,				//英雄升星
    CMD_HERO_EQUIP_SC,				//英雄装备
	CMD_HERO_ACTIVETALENT_SC,		//激活天赋
	CMD_HERO_SCEND,
};

//HERO_BUYCARD_CS
struct HeroBuyCardCS
{
	int buyType;                // 0抽一次,1抽十次
};

struct HeroBuyCardInfo
{
	int heroId;					// 英雄id
	int heroStar;				// 英雄星级
};

//HERO_BUYCARD_SC
struct HeroBuyCardSC
{
	int cardCount;              // 返回卡片的数量
	// HeroBuyCardInfo
};

//CMD_HERO_BUYCARD_CS
struct HeroGenCS
{
	int heroId;					// 生成英雄
};

//CMD_HERO_BUYCARD_SC
struct HeroGenSC
{
	//HeroInfo					// 英雄信息
};

//HERO_UPGRDE_CS
struct HeroUpgradeCS
{
    int heroId;                 // 要升级的英雄ID
	int itemCount;              // 道具材料数量(包括技能书、经验书)
	//... array of itemID
};

//HERO_UPGRADE_SC
struct HeroUpgradeSC
{
    int ret;					// 0失败,1成功
	int addExp;					// 添加的总经验
	int multiple;				// 翻倍倍数
	int moneyType;				// 消耗的货币类型
	int money;					// 消耗的货币数值
};

//HERO_UPSTAR_CS
struct HeroUpStarCS
{
    int heroId;					// 要升级的英雄ID
};

//HERO_UPSTAR_SC
struct HeroUpStarSC
{
    int heroId;                 // 英雄唯一ID
	int newStar;                // 新星级
};

struct HeroEquipInfo
{
    int equipId;                // 装备唯一ID
    int equipPos;               // 装备位置
};

//HERO_EQUIP_CS
struct HeroEquipCS
{
	int heroId;                 // 英雄唯一ID
    int equipCnt;               // 装备数量
    // equipCnt EquipInfo
};

//HERO_EQUIP_SC
struct HeroEquipSC
{
	int heroId;                 // 装备的英雄
	int equipCnt;               // 装备个数
    // HeroEquipInfo
};

//CMD_HERO_TALENT_CS
//CMD_HERO_TALENT_SC
struct HeroTalentCSC
{
    int heroId;                 // 英雄唯一ID
    unsigned char talent[8];    // 天赋
};

#pragma pack()

#endif //__HERO_PROTOCOL_H__

