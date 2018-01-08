#ifndef __CHALLENGE_ROOM_H__
#define __CHALLENGE_ROOM_H__

#include <vector>
#include <map>
/*
挑战房间, 记录一些挑战的基本信息, 用于辅助验证提交信息合法性
CChallengeRoom: 挑战类关卡父类
CStageRoom:		负责普通关卡, 精英关卡, 以及副本活动关卡的数据记录
3大试练room: 
*/

class CGameUser;
class StageConfItem;
class CChallengeRoom
{
public:
	CChallengeRoom();
	virtual ~CChallengeRoom();

public:
	//初始化
	virtual bool init(int uid, int battleType, int stageId, int stageLevel);
	//扩展字段1
	virtual void setExt1(int ext) { m_nExt1 = ext; }
	//扩展字段2
	virtual void setExt2(int ext) { m_nExt2 = ext; }
	//设置召唤师
	virtual bool setSummonerId(int summonerId);
	//添加英雄
	virtual bool addHeroId(int heroId);
	//添加BUFF
	virtual void addBuff(int nID);
	//血量百分比
	virtual void setHPPercent(int percent) { m_nHPPercent = percent; }
	//消耗水晶数
	virtual void setUseCrystal(int useCrystal) { m_nUseCrystal = useCrystal; }
	//耗时
	virtual void setTick(int tick) { m_nTick = tick; }
	//获得房间数据
	virtual void getRoomData(char *data, int &len);
	//获得房间数据的长度
	virtual int getRoomDataLength();
	//获得UID
	virtual int getUid() { return m_nUid; }
	//获得房间类型
	virtual int getBattleType()	{ return m_nBattleType; }
	//开始挑战
	virtual void challengeBegin();
	//挑战结束
	virtual void challengeEnd();
	//挑战关卡ID
	virtual int GetChallengeID() { return m_nStageId; }
    //取消战斗
    virtual void cancelChallenge();

protected:

	virtual int getStarInfo();

protected:

	int					m_nUid;				//用户id
	int					m_nStageId;			//关卡id
	int					m_nStageLevel;		//关卡等级
	int					m_nBattleType;		//房间类型
	int					m_nExt1;			//拓展字段1
	int					m_nExt2;			//拓展字段2
	int					m_nBeginTime;		//开始时间
	int					m_nEndTime;			//结束时间
	int					m_nSummonerId;		//召唤师id
	int					m_nHPPercent;		//血量百分比
	int					m_nUseCrystal;		//消耗的水晶数量
	int					m_nTick;			//耗时
	int					m_nStar2Reason;		//获得星星2原因
	int					m_nStar3Reason;		//获得星星3原因

	CGameUser *			m_pUserInfo;		//玩家信息
	const StageConfItem *m_pStageConf;		//关卡配置
	std::vector<int>	m_HeroList;			//英雄id
	std::vector<int>	m_vectBuffID;		//外部buffId列表
};


// 副本挑战房间
class ActivityInstanceItem;
class CInstanceRoom : public CChallengeRoom
{
public:
	CInstanceRoom();
	~CInstanceRoom();

public:
	//初始化
	bool init(int uid, int instanceId, int difficulty);
	//获得副本id
	int getInstanceId() { return m_nInstanceId; }
	//获得副本难度
	int getDifficulty() { return m_nDifficulty; }
	//挑战结束
	void challengeEnd();

private:
	//奖励数据
	void getRewardData();

private:

	int						m_nInstanceId;		//副本id
	int						m_nDifficulty;		//副本难度
	int						m_nFinalStar;		//
	const ActivityInstanceItem *m_pInstanceConf;//副本配置
};

#endif //__CHALLENGE_ROOM_H__
