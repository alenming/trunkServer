#ifndef __CONF_STAGE_H__
#define __CONF_STAGE_H__

#include "ConfManager.h"

#define MAX_CHAPTER_BOX_INDEX 3
  
//角色信息
struct SRoleInfo     
{                    
	int						RoldID;					//ID
	int						RoldLv;					//等级
};

//NPC信息
struct SNPCInfo
{
	int						NpcId;					//npcId
	int						NpcLv;					//npc等级
	int						NpcPosX;				//npc位置x
	int						NpcPosY;				//npc位置y
};     

//章节额外信息
struct ChapterExInfo
{
	int						PrevID;					//前一章节
	int						NextID;					//后一章节
	int						FirstStageID;			//第一关卡
};

//关卡额外信息
struct StageExInfo
{
	int						NextID;					//下一个关卡
};
                    
//关卡信息
struct StageInfo
{
    VecInt					ID;						//ID+等级								
    int						Name;					//标题
    int						Desc;					//描述
	VecInt					Drop;					//掉落
	StageExInfo				ExInfo;					//额外信息
    std::string				Thumbnail;				//缩略图
	std::string				BG;						//缩略图
};

//发兵信息
struct SDispatchInfo
{
	float					Delay;					 //延迟时间
	int						DispatchLine;			 //发兵线路
	int						DispatchSoldierID;		 //发兵ID序号
	VecInt					ConditionOpenID;		 //开启条件ID
	VecInt					ConditionCloseID;		 //关闭条件ID
};

// 场景召唤物
struct SSceneCall
{
    int                     callID;     // 召唤物ID
    float                   callPosX;   // 召唤物坐标x
    float                   callPosY;   // 召唤物坐标y
};

//地图配置项
class MapConfItem
{
public:
	int						ID;						//地图ID
	std::string				Sky;					//天空层资源名称
	std::string				Map;					//地图层资源名称
    std::string				Fog;					//迷雾层资源名称
    std::vector<std::string> MoodEffect;             //地图音效
};

struct ChapterBoxData
{
	int Star;									//奖励所需星星数量
	std::vector<AwardInfo>	StarAward;			//奖励
};

//章节配置项
class ChapterConfItem
{
public:
	int						ID;						//章节ID
	int						Name;					//章节名称						
	int						Type;					//章节类型
	int						MapID;					//地图ID
	int						UnlockLevel;			//解锁等级
	int						UnlockDiamond;			//解锁需要钻石数量
	int						TotalStageNum;			//包含关卡总数
	std::vector<AwardInfo>  ChapterRewardPreview;	//预览章节奖励
	VecInt					UnlockChapters;			//解锁章节
	ChapterBoxData			Reward[3];				//三组奖励
	std::map<int, StageInfo> Stages;				//所含关卡
	ChapterExInfo			ExInfo;					//额外信息
};

//地图场景配置项
class StageSceneConfItem
{
public:
	int                     Id;                     //场景配置id
	int                     BlueHeroPos;            //蓝色方英雄位置
	int                     RedHeroPos;             //红色方英雄位置
    int                     BgMusicId;              //背景音乐
	float                   FrontScene_Size;        //前景层长度
	float                   FightScene_Size;        //战斗层长度
	float                   BgScene_Size;           //背景层长度
	float                   FarScene_Size;          //远景层长度
	std::string				FrontScene_ccs;			//前景层ccs
	std::string				FightScene_ccs;			//战斗层ccs
	std::string				BgScene_ccs;			//背景层ccs
	std::string				FarScene_ccs;			//远景层ccs
	std::vector<float>      BlueSoldierPos;         //蓝色方士兵位置
	std::vector<float>      RedSoldierPos;          //红色方士兵位置
	std::vector<float>      Walkline;               //士兵移动路线
    std::vector<std::vector<int> >   IMSControl1;    //背景音乐切换条件1
    std::vector<std::vector<int> >   IMSControl2;    //背景音乐切换条件2
};

//出兵配置项
class CDispatchItem
{
public:
    int						ID;						 //序号
    int						Group;					 //分组0代表任何分组都要加载
    int						ConditionType;			 //条件类型
    int						ConditionParam;			 //条件类型参数	
    bool					IsOpen;					 //0开启1关闭
    SDispatchInfo			DispatchInfo;			 //发兵信息
};

