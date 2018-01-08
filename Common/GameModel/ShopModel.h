#ifndef _SHOP_MODEL_H__
#define _SHOP_MODEL_H__

#include <map>
#include <list>

#include "IDBModel.h"
#include "Storage.h"
#include "ConfHall.h"

//次数+事件戳,field 对应商店ID

struct SShopInfo
{
	int nTimes;				//刷新次数
	int nFreshEndTime;		//下次刷新开始时间点
};

struct SShopGoods
{
	int nShopGoodsID;				//商品ID
	int nGoodsNum;					//商品物品个数

	SShopGoods()
	{
		nShopGoodsID	= 0;
		nGoodsNum = 0;
	}
};

enum
{
	enFreshStartTime = 0,						//刷新时间点
};

typedef std::map<int, SShopGoods> ShopGoodsMap;

class CShopModel : public IDBModel
{
public:
	CShopModel();
	~CShopModel();

	bool init(int uid);

	bool Refresh();

	bool SetShopNum(int ShopID,int nIndex,int ShopNum);

	bool GetShopGoodsInfo(int ShopID, int nIndex, int &nShopGoodsID, int &ShopNum);

	//添加
	bool AddShopData(int ShopID, ShopGoodsMap &GoodsMap);

	//设置
	bool SetShopInfo(int nShopID,int nTimes,int nFreshTime);

	bool GetShopInfo(int nShopID, int &nTimes, int &nFreshTime);

	ShopGoodsMap* GetShopData(int nShopID);

	std::map<int, ShopGoodsMap>& GetAllShop() { return m_MapShopGoods; }

	std::map<int, int>& GetDiamondDouble() { return m_MapDiamond; }

	std::map<int, SShopInfo>& GetShopIDInfo() { return m_MapShopIDInfo; }

	bool SetFieldValue(int nField, int nValue);

	bool GetFieldValue(int nField, int &nValue);

protected:

	bool SaveShopToDB(int ShopID);

	bool SaveShopIDToDB(int ShopID);

	bool SaveShopInfoToDB(int nField, int nValue);

private:

	std::map<int, ShopGoodsMap>				m_MapShopGoods;				//角色个人商店信息
	std::map<int,SShopInfo>					m_MapShopIDInfo;			//个人商店信息
	std::map<int, int>						m_MapShopInfo;				//商店配置信息
	std::map<int, int>						m_MapDiamond;				//钻石商店首冲信息

	int										m_nUid;						//角色ID
	std::string								m_szKey;					//商店Key
	std::string								m_szShopIDKey;				//商店对应信息Key
	std::string								m_ShopInfoKey;				//商店配置信息Key
	std::string								m_ShopDiamondKey;			//钻石商店双倍信息Key
	Storage*								m_pStorage;					//数据库存储指针
	char									m_szBuffer[1024];			//字符串缓冲区
};


#endif //_SHOP_MODEL_H__
