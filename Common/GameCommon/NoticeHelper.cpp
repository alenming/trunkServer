#include "NoticeHelper.h"
#include "Protocol.h"
#include "KxMemPool.h"
#include "ServerProtocol.h"
#include "KxCommManager.h"
#include "GameUserManager.h"

using namespace KxServer;

int CNoticeHelper::getStringNoticeInfo(char *buff, int noticeId, std::string szVal)
{
    int nLen = 0;
    NoticeInfo *pNoticeInfo = reinterpret_cast<NoticeInfo*>(buff);
    pNoticeInfo->noticeId = noticeId;
    pNoticeInfo->paramCount = 1;
    nLen += sizeof(NoticeInfo);

    // ×Ö·û´®
    getStringParamContent(buff + nLen, nLen, szVal);

    return nLen;
}

int CNoticeHelper::getStringIntNoticeInfo(char *buff, int noticeId, std::string szVal, int iVal)
{
    int nLen = 0;
    NoticeInfo *pNoticeInfo = reinterpret_cast<NoticeInfo*>(buff);
    pNoticeInfo->noticeId = noticeId;
    pNoticeInfo->paramCount = 2;
    nLen += sizeof(NoticeInfo);

    // ×Ö·û´®
    getStringParamContent(buff + nLen, nLen, szVal);
    // INT
    getIntParamContent(buff + nLen, nLen, iVal);

    return nLen;
}

int CNoticeHelper::getStringIntIntNoticeInfo(char *buff, int noticeId, std::string szVal, int iVal1, int iVal2)
{
    int nLen = 0;
    NoticeInfo *pNoticeInfo = reinterpret_cast<NoticeInfo*>(buff);
    pNoticeInfo->noticeId = noticeId;
    pNoticeInfo->paramCount = 3;
    nLen += sizeof(NoticeInfo);

    // ×Ö·û´®
    getStringParamContent(buff + nLen, nLen, szVal);
    // INT1
    getIntParamContent(buff + nLen, nLen, iVal1);
    // INT2
    getIntParamContent(buff + nLen, nLen, iVal2);

    return nLen;
}

int CNoticeHelper::getStringStringNoticeInfo(char *buff, int noticeId, std::string szValOne, std::string szValTwo)
{
    int nLen = 0;
    NoticeInfo *pNoticeInfo = reinterpret_cast<NoticeInfo*>(buff);
    pNoticeInfo->noticeId = noticeId;
    pNoticeInfo->paramCount = 2;
    nLen += sizeof(NoticeInfo);

    // ×Ö·û´®1
    getStringParamContent(buff + nLen, nLen, szValOne);
    // ×Ö·û´®2
    getStringParamContent(buff + nLen, nLen, szValTwo);

    return nLen;
}

int CNoticeHelper::getStringStringInfoLen(int szLen1, int szLen2)
{
    return sizeof(NoticeInfo)+2 * (sizeof(NoticeParamContent)+sizeof(short)) + szLen1 + szLen2;
}

int CNoticeHelper::getStringIntIntInfoLen(int szLen)
{
    return sizeof(NoticeInfo)+3 * sizeof(NoticeParamContent) + 2*sizeof(int)+sizeof(short)+szLen;
}

int CNoticeHelper::getStringIntInfoLen(int szLen)
{
    return sizeof(NoticeInfo)+2*sizeof(NoticeParamContent)+sizeof(short)+szLen+sizeof(int);
}

int CNoticeHelper::getStringInfoLen(int szLen)
{
    return sizeof(NoticeInfo)+sizeof(NoticeParamContent)+sizeof(short)+szLen;
}

int CNoticeHelper::getIntInfoLen()
{
    return sizeof(NoticeInfo)+sizeof(NoticeParamContent)+sizeof(int);
}

void CNoticeHelper::getStringParamContent(char *buff, int &buffLen, std::string szVal)
{
    NoticeParamContent *pStringParamContent = reinterpret_cast<NoticeParamContent*>(buff);
    pStringParamContent->paramType = PARAM_TYPE_STRING;
    buffLen += sizeof(NoticeParamContent);

    short *pLen = reinterpret_cast<short*>(pStringParamContent + 1);
    *pLen = szVal.length() + 1;
    buffLen += sizeof(short);

    char *pData = reinterpret_cast<char*>(pLen + 1);
    memcpy(pData, szVal.c_str(), *pLen);
    buffLen += *pLen;
}

void CNoticeHelper::getIntParamContent(char *buff, int &buffLen, int iVal)
{
    NoticeParamContent *pIntParamContent = reinterpret_cast<NoticeParamContent*>(buff);
    pIntParamContent->paramType = PARAM_TYPE_INT;
    buffLen += sizeof(NoticeParamContent);

    int *pIntData = reinterpret_cast<int*>(pIntParamContent + 1);
    *pIntData = iVal;
    buffLen += sizeof(int);
}

void CNoticeHelper::noticeAll(int uid, char *buff, int len, KxServer::IKxComm *target)
{
    CKxCommManager::getInstance()->forwardData(uid, CMD_NOTICE, CMD_NOTICE_SS, buff, len, target);
}
