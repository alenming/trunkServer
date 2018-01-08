/*
 * 公会协议
*/

#ifndef __UNION_PROTOCOL_H__
#define __UNION_PROTOCOL_H__

#pragma pack(1)

enum UNIONPROTOCOL
{
    CMD_UNION_CSBEGIN,
    CMD_UNION_INFO_CS,			// 玩家所在公会信息
    CMD_UNION_MEMBERS_CS,		// 玩家所在公会所有成员
    CMD_UNION_CREATE_CS,		// 创建公会
    CMD_UNION_APPLY_CS,			// 申请加入公会
    CMD_UNION_EXIT_CS,			// 退会
    CMD_UNION_LOGLIST_CS,		// 玩家所在公会事件列表
    CMD_UNION_AUDITLIST_CS,		// 玩家所在公会审核列表
    CMD_UNION_AUDIT_CS,		    // 审核信息
    CMD_UNION_FUNCTION_CS,		// 公会相关操作(踢出、撤任、权利移交等)
    CMD_UNION_UNIONLIST_OUT_CS,	// 公会列表
    CMD_UNION_SEARCH_CS,		// 搜索公会
    CMD_UNION_EMBLEM_CS,		// 设置会徽
    CMD_UNION_NAME_CS,  		// 更改公会名
    CMD_UNION_NOTICE_CS,  		// 更改公告
    CMD_UNION_WELFARE_CS,  		// 福利领取
    CMD_UNION_SETAUDIT_CS,      // 设置审核
	CMD_UNION_ALLMERCENARY_CS,	// 所有佣兵信息
	CMD_UNION_DISPATCH_CS,		// 派遣佣兵
	CMD_UNION_RECALLMER_CS,		// 召回佣兵
	CMD_UNION_SINGLEMER_CS,		// 请求单个佣兵详细信息
    CMD_UNION_CSEND,

    CMD_UNION_SCBEGIN = 100,
    CMD_UNION_INFO_SC,	        // 回发公会信息
    CMD_UNION_MEMBERS_SC,		// 回发公会成员
    CMD_UNION_CREATE_SC,		// 回发创建公会
    CMD_UNION_APPLY_SC,			// 回发申请状态
    CMD_UNION_EXIT_SC,			// 回发退会
    CMD_UNION_LOGLIST_SC,		// 回发公会事件列表
    CMD_UNION_AUDITLIST_SC,		// 回发公会审核列表
    CMD_UNION_AUDIT_SC,		    // 回发审核信息
    CMD_UNION_FUNCTION_SC,		// 回发公会相关操作(踢出、撤任、权利移交等)
    CMD_UNION_UNIONLIST_OUT_SC, // 回发公会列表
    CMD_UNION_SEARCH_SC,		// 回发搜索公会
    CMD_UNION_EMBLEM_SC,		// 回发设置会徽
    CMD_UNION_NAME_SC,  		// 回发更改公会名
    CMD_UNION_NOTICE_SC,  		// 回发更改公告
    CMD_UNION_WELFARE_SC,  		// 回发福利领取
    CMD_UNION_SETAUDIT_SC,      // 回发设置审核

    CMD_UNION_BEAUDIT_SC,       // 被审核者应答
    CMD_UNION_BEFUNCTION_SC,    // 被操作者应答
	CMD_UNION_MERCENARY_SC,		// 公会佣兵
    CMD_UNION_MESSAGE_SC,       // 公会信息通知(前端红点)
    CMD_UNION_FORWARD_SS,       // 其他服务器给公会服转发的数据
	CMD_UNION_RECALL_SC,		// 召回应答
	CMD_UNION_SINGELMER_SC,		// 应答单个佣兵详细信息

    CMD_UNION_WELFARE_SS = 200, // 领取福利转发ServiceServier
    CMD_UNION_SCEND
};

//CMD_UNION_INFO_CS 无包体
//CMD_UNION_INFO_SC
//UnionInfo

