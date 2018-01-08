/*
 * 战斗场景
 * 
 * 1.获取外部的数据，用户信息，房间信息等等
 * 2.在客户端加载战斗场景显示内容，并初始化
 * 3.创建BattleHelper，EventManager等一系列对象，并初始化
 * 4.战斗指令的驱动
 *
 * 2014-12-18 By 宝爷
 */
#ifndef __BATTLE_SCENE_H__
#define __BATTLE_SCENE_H__

#include "KxCSComm.h"
#include "BattleHelper.h"
#include "EventManager.h"

class CPvpRoom;
class CRoomModel;
class CBattlePlayerModel;
class CBattleHelper;
class CBattleLayer : public Node
{
public:
    CBattleLayer();
    ~CBattleLayer();

public:
    
	bool init(CPvpRoom *room, CEventManager<int> *manager);

    void update(float dt);

    void setTickSpeed(int frame);

    float getTickDelta();

    void logicUpdate(float delta);

    void onResponse(void *data, int len);

    CBattleHelper *getBattleHelper();

private:

    float                     m_fDelta;
    float                     m_fTickDelta;
	CPvpRoom*			      m_pRoom;
    CRoomModel *              m_pRoomModel;
    CBattleHelper *           m_pHelper;
    CBattlePlayerModel *      m_pBlueModel;
    CBattlePlayerModel *      m_pRedModel;
};

#endif
