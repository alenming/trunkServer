/*
英雄试炼模型
1、存储类型为hash,field为0存储刷新时间(每个关卡刷新时间一致)
2、其他位置的field为英雄试炼表相关ID(>0)
*/

#ifndef __HEROTESTMODEL_H__
#define __HEROTESTMODEL_H__

#include "IDBModel.h"
#include <string>
#include <map>

enum HeroTestField
{
	HEROTEST_FD_RESETSTAMP = 0,			//刷新时间戳Field
};

class Storage;
class CHeroTestModel : public IDBModel
{
public:
    CHeroTestModel();
    ~CHeroTestModel();

public:
	//初始化
	bool init(int uid);
	//数据刷新
	bool Refresh();
    // 获取挑战结束时间戳
    int GetResetStamp(){ return m_nResetStamp; }
    // 获取某副本的挑战次数
    int GetChallengeTimes(int heroTestID);
    // 获取所有挑战的次数
    std::map<int, int>& GetAllChallengeTimes(){ return m_mapTimes; }
    // 重置英雄试炼
    bool ResetHeroTest(int useStamp);
    // 添加挑战次数
    bool AddChallengeCount(int heroTestID, int count);

private:

    int			       m_nUID;			// 用户ID
    int	               m_nResetStamp;   // 刷新时间戳
	std::string        m_strKey;		// key
	Storage *	       m_pStorage;		// storage

    std::map<int, int> m_mapTimes;		//键：副本ID 值：次数
};

#endif