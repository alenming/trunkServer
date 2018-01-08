#ifndef __USER_PROTOCOL_H__
#define __USER_PROTOCOL_H__

#pragma pack(1)

enum USERPROTOCOL
{
	CMD_USER_CSBEGIN,
	CMD_USER_BUY_CS,		   //玩家数据购买
	CMD_USER_RENAME_CS,		   //玩家重命名
    CMD_USER_HEADICONCHANGE_CS,//更换头像
	CMD_USER_SIGN_CS,		   //签到
	CMD_USER_GIFTKEY_CS,		//礼包
	CMD_USER_FIRSTPAY_CS,		//首冲领取
	CMD_USR_FUND_CS,			//基金
	CMD_USR_MODIFYPS_CS,		//角色账号密码修改
	CMD_USER_CSEND,

    CMD_USER_SCBEGIN = 100,
	CMD_USER_BUY_SC,		   //玩家数据购买下发
	CMD_USER_RENAME_SC,		   //玩家重命名下发
    CMD_USER_HEADICONCHANGE_SC,//更换头像下发
	CMD_USER_SIGN_SC,		   //签到
	CMD_USER_GIFTKEY_SC,		//礼包
	CMD_USER_FIRSTPAY_SC,		//首冲领取
	CMD_USR_FUND_SC,			//基金
	CMD_USR_MODIFYPS_SC,		//角色账号密码修改

    CMD_USER_FORWARD_SS = 200,  //服务器内部转发数据到ServiceServer
	CMD_USER_SCEND,
};

enum UserAttrType
{
	ATTR_ENERGY,			//体力
	ATTR_GOLD,				//金币
	ATTR_DIAMOND,			//钻石
	ATTR_VIP,				//vip
    ATTR_MONTH_CARD,        //月卡
};

struct UserBuyCS
{
	int attrType;			//购买的属性
	int extend;			    //购买扩展字段(vip充值的面额,月卡的月数)
};

struct UserBuySC
{
	int attrType;			//购买的类型
	int addCount;			//增加的数量
	int result;				//结果
    int extend;				//金币购买为倍数,
};

struct UserRenameCS
{
	char userName[32];		//重命名
};

struct UserRenameSC
{
	int result;				//0失败,1成功
};

struct UserHeadIconCSC
{
    int headIconID;			//头像ID
};

struct SUserSignSC
{
	int nFlag;
};

struct SUserGiftKeyCS
{
	unsigned char cPatform;			//平台类型
	unsigned int nChannelID;		//渠道
	char GiftKey[17];
};

//struct SGoodsData
//{
//	int nGoodsID;
//	int nGoodsNum;
//};

struct SUserGiftKeySC
{
	int nNum;			//物品组数
};

//CMD_USER_FIRSTPAY_SC,		//首冲领取
struct SUserFirstPaySC
{
	int nFlag;
};

//CMD_USR_FUND_SC,			//基金
struct SUserFundCS
{
	int nBuyTime;				//购买时间
	int nGetTime;				//领取时间
};

//CMD_USR_MODIFYPS_CS
struct SUsrModifyPsCS
{
	int		 nChannelID;		//渠道ID
	char	 Ps[32];			//密码
};

struct SUsrModifyPsSC
{
	int nFlag;				//修改密码应答标记
};

#pragma pack()

#endif //__USER_PROTOCOL_H__
