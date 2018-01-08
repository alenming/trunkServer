#ifndef __PASSTEAMMODEL_H__
#define __PASSTEAMMODEL_H__

#include <map>
#include <string>
#include "IDBModel.h"

enum EPassTeamType
{
	PASSTEAM_FIRST = 0,
	PASSTEAM_TEAM1,
	PASSTEAM_TEAM2,
	PASSTEAM_TEAM3,
};

struct PassTeam
{
	int			stamp;
	int			summonerId;
	int			userLv;
	int			heroCount;
	std::string usrname;
};

struct PassTeamHero
{
	int			confId;
	int			star;
	int			level;
	int			talent;
	int			equipCount;
};

struct passTeamEquip
{
	int			confId;
};

struct PassTeamData
{
	char *data;
	int len;
};

class Storage;
class CPassTeamModel : public IDBModel
{
public:
	CPassTeamModel();
	~CPassTeamModel();

public:
	//初始化
	bool init();
	//刷新最新信息
	bool Refresh();
	// 设置通关队伍, 数据在调用时打包, 写入时会在数据头部压入队伍类型teamtype
	bool SetPassTeam(int stageId, int teamType, void *data, int len);
	// 获得通关队伍, passTeamData头4个字节为队伍类型teamType
	void GetPassTeam(int stageId, int teamType, PassTeamData &teamData);
	// 获得指定关卡的所有类型的通关队伍
	void GetPassTeam(int stageId, std::map<int, PassTeamData>& passTeamMap);

private:
	Storage *									  m_pStorage;
	std::string									  m_strPassTeamKey;
	std::map<int, std::map<int, PassTeamData> >   m_mapPassTeam;
};

#endif 