struct OnlineMemberInfo
{
    int userId;                 // 玩家ID
    int lv;                     // 等级
    int identity;               // 身份(如蓝钻,蓝钻等级*10+蓝钻类型)
    char pos;                   // 职位
    char userName[20];          // 用户名
};

struct UnionInfo
{
    int unionID;                // ID
    int unionLv;                // 等级
    int liveness;               // 活跃度
    int unionRank;              // 声望排行
    int reputation;             // 公会声望
    int limitLv;                // 申请等级限制
    int emblem;                 // 会徽
    int isAutoAudit;            // 是否自动审核
    int dangerousTag;           // 危险标识
    int welfareTag;             // 福利领取标识
    int chairIdentity;          // 会长身份(如蓝钻,蓝钻等级*10+蓝钻类型)
    char pos;                   // 请求者的职位
    char memberCount;           // 成员人数
    char onlineMemberCount;     // 在线人数
    char unionName[20];         // 公会名称
    char chairmanName[20];      // 会长名称
    char notice[128];           // 公告
    // OnlineMemberInfo列表
};

//CMD_UNION_MEMBERS_CS
struct UnionMembersCS
{
    int unionID;                // 公会ID
};

//CMD_UNION_MEMBERS_SC
struct UnionMembersSC
{
    int memberCount;            // 成员数
    // memberCount ->UnionMembersInfo
};

struct UnionMembersInfo
{
    int userID;                 // 玩家ID
    int totalContrib;           // 累计贡献
    char position;              // 职位
    int identity;               // 身份(如蓝钻,蓝钻等级*10+蓝钻类型)
    int userLv;                 // 等级
    int todayLiveness;          // 今日活跃度
    int lastLoginTime;          // 最后登录时间戳
    char userName[20];          // 玩家名称
};

//CMD_UNION_CREATE_CS
struct UnionCreateCS
{
    char unionName[20];         // 公会名称
};

//CMD_UNION_CREATE_SC
struct UnionCreateSC
{
    char result;                // 创建结果 0失败 1成功
    int extend;                 // 失败->相应的错误码 成功->公会ID
};

//CMD_UNION_APPLY_CS
struct UnionApplyCS
{
    int unionID;                // 公会ID
};

//CMD_UNION_APPLY_SC
struct UnionApplySC
{
    char errorCode;             // 错误码
    int unionID;                // 申请的公会ID
};

//CMD_UNION_EXIT_CS  无包体
//CMD_UNION_EXIT_SC
struct UnionExitSC
{
    int timeStamp;              // 时间戳 >0为退出成功,下次申请公会的时间戳
};

//CMD_UNION_LOGLIST_CS  无包体
//CMD_UNION_LOGLIST_SC
struct UnionLogListSC
{
    char logCount;             // 事件数
    // logCount ->UnionLogInfo
};

//CMD_UNION_AUDITLIST_CS  无包体
//CMD_UNION_AUDITLIST_SC
struct UnionAuditListSC
{
    char auditsCount;           // 审核数量
    // auditsCount ->AuditsInfo
};

struct AuditsInfo
{
    int userID;                 // 玩家ID
    int userLv;                 // 玩家等级
    int identity;               // 身份(如蓝钻,蓝钻等级*10+蓝钻类型)
    char userName[20];          // 玩家名称
};

//CMD_UNION_AUDIT_CS
struct UnionAuditCS
{
    char isAgree;               //是否同意 0不同意 1同意
    char auditCount;            //审核个数
    // auditCount ->int array
};

//CMD_UNION_AUDIT_SC
struct UnionAuditSC
{
    char result;                //相关错误码
    char agreeCount;            //同意加入公会的人数
    char refuseCount;           //拒绝加入公会的人数
    // agreeCount ->userid
    // refuseCount ->userid
};

//被审核者审核通过时接受消息
//CMD_UNION_BEAUDIT_SC
//OwnUnionInfo

