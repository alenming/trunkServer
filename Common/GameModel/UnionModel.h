/*
 * 公会模型
 * 1.公会具体的信息(ID,会长ID,公会名称,公会等级等)
 * 2.审核的信息列表
 * 3.事件的信息列表
 * 4.成员列表
 */

#ifndef __UNION_MODEL_H__
#define __UNION_MODEL_H__

#include <map>
#include "IDBModel.h"
#include "Storage.h"
#include "RedisStorer.h"

enum EUnionField
{
    UNION_FIELD_NON,
    UNION_FIELD_UNIONNAME,          // 公会名
    UNION_FIELD_NOTICE,             // 公会公告

    UNION_FIELD_LV = 10,            // 等级(10以免以后补充新的字段导致覆盖)
    UNION_FIELD_LIVENESS,           // 活跃度
    UNION_FIELD_CHAIRMAN,           // 会长ID
    UNION_FIELD_LIMITLV,            // 申请公会等级限制
    UNION_FIELD_REPUTATION,         // 公会声望
    UNION_FIELD_EMBLEM,             // 会徽
    UNION_FIELD_LOGIDREF,           // 自增日志id
    UNION_FIELD_AUTOAUDIT,          // 是否自动审核
    UNION_FIELD_DANGEROUSTAG,       // 危险标识(一天活跃度为0加1)
    UNION_FIELD_ORIGINLV,           // 今日原始公会等级

    UNION_FIELD_MAX,
};

enum
{
	SRV_FD_NONE,
    SRV_FD_UNIONREF,				// 公会ID索引
    SRV_FD_DAYRESETSTAMP,           // 日重置时间戳
	SRV_FD_END,
};

struct UnionLogInfo
{
    int eventType;                   // 事件类型(加入公会、退出公会等EUnionFuncType)
    int timeStamp;                   // 时间戳
    char userName1[32];              // 主动者玩家名字
    char userName2[32];              // 被动者玩家名字
    int extend;                      // 扩展(分配物品ID等)
};

class CUnionModel : public IDBModel
{
public:
    CUnionModel();
    ~CUnionModel();

    //创建服务器所有公会
    static bool CreateUnionList(int nServerID);

    //删除服务器所有公会
    static bool CloseUnionList(int nServerID);

    static std::vector<int>& GetUnionIDList() { return m_VectUnionID; }

    bool init(int unionID);
    // 新公会模型
    bool NewUnion(int unionID, int chairmanID, std::string unionName);
	// 刷新公会数据
	bool Refresh();
    // 获取公会信息
    bool GetUnionFieldVal(int field, int &value);
    // 设置公会信息
    bool SetUnionFieldVal(int field, int value);
    bool SetUnionFieldVals(std::map<int, int> info);
    bool AddUnionFieldVal(int field, int value);
    // 添加成员
    bool AddMember(int userID, int pos);
    // 添加审核信息
    bool AddAudit(int uid, int auditTime);
    // 添加事件信息
    bool AddLog(UnionLogInfo& info);
    // 移除成员
    bool RemoveMember(int userID);
    // 移除审核信息
    bool RemoveAudit(int uid);
    // 移除事件信息
    bool RemoveLog(int logid);
    // 获取成员列表
    std::map<int, int>& GetMembers(){ return m_MemberMap; }
    void GetMembers(std::vector<int> &members);
    // 获取审核信息列表
    std::map<int, int>& GetAudits(){ return m_AuditMap; }
    // 获取事件信息列表
    std::map<int, UnionLogInfo>& GetLogs() { return m_LogMap; }
    // 设置成员职位
    bool SetMemberPos(int userID, int pos);
    // 获取成员职位
    int GetMemberPos(int userID);
    bool MemberExist(int userID);
    // 获取审核信息
    bool GetAuditInfo(int uid, int &auditTime);
    // 设置公会名字
    bool SetUnionName(std::string unionName);
    // 设置公会公告
    bool SetUnionNotice(std::string unionNotice);
    // 获取公会名字
    std::string GetUnionName(){ return m_UnionName; }
    // 获取公会公告
    std::string GetUnionNotice(){ return m_UnionNotice; }
    // 公会ID
    int GetUnionID(){ return m_nUnionID; }
    // 删除公会
    bool DeleteUnion();

    /////////////////////////////通用///////////////////////
    static bool addUnion(int nServerID, int unionID);

    static bool deleteUnion(int nServerID, int unionID);

    static bool hasUnion(int nServerID, int unionID);

    static bool getUnionField(int nServerID, int field, int &val);

    static bool setUnionField(int nServerID, int field, int val);
    // 获取最新公会ID(+1)
    static bool getNewUnionId(int nServerID, int &unionId);

private:
    int                             m_nUnionID;       // 公会ID
    std::string                     m_UnionName;      // 公会名称
    std::string                     m_UnionNotice;    // 公会公告
    std::map<int, int>              m_UnionInfoMap;   // 公会信息<field, value>
    std::map<int, int>              m_MemberMap;      // 成员列表<UserID, 职位>
    std::map<int, int>              m_AuditMap;       // 审核列表<uid, time>
    std::map<int, UnionLogInfo>     m_LogMap;         // 事件列表<logid, info>
    Storage                    *    m_pStorage;       // 数据库

    static std::vector<int>         m_VectUnionID;    //帮派ID列表
    static int                      m_DynUnionID;     //动态自增帮派ID值

    static std::map<int, int>       m_UnionInfo;      //帮派相关配置
};

#endif