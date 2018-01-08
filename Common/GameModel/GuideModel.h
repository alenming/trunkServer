#ifndef __STORY_MODEL__
#define __STORY_MODEL__

#include "Storage.h"
#include "IDBModel.h"

enum GuideType
{
	NewPlayerGuideType,					//新手引导类型
};

class CGuideModel : public IDBModel
{
public:
	CGuideModel();
	~CGuideModel();

	bool init(int uid);

	bool Refresh();

	bool AddGuideID(int nValue);

	bool DelGuideID(int nValue);

	std::set<int>& GetNewGuideID() { return m_SetGuide; }

protected:
	//存储数据到数据库
	bool SaveGuideToDB();

private:

	int                 m_nUid;
	std::string			m_strGuideKey;
	Storage             *m_pStorage;		// 数据库
	std::set<int>		m_SetGuide;			//引导集合
};

#endif