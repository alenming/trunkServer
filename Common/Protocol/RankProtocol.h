#ifndef _RANK_PROTOCOL_H__
#define _RANK_PROTOCOL_H__

#pragma pack(1)

//排行榜协议
enum RankInfoProtocol
{
	CMD_RANKNFO_CSBEGIN,
	CMD_RANKNFO_UI_CS,	    //获取排行榜数据请求
	CMD_RANKNFO_CSEND,

	CMD_RANKNFO_SCBEGIN = 100,
	CMD_RANKNFO_UI_SC,			//应答排行榜数据信息
	CMD_RANKNFO_SCEND,
};

enum
{
	PVPRANK_TYPE,				//PVP
	LEVELRANK_TYPE,				//等级排行榜
	UNIONRANK_TYPE,				//公会排行榜
	TOWERRANK_TYPE,				//爬塔排行榜
	CHAMPIONRANK_TYPE,			//竞标赛排行榜
};

//请求获取排行榜数据
//CMD_RANKNFO_UI_CS
struct SRankInfoUI_CS
{
	int nType;			//排行榜类型0-PVP ,1-等级,2-公会，3-爬塔 4-竞标赛
};

//PVP排行榜数据/竞标赛
struct SRankShowPvpData
{
	int		nIndex;				//排名
	int		nHeadID;			//头像ID
	char	szName[32];			//名称
	char	szUnionName[32];	//公会名称
	int		nScore;				//竞技积分
	int		nLevel;				//玩家等级
	unsigned char cBDType;		//蓝钻类型
	unsigned char cBDLev;		//蓝钻等级
};

//玩家等级排行数据
struct SRankShowLevelData
{
	int		nIndex;				//排名
	int		nHeadID;			//头像ID
	char	szName[32];			//名称
	char	szUnionName[32];	//公会名称
	int		nLevel;				//玩家等级
	unsigned char cBDType;		//蓝钻类型
	unsigned char cBDLev;		//蓝钻等级
};

//服务器公会排行
struct SRankShowUnionData
{
	int		nIndex;				//排名
	char	szUnionName[32];	//公会名称
	int			nEmblemID;		//公会会徽
	int		nUnionNum;			//公会人数
	int		nUnionLevel;		//公会等级
};

//爬塔积分排行
struct SRankShowTowerData
{
	int		nIndex;				//排名
	int		nHeadID;			//头像ID
	char	szName[32];			//名称
	int		nMaxTowerLevel;		//最高楼层
	int		nScore;				//爬楼积分
	int		nLevel;				//玩家等级
	unsigned char cBDType;		//蓝钻类型
	unsigned char cBDLev;		//蓝钻等级
	char    szUnionName[32];	//公会名

	SRankShowTowerData()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SRankInfoUI_SC
{
	int nType;			//排行榜类型
	int nNum;			//数据个数
	int nIndex;			//个人对应的排名，如果为0，则表示不在排行榜上
	//对应类型的数据结构
};

#pragma pack()

#endif
