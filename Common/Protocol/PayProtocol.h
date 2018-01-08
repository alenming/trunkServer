/*
*   支付协议
*
*   2016-6-29 by 宝爷
*/
#ifndef __PAY_PROTOCOL_H__
#define __PAY_PROTOCOL_H__

#pragma pack(1)

enum PAYROTOCOL
{
    CMD_PAY_CSBEGIN,
    CMD_PAY_YSDK_CS,
    CMD_PAY_CSEND,

    CMD_PAY_SCBEGIN = 100,
    CMD_PAY_YSDK_SC,
    CMD_PAY_SCEND,
};

//////////////////////////////////腾讯支付////////////////////////////////////////

struct PayYSDKSC
{
	int result;         // 充值结果 0 失败 -1不存在，1-成功
	int nVipLv;			//Vip等级
	int nVipNum;		//Vip充值数额
	int nDiamond;		//总钻石数
	int nPid;			//充值物品编号
};

#pragma pack()

#endif