#ifndef _EQUIP_COMM_H__
#define _EQUIP_COMM_H__ 

#include <string.h>

#define MAX_EQUIP_EFFECT_NUM 8			//最大效果条目


#pragma pack(1)

struct SItemBase
{
	int nType;					//类型
	int nItemID;				//物品ID
};

struct SItemInfo :public SItemBase
{
	int nCount;					//物品个数
};

struct SItemFrag : public SItemBase
{
	int nHeroId;
	int nCount;
};

struct SHeroInfo :public SItemBase
{
	int nStart;					//英雄星级
	int nCardID;				//英雄配置roleID
};

struct SEquipInfo :public SItemBase
{
	int nDnycEquipID;					           //动态装备ID
	unsigned char cMainPropNum;					   //主属性个数
	unsigned char cEffectID[MAX_EQUIP_EFFECT_NUM];
	unsigned short sEffectValue[MAX_EQUIP_EFFECT_NUM];

	SEquipInfo()
	{
		memset(this, 0, sizeof(*this));
		nType = 1;
	}
};

#pragma  pack()

#endif //_EQUIP_COMM_H__



