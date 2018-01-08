/*
* 公会服务层
* 1.公会基础操作(申请、任命、撤职、创建等)
* 2.公会任务挑战(公会个人任务和公会团队任务)
*/

#ifndef __UNION_SERVICE_H__
#define __UNION_SERVICE_H__

#include "KxCommInterfaces.h"

class CUnionService
{
public:
    /////////////////////////////// 公会基础操作 ///////////////////////////////////////////
	static void processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 请求公会信息
	static void processUnionInfoService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 请求成员列表
	static void processMembersService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 请求创建公会
	static void processCreateService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 请求申请加入公会
    static void processApplyService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 退会
    static void processExitService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 玩家所在公会事件列表
    static void processLogListService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 玩家所在公会审核列表
    static void processAuditListService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 审核信息
    static void processAuditService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 公会相关操作(踢出、撤任、权利移交等)
    static void processFunctionService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 公会列表(没有公会,请求公会列表)
    static void processOutUnionListService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 搜索公会
    static void processSearchService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 设置会徽
    static void processEmblemService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 设置公会名
    static void processUnionNameService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 设置公告
    static void processUnionNoticeService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 领取福利
    static void processWelfareService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 设置审核信息
    static void processSetAuditService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 请求公会全部佣兵信息
	static void processMercenaryService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 派遣佣兵
	static void processDispatchMercenaryService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 召回佣兵
	static void processRecallMercenaryService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 请求单个佣兵详细信息
	static void processSingleMercenaryService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
    // 其他服务器转发的数据
    static void processUnionForwardDataService(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__UNION_SERVICE_H__
