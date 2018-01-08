#ifndef __SUMMON_CARD_MODEL__
#define __SUMMON_CARD_MODEL__

#include <set>
#include "Storage.h"
#include "IDBModel.h"
#include <algorithm>

class CSummonModel : public IDBModel
{
public:
    CSummonModel();
    ~CSummonModel();

public:

    bool init(int uid);
	// 刷新玩家召唤师数据
	bool Refresh();
	// 是否拥有召唤师
	bool HaveSummoner(int cardId,bool bNew = false);
    // 添加一张召唤师卡片
    bool AddSummon(const int& cardId);
    // 获取所有的召唤师卡片信息
	std::vector<int> &GetAllSummon();
	// 删除玩家召唤师数据
    bool DeleteSummon();

private:
    int                 m_nUID;
    std::string         m_strSummonKey;
    std::vector<int>	m_VectAllSummon; // 召唤师列表
    Storage             *m_pStorage;    // 数据库
};

#endif