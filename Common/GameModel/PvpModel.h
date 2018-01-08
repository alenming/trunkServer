#ifndef __PVPMODEL_H__
#define __PVPMODEL_H__

#include <string>
#include <list>
#include <map>

#include "IDBModel.h"
#include "Storage.h"

enum PvpField
{
	PVP_FD_NONE,						//无

	PVP_FD_MMR,							//公平竞技MMR
	PVP_FD_INTEGRAL,					//公平竞技竞技积分
	PVP_FD_CONTINUSWINTIMES,			//公平竞技连续胜/败场
	PVP_FD_BATTLETIMES,					//公平竞技总战斗次数
	PVP_FD_DANCONTINUSWINTIMES,			//公平竞技段位连续胜场
	PVP_FD_TOTALWINTIMES,				//公平竞技总胜场次数
	PVP_FD_HISTORYHIGHESTRANK,			//公平竞技历史最高排名
	PVP_FD_HISTORYHIGHESTINTEGRAL,		//公平竞技历史最高积分
	PVP_FD_HISTORYCONTINUSWINTIMES,		//公平竞技历史最高连胜场数

	CPN_FD_WEEKRESETSTAMP,				//锦标赛周重置时间(星期1早上0点)
	CPN_FD_GRADINGNUM,					//锦标赛定级赛场数
	CPN_FD_GRADINGDVAL,					//锦标赛定级赛分差
	CPN_FD_MMR,							//锦标赛MMR
	CPN_FD_INTEGRAL,					//锦标赛竞技积分
	CPN_FD_CONTINUSWINTIMES,			//锦标赛连续胜/败场
	CPN_FD_BATTLETIMES,					//锦标赛竞技总战斗次数
	CPN_FD_DANCONTINUSWINTIMES,			//锦标赛段位连续胜场
	CPN_FD_TOTALWINTIMES,				//锦标赛总胜场次数
	CPN_FD_HISTORYHIGHESTRANK,			//锦标赛历史最高排名
	CPN_FD_HISTORYHIGHESTINTEGRAL,		//锦标赛历史最高积分
	CPN_FD_HISTORYCONTINUSWINTIMES,		//锦标赛历史最高连胜场数

	PVPCOMM_FD_BATTLESTAMP,				//最近一场游戏时间戳
	PVPCOMM_FD_DAYRESETSTAMP,			//日重置时间
	PVPCOMM_FD_DAYWINTIMES,				//日胜场
	PVPCOMM_FD_DAYCONTINUSWINTIMES,		//日连续胜场数
	PVPCOMM_FD_DAYMAXCONITNUSWINTIMES,	//日最高连胜场数
	PVPCOMM_FD_DAYBATTLETIMES,			//日战斗场数
	PVPCOMM_FD_REWARDFLAG,				//日奖励领取标示符
	PVPCOMM_FD_ROBOTTIMES,				//匹配机器人次数
	PVPCOMM_FD_LASTCHESTGENTIME,		//最后宝箱生成时间
	PVPCOMM_FD_CHESTSTATUS,				//宝箱状态
	PVPCOMM_FD_CHESTORDER,				//宝箱品质顺序
	PVPCOMM_FD_DAYBUYCHESTTIMES,		//日购买宝箱次数
    PVPCOMM_FD_ROBOTINDEX1,				//匹配机器人序号1(初级)
	PVPCOMM_FD_ROBOTINDEX2,				//匹配机器人序号2(普通)
	PVPCOMM_FD_ROBOTINDEX3,				//匹配机器人序号3(高级)

	PVP_FD_MAX
};

enum PvpVerifyField
{
	PVPVERIFY_FD_NONE,					//无
	PVPVERIFY_FD_BATTLEID,				//战斗id
	PVPVERIFY_FD_ROBOTID				//机器人id
};

struct PVPReport
{
	int oppUserLv;					//战时等级
	int oppHeadIcon;				//玩家头像
	int battleStamp;				//战时时间戳
	int battleResult;				//战斗结果
	int rankDV;						//排名差值
	char oppUserName[32];			//战斗玩家名字
};

class CPvpModel : public IDBModel
{
public:
	CPvpModel();
	~CPvpModel();

	int GetUid() { return m_nUid; }

public:
	//初始化
	bool init(int uid);
	//刷新pvp数据
	bool Refresh();
	//设置pvp战斗标示符, 并设置过期时间
	bool SetPvpBattleId(int battleId, int robotId);
	//获得pvpbattleId
	bool GetPvpBattleId(int &battleId);
	//获得pvp robotId
	bool GetPvpRobotId(int &robotId);
	//取消过期时间
	bool PersistPvpBattleKey();
	//删除pvp过期时间
	bool DeletePvpBattleKey();
	//设置pvp数据
	bool SetPvpField(int field, int value);
	//批量设置pvp数据
	bool SetPvpField(std::map<int, int> &mapValues);
	//获得指定pvp值
	bool GetPvpField(int field, int &value, bool bNew = false);
	//批量获取pvp数据
	bool GetPvpField(std::map<int, int> &mapValues, bool bNew = false);

protected:

	bool GetRealFieldFromDB(int field, int &value);

private:

	int						m_nUid;					//角色ID
	Storage*                m_pStorage;				//数据库
	std::string				m_strPvpKey;			//pvpkey
	std::string				m_strPvpBattleKey;		//pvp战斗idkey
	std::map<int, int>		m_MapPvpValues;			//pvp数据对应的key值
};

#endif //__PVPMODEL_H__
