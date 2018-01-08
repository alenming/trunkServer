#include "ConfOther.h"

using namespace std;

bool CConfAnimationRes::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
    CHECK_RETURN(loadCsv(pLoader, str.c_str(), 3));

	//如果有数据 
	while (pLoader.NextLine())
	{
		SResPathItem* resInfo = new SResPathItem;
		resInfo->AnimationID = pLoader.NextInt();
		resInfo->ResType = static_cast<EResType>(pLoader.NextInt());
		resInfo->Path = pLoader.NextStr();
		resInfo->AtlasPath = pLoader.NextStr();
		resInfo->Skin = pLoader.NextStr();
		// 骨骼动画名
		std::string::size_type nPosBegin = resInfo->Path.find_last_of("/");
		std::string::size_type nPosEnd = resInfo->Path.find_last_of(".");
		if (nPosBegin != std::string::npos
			&& nPosEnd != std::string::npos)
		{
			resInfo->ResName = resInfo->Path.substr(nPosBegin + 1, nPosEnd - nPosBegin - 1);
			m_Datas[resInfo->AnimationID] = resInfo;
		}
		else
		{
			SAFE_DELETE(resInfo);
		}
	}
	return true;
}

CConfRoleResPreload::~CConfRoleResPreload()
{
    std::map<int, std::map<int, SRoleResItem*> >::iterator iter = m_mapRoleResItem.begin();
    for (; iter != m_mapRoleResItem.end(); ++iter)
    {
        deleteAndClearMap(iter->second);
    }

    m_mapRoleResItem.clear();
}

bool CConfRoleResPreload::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
    CHECK_RETURN(loadCsv(pLoader, str.c_str(), 3));

	//如果有数据 
	while (pLoader.NextLine())
	{
		SRoleResItem* roleRes = new SRoleResItem;
		roleRes->RoleID = pLoader.NextInt();
		roleRes->RoleStar = pLoader.NextInt();
		CConfAnalytic::ToJsonInt(pLoader.NextStr(), roleRes->ResIDs);
        CConfAnalytic::ToJsonStr(pLoader.NextStr(), roleRes->MusicRess);

		m_mapRoleResItem[roleRes->RoleID][roleRes->RoleStar] = roleRes;
	}
	return true;
}
