#ifndef __BAG_MODEL_H__
#define __BAG_MODEL_H__

#include <map>
#include "IDBModel.h"

enum
{
	PackageCureSize,			//背包已占格子数
};

class Storage;
class CBagModel : public IDBModel
{
public:
	CBagModel();
	~CBagModel();

public:

	bool init(int uid);

	bool Refresh();

	std::map<int, int>& GetItems();

	bool GetItem(int itemid, int &val,bool bNew = false);

	bool AddItem(int itemid, int val);
	
	bool RemoveItem(int itemId);

	bool DeleteBag();

	bool ExistItem(int itemid, bool bNew = false);
    // 获取背包当前容量
    int GetCapacity();

	//增加背包容量
	int AddCapacity(int val);

protected:
	bool GetRealDataFromDB(int itemid, int &val);

private:

	int					 m_nUid;
	Storage *			 m_pStorage;
	std::map<int, int>	 m_Items;
	std::map<int, int>	 m_PackageInfo;
};

#endif //__BAG_MODEL_H__
