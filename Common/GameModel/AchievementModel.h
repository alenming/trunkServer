#ifndef __ACHIEVEMENT_MODEL_H__
#define __ACHIEVEMENT_MODEL_H__

#include <map>
#include "Storage.h"
#include "IDBModel.h"

struct AchieveDBInfo
{
    int achieveVal;              // 成就值
    int achieveStatus;           // 成就状态
};

enum EAchieveStatus
{
    EACHIEVE_STATUS_UNACTIVE = -1,// 未激活
    EACHIEVE_STATUS_ACTIVE,       // 激活
    EACHIEVE_STATUS_FINISH,       // 完成
    EACHIEVE_STATUS_GET,          // 领取
};

class CAchievementModel : public IDBModel
{
public:
    CAchievementModel();
    ~CAchievementModel();

    bool init(int uid);

    bool Refresh();
    // 添加成就
    bool AddAchievement(int achieveID, AchieveDBInfo &info);
    // 存储成就
    bool SetAchievement(int achieveID, AchieveDBInfo &info);
    // 获取某个成就的值
	bool GetAchievementByID(int achieveID, AchieveDBInfo &info, bool bNew = false);
    // 获取所有成就
    std::map<int, AchieveDBInfo>& GetAllAchievement();
    // 删除某个成就
    bool RemoveAchievementByID(int achieveID);
    // 删除成就
    bool RemoveAchievement();

protected:
	bool GetAchievementFromDB(int achieveID, AchieveDBInfo &info);

private:
    int					 m_nUid;
    Storage *			 m_pStorage;
    std::map<int, AchieveDBInfo>   m_mapAllAchievement;   // <成就ID, 成就信息>
};

#endif