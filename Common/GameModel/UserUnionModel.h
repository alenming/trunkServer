#ifndef _USERUNIONMODEL_H__
#define _USERUNIONMODEL_H__

#include "IDBModel.h"
#include "StorageManager.h"
#include <set>
#include <map>

enum 
{
	USER_UNION_EXPIDITION_REWARD,				//奖励数据，字符串

    USER_UNION_EXPIDITION_TIME = 10,            //远征次数
	USER_UNION_ID,								//公会ID
	USER_UNION_INDEX,							//当前正在征战关卡序列
    USER_UNION_CONTRIBUTION,                    //累计贡献
    USER_UNION_DAYPVPLIVENESS,                  //今日PVP活跃度
    USER_UNION_DAYSTAGELIVENESS,                //今日关卡活跃度
    USER_UNION_APPLYUNIONSTAMP,                 //申请公会时间戳
    USER_UNION_DAYAPPLAYCOUNT,                  //今日申请次数
    USER_UNION_WELFARE,                         //福利礼包领取状态
    USER_UNION_RESETSTAMP,                      //重置时间戳
	USER_UNION_MAX,								//最大值
};


class CUserUnionModel : public IDBModel
{
public:
	CUserUnionModel();
	~CUserUnionModel();

	bool init(int nUid);
	bool Refresh();

    bool setUserUnionValue(int nField, int nValue);
    bool setUserUnionValues(std::map<int, int> mapValue);
	bool getUserUnionValue(int nField, int &nValue);
	
	// 获得远征
	const std::vector<int>& getUserExpiditionValue();

	bool setUserExpiditionValue(std::vector<int> RewardValue);
    bool addUserUnionValue(int nField, int nValue = 1);
    // 添加申请公会信息
    bool addApplyInfo(int unionID, int liveTime);
    // 删除申请公会信息
    bool removeApplyInfo(int unionID);
    // 删除所有申请公会信息
    bool removeAllApplyInfo();
    // 获取申请公会信息
    std::map<int, int>& getApplyInfo(){ return m_MapApplyInfo; }
    // 是否有远征奖励
    bool hasExpiditionReward();
    // 设置信息(不存数据库)
    bool alterUserUnionValue(int nField, int nValue);
    bool alterUserUnionValueAdd(int nField, int nValue);

	//角色不在线时，添加角色奖励数据
	static bool setOffLineExpiditionValue(int nUid, std::vector<int> RewardValue);
    //获取不在线玩家信息
    static bool getOffLineUserUnionValue(int nUid, int nField, int &nValue);
    //设置不在线玩家信息
    static bool setOffLineUserUnionValue(int nUid, int nField, int nValue);
    //清理公会申请列表
    static bool removeOffLineAllApplyInfo(int nUid);
    //清理公会申请列表中的公会
    static bool removeOffLineApplyInfo(int nUid, int nUnionId);

private:
	bool saveUserRewardData();
private:

	int					 m_nUid;
	Storage *			 m_pStorage;
	std::string			 m_strUserUnionkey;
    std::map<int, int>	 m_MapValue;
    std::map<int, int>	 m_MapApplyInfo;     // 申请公会信息<unionID, livetime>
	std::vector<int>	 m_VectReardValue;
};


#endif //_USERUNIONMODEL_H__
