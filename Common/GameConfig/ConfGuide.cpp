#include "ConfGuide.h"

bool CConfCamera::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

	//如果有数据 
	while (csvLoader.NextLine())
	{
		CameraConfItem* camera = new CameraConfItem;
		camera->ID = csvLoader.NextInt();
        camera->MoveX = csvLoader.NextInt();
        camera->MoveType = csvLoader.NextInt();
		camera->MoveTime = csvLoader.NextInt();
		camera->Scale = csvLoader.NextInt();
		camera->ScaleTime = csvLoader.NextInt();
		camera->Time = csvLoader.NextInt();
		camera->NextCamera = csvLoader.NextInt();
		m_Datas[camera->ID] = camera;
	}
	return true;
}

bool CConfGuide::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

	//如果有数据
	while (csvLoader.NextLine())
	{
		GuideConfItem* item = new GuideConfItem;
		item->ID = csvLoader.NextInt();
		item->Listen = csvLoader.NextInt();
		for (int i = 0; i < 4; i++)
		{
			GuideCondition condition;
			condition.Type = csvLoader.NextInt();
			CConfAnalytic::ToJsonInt(csvLoader.NextStr(), condition.Param);
			if (condition.Type > 0)
			{
				item->StartCondition.push_back(condition);
			}
		}
		for (int i = 0; i < 2; i++)
		{
			GuideCondition condition;
			condition.Type = csvLoader.NextInt();
			CConfAnalytic::ToJsonInt(csvLoader.NextStr(), condition.Param);
			if (condition.Type > 0)
			{
				item->SkipCondition.push_back(condition);
			}
		}
        CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->Nexts);
        CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->Closes);
		m_Datas[item->ID] = item;
	}
	return true;
}

CConfGuideStep::~CConfGuideStep()
{
    std::map<int, std::map<int, GuideStepConfItem*> >::iterator iter = m_mapGuideSteps.begin();
    for (; iter != m_mapGuideSteps.end(); ++iter)
    {
        deleteAndClearMap(iter->second);
    }

    m_mapGuideSteps.clear();
}

bool CConfGuideStep::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

	//如果有数据
	while (csvLoader.NextLine())
	{
		GuideStepConfItem* item = new GuideStepConfItem;
		item->GuideID = csvLoader.NextInt();
		item->StepID = csvLoader.NextInt();
        item->EndType = csvLoader.NextInt();
		item->EndCondition.Type = csvLoader.NextInt();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->EndCondition.Param);
		item->ButtonID = csvLoader.NextInt();
		item->IsLock = csvLoader.NextInt();
        item->HighlightRes = csvLoader.NextStr();
        item->HighlightAni = csvLoader.NextStr();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->HighlightPos);
        item->TipsRes = csvLoader.NextStr();
        item->TipsAni = csvLoader.NextStr();
		item->TipsContent = csvLoader.NextInt();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->TipsPos);
		item->DialogRes = csvLoader.NextStr();
        item->DialogAni = csvLoader.NextStr();
		item->DialogContent = csvLoader.NextInt();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->DialogPos);
		item->HeadName = csvLoader.NextInt();
		item->HeadRes = csvLoader.NextStr();
		item->HeadTag = csvLoader.NextStr();
		CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->ShowButton);
        item->ShowCSB = csvLoader.NextStr();
		item->ShowTag = csvLoader.NextStr();
		item->CameraID = csvLoader.NextInt();
		item->IsPause = csvLoader.NextInt();
		item->BgRes = csvLoader.NextStr();
		item->BgTag = csvLoader.NextStr();
		item->IsHideUI = csvLoader.NextInt();
		item->EffectType = csvLoader.NextInt();
		item->EffectTime = csvLoader.NextInt();
		item->EffectParam = csvLoader.NextInt();
		item->TotalTime = csvLoader.NextInt();
        item->Anchor = csvLoader.NextInt();
		m_mapGuideSteps[item->GuideID][item->StepID] = item;
	}
	return true;
}

bool CConfUINode::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

	//如果有数据
	while (csvLoader.NextLine())
	{
		UINodeConfItem* item = new UINodeConfItem;
		item->NodeID = csvLoader.NextInt();
		item->UIID = csvLoader.NextInt();
		item->NodePath = csvLoader.NextStr();
		m_Datas[item->NodeID] = item;
	}
	return true;
}

CConfUIStatus::~CConfUIStatus()
{
    std::map<int, std::map<int, UIStatusConfItem*> >::iterator iter = m_mapUIStatusItems.begin();
    for (; iter != m_mapUIStatusItems.end(); ++iter)
    {
        deleteAndClearMap(iter->second);
    }

    m_mapUIStatusItems.clear();
}

bool CConfUIStatus::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
	CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

	//如果有数据
	while (csvLoader.NextLine())
	{
		UIStatusConfItem* item = new UIStatusConfItem;
		item->UIID = csvLoader.NextInt();
		item->ButtonLockCount = csvLoader.NextInt();
		item->NodeID = csvLoader.NextInt();
		item->CSB = csvLoader.NextStr();
        m_mapUIStatusItems[item->UIID][item->ButtonLockCount] = item;
	}
	return true;
}

bool CConfGuideBattle::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

    if (csvLoader.NextLine())
    {
        m_confItem.StageId = csvLoader.NextInt();
        csvLoader.NextStr();
        m_confItem.HeroId = csvLoader.NextInt();
        m_confItem.HeroLv = csvLoader.NextInt();
        for (int i = 0; i < 7; ++i)
        {
            GuideSoliderInfo soldierInfo;
            soldierInfo.SoliderId = csvLoader.NextInt();
            soldierInfo.SoliderLevel = csvLoader.NextInt();
            soldierInfo.SoliderStar = csvLoader.NextInt();
            if (soldierInfo.SoliderId > 0)
            {
                m_confItem.Soliders.push_back(soldierInfo);
            }
        }
    }

    return true;
}