//CMD_UNION_FUNCTION_CS
struct UnionFunctionCS
{
    char funcType;              // 功能类型
    int userID;                 // 玩家ID
};

//CMD_UNION_FUNCTION_SC
struct UnionFunctionSC
{
    char result;                // 0失败 1成功
    char funcType;              // 功能类型
    int userID;                 // 玩家ID
};

//被操作者应答
//CMD_UNION_BEFUNCTION_SC
struct UnionBeFunctionSC
{
    char funcType;              // 功能类型
    char unionName[20];         // 公会名字
};

//CMD_UNION_UNIONLIST_OUT_CS
struct UnionListOutCS
{
    char type;                  // 请求类型：排名，未满员
    char count;                 // 请求的公会个数
    int orignNum;               // 从排名第几开始
};

struct UnionListInfo
{
    int unionID;                // ID
    int unionLv;                // 等级
    int limitLv;                // 申请等级限制
    int emblem;                 // 会徽
    int identity;               // 会长身份(如蓝钻,蓝钻等级*10+蓝钻类型)
    char memberCount;           // 成员人数
    char unionName[20];         // 公会名称
    char chairmanName[20];      // 会长名称
    char notice[128];           // 公告
};

//CMD_UNION_UNIONLIST_OUT_SC
struct UnionListOutSC
{
    char unionCount;            // 返回公会个数
    // unionCount ->UnionListInfo
};

//CMD_UNION_SEARCH_CS
struct UnionSearchCS
{
    char searchName[20];        // 搜索名字,可能是ID或名称
};

//CMD_UNION_SEARCH_SC
struct UnionSearchSC
{
    char unionCount;            // 返回公会个数
    // unionCount ->UnionListInfo
};

//CMD_UNION_EMBLEM_CS
struct UnionEmblemCS
{
    int emblemId;              // 会徽id
};

//CMD_UNION_EMBLEM_SC
struct UnionEmblemSC
{
    int emblemId;              // 会徽id为0失败
};

//CMD_UNION_NAME_CS
struct UnionNameCS
{
    char unionName[20];         // 公会名称
};

//CMD_UNION_NAME_SC
struct UnionNameSC
{
    char result;                // 创建结果 0失败 1成功
};

//CMD_UNION_NOTICE_CS
struct UnionNoticeCS
{
    char notice[128];           // 公告
};

//CMD_UNION_NOTICE_SC
struct UnionNoticeSC
{
    char result;                // 创建结果 0失败 1成功
};

//CMD_UNION_WELFARE_CS
struct UnionWelfareCS
{
    char welfareType;           // 福利类型
};

//CMD_UNION_WELFARE_SC
struct UnionWelfareSC
{
    char welfareType;           //领取福利的类型
    int itemCount;			    //物品个数
    //... DropItemInfo
};

//CMD_UNION_SETAUDIT_CS
struct UnionSetAuditCS
{
    int limitLv;                //限制等级
    int isAuto;                 //是否自动通过
};

//CMD_UNION_SETAUDIT_SC
struct UnionSetAuditSC
{
    char result;                //结果0失败 1成功
    int limitLv;                //限制等级
    int isAuto;                 //是否自动通过
};

enum MercenaryProtocol
{
	ApplyAllMercenaryInfo,
	DispatchMercenary,
	RecallMercenary,
	ApplySingleMercenary,
};

//CMD_UNION_MESSAGE_SC
struct UnionMessageSC
{
    int type;                   //类型 0活跃度 1审核
};

//CMD_UNION_FORWARD_SS
struct UnionForwardSS
{
    int type;                   //类型EUnionForwardData
    int val;                    //值
    char szExtend[20];          //字符串
};

//CMD_UNION_WELFARE_SS
struct UnionWelfareSS
{
    char welfareType;           //福利类型
    int dropId;                 //掉落id
};

#pragma pack()

#endif //__UNION_PROTOCOL_H__