#ifndef _UNION_SHOP_MODEL__
#define _UNION_SHOP_MODEL__

#include <map>
#include <string>
#include "IDBModel.h"
#include "Storage.h"
#include "RedisStorer.h"

class CUnionShopModel : public IDBModel
{
public:
	CUnionShopModel();
	~CUnionShopModel();

	bool init(int unionID);
	// 刷新公会数据
	bool Refresh();
	//设置商店物品个数
	bool setUnionShopGoodsData(int nShopGoodsID, int &nGoodsNum);
	//获取商店物品个数
	bool getUnionShopGoodsData(int nShopGoodsID, int &nGoodsNum);
	//增加商店物品个数
	bool incrUnionShopGoodsNum(int nShopGoodsID, int nAddNum);
	//清空商品列表
	bool clearUnionShop();
	// 批量处理
	bool setUnionShop(std::map<int, int> &kvs);
	//获取商店商品列表
	std::map<int, int> &getUnionShopMap() { return m_UnionShopGoods; }

private:
	Storage*								m_pStorage;			// 数据库
	std::string								m_UnionShopKey;		// 公会商店Key
	std::map<int, int>						m_UnionShopGoods;	// 商店物品列表,公会商品物品唯一

};


#endif //_UNION_SHOP_MODEL__
