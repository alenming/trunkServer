#ifndef __PVP_SETTLE_RESULT_H__
#define __PVP_SETTLE_RESULT_H__

#define PVP_INTEGRAL_BOUNDARY   750

class CPvpSettleResult
{
public:
	CPvpSettleResult();
	~CPvpSettleResult();

public:
	// 初始化
	bool init(int roomType);
	// 结算
	void pvpSettleAccount(int winner, int loser, int result,int winerCryLv,int loserCryLv,int dt);
	
	// 设置胜利者的信息
	static void setWinnerPvpModel(int roomType, int uid, int changeMMR, int changeInte, int newRank);
	// 设置失败者的信息
	static void setLoserPvpModel(int roomType, int uid, int changeMMR, int changeInte, int newRank);
	// 设置平局信息, 平局时, 改变的mmr和积分都为0
	static void setDrawPvpModel(int roomType, int uid, int changeMMR, int changeInte, int newRank);
	// 传入本人的MMR, 对手的MMR, 连胜/连败系数, 胜负结果, 返回计算之后的改变的mmr值, 可能为负
	static int calcMMR(int myMMR, int otherMMR, int k, int result);
	// 传入本人的积分, 对手的积分, 胜负结果, 返回计算之后的改变的积分值, 可能为负
	static int calcIntegral(int myIntegral, int otherIntegral, int result);

    // 段位升级处理
    static void integralDanUp(int uid, int oldDan, int newDan, bool isFirst);

private:
	// 结算公平竞技
	void overFairPvp(int winner, int loser, int result, int winerCryLv, int loserCryLv, int dt);
	// 结算锦标赛
	void overChampionPvp(int winner, int loser, int result, int winerCryLv, int loserCryLv, int dt);

private:

	int m_nRoomType;
};

#endif //__PVP_SETTLE_RESULT_H__
