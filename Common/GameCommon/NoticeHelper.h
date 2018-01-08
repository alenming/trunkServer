/*
* 通知辅助类
* 1、提供发送全服接口
*/

#ifndef __NOTICE_HELPER_H__
#define __NOTICE_HELPER_H__

#include "KxCommInterfaces.h"
#include "NoticeProtocol.h"
#include <string>

class CNoticeHelper
{
public:
    static void noticeAll(int uid, char *buff, int len, KxServer::IKxComm *target);

    // 1个字符串通知,例如XXX退出公会
    static int getStringNoticeInfo(char *buff, int noticeId, std::string szVal);
    // 1个字符串+1个int通知,例如XXX获得YY道具
    static int getStringIntNoticeInfo(char *buff, int noticeId, std::string szVal, int iVal);
    static int getStringIntIntNoticeInfo(char *buff, int noticeId, std::string szVal, int iVal1, int iVal2);
    // 2个字符串通知,例如XXX会长转职给YYY
    static int getStringStringNoticeInfo(char *buff, int noticeId, std::string szValOne, std::string szValTwo);

    static int getStringStringInfoLen(int szLen1, int szLen2);
    static int getStringIntIntInfoLen(int szLen);
    static int getStringIntInfoLen(int szLen);
    static int getStringInfoLen(int szLen);
    static int getIntInfoLen();

private:
    static void getStringParamContent(char *buff, int &buffLen, std::string szVal);
    static void getIntParamContent(char *buff, int &buffLen, int iVal);
};

#endif // __NOTICE_HELPER_H__
