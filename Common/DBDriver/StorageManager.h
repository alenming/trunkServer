/*
 * storagemanager， 基于baseframe内， 
 * 建议根据baseframe项目配置进行使用
 * 2014-02-10
 */

#ifndef _STORAGE_MANAGER_H__
#define _STORAGE_MANAGER_H__

#include <string>
#include <map>
#include "Storage.h"

// 具体model相关的storageId
enum StorageID
{
    // all of redis
	STORAGEID_NONE = 0,			   //无
	STORAGEID_SERVER,			   //服务器相关信息
	STORAGEID_USER,				   //用户数据
	STORAGEID_BAG,				   //背包数据
	STORAGEID_EQUIP,			   //装备数据
	STORAGEID_HERO,				   //英雄数据
	STORAGEID_SUMMONER,			   //召唤师数据
	STORAGEID_STAGE,			   //关卡数据
    STORAGEID_TEAM,                //队伍数据
    STORAGEID_TASK,                //任务数据
    STORAGEID_ACHIEVEMENT,         //成就数据
	STORAGEID_STORY,			   //剧情数据
	STORAGEID_PASSTEAM,			   //通关队伍
    STORAGEID_UNION,               //公会数据
    STORAGEID_MAIL,                //邮件数据
	STORAGEID_ACTIVITY,			   //活动副本
	STORAGEID_GOLDTEST,			   //金币试炼
	STORAGEID_HEROTEST,			   //英雄试炼
	STORAGEID_TOWERTEST,		   //爬塔试炼
	STORAGEID_PVP,				   //PVP
	STORAGE_SHOP,				   //商店数据
	STORAGE_STAT,				   //统计数据
	STORAGEID_GLOBALMAIL,		   //全局邮件
	STORAGE_CHECK,				   //登陆//礼包
	STORAGE_PAY,				   //充值验证
};

#define STORAGE_XML_FILE  "..//GameConfig//module_config.xml"
typedef std::map<int, Storage*>     MapStorage;

class StorageManager
{
private:
    StorageManager();
    ~StorageManager();

public:
    static StorageManager* getInstance();
    static void destroy();

public:
	//通过配置表初始化
	bool InitWithXML(std::string xmlFile);
    //获取存储数据库对象
    Storage* GetStorage(int storageId);
    //获得存储对象
    IStorer* GetStorer(int storageId, int uid);
	//通过IP
	IStorer* GetStorer(std::string ip, int port);
    //插入存储数据库对象
	bool InsertStorer(STORER_TYPE type, int storageId, std::string ip, int port, DBRule rule);
    
private:

    static StorageManager *				m_pInstance;
	MapStorage							m_mapStorage;	// <storageId, storage>
	std::map<std::string, IStorer *>	m_mapStorers;	// <ip:port, storer>
};

#endif //_STORAGE_MANAGER_H__
