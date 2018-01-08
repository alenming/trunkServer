#ifndef __ROOM_H__
#define __ROOM_H__

#include <map>
#include <vector>
#include "KxTimerManager.h"
#include "KxCommInterfaces.h"
#include "CommStructs.h"

#define PVPROOM_PREPARE_EXPIRE	 10  //PVP进入房间超时时间
#define PVPROOM_LOADING_EXPIRE	 60	 //PVP加载最长时间

class CBufferData;
class CPlayer;
class CBattleLayer;
class Director;
template <typename T>
class CEventManager;
class CPvpModel;
class CPvpSettleResult;

class IRoom
{
public:
	IRoom();
	virtual ~IRoom();

public:
	// 获得房间类型
	virtual int getRoomType() { return m_nRoomType; }

protected:

	int		m_nRoomType;
};

class CPvpRoom : public IRoom, public KxServer::KxTimerObject
{
public:
	CPvpRoom(void);
	~CPvpRoom(void);

public:
	// 游戏开始, 初始化游戏帧数, 地图
	bool initRoom(int battleId, int stageId, int pvpType = PVPROOMTYPE_PVP);
	// 开始加载
	void loadingRoom();
	// 游戏开始
	bool startRoom();
	// 战斗中
	void fightingRoom();
	// 退出房间
	void exitRoom();
	// timer回调
	void onTimer(const KxServer::kxTimeVal& now);

	// 执行游戏 返回最后执行帧数
	void executeGame(float dt);
	// 处理前端游戏数据
	void onProcessGame(int cmd, int subcmd, void *data, int len);
	// 处理后续, 计算玩家获得
	void overGame();

    // 处理胜利者
    void processPlayer(int result, CPlayer* player,int nEnemySummerID,int mmr,
        int integral, int newintegral, int crylv, int dt);

	// 玩家加入房间
	bool playerJoin(int uid, int summonerId, std::vector<int> &heroIds);
    bool playerJoin(int uid, char* playerData, int len);

	// 玩家退出房间
	bool playerQuit(int uid);

	// 房间是否已满
	bool isFull();
	// 是否都准备完毕
	bool isAllReady();
	// 是否已加入房间
	bool isRoomPlayer(int uid);
	// 是否全部断线
	bool isAllDisconnect();
	// 玩家是否断线
	bool isPlayerDisconnect(int uid);

	// 记录玩家加载进度
	void playerLoading(int uid, int progress);
	// 玩家准备
	void playerReady(int uid);
	// 玩家重连
	void playerReconnect(int uid);
	// 有玩家掉线
	void playerDisconnect(int uid);

	// 房间状态
	int getRoomState();
	// 获得对手uid
	int getOpponentUid(int uid);
	// 获得玩家进度
	int getPlayerProgress(int uid);

	// 地图id
	int getStageId();
	// 获得战斗id
	int getBattleId();
	// 该局游戏在调用该函数时该执行第几帧, 会加上一个延时帧数    
	int getCurrentTick();
	// 获得房间计时
	int getRoomTick();
	// 获得战斗预备剩余时间
	float getPrepareTick();
	// 获得房间玩家
	CPlayer *getRoomPlayer(int uid);
	// 获得房主
	CPlayer *getMasterPlayer();
    // 获得非房主玩家
    CPlayer *getOtherPlayer();
	// 获得房间所有玩家
	std::map<int, CPlayer*> &getRoomPlayers();
	// 获得房间信息, 包括房间内玩家
	void getRoomData(CBufferData& bufferData);
	// 获得战斗信息
	void getBattleData(CBufferData &bufferData);

	// 秒数
	void setRoomTick(int tick);
	// 地图id
	int setStageId();
	// 设置胜利者
	void setWinnerId(int uid);
	// 设置失败者
	void setLoserId(int uid);
	// 设置挑战结果, 挑战结果以蓝方为参考, 则m_nChallengeResult为赢时红方输
	void setChallengeResult(int result) { m_nChallengeResult = result; }
	// 设置玩家连接
	void setPlayerCommunication(int uid, KxServer::IKxComm *target);
	// 房间内广播信息
	int sendRoom(int maincmd, int subcmd, char *data, unsigned int len);
	// 指定玩家发送
	int sendPlayer(int uid, int maincmd, int subcmd, char *data, unsigned int len);

private:
	// room info
	bool                     m_bClose;                      //房间是否准备关闭
	int                      m_nStartStamp;                 //开始时间  (毫秒)
	int                      m_nCurStamp;                   //当前时间戳 (毫秒)
	int                      m_nPrevStamp;                  //上次执行的时间戳 (毫秒)
	int                      m_nFPS;                        //游戏帧速
	int                      m_nStageId;                    //地图Id
	int						 m_nBattleId;					//战斗id
	int                      m_nRoomState;                  //房间进行的状态
	int                      m_nRoomCapacity;               //房间最多容纳人数
	int						 m_nRoomTick;					//房间计时
	float					 m_fPrepareFightTick;			//准备战斗时间计时

	//结束信息
	int						 m_nWinnerId;					//胜利者
	int						 m_nLoserId;					//失败者
	int						 m_nChallengeResult;			//挑战结果(蓝方结果)
	Director*                m_pDirector;                   //负责游戏逻辑的导游类
	CBattleLayer*            m_pBattleScene;                //战斗场景
	CPlayer*                 m_pMaster;                     //房主
    CPlayer*                 m_pOther;                      //对方
    KxServer::KxTimerManager *m_pTimerManager;				//计时器
	CEventManager<int>*      m_pEventManager;               //房间事件管理器
	KxServer::kxTimeVal      m_LastTime;
	std::map<int, CPlayer*>  m_Players;						//房间的角色
	std::map<int, int>       m_PlayerProgress;              //玩家进度
	std::map<int, KxServer::IKxComm *> m_PlayerCommu;		//玩家session连接
};

#endif //__ROOM_H__
