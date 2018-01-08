#include "ConfStage.h"

bool CConfMap::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    if (!loadCsv(csvLoader, str.c_str(), 3))
    {
        LOG("CConfMap Load CSV Faile %s", str.c_str());
        return false;
    }

    //如果有数据 
    while (csvLoader.NextLine())
    {
        MapConfItem* item = new MapConfItem;
        item->ID = csvLoader.NextInt();
        item->Sky = csvLoader.NextStr();
        item->Map = csvLoader.NextStr();
        item->Fog = csvLoader.NextStr();
        CConfAnalytic::ToJsonStr(csvLoader.NextStr(), item->MoodEffect);
        m_Datas[item->ID] = item;
    }

    return true;
}

bool CConfChapter::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
    if (!loadCsv(loader, str.c_str(), 3))
    {
        LOG("CConfChapter Load CSV Faile %s", str.c_str());
        return false;
    }

	//如果有数据 
	while (loader.NextLine())
	{
		ChapterConfItem* item = new ChapterConfItem;
		item->ID = loader.NextInt();
		loader.NextStr();
		item->Name = loader.NextInt();
		item->Type = loader.NextInt();
		item->MapID = loader.NextInt();
		item->UnlockLevel = loader.NextInt();
		item->UnlockDiamond = loader.NextInt();
		item->TotalStageNum = loader.NextInt();
		CConfAnalytic::ToAward(loader.NextStr(), item->ChapterRewardPreview);

		for (int i = 0; i < MAX_CHAPTER_BOX_INDEX; i++)
		{
			item->Reward[i].Star = loader.NextInt();
			CConfAnalytic::ToAward(loader.NextStr(), item->Reward[i].StarAward);
		}

		for (int i = 0; i < item->TotalStageNum; i++)
		{
			StageInfo stage;
			CConfAnalytic::ToJsonInt(loader.NextStr(), stage.ID);
			stage.Desc = loader.NextInt();
			stage.Name = loader.NextInt();
			stage.Thumbnail = loader.NextStr();
			stage.BG = loader.NextStr();
			CConfAnalytic::ToJsonInt(loader.NextStr(), stage.Drop);
			stage.ExInfo.NextID = 0;
			int id = stage.ID[0];
			item->Stages[id] = stage;
		}
		m_Datas[item->ID] = item;
	}
	toExInfo();

	return true;
}

void CConfChapter::toExInfo()
{
    int prevType = 0;
    int prevID = 0;
	for (std::map<int, void*>::iterator iter = m_Datas.begin(); iter != m_Datas.end(); ++iter)
	{
		int curChapterID = iter->first;
		ChapterConfItem* curChapterItem = static_cast<ChapterConfItem*>(iter->second);
        if (prevType != curChapterItem->Type)
        {
            prevType = curChapterItem->Type;
            prevID = 0;
        }
        curChapterItem->ExInfo.PrevID = prevID;
        prevID = curChapterID;
		curChapterItem->ExInfo.NextID = 0;
		curChapterItem->ExInfo.FirstStageID = 0;
		for (VecInt::iterator it = curChapterItem->UnlockChapters.begin(); it != curChapterItem->UnlockChapters.end(); ++it)
		{
			int lock = *it;
			std::map<int, void*>::iterator iter = m_Datas.find(lock);
			if (iter == m_Datas.end())
			{
				continue;
			}
			ChapterConfItem* lockChapterItem = static_cast<ChapterConfItem*>(iter->second);
			//lockChapterItem->ExInfo.PrevID = curChapterID;
			//curChapterItem->ExInfo.NextID = 0;
			//curChapterItem->ExInfo.FirstStageID = 0;
			if (lockChapterItem->Type == curChapterItem->Type)
			{
				curChapterItem->ExInfo.NextID = lock;
                break;
			}
		}

		int preStageID = 0;
		for (std::map<int, StageInfo>::iterator it = curChapterItem->Stages.begin(); it != curChapterItem->Stages.end(); ++it)
		{
			int curStageID = it->first;
			std::map<int, StageInfo>::iterator iter = curChapterItem->Stages.find(preStageID);
			if (iter != curChapterItem->Stages.end())
			{
				iter->second.ExInfo.NextID = curStageID;
			}
			else
			{
				curChapterItem->ExInfo.FirstStageID = curStageID;
			}
			preStageID = curStageID;
		}
	}
}

