#ifndef __SHOP_PROTOCOL_H__
#define __SHOP_PROTOCOL_H__

#pragma pack(1)

enum SHOPPROTOCOL
{
    CMD_SHOP_CSBEGIN,
    CMD_SHOP_BUY_CS,        // 购买商品
    CMD_SHOP_REFRESH_CS,    // 刷新商品
	CMD_UNIONSHOP_BUY_CS,	// 购买公会商品

    CMD_SHOP_SCBEGIN = 100,
    CMD_SHOP_BUY_SC,        // 购买商品
    CMD_SHOP_REFRESH_SC,    // 刷新商品
	CMD_UNIONSHOP_BUY_SC,	// 购买公会商品
	CMD_UNIONSHOP_FRESHSC,	// 公会商店物品时刷新
};

///////////////////////////购买协议
// CMD_SHOP_BUY_CS
struct ShopBuyCS
{
	int itemNum;				 // 商品数量
	unsigned char cShopID;		  // 商店ID
    unsigned char cIndex;         // 商品索引
};

// CMD_SHOP_BUY_SC
struct ShopBuySC
{
	int nNum;						//购买个数
	unsigned char cIndex;			//购买索引
};

///////////////////////////刷新协议
// CMD_SHOP_REFRESH_CS
struct ShopRefreshCS
{
    unsigned char cShopID;           // 商店ID
	unsigned char cShopType;		//刷新类型，0-自动，1-手动
};

// CMD_SHOP_REFRESH_SC
//商店结构
struct ShopRefreshSC
{
	int				nTimes;					//已经刷新的次数
	int				nNextFreshTime;			//下次刷新时间点
	unsigned char	cType;					//刷新类型
	unsigned char	cShopID;				//商店ID
	unsigned char   cCurCount;				//当前物品数
	//商品结构
};

//商品结构
struct SSendShopGoodsData
{
	int nGoodsID;							//道具ID
	int nGoodsNum;							//道具个数
	int nCoinNum;							//价格
	unsigned short  sGoodsShopID;			//商品ID 不超过65535
	unsigned char	cSale;					//折扣值
	unsigned char	cCoinType;				//货币类型
	unsigned char	cIndex;					//商品索引
};

///////////////////////////////////////////////////
//CMD_UNIONSHOP_BUY_CS
struct SUnionShopBuyCS
{
	unsigned short sShopGoodsID;				//购买的商品ID
	unsigned char cGoodsNum;					//购买个数
};

struct SUnionShopBuySC
{
	int nBuyerId;								//购买者UID
	unsigned short sShopGoodsID;				//购买的商品ID
	unsigned char cGoodsNum;					//购买个数
};

struct SUnionShopFreshSC
{
	short sNum;						//商品个数
	unsigned char cFlag;			//0-普通状态，1-正在上架状态，2-首次
	//... SUnionShopGoodsData
};

struct SUnionShopGoodsData
{
	int nGoodsID;							//道具ID
	int nCoinNum;							//价格
	unsigned short sGoodsShopID;			//商品ID
	unsigned char cGoodsNum;				//道具个数
	unsigned char cCoinType;				//货币类型
	unsigned char cSale;					//折扣值
};

#pragma pack()

#endif //__SHOP_PROTOCOL_H__
