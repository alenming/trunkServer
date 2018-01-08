#ifndef __BAG_PROTOCOL_H__
#define __BAG_PROTOCOL_H__

#pragma pack(1)

enum BAGPROTOCOL
{
	CMD_BAG_CSBEGIN,
	CMD_BAG_SALE_CS,					//背包出售物品
	CMD_BAG_UNLOCK_CS,					//背包解锁
	CMD_BAG_USE_CS,						//背包使用
	CMD_BAG_MAKEEQUIPMENT_CS,			//装备打造
	CMD_BAG_DECOMPOSEEQUIPMENT_CS,		//装备分解
	CMD_BAG_CSEND,

	CMD_BAG_SCBEGIN = 100,
	CMD_BAG_SALE_SC,					//背包出售物品
	CMD_BAG_UNLOCK_SC,					//背包解锁
	CMD_BAG_USE_SC,						//背包使用
	CMD_BAD_ITEM_ADD_SC,				//背包物品添加
	CMD_BAG_EQUIP_MAKE_SC,				//装备打造
	CMD_BAG_EQUIP_DECOMPOSE_SC,			//装备分解
	CMD_BAG_SCEND,
};

//CMD_BAG_SALE_CS
struct BagSaleCS
{
	int saleId;			  //出售id
	int saleCount;		  //出售个数, 装备永远是1个
};

//CMD_BAG_UNLOCK_CS
// 失败出售丢弃命令不返回信息
struct BagSaleSC
{
	int saleId;			  //出售的id
	int saleCount;		  //出售个数
	int gold;			  //获得金币
};

//CMD_BAG_UNLOCK_CS
struct BagUnlockCS
{
	int bagLine;		  //解锁几行
};

//CMD_BAG_UNLOCK_SC
struct BagUnlockSC
{
	int diamond;		  //消耗钻石
	int bagLine;		  //解锁几行
};

//CMD_BAG_USE_CS
struct BagUseCS
{
	int id;				  //物品id
	int val;			  //物品数量或装备配置id
};

//CMD_BAG_USE_SC
//金币, 钻石, 经验, 体力, 召唤师 
struct BagUseSC
{
	int count;			  //打开个数
	//count * DropItemInfo
};

// CMD_BAG_MAKEEQUIPMENT_CS
struct MakeEquipmentCS
{
    int row;
    int eqLevel;
    int eqPart;
    int eqVocation;
    bool isDelicate;
};

struct DecomposeEquimentCS
{
    int equipCount;
    // EquipInfo*
};

struct ItemDataInfo
{
	int nCount;							//装备总类
    //装备类 SEquipInfo/非装备类 SHeroInfo/SItemInfo/SItemBase
};

#pragma pack()

#endif //__BAG_PROTOCOL_H__
