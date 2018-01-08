#ifndef __TEAM_MODEL_H__
#define __TEAM_MODEL_H__

#include <map>
#include <vector>
#include "Storage.h"
#include "IDBModel.h"

#define MAX_HERO_COUNT 7

struct TeamInfo
{
    int teamType;       // 队伍类型
    int summonerId;     // 召唤师ID
	int heroId[MAX_HERO_COUNT];		// 英雄id
};

class CTeamModel : public IDBModel
{
public:
    CTeamModel();
    ~CTeamModel();

    bool init(int uid);

    bool Refresh();
    // 设置队伍
    bool SetTeamInfo(int teamType, TeamInfo &teamInfo);
    // 获取队伍
    bool GetTeamInfo(int teamType, TeamInfo& teamInfo);
    // 某个英雄是否存在队伍中,所有队伍
    bool IsExistInTeams(int heroId);
    // 删除队伍中的某个英雄,所有队伍
    void RemoveHeroFromTeams(int heroId);

    std::map<int, TeamInfo>& getAllTeamInfo(){ return m_mapTeamInfo; }

private:
    int                      m_nUID;             // 用户ID
    std::string              m_strTeamKey;       // 队伍KEY
    std::map<int, TeamInfo>  m_mapTeamInfo;      // 队伍类型,队伍信息
    Storage*                 m_pStorage;		 // 数据库
};

#endif