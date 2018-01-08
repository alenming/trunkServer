/*
 * 邮件协议
*/

#ifndef __MAIL_PROTOCOL_H__
#define __MAIL_PROTOCOL_H__

#pragma pack(1)

enum MAILPROTOCOL
{
    CMD_MAIL_CSBEGIN,
    CMD_MAIL_READMAIL_CS,              // 读取邮件
	CMD_MAIL_GETMAILGOODS_CS,		   // 领取邮件
    CMD_MAIL_WEBMAIL_CS,               // 添加web邮件
    CMD_MAIL_CSEND,

    CMD_MAIL_SCBEGIN = 100,
    CMD_MAIL_READMAIL_SC,              // 回发读取邮件
	CMD_MAIL_GETMAILGOODS_SC,		   // 领取邮件
    CMD_MAIL_WEBMAIL_SC,               // 回发添加web邮件
    CMD_MAIL_SENDMAIL_SC,              // 服务器发送新邮件

    CMD_MAIL_UPDATEMAIL_SS,            // 更新邮件模型
    CMD_MAIL_SCEND
};

// CMD_MAIL_READMAIL_CS
struct ReadMailCS
{
	int mailType;						 // 邮件类型 EMailType
	int mailID;							 // 邮件id
};

// CMD_MAIL_READMAIL_SC
struct ReadMailSC
{
	int mailType;					     // 邮件类型
	int mailID;							 // 邮件id
	int nCount;							 // 邮件附件物品
	int nContextLen;					 // 邮件内容长度
	//DropItemInfo
	//邮件内容
};

struct GetMailGoodsCS
{
	char normalMailCount;                // 普通邮件数量
	char webMailCount;                   // web邮件数量
	// normalMailCount ->int 邮件ID列表
	// webMailCount ->int WEB邮件ID列表
};

struct GetMailGoodsSC
{
	char normalMailCount;                // 普通邮件数量
	char webMailCount;                   // web邮件数量
	char itemCount;                      // 是否有奖励
	// normalMailCount ->int 邮件ID列表
	// webMailCount ->int WEB邮件ID列表
	// itemCount ->DropItemInfo
};


// CMD_MAIL_WEBMAIL_CS
struct WebMailCS
{
    char title[16];                      // 标题
    char sender[16];                     // 来自
    char content[128];                   // 内容
    int receiver;                        // 全服或特定玩家
    int liveTime;                        // 保质期(天)
    int itemCount;                       // 道具个数
    // itemCount ->DropItemInfo
};

// CMD_MAIL_WEBMAIL_SC
struct WebMailSC
{
    char result;
};

// CMD_MAIL_SENDMAIL_SC
struct SendMailSC
{
    char mailType;                      // 邮件类型EMailType
	int  nMailID;						// 邮件ID
    // 普通邮件 ->BaseMailInfo
    // web邮件 ->BaseMailInfo
};

#pragma pack()

#endif //__MAIL_PROTOCOL_H__