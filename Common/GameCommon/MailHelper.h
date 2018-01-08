#ifndef __Mail_HELPER_H__
#define __Mail_HELPER_H__

#include <vector>
#include <string>

struct DropItemInfo;
struct BaseMailInfo;
class CMailHelper
{
public:
    // 保存邮件并且发送
    static bool saveAndSendBagFullMail(int uid, std::vector<DropItemInfo> &dropItem);
    // 发送普通邮件
	static bool sendNormalMail(int uid, const BaseMailInfo &noramlMailInfo, int &nMailID);
	//PVP每日邮件
	static bool sendPvpRankMail(int uid, std::vector<DropItemInfo> &dropItem);
    // 公会邮件
    static bool sendUnionMail(int uid, int configID, std::string unionName);
	//爬塔邮件
	static bool sendTowerMail(int uid, std::vector<DropItemInfo> &dropItem);
};

#endif //__Mail_HELPER_H__