bool CConfDispatch::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    if (!loadCsv(csvLoader, str.c_str(), 3))
    {
        LOG("CConfDispatch Load CSV Faile %s", str.c_str());
        return false;
    }

    while (csvLoader.NextLine())
    {
        CDispatchItem *pDispatchSoldierItem = new CDispatchItem;
        // 序号
        pDispatchSoldierItem->ID = csvLoader.NextInt();
        csvLoader.NextStr();//备注
        // 分组
        pDispatchSoldierItem->Group = csvLoader.NextInt();

        // 分组列表,不包括0
        if (0 != pDispatchSoldierItem->Group)
        {
            std::vector<int>::iterator iter
                = std::find(m_vecGroup.begin(), m_vecGroup.end(), pDispatchSoldierItem->Group);
            if (iter == m_vecGroup.end())
            {
                m_vecGroup.push_back(pDispatchSoldierItem->Group);
            }
        }

        // 是否默认开启
        pDispatchSoldierItem->IsOpen = !CConfAnalytic::ToBool(csvLoader.NextStr());
        // 条件类型
        pDispatchSoldierItem->ConditionType = csvLoader.NextInt();
        // 条件类型参数
        pDispatchSoldierItem->ConditionParam = csvLoader.NextInt();
        // 延迟时间
        pDispatchSoldierItem->DispatchInfo.Delay = csvLoader.NextFloat() / 1000.0f;
        // 发兵ID
        pDispatchSoldierItem->DispatchInfo.DispatchSoldierID = csvLoader.NextInt();
        // 发兵路线
        pDispatchSoldierItem->DispatchInfo.DispatchLine = csvLoader.NextInt();
        // 开启条件ID
        CConfAnalytic::ToJsonInt(csvLoader.NextStr(), pDispatchSoldierItem->DispatchInfo.ConditionOpenID);
        // 关闭条件ID
        CConfAnalytic::ToJsonInt(csvLoader.NextStr(), pDispatchSoldierItem->DispatchInfo.ConditionCloseID);

        m_Datas[pDispatchSoldierItem->ID] = pDispatchSoldierItem;
    }

    return true;
}

bool CConfStageScene::LoadCSV(const std::string& str)
{
	CCsvLoader csvLoader;
    if (!loadCsv(csvLoader, str.c_str(), 3))
    {
        LOG("CConfStageScene Load CSV Faile %s", str.c_str());
        return false;
    }

	//如果有数据 
	while (csvLoader.NextLine())
	{
		StageSceneConfItem *item = new StageSceneConfItem;
		item->Id = csvLoader.NextInt();
		csvLoader.NextStr();
		item->FrontScene_ccs = csvLoader.NextStr();
		item->FrontScene_Size = csvLoader.NextFloat();
		item->FightScene_ccs = csvLoader.NextStr();
		item->FightScene_Size = csvLoader.NextFloat();
		item->BgScene_ccs = csvLoader.NextStr();
		item->BgScene_Size = csvLoader.NextFloat();
		item->FarScene_ccs = csvLoader.NextStr();
		item->FarScene_Size = csvLoader.NextFloat();
		item->BlueHeroPos = csvLoader.NextInt();
		CConfAnalytic::ToJsonFloat(csvLoader.NextStr(), item->BlueSoldierPos);
		item->RedHeroPos = csvLoader.NextInt();
		CConfAnalytic::ToJsonFloat(csvLoader.NextStr(), item->RedSoldierPos);
		CConfAnalytic::ToJsonFloat(csvLoader.NextStr(), item->Walkline);
        item->BgMusicId = csvLoader.NextInt();
        CConfAnalytic::ToVecVecInt(csvLoader.NextStr(), item->IMSControl1);
        CConfAnalytic::ToVecVecInt(csvLoader.NextStr(), item->IMSControl2);

		m_Datas[item->Id] = item;
	}

	return true;
}

void toScencCall(std::string str, std::vector<SSceneCall>& vec)
{
    if ("[]" == str)
    {
        return;
    }

    VecVecInt temp;
    CConfAnalytic::ToVecVecInt(str, temp);

    SSceneCall sceneCall;
    for (VecVecInt::iterator iter = temp.begin(); iter != temp.end(); iter++)
    {
        if (3 <= iter->size())
        {
            sceneCall.callID = (*iter)[0];
            sceneCall.callPosX = (*iter)[1];
            sceneCall.callPosY = (*iter)[2];
            vec.push_back(sceneCall);
        }
    }
}

CConfStage::~CConfStage()
{
    deleteAndClearMap(m_StageDispatchList);
}

