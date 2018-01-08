#ifndef __ACHIEVEMENT_H__
#define __ACHIEVEMENT_H__

#include "UserActionListener.h"
#include "AchievementModel.h"
#include "ConfHall.h"

class CAchievement : public IUserActionListener
{
public:
    CAchievement();
    ~CAchievement();

    // 初始化成就
    bool init(int uid, int achieveID, AchieveDBInfo &info);
    // 动作操作
    virtual bool onAction(int actID, void *data, int len);

private:
    // 各类成就的监听处理
    bool onUserLevelUpAction(int actID, void *data, int len);
    bool onAccumulateGoldAction(int actID, void *data, int len);
    bool onUpgradeHeroLvAction(int actID, void *data, int len);
    bool onHeroTestAction(int actID, void *data, int len);
    bool onGoldTestAction(int actID, void *data, int len);
    bool onShopBuyAction(int actID, void *data, int len);
    bool onUseSameRaceAction(int actID, void *data, int len);
    bool onDrawCardAction(int actID, void *data, int len);
    bool onBuyGoldAction(int actID, void *data, int len);

private:
    bool canFinishAchieve(int val = 1);
    bool finishHideAchieve();

private:
    int                 m_nUserID;          // 用户ID
    int                 m_nAchieveID;       // 成就ID 
    AchieveDBInfo       m_AchieveInfo;      // 状态/累计值
    CAchievementModel*  m_pAchieveModel;    // 成就数据库模型
    const AchieveItem*  m_pAchieveItemConf; // 配置表
};

#endif