/*
* 邮件模型
* 1.存储背包不足时发送的邮件
* 2.存储竞技等活动某时间发送的邮件
* 3.存储已经领取手动(网页上填写)发送的邮件
*   数据存放于ServerModel,登录的玩家才会下发,邮件过期了再从模型移除
*/

#ifndef __MAIL_MODEL_H__
#define __MAIL_MODEL_H__

#include "Storage.h"
#include "IDBModel.h"
#include "UnionComm.h"
#include "ItemDrop.h"
#include "CommStructs.h"

#include <map>

class CMailModel : public IDBModel
{
public:
    CMailModel();
    ~CMailModel();

    bool init(int uid);
	// 刷新
    bool Refresh();
    // 添加新邮件
    bool AddNewMail(const BaseMailInfo& info, const std::vector<DropItemInfo> &items,int &nMailID);
    // 添加新邮件(没有物品)
    bool AddNewMail(const BaseMailInfo& info, int &nMailID);
    // 获取邮件信息
    bool GetMailInfo(int mailID, BaseMailInfo &info, std::vector<DropItemInfo> &items,bool bNew = false);
    // 读邮件,设置邮件为已读状态
    bool ReadMail(int mailID);
    // 移除邮件
    bool RemoveMail(int mailID);
    // 添加读取的web邮件
	bool AddWebMail(int webMailID, int nState);
    // 移除web邮件
    bool RemoveWebMail(int webMailID);
	// 是否度过webMail
	bool IsReadWebMail(int webMailId);
    // 获取所有邮件基础信息
    std::map<int, BaseMailInfo>& GetMailBasicInfo();
    // 获取所有邮件道具信息
    std::map<int, std::vector<DropItemInfo> >& GetMailItemInfo();
    // 获取已经读取的web邮件
	std::map<int, int>& GetWebMails();
	//获取所有自定义邮件内容
	std::map<int, std::string>& GetMailContextInfo();
    // 删除邮件
    bool DeleteMail();

    // 添加不在线玩家邮件
    static bool AddOfflineNewMail(int uid, const BaseMailInfo& info, const std::vector<DropItemInfo> &items);

private:

    void ClearData();

    bool SaveMailData(int mailID);

	bool GetMailsFromDB();

	bool GetMailsFromDB(int nMailID);

private:

    int                                         m_nUID;             // 用户ID
	Storage*                                    m_pStorage;         // 数据库
	std::string									m_NormalMailKey;	// 普通邮件Key
    std::map<int, BaseMailInfo>					m_MailInfoMap;      // <邮件ID, 邮件信息>
    std::map<int, std::vector<DropItemInfo> >   m_MailItemInfoMap;  // <邮件ID, 所有的道具列表>
	std::map<int,std::string>					m_MailContextMap;	// 邮件内容
	std::map<int, int>							m_GMMailStatMap;	// GM邮件状态

};

#endif