bool CConfStage::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    if (!loadCsv(csvLoader, str.c_str(), 3))
    {
        LOG("CConfStage Load CSV Faile %s", str.c_str());
        return false;
    }

    //如果有数据 
    while (csvLoader.NextLine())
    {
        StageConfItem* item = new StageConfItem;
        item->ID = csvLoader.NextInt();
        csvLoader.NextStr();
        item->StageSenceID = csvLoader.NextInt();
        item->Boss = csvLoader.NextInt();
        for (int i = 0; i < 7; ++i)
        {
            int monsterId = csvLoader.NextInt();
            if (monsterId != 0)
            {
                item->Monsters.push_back(monsterId);
            }
        }
        item->Type = csvLoader.NextInt();
        CConfAnalytic::ToJsonFloat(csvLoader.NextStr(), item->TypeParam);
        item->TimeLimit = csvLoader.NextInt();
        CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->Win);
        CConfAnalytic::ToJsonInt(csvLoader.NextStr(), item->Fail);
        item->WinStar1 = csvLoader.NextInt();
        item->WinStar1Param = csvLoader.NextInt();
        item->WinStar2 = csvLoader.NextInt();
        item->WinStar2Param = csvLoader.NextInt();
		for (int i = 0; i < 3; ++i)
		{
			//3种星级掉落id
			item->ItemDrop.push_back(csvLoader.NextInt());
		}
		item->FirstItemDrop = csvLoader.NextInt();
        toScencCall(csvLoader.NextStr(), item->SceneCall);
		m_Datas[item->ID] = item;
#ifndef RunningInServer
        // 发兵配置,后端无需使用
        char szPath[128] = { 0 };
        snprintf(szPath,sizeof(szPath),"config/Stage/Stages/%d_Stage.csv", item->ID);

        CConfDispatch *pConfDispatch = new CConfDispatch;
        if (!pConfDispatch->LoadCSV(szPath))
        {
            delete pConfDispatch;
            continue;
        }
        m_StageDispatchList[item->ID] = pConfDispatch;
#endif
    }

    return true;
}

void CConfStage::ToRoleInfo(std::string str, SRoleInfo &info)
{
    std::vector<std::string> vecStr;
    CConfAnalytic::StringSplit(str, vecStr, "+");
    if ((int)vecStr.size() == 2)
    {
        info.RoldID = atoi(vecStr[0].c_str());
        info.RoldLv = atoi(vecStr[1].c_str());
    }
}

void CConfStage::ToNpc(const std::string &str, std::vector<SNPCInfo>& vec)
{
    std::vector<std::string> vecStr;
    std::vector<std::string> vecSubStr;

    CConfAnalytic::StringSplit(str, vecStr, "|");
    for (std::vector<std::string>::iterator iter = vecStr.begin();
        iter != vecStr.end(); ++iter)
    {
        vecSubStr.clear();
        CConfAnalytic::StringSplit(*iter, vecSubStr, "+");
        if ((int)vecSubStr.size() == 4)
        {
            SNPCInfo info;
            info.NpcId = atoi(vecSubStr[0].c_str());
            info.NpcLv = atoi(vecSubStr[1].c_str());
            info.NpcPosX = atoi(vecSubStr[2].c_str());
            info.NpcPosY = atoi(vecSubStr[3].c_str());
            vec.push_back(info);
        }
    }
}

void CConfStage::ToTime(const std::string& str, int type, TimeInfo& info)
{
    if (str == "0" || str == "")
    {
        return;
    }
    memset(&info, 0, sizeof(info));
    std::vector<std::string> vecStr;
    CConfAnalytic::StringSplit(str.substr(1, str.size() - 2), vecStr, "+");
    switch (type)
    {
    case 2://周恢复
        if ((int)vecStr.size() == 3)
        {
            info.Week = atoi(vecStr[0].c_str());
            info.Hour = atoi(vecStr[1].c_str());
            info.Min = atoi(vecStr[2].c_str());
        }
        break;
    case 1: //日恢复
        if ((int)vecStr.size() == 2)
        {
            info.Hour = atoi(vecStr[0].c_str());
            info.Min = atoi(vecStr[1].c_str());
        }
        break;
    case -2: //每隔多少秒恢复
        if ((int)vecStr.size() == 1)
        {
            info.Sec = atoi(vecStr[0].c_str());
        }
        break;
    case 0:	//不恢复
    case -1: //立即恢复
        break;
    default:
        break;
    }
}

CConfDispatch * CConfStage::getDispatchData(const int &stageID)
{
    std::map<int, CConfDispatch*>::iterator iter = m_StageDispatchList.find(stageID);
    if (iter != m_StageDispatchList.end())
    {
        return iter->second;
    }

    return NULL;
}

bool CConfPvpScene::LoadCSV(const std::string& str)
{
	CCsvLoader loader;
	CHECK_RETURN(loadCsv(loader, str.c_str(), 3));

	//如果有数据 
	while (loader.NextLine())
	{
		PvpSceneConfItem *pPvpSceneConfItem = new PvpSceneConfItem;
		pPvpSceneConfItem->Dan = loader.NextInt();
		CConfAnalytic::ToJsonInt(loader.NextStr(), pPvpSceneConfItem->Stage);
		m_Datas[pPvpSceneConfItem->Dan] = pPvpSceneConfItem;
	}

	return true;
}