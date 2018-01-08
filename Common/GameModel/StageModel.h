#ifndef __STAGE_MODEL_H__
#define __STAGE_MODEL_H__

#include <map>
#include "Storage.h"
#include "IDBModel.h"

enum EStageInfoField
{
	SIF_CANUSETIMES,
	SIF_USESTAMP,
	SIF_BUYTIMES,
	SIF_BUYSTAMP,
	SIF_NORMALCHAPTER_STATUS,
	SIF_ELITECHAPTER_STATUS,
	SIF_CURSTAGE,
	SIF_CURELITE,
};

struct DBEliteStageInfo
{
    int challengeTimes;		   //精英关卡挑战次数
	int useRecoverStamp;	   //挑战时间
	int buyTimes;			   //购买次数
	int buyRecoverStamp;	   //购买时间
};

enum EStageStatusType
{
	STAGESTATUS_HIDE = 0,		//隐藏
	STAGESTATUS_LOCK,			//未解锁
	STAGESTATUS_UNLOCK,			//已解锁
	STAGESTATUS_STAR1,			//1星状态
	STAGESTATUS_STAR2,			//2星状态
	STAGESTATUS_STAR3,			//3星状态
};

enum EChapterStatusType
{
	CHAPTERSTATUS_LOCK = 0,		//未解锁
	CHAPTERSTATUS_UNLOCK,		//已解锁
	CHAPTERSTATUS_FINISH,		//已完成
	CHAPTERSTATUS_AWARD,		//已领取
};

enum EChapterBoxState
{
	CHAPTERBOX_UNGET,			//未领取
	CHAPTERBOX_GET,				//领取
};

class CStageModel : public IDBModel
{
public:
    CStageModel();
    ~CStageModel();

public:
	// 初始化
    bool init(int uid);
	// 刷新关卡数据
	bool Refresh();

	// 设置普通关卡状态
	bool SetStageStatus(int stageId, int status, bool remove = true);
	// 设置最新关卡
	bool SetCurStage(int stageId);
	// 获得关卡状态, 返回关卡状态
	int GetStageStatus(int stageId);

	// 设置章节状态
	bool SetChapterStatus(int chapterId, int status);
	// 获得章节状态, 返回章节状态
	int GetChapterStatus(int chapterId);
	// 设置精英章节状态
	bool SetEliteChapterStatus(int chapterId, int status);
	// 获得精英章节状态, 返回章节状态
	int GetEliteChapterStatus(int chapterId);

	// 设置精英关卡状态
	bool SetEliteStatus(int stageId, int status, int remove = true);
	// 设置新的精英关卡
	bool SetCurElite(int stageId);
	// 获得精英关卡状态
	int GetEliteStatus(int stageId);
	// 设置精英关卡状态
	bool SetEliteInfo(int stageId, DBEliteStageInfo &info);
	// 设置精英关卡单个数据
	bool SetEliteInfo(int stageId, int field, int value);
	// 获得指定精英关卡信息
	bool GetEliteInfo(int stageId, DBEliteStageInfo& info);
	// 移除指定精英关卡的额外信息
	bool RemoveEliteInfo(int stageId);
	// 删除玩家关卡数据
    bool DeleteStage();

	// 当前关卡信息
	int GetCurStage() { return m_nCurStage; }
	int GetCurElite() { return m_nCurElite; }
	
	// 获得玩家所有数据
	std::map<int, int>& GetStageStatusMap() { return m_mapStageStatus; }
	std::map<int, int>& GetEliteStatusMap() { return m_mapEliteStatus; }
	std::map<int, int>& GetChapterStatusMap() { return m_mapChapterStatus; }
	std::map<int, int>& GetEliteChapterStatusMap() { return m_mapEliteChapterStatus; }
	std::map<int, DBEliteStageInfo>& GetEliteStageInfo() { return m_mapEliteStageInfo; }
	
	//获取章节的所有宝箱信息
	std::map<int, int>* getChapterBoxStatusMap(int chapterId);
	bool getChapterBoxStatus(int chapterId, int nIndex, int &nStatus);
	//设置章节宝箱信息
	bool setChapterBoxStatus(int chapterId, int nIndex, int nStatus);
	std::map<int, std::map<int, int> >& getAllChapterBoxMap() { return m_mapChanpterBoxStatus; }
private:

	std::string GetStageInfoField(int id, int fieldType);

private:

	int								m_nUID;						// 用户ID
	int								m_nCurStage;				// 可以挑战的普通关卡
	int								m_nCurElite;				// 可以挑战的精英关卡
	std::string						m_strStageStatusKey;		// 关卡Key    <stageId, status>
	std::string						m_strEliteStatusKey;		// 精英关卡key <eliteId, status>
	std::string						m_strEliteInfoKey;			// 关卡信息key <stageId/eliteId, usetimes/usestamp/buytimes/buystamp>
	std::string						m_strStageBoxStatusKey;		//章节宝箱Key
	std::map<int, int>				m_mapStageStatus;			// 普通关卡状态
	std::map<int, int>				m_mapEliteStatus;			// 精英关卡状态
	std::map<int, int>				m_mapChapterStatus;		    // 章节状态
	std::map<int, std::map<int, int> > m_mapChanpterBoxStatus;	// 章节宝箱状态
	std::map<int, int>				m_mapEliteChapterStatus;    // 精英章节状态
	std::map<int, DBEliteStageInfo>	m_mapEliteStageInfo;		// 精英关卡信息
	Storage *						m_pStorage;					// 数据库
};

#endif