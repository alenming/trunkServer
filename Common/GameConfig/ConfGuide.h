#ifndef __CONF_GUIDE_H__
#define __CONF_GUIDE_H__

#include "ConfManager.h"

enum CameraMoveType
{
    CameraMovePosition,
    CameraMoveMySummoner,
    CameraMoveEnemySummoner,
};

//镜头
class CameraConfItem
{
public:
	int									ID;						//镜头ID	
	int									MoveX;				    //镜头横移距离
    int                                 MoveType;               //0坐标移动，1移动到己方召唤师，2移动到敌方召唤师
	int									MoveTime;				//镜头横移时间
	int									Scale;					//拉伸比例
	int									ScaleTime;				//拉伸时间
	int									Time;					//镜头动作总时间
	int									NextCamera;				//后续镜头ID
};

//引导条件
struct GuideCondition
{
	int									Type;					//类型
	VecInt								Param;					//参数
};

//引导项
class GuideConfItem
{
public:
	int									ID;						//引导ID
	int									Listen;					//监听消息
	VecInt								Nexts;					//下个引导
    VecInt                              Closes;                 //要关闭的引导
	std::vector<GuideCondition>			StartCondition;			//开始条件
	std::vector<GuideCondition>			SkipCondition;			//跳过条件
};

//引导步骤项
class GuideStepConfItem
{
public:
	int									GuideID;				//引导ID
	int									StepID;					//步骤ID
	int									ButtonID;				//按钮ID
	int									CameraID;				//镜头ID
	int									IsLock;					//是否锁屏
	int									IsPause;				//是否暂停
	int									IsHideUI;				//是否隐藏UI
	int									TipsContent;			//提示内容
	int									DialogContent;			//对话框内容
	int									HeadName;				//角色名称
	int									EffectType;				//特效类型
	int									EffectTime;				//特效时间
	int									EffectParam;			//特效参数
	int									TotalTime;				//持续时间
    int                                 EndType;                //结束类型
    int                                 Anchor;                 //统计锚点
	VecInt								HighlightPos;			//高亮位置
	VecInt								TipsPos;				//提示位置
	VecInt								DialogPos;				//对话框位置
	VecInt								ShowButton;				//开启按钮
	GuideCondition						EndCondition;			//结束条件
    std::string							HighlightRes;			//高亮资源
    std::string							HighlightAni;			//高亮动画标签
    std::string							TipsRes;				//提示框资源
    std::string							TipsAni;				//提示框动画标签
	std::string							DialogRes;				//对话框资源
    std::string							DialogAni;				//对话框动画标签
	std::string							HeadRes;				//头像资源
    std::string							HeadTag;				//头像标签
    std::string                         ShowCSB;				//开启动画
	std::string							ShowTag;				//开启动画标签
	std::string							BgRes;					//背景资源
	std::string							BgTag;					//播放标签
};

//UI节点项
class UINodeConfItem
{
public:
	int									NodeID;					//节点ID
	int									UIID;					//界面ID
	std::string							NodePath;				//节点路径
};

//UI状态项
class UIStatusConfItem
{
public:
	int									UIID;					//界面ID
	int									ButtonLockCount;		//按钮数量
	int									NodeID;					//播放动画
	std::string							CSB;					//播放动作
};

struct GuideSoliderInfo
{
    int                                 SoliderId;              //士兵ID
    int                                 SoliderLevel;           //士兵等级
    int                                 SoliderStar;            //士兵星级
};

// 战斗配置
class GuideBattleConfItem
{
public:
    int                                 StageId;
    int                                 HeroId;
    int                                 HeroLv;
    std::vector<GuideSoliderInfo>       Soliders;
};

//镜头解析
class CConfCamera : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

//引导解析
class CConfGuide : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

//引导步骤解析
class CConfGuideStep : public CConfBase
{
public:
    virtual ~CConfGuideStep();
	virtual bool LoadCSV(const std::string& str);

	GuideStepConfItem* getStep(int guideID, int stepID)
	{
		std::map<int, std::map<int, GuideStepConfItem*> >::iterator iter = m_mapGuideSteps.find(guideID);
		if (iter != m_mapGuideSteps.end())
		{
			std::map<int, GuideStepConfItem*>::iterator iterItem = iter->second.find(stepID);
			if (iterItem != iter->second.end())
			{
				return iterItem->second;
			}
		}
		return NULL;
	}

	std::map<int, GuideStepConfItem*>* getSteps(int guideID)
	{
		std::map<int, std::map<int, GuideStepConfItem*> >::iterator iter = m_mapGuideSteps.find(guideID);
		if (iter != m_mapGuideSteps.end())
		{
			return &(iter->second);
		}
		return NULL;
	}
private:
	std::map<int, std::map<int, GuideStepConfItem*> > m_mapGuideSteps;
};

//引导UI节点解析
class CConfUINode : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

//引导UI状态解析
class CConfUIStatus : public CConfBase
{
public:
    virtual ~CConfUIStatus();
	virtual bool LoadCSV(const std::string& str);

	UIStatusConfItem* getItem(int ui, int count)
	{
		std::map<int, std::map<int, UIStatusConfItem*> >::iterator iter = m_mapUIStatusItems.find(ui);
		if (iter != m_mapUIStatusItems.end())
		{
			std::map<int, UIStatusConfItem*>::iterator iterItem = iter->second.find(count);
			if (iterItem != iter->second.end())
			{
				return iterItem->second;
			}
		}
		return NULL;
	}

	std::map<int, std::map<int, UIStatusConfItem*> >& getItems()
	{
		return m_mapUIStatusItems;
	}

private:
	std::map<int, std::map<int, UIStatusConfItem*> > m_mapUIStatusItems;
};

class CConfGuideBattle : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
    const GuideBattleConfItem* getConfItem() { return &m_confItem; }

private:
    GuideBattleConfItem m_confItem;
};

//查询引导步骤
inline const GuideStepConfItem* queryConfGuideStep(int guideID, int stepID)
{
	CConfGuideStep* conf = dynamic_cast<CConfGuideStep*>(CConfManager::getInstance()->getConf(CONF_GUIDE_STEP));
	return static_cast<GuideStepConfItem*>(conf->getStep(guideID, stepID));
}

//查询UI状态
inline const UIStatusConfItem* queryConfUIStatus(int uiID, int btnCount)
{
	CConfUIStatus* conf = dynamic_cast<CConfUIStatus*>(CConfManager::getInstance()->getConf(CONF_UI_STATUS));
	return static_cast<UIStatusConfItem*>(conf->getItem(uiID, btnCount));
}

#endif