#ifndef __NOTICE_PROTOCOL_H__
#define __NOTICE_PROTOCOL_H__

#pragma pack(1)

enum NOTICEPROTOCOL
{
	CMD_NOTICE_CSBEGIN,
	CMD_NOTICE_CSEND,
        
    CMD_NOTICE_SCBEGIN = 100,
    CMD_NOTICE_SC,                 // 发送通知
    CMD_NOTICE_SS,                 // 服务器内部转发
	CMD_NOTICE_SCEND,
};

// 参数类型
enum EParamType
{
    PARAM_TYPE_INT,
    PARAM_TYPE_STRING,
};

// CMD_NOTICE_SC
struct NoticeSC
{
    char noticeCount;
};

// 通知信息
struct NoticeInfo
{
    int noticeId;
    char paramCount;
};

// 通知参数内容
struct NoticeParamContent
{
    char paramType;
    // int
    // short->len string
};

#pragma pack()

#endif //__NOTICE_PROTOCOL_H__
