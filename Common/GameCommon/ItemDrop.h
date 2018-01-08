#ifndef __ITEM_DROP_H__
#define __ITEM_DROP_H__

#include <vector>
#include <map>

struct DropItemInfo
{
	int id;					//物品id
	int num;				//物品个数
};

class CardGambleItem;
class CItemDrop
{
public:
	//掉落
	static void Drop(int id, std::vector<DropItemInfo> &vecDropItem, bool isExtra = false);
	//公会商店掉落
	static void UnionShopDrop(int id, std::map<int,int> &MapDropShop);
	//抽卡 randType = 0为普通抽卡, randType = 1为高级抽卡
	static CardGambleItem * RandHeroCard(int randType);
};

#endif //__ITEM_DROP_H__
