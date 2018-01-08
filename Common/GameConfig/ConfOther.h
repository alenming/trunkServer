#ifndef __CONF_OTHER_H__
#define __CONF_OTHER_H__

#include "ConfManager.h"

/////////////////////////////////配表数据/////////////////////////////////////////
enum EResType
{
	RT_ARMATURE = 1,						// cocostudio 动画文件
	RT_SPINE,								// spine动画文件
    RT_CSB2,                                // CSB2.0动画文件
};

// 资源配置
struct SResPathItem
{
	EResType            ResType;			// 1为armature,2为spine,3为csb2.0特效,4为音效bank
	int					AnimationID;		// 资源ID
	std::string         ResName;            // 资源名字
	std::string			Path;				// spine为json路径,armature为csb路径
	std::string         AtlasPath;			// 专为spine图集使用,armature为空
	std::string			Skin;				// 专为spine皮肤使用
};

//角色对应资源
struct SRoleResItem
{
	int RoleID;		// 角色ID
	int RoleStar;	// 角色星级
	VecInt ResIDs;	// 对应资源ID
    std::vector<std::string> MusicRess;	// 对应音效资源
};

////////////////////////////////解析配表//////////////////////////////////////////
class CConfAnimationRes : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfRoleResPreload : public CConfBase
{
public:
    virtual ~CConfRoleResPreload();
	virtual bool LoadCSV(const std::string& str);

public:
	SRoleResItem* getData(int id, int star)
	{
		std::map<int, std::map<int, SRoleResItem*> >::iterator iter = m_mapRoleResItem.find(id);
		if (iter != m_mapRoleResItem.end())
		{
			std::map<int, SRoleResItem*>::iterator iterItem = iter->second.find(star);
			if (iterItem != iter->second.end())
			{
				return iterItem->second;
			}
		}
		return NULL;
	}

	std::map<int, std::map<int, SRoleResItem*> >& getData()
	{
		return m_mapRoleResItem;
	};

private:
	std::map<int, std::map<int, SRoleResItem*> > m_mapRoleResItem;
};

////////////////////////////////查询配表//////////////////////////////////////////
inline const SResPathItem* queryConfSResInfo(const int& id)
{
	CConfAnimationRes *confRes = dynamic_cast<CConfAnimationRes*>(
		CConfManager::getInstance()->getConf(CONF_RESPATH));
	return static_cast<SResPathItem*>(confRes->getData(id));
}

inline const SRoleResItem* queryConfSRoleResInfo(const int& id, const int& star)
{
	CConfRoleResPreload *confRoleRes = dynamic_cast<CConfRoleResPreload*>(
		CConfManager::getInstance()->getConf(CONF_ROLERES));
	return confRoleRes->getData(id, star);
}

#endif
