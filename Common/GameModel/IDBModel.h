#ifndef __IDBMODEL_H__
#define __IDBMODEL_H__

#include "GameDef.h"

enum ModelType
{
	MODELTYPE_NONE,					//无
	MODELTYPE_SERVER,				//服务器
	MODELTYPE_PASSTEAM,				//通关队伍
	MODELTYPE_USER,					//用户
	MODELTYPE_BAG,					//背包
	MODELTYPE_EQUIP,				//装备
	MODELTYPE_HERO,					//英雄
	MODELTYPE_SUMMONER,				//召唤师
	MODELTYPE_STAGE,				//关卡
	MODELTYPE_TEAM,					//队伍
	MODELTYPE_TASK,					//任务
	MODELTYPE_ACHIEVEMENT,			//成就
	MODELTYPE_GUIDE,				//新手引导
	MODELTYPE_MAIL,					//个人邮件
	MODELTYPE_INSTANCE,				//副本试练
	MODELTYPE_HEROTEST,				//英雄试练
	MODELTYPE_GOLDTEST,				//金币试练
	MODELTYPE_TOWERTEST,			//爬塔试练
	MODELTYPE_PVP,					//pvp
	MODELTYPE_SHOP,					//商店
	MODELTYPE_ACTIVE,				//活动个人数据
	MODELTYPE_HEAD,					//个人解锁头像数据
	MODELTYPE_USERUNION,			//个人公会数据
	MODELTYPE_MERCENARY,			//佣兵系统
	MODELTYPE_PVPTASK,				//pvp任务
	MODELTYPE_BDACTIVE,				//蓝钻活动
    MODELTYPE_PVPCHEST,				//pvp宝箱
};

class IDBModel
{
public:
	IDBModel();
	virtual ~IDBModel();

public:

	virtual bool init(int uid) { return true; }
	virtual bool Refresh() { return true; }
};

#endif //__IDBMODEL_H__