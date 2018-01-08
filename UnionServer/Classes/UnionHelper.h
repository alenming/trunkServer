#include "PersonMercenaryModel.h"
#ifndef _UNION_HELPER_H__
#define _UNION_HELPER_H__

#include <vector>
#include <list>
#include "ServiceDef.h"
#include "UnionMercenaryModel.h"
#include "PersonMercenaryModel.h"
#include "UnionShopModel.h"

class CUnionExpiditionModel;
class CUnionModel;
class CUnion;
struct SExpeditonWorldItem;
struct UnionListInfo;
struct OwnUnionInfo;
struct BuffData;

class CUnionHelper
{
public:
	//对世界生成随机地图
	static int getRandomMapID(const SExpeditonWorldItem *pExpiditionWorldData);
	//校验地图序列是否合法,如果合法，获取关卡ID
	static bool checkIndexData(CUnionExpiditionModel *pModel, int nIndex, int &nStageID, int &nStageLv);
	//获取角色的驻站BUFF
	static bool getExpiditionBuff(CUnionExpiditionModel *pModel, std::vector<BuffData> &Buff);
	//征战结束设定
	static bool finishExpiditionWar(CUnionExpiditionModel *pModel, bool bWin = false);
	//刷新公会商店商品
	static bool freshUnionShopData(CUnionShopModel *pModel, int nUnionDropID);
    // 添加公会活跃度
    static bool addUnionLiveness(int unionId, int liveness);
    // 添加声望
    static bool addUnionReputation(int unionId, int reputation);
    // 通过声望计算等级
    static int calcUnionLv(int curLv, int curReputation);

    // 检测创建公会(返回相关错误码)
    static int checkCreate(int uid, std::string unionName);
    // 检测公会名字是否合法(返回相关错误码)
    static int checkUnionName(std::string unionName);
    // 检测能否申请公会
    static EUnionErrorCode checkApply(int uid, int unionId);
	
	// 获得玩家公会对象, 如果没有返回NULL
	static CUnion *getUserUnion(int uid);
    // 能否踢出公会
    static bool canKick(int initiativePos, int passivePos);
    // 能否撤任
    static bool canRelieve(int initiativePos, int passivePos);
    // 能否转移权利
    static bool canTransfer(int initiativePos, int passivePos);
    // 能否任命
    static bool canAppoint(int initiativePos, int passivePos);

    // 搜索公会名称(ID)
    static void searchUnion(std::string searchName, std::list<UnionListInfo> &searchList);
    // 获取公会信息
    static void fillUnionInfo(CUnionModel *pUnionModel, UnionListInfo &info);
    // 退出公会,返回下次申请公会时间戳
    static int quitUnion(int uid);
    // 审核玩家
    static EUnionErrorCode auditUser(int nUnionID, int uid, bool agree);
    // 添加日志
    static bool addUnionLog(int nUnionId, int eventType, std::string nameOne, std::string nameTwo = "", int extend = 0);
	// 佣兵奖励
	static int mercenaryPrize(UnionMercenaryInfo& info);
    // 发送公会信息(被审核通过或自动通过)
    static void sendOwnUnionInfo(int uid, int unionId);
    // 发送被操作信息(被升职、拒绝通过等)
    static void sendBeFunctionInfo(int uid, int unionId, int funcType);
    // 发送提示信息(可领奖、可审核)
    static void sendMessageTips(CUnionModel *pUnionModel, int tipsType);
    // 通过玩家id获取所在公会模型
    static CUnionModel *getUnionModel(int uid);
    // 填充登录公会信息
    static bool fillOwnUnionInfo(int uid, int unionId, OwnUnionInfo &ownUnionInfo);

    /////////////////////////////////// 公会通知 ///////////////////////////////////////
    static void noticeUnionMembers(int unionId, char *buff, int len);
    // 通知公会成员
    static void noticeUnion(int unionId, int noticeId);
    // 通知公会成员
    static void noticeUnion(int unionId, int noticeId, std::string szVal);
    // 通知公会成员
    static void noticeUnion(int unionId, int noticeId, std::string szValOne, std::string szValTwo);
};

#endif //_UNION_HELPER_H__

