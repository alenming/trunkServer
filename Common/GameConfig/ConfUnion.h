#ifndef _CONF_UNION_H_
#define _CONF_UNION_H_

#include "ConfManager.h"
#include "ConfGameSetting.h"

#define MAX_MAP_INDEX 15		//一个地图最多15个序列点

/////////////////////////////////配表数据/////////////////////////////////////////
class UnionLevelItem
{
public:
    int UnionLv;            // 公会等级
    int ViceChairmanNum;    // 副会长人数
    int MemberLimit;        // 成员上限
    int ActiveMin;          // 不活跃标准
    int ActiveReward;       // 达到奖励的活跃度标准
    int RewardID;           // 奖励的活跃度奖励
    int ActiveSReward;      // 达到超级活跃度奖励的标准
    int SRewardID;          // 超级活跃度奖励
    int UpLevelCost;        // 升级所需声望
    int DownLevelCost;      // 降级所需声望
    int UnActiveReduce;     // 不满足扣除声望
};

struct UnionItem
{
    int UnLockLv;           // 解锁公会等级
    int CostCoin;           // 创建公会金币
    int AuditTime;          // 审核失效时间(单位:分)
    int ApplyCD;            // 退会再申请时间(单位:分)
    int ApplyCount;         // 申请次数
    int ChangeNameCost;     // 改名费用(钻石)
	int nExpiditionTimes;	// 远征次数
	int nRewardSendTime;	// 奖励发放时间间隔
};

struct SMapWeight
{
    int mapId;
    int weight;

    SMapWeight()
    {
        mapId = 0;
        weight = 0;
    }
};

struct SExpeditonWorldItem
{
    int nWorldID;							  //世界地图id
    int nUnLockLv;							  //解锁等级
    std::string strIcon;					  //美术标签
	int nFightTime;							  //征战时长
	int nColdTime;							  //征战休息时间
    std::string strName;					  //区域名称
    std::string strDesc;					  //区域描述
	std::vector<SMapWeight> vecMap;			  //地图

    SExpeditonWorldItem()
    {
        nWorldID = 0;
        nUnLockLv = 0;
        strIcon.clear();
        vecMap.clear();
        nColdTime = 0;
        nFightTime = 0;
        strName.clear();
        strDesc.clear();
    }
};

//关卡序列数据
struct SExpeditonIndexData
{
	int				nStageID;				  //关卡ID
	int				nStageLv;				  //关卡等级
	int				nDescID;				  //关卡描述
	int				nTitleID;				  //关卡标题
	int		        nHeadDesc;				  //关卡助战头像tips
	int             nHeadName;				  //关卡助战名
	int				nColor;					  //关卡线路颜色
	int				nBossHp;				  //关卡boss血量
	std::string     strHeadRes;				  //关卡助战半身像
	std::string     strHeadTag;				  //关卡半身像标签
	std::string		strThumbnail;			  //关卡缩略图图标
	std::string		strBackground;			  //关卡背景图图片
	std::string		strHeadIcon;			  //关卡助战头像
	VecInt			unlockIndexs;			  //关卡解锁关卡
	VecInt			StageMapBuff;			  //关卡Buff
	
    SExpeditonIndexData()
    {
        nStageID = 0;
        nStageLv = 0;
        nDescID = 0;
        nTitleID = 0;
        strThumbnail.clear();
        strBackground.clear();
        strHeadIcon.clear();
        nHeadDesc = 0;
        nHeadName = 0;
        strHeadRes.clear();
        strHeadTag.clear();
        unlockIndexs.clear();
        nColor = 0;
        StageMapBuff.clear();
        nBossHp = 0;
    }
};

struct SExpeditionMapData
{
	int nStageMapID;							//远征关卡地图ID
	int nUnlockLv;								//地图解锁等级
	int nTotal;									//共多少关
	int nMapName;								//关卡地图名字
	int nWinGoodsID;							//通关礼包奖励
	int nUnionPrestige;							//公会声望
	int nGoodsID[4];							//伤害第一, 第二, 第三, 4-10额外奖励
	VecInt vecShopGoods;						//公会商店新增物品
	VecInt vecStartStages;						//默认开启的关卡序号
    std::string strMapScence;					//关卡地图场景
    SExpeditonIndexData stageList[15];			//关卡列表信息
	
