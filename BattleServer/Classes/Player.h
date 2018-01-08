#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <vector>
#include "CommStructs.h"
#include "BattleModels.h"

class CGameUser;
class CPvpRoom;
class CPlayer : public CPlayerModel
{
public:
    CPlayer(void);
    ~CPlayer(void);

public:
    //通过uid及房间初始化角色
    bool initWithData(char* data, int& len);
    //设置连接状态
	void setIsConnecting(bool connecting);
	//获得连接状态
	bool isConnecting();
    //获取竞技积分
    inline int getIntegral() { return m_Integral; }
    inline int getMMR() { return m_MMR; }
    inline int getCoutinueWin() { return m_ContinueWin; }
    inline int getDayWinTimes() { return m_DayWinTimes; }

private:
	bool						m_bIsConnecting;		//是否连接中
    int                         m_Integral;             //玩家的竞技积分
    int                         m_MMR;                  //玩家的MMR值
    int                         m_ContinueWin;          //连胜场数
    int                         m_DayWinTimes;          //日胜场数
};

#endif 