//关卡配置项
class StageConfItem
{
public:
	int						ID;						//关卡ID		
	int                     StageSenceID;           //场景id
	int						Boss;					//Boss
	int						Type;					//关卡类型
	int						TimeLimit;				//关卡限时
	int						WinStar1;				//两星条件类型
	int						WinStar1Param;			//两星条件参数
	int						WinStar2;				//三星条件类型
	int						WinStar2Param;			//三星条件参数
	int						FirstItemDrop;			//第一次通关掉落id
	VecInt					Monsters;				//士兵
	VecInt					Win;					//胜利条件
	VecInt					Fail;					//失败条件
	VecFloat				TypeParam;				//类型参数
	VecInt					ItemDrop;				//掉落
    std::vector<SSceneCall> SceneCall;              //场景召唤物
};

//地图解析
class CConfMap : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class PvpSceneConfItem
{
public:
	int						Dan;					//段位
	VecInt					Stage;					//段位对应场景id列表
};

//章节解析
class CConfChapter : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
	void toExInfo();
};

//关卡场景解析
class CConfStageScene : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
};

//关卡出兵配置解析
class CConfDispatch : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
    std::vector<int>& getGroup(){ return m_vecGroup; }
private:
	std::vector<int> m_vecGroup;
};

//关卡解析
class CConfStage : public CConfBase
{
public:
	~CConfStage();
	virtual bool LoadCSV(const std::string& str);
	void ToRoleInfo(std::string str, SRoleInfo &info);
	void ToNpc(const std::string &str, std::vector<SNPCInfo>& vec);
	void ToTime(const std::string& str, int type, TimeInfo& info);
	CConfDispatch *getDispatchData(const int &stageID);
private:
	std::map<int, CConfDispatch*> m_StageDispatchList;
};

class CConfPvpScene : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

// 查询章节信息
inline const ChapterConfItem * queryConfChapter(int chapterId)
{
	CConfChapter *conf = dynamic_cast<CConfChapter *>(
		CConfManager::getInstance()->getConf(CONF_CHAPTER));
	return static_cast<ChapterConfItem*>(conf->getData(chapterId));
}

// 查询关卡配表
inline const StageConfItem* queryConfStage(int stageId)
{
    CConfStage *conf = dynamic_cast<CConfStage*>(
        CConfManager::getInstance()->getConf(CONF_STAGE));
    return static_cast<StageConfItem*>(conf->getData(stageId));
}

// 查询发兵配表
inline CConfDispatch* queryConfDispatch(int stageId)
{
    CConfStage *conf = dynamic_cast<CConfStage*>(
        CConfManager::getInstance()->getConf(CONF_STAGE));
    return static_cast<CConfDispatch*>(conf->getDispatchData(stageId));
}

// 查询关卡资源配置
inline const StageSceneConfItem* queryConfStageScene(int stageSceneId)
{
    CConfStageScene *conf = dynamic_cast<CConfStageScene*>(
        CConfManager::getInstance()->getConf(CONF_STAGE_SCENE));
    return static_cast<StageSceneConfItem*>(conf->getData(stageSceneId));
}

// 查询活动副本配表
inline const StageInfo *queryStageInfoInChapter(int chapterId, int stageId)
{
    CConfChapter *conf = dynamic_cast<CConfChapter *>(
        CConfManager::getInstance()->getConf(CONF_CHAPTER));
    const ChapterConfItem * pChapterConf = static_cast<ChapterConfItem*>(conf->getData(chapterId));
    for (std::map<int, StageInfo>::const_iterator iter = pChapterConf->Stages.begin();
        iter != pChapterConf->Stages.end(); ++iter)
    {
        if ((*iter).second.ID[0] == stageId)
        {
            return &(iter->second);
        }
    }
    return NULL;
}

inline const int qureyRandomPvpStage(int dan)
{
	CConfPvpScene *conf = dynamic_cast<CConfPvpScene *>(
		CConfManager::getInstance()->getConf(CONF_PVPSCENE));
	PvpSceneConfItem *item = reinterpret_cast<PvpSceneConfItem*>(conf->getData(dan));
	if (NULL != item)
	{
		if (item->Stage.size() > 0)
		{
			int index = rand() % item->Stage.size();
			return item->Stage[index];
		}
	}
	return 0;
}

#endif
