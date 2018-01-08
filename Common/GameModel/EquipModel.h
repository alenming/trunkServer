#ifndef __EQUIP_MODEL_H__
#define __EQUIP_MODEL_H__

#include <string>
#include <map>
#include "IDBModel.h"
#include "EquipComm.h"

class Storage;
class CEquipModel : public IDBModel
{
public:
	CEquipModel();
	~CEquipModel();

public:

	bool init(int uid);

	bool Refresh();

	std::map<int,SEquipInfo>& GetEquipsInfo();

	bool AddEquip(SEquipInfo &EquipData);

	bool RemoveEquip(int equipId);

	SEquipInfo* GetEquipData(int equipId,bool bNew = false);

protected:
	bool GetRealEquipDataFromDB(int equipId, SEquipInfo* pEquipInfo);

private:

	int				   m_nUid;
	Storage *		   m_pStorage;
	std::string		   m_szKey;
	std::map<int, SEquipInfo> m_EquipsInfo;
};

#endif //__EQUIP_MODEL_H__
