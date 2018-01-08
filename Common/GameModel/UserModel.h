#ifndef __USER_MODEL_H__
#define __USER_MODEL_H__

#include <string>
#include <map>
#include "IDBModel.h"

enum USERFIELD
{
    USR_FD_NONE,			    //
	USR_FD_USERNAME,		    // 名称
    USR_FD_USERID = 10,         // id
    USR_FD_USERLV,			    // 等级
    USR_FD_EXP,				    // 经验
    USR_FD_GOLD,			    // 金币
    USR_FD_DIAMOND,			    // 钻石
    USR_FD_ENERGY,			    // 体力(1.5版本去除)
    USR_FD_TOWERCOIN,		    // 塔币
    USR_FD_PVPCOIN,			    // PVP币
    USR_FD_UNIONCONTRIB,	    // 公会贡献
    USR_FD_FLASHCARD,		    // 卡券
	USR_FD_FLASHCARD10,			// 十连抽卡卷

    USR_FD_HEADICON,            // 头像
    USR_FD_HEROREF,			    // 英雄id次数
    USR_FD_EQUIPREF,		    // 装备id计数
    USR_FD_MAILREF,             // 邮件id计数
    USR_FD_BAGCAPACITY,		    // 背包格子个数
    USR_FD_HEROCAPCITY,		    // 英雄卡包个数
    USR_FD_FREEHEROTIMES,	    // 免费英雄抽取次数

    USR_FD_EXTRAHEROTIMES,	    // 抽取英雄次数
    USR_FD_EXTRAHERTOTALTIMES,  // 抽取英雄总次数
    USR_FD_BUYGOLDTIMES,        // 购买金币次数
    USR_FD_BUYENERGYTIMES,      // 购买体力次数
    USR_FD_ACCUMULATEGOLD,      // 累计获得金币

    USR_FD_RESETTIMESTAMP,      // 恢复时间戳(包括金币购买次数, 体力购买次数, 体力恢复)
    USR_FD_FREERENAME,          // 改名字(0为免费,1需要费用)

    USR_FD_LOGINTIME,		    // 角色登陆时间
    USR_FD_LOGINOUTTIME,	    // 角色登出时间
    USR_FD_CREATETIME,		    // 角色创建时间
	USR_FD_LOGIN_DAY,			// 累计登陆天数
	USR_FC_CONLOGIN_DAY,		// 连续登陆天数

    USR_FD_PAYMENT,				//充值数额
    USR_FD_MONTHCARDSTAMP,      // 月卡过期时间

    USR_FD_BANTIME,			    //角色封停截止时间戳
	USR_FD_SIGNDAY,				//累计签到天数
	USR_FD_MONTHDAY,			//当月累计签到天数
	USR_FD_DAYFLAG,				//当天签到标识
	USR_FD_INCSIGNFLAG,			//累计签到次数标识
	USR_FD_PAYTIMES,			//充值次数

	//累计充值领取标识
	USR_FD_FIRSTPAYFLAG,		//首冲领取标识
	//基金
	USR_FD_FUND_STARTTIME,		//基金开始标记
	USR_FD_FUND_GETTIME,		//基金领取标记

    USR_FD_BILLNO,              // 当前正在支付的流水号 - 默认为0或空
	USR_FD_QQVIPLV,				//蓝钻等级
	USR_FD_QQVIPTIMESTAMP,		//蓝钻结束时间点
	USR_FD_QQVIPTYPE,			//蓝钻类型 1,3,5,7
	USR_FD_END
};

class Storage;
class CUserModel : public IDBModel
{
public:
	CUserModel();
	~CUserModel();

public:

	bool init(int uid);
	// 刷新用户数据
	bool Refresh();
	// 新用户
	bool NewUser(int uid, std::string name, std::map<int, int> &info);
	// 获得用户名字
	std::string& GetName();
	// 获得用户基本信息
	std::map<int, int>& GetUserInfo();
	// 获得用户指定信息
	bool GetUserFieldVal(int field, int &value,bool bNew = false);
	// 批量获得用户数据接口
	bool GetUserFieldVal(std::map<int, int> &kvs, bool bNew = false);
	// 修改用户指定信息
	bool SetUserFieldVal(int field, int value);
	// 增加某些属性的值 注意不要乱用, 比如头像, 名字等等
	bool AddUserFieldVal(int field, int value);
	// 递增某个指定值, 返回递增之后的值
	int IncreaseFieldVal(int field, int increase = 1);
	// 批量处理
	bool SetUserFieldVal(std::map<int, int> &kvs);
	// 修改用户名
	bool ModUserName(std::string name);
    // 修改用户名(不存数据库)
    void AlterUserName(std::string name) { m_strUserName = name; }
    // 修改用户信息(不存数据库)
    bool AlterUserFieldVal(int field, int value);
	// 删除玩家数据
	bool DeleteUser();
    // 设置用户基本信息
    bool SetUserInfo(std::map<int, int> &userInfo);
    int GetUserID(){ return m_nUid; }
    // 获取身份(蓝钻)信息(等级*10+类型)
    int getIdentity();

protected:

	bool GetRealDataFromDB(int field, int &value);

private:

	int						m_nUid;
	Storage *				m_pStorage;
	std::string				m_strUsrKey;
	std::string				m_strUserName;
	std::map<int, int>		m_mapUserInfo;
};

#endif //__USER_MODEL_H__
