#ifndef _ACTIVE_PROTOCOL_H__
#define _ACTIVE_PROTOCOL_H__

#pragma pack(1)

enum ACTIVEPROTOCOL
{
	CMD_ACTIVE_CSBEGIN,
	CMD_ACTIVE_GET_CS,				//活动领取
	CMD_ACTIVE_SEVENDAY_CS,			//七天活动领取
	CMD_ACTIVE_BLUEDIMAND_CS,		//蓝钻活动领取
	CMD_EXCHANGE_GET_CS,			//兑换活动领取
	CMD_ACTIVE_CSEND,

	CMD_ACTIVE_SCBEGIN = 100,
	CMD_ACTIVE_GET_SC,				//活动领取
	CMD_ACTIVE_TASKUPDATE_SC,		//活动数据更新,对于任务类型活动的条件参数数据更新
	CMD_ACTIVE_SEVENDAY_SC,			//七天活动领取
	CMD_ACTIVE_BLUEDIMAND_SC,		//蓝钻活动领取
	CMD_EXCHANGE_GET_SC,			//兑换活动领取
	CMD_ACTIVE_SCEND,
};

////CMD_ACTIVE_FRESH_CS
//struct SActiveFreshCS
//{
//	int nActiveID;
//};
//
////CMD_ACTIVE_FRESH_SC,	  //活动数据刷新
//struct SActiveFreshSC
//{
//	int nActiveID;
//	int nActiveType;
//	int nStartTime;		//开始时间
//	int nEndTime;		//结束时间
//	//具体数据 SActiveFreshActiveShop/SLoginActiveTaskData
//};
//
//struct SActiveFreshActiveShop
//{
//	int nShopNum;				//礼包个数
//	//礼包具体数据 SLoginActiveShopData
//};
//
//struct SActiveFreshShopData
//{
//	int nGoodsID[4];
//	int nGoodsNum[4];
//	int nGoldType;					//货币类型
//	int nPrice;
//	int nSaleRate;
//	int nMaxBuyTimes;
//	int nBuyTimes;					//已经购买次数
//};
//
//struct SActiveFreshTask
//{
//	int nActiveTaskNum;				//活动任务数
//};
//
//struct SActiveFreshTaskData
//{
//	int			nFinishCondition;
//	int			nConditionParam[2];
//	int			nRewardDimand;
//	int			nRewardGold;
//	int			nRewardEnergy;
//	int			nRewardGoodsID;
//	int			nRewardGoodsNum;
//	int			nValue;					//任务完成进度
//	int			nFinishFlag;			//是否领取，0-未领取，1-领取
//};

//CMD_ACTIVE_SEVENDAY_CS
//CMD_ACTIVE_GET_CS
struct SActiveGet_CS
{
	unsigned short sActiveID;
	unsigned char  cParamID;			//礼包ID/任务ID
};

//CMD_ACTIVE_GET_SC
//CMD_ACTIVE_SEVENDAY_CS
struct SActiveGet_SC
{
	unsigned short sActiveID;
	unsigned char  cParamID;			//礼包ID/任务ID
	unsigned char  cFlag;				
};

//CMD_ACTIVE_TASKUPDATE_SC
struct SActiveTaskUpdateSC
{
	unsigned short sActiveID;
	unsigned short sCount;				//个数
	//SActiveTaskUpdateData
};

struct SActiveTaskUpdateData
{
	unsigned int  nParam;			//参数更新
	unsigned char  cID;				//活动任务ID
};


#pragma pack()

#endif //_ACTIVE_PROTOCOL_H__
