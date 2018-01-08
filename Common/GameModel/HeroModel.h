#ifndef __HERO_MODEL_H__
#define __HERO_MODEL_H__

#include <map>
#include <string.h>
#include "IDBModel.h"
#include "Storage.h"

#define TALENT_COUNT 8

// 装备部件类型
enum EquipPartType
{
	WEAPON = 1,					// 武器
	HEADWEAR,					// 头饰
	CLOTH,						// 衣服
	SHOES,						// 鞋子
	ACCESSORY,					// 饰品
	TREASURE,					// 宝具
};

// 数据库英雄属性
struct DBHeroAttr
{
	int fragment;				//碎片
	int star;					//星级
	int level;					//等级
	int exp;					//经验
    unsigned char talent[TALENT_COUNT];    //天赋
	std::map<int, int> equipId;	//装备, 只能6个装备

	DBHeroAttr()
		:fragment(0)
		, star(0)
		, level(0)
		, exp(0)
	{
        memset(talent, 0, sizeof(talent));
		for (int i = WEAPON; i < TREASURE + 1; ++i)
		{
			equipId[i] = 0;
		}
	}

};

class CRedisStorer;
class CHeroModel : public IDBModel
{
public:
	CHeroModel();
	~CHeroModel();

public:

	bool init(int uid);
	// 获取所有英雄卡片的信息
	bool Refresh();

	// 增加英雄
	bool SetHero(int heroId, DBHeroAttr &heroAttr);
	// 批量添加英雄
	bool SetHero(std::map<int, DBHeroAttr> &heros);
	
	// 更新英雄属性, 与添加差不多, 该接口查询不到不更新
	bool UpdateHero(int heroId, DBHeroAttr &heroAttr);
	// 更新英雄碎片
	bool UpdateHeroFragment(int heroId, int frag);
	// 更新天赋
	bool UpdateHeroTalent(int heroId, char* talent, int len);
	// 更新装备
	bool UpdateHeroEquip(int heroId, int part, int equipId);
	// 更新装备
	bool UpdateHeroEquip(int heroId, std::map<int, int> &equipIds);
	// 移除英雄
	bool RemoveHero(int heroId);
	// 英雄是否存在
	bool ExsitHero(int heroId);
	// 获得英雄信息
	bool GetHero(int heroId, DBHeroAttr& heroAttr, bool fromDB = false);
	// 获得所有英雄信息
	std::map<int, DBHeroAttr>& GetHeros(bool fromDB = false);

private:
	// 将属性转成字符串
	bool AttrToString(DBHeroAttr & heroAttr, std::string &attrStr);
	// 将字符串转成属性
	bool StringToAttr(std::string attrStr, DBHeroAttr & heroAttr);
	
private:

	CRedisStorer *				m_pRedisStorer;	    // 数据库对象
	int							m_nUid;			    // uid
	std::string					m_strHeroKey;	    // redis英雄key
	std::map<int, DBHeroAttr>	m_mapHeros;		    // 所有英雄信息
};

#endif //__HERO_MODEL_H__
