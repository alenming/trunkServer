/*
 * 公会类
 * 1.包含公会的所有(各个建筑)数据模型
 * 2.获取相关数据模型
 */

#ifndef __UNION_H__
#define __UNION_H__

#include "UnionModel.h"
#include "UnionExpiditionModel.h"
#include "ExpiditonRankModel.h"
#include "KXServer.h"
#include "UnionMercenaryModel.h"
#include "UnionShopModel.h"

class CUnion : public KxServer::KxTimerObject
{
public:
    CUnion();
    ~CUnion();

	void onTimer(const KxServer::kxTimeVal& now);

    // 新公会初始化(还没有模型)
    bool init(int unionID, int chairmanID, std::string unionName);
    // 已存在公会根据模型初始化
    bool init(int unionID);

    CUnionModel* getUnionModel() { return m_pUnionModel; }
	CExpiditonRankModel* getExpiditionRankModel() { return m_pExpiditionRankModel; }
	CUnionExpiditionModel* getExpiditionModel() { return m_pUnionExpiditionModel; }
	CUnionMercenaryModel* getMercenaryModel(){ return m_pMercenaryModel; }
	CUnionShopModel*	  getUnionShopModel() { return m_pUnionShopModel; }
    // 获取公会名称
    std::string getUnionName();
    // 移除公会
    bool removeUnion();
    // 删除公会
    bool deleteUnion();
	//设置奖励地图ID
	void setRewardMapID(int nMapID) { m_RewardMapID = nMapID; }
	//获取奖励地图ID
	int getRewardMapID() { return m_RewardMapID; }
	//下发刷新公会商店UI
	bool sendUnionShopData(int nUid);

private:
    CUnionModel*			m_pUnionModel;
	CExpiditonRankModel*	m_pExpiditionRankModel;
	CUnionExpiditionModel*	m_pUnionExpiditionModel;
	CUnionShopModel*		m_pUnionShopModel;
	int						m_RewardMapID;
	CUnionMercenaryModel*	m_pMercenaryModel;
};

#endif