    SExpeditionMapData()
    {
        nStageMapID = 0;
        nUnlockLv = 0;
        nTotal = 0;
        nMapName = 0;
        nWinGoodsID = 0;
        memset(nGoodsID, 0, sizeof(nGoodsID));
		nUnionPrestige = 0;
    }
};

//掉落单项
struct UnionDropIdData
{
	int DropID;
	int DropRate;
	VecInt DropNum;
};

// 掉落规则
class UnionShopDropData
{
public:
	int						DropRuleID;			   //掉落规则ID
	VecInt					MeanwhileDropNum;	   //单次掉落种类数量
	std::vector<UnionDropIdData>	DropIDs;	   //掉落ID项

	UnionShopDropData& operator = (UnionShopDropData &Item)
	{
		DropRuleID = Item.DropRuleID;
		MeanwhileDropNum.assign(Item.MeanwhileDropNum.begin(), Item.MeanwhileDropNum.end());
		DropIDs.assign(Item.DropIDs.begin(), Item.DropIDs.end());
		return (*this);
	}
};


////////////////////////////////解析配表//////////////////////////////////////////
class CConfUnionLevel : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
};

class CConfUnion : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
    NO_KEY_DATA(UnionItem);
};

class CConfExpeditionWorld : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
};

class CConfExpeditionMap : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
};

class CConfUnionBadge : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
    std::map<int, std::string>& getBadges(){ return m_MapUnionBadge; }

private:
    std::map<int, std::string> m_MapUnionBadge;
};

class CConfUnionDrop : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

////////////////////////////////查询配表//////////////////////////////////////////
inline const UnionLevelItem* queryConfUnionLevel(const int& lv)
{
    CConfUnionLevel *confUnionLevel = dynamic_cast<CConfUnionLevel*>(
        CConfManager::getInstance()->getConf(CONF_UNION_LEVEL));
    return static_cast<UnionLevelItem*>(confUnionLevel->getData(lv));
}

inline const UnionItem& queryConfUnion()
{
    CConfUnion* conf = dynamic_cast<CConfUnion*>(
        CConfManager::getInstance()->getConf(CONF_UNION));
    return conf->getData();
}

inline const SExpeditonWorldItem* queryConfExpiditionWordData(int nWorldID)
{
    CConfExpeditionWorld* conf = dynamic_cast<CConfExpeditionWorld*>(
        CConfManager::getInstance()->getConf(CONF_UNIONEXPIDTION));
    return static_cast<SExpeditonWorldItem*>(conf->getData(nWorldID));
}

inline SExpeditionMapData* queryConfExpiditionMapData(int nMapID)
{
    CConfExpeditionMap* conf = dynamic_cast<CConfExpeditionMap*>(
        CConfManager::getInstance()->getConf(CONF_UNIONEXPIDTION_MAP));
    return static_cast<SExpeditionMapData*>(conf->getData(nMapID));
}

inline bool hasUnionBadge(int nBadgeID)
{
    CConfUnionBadge* conf = dynamic_cast<CConfUnionBadge*>(
        CConfManager::getInstance()->getConf(CONF_UNIONBADGE));

    std::map<int, std::string>& mapBadges = conf->getBadges();

    return mapBadges.find(nBadgeID) != mapBadges.end();
}

// 查询掉落规则配表
inline const UnionShopDropData* queryConfUnionDropProp(int UniondropId)
{
	CConfUnionDrop *conf = dynamic_cast<CConfUnionDrop*>(
		CConfManager::getInstance()->getConf(CONF_UNIONSHOPDROP));
	return static_cast<UnionShopDropData*>(conf->getData(UniondropId));
}

#endif //_CONF_UNION_H_
