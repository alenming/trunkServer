#ifndef __TARGET_SEARCHER_H__
#define __TARGET_SEARCHER_H__

#include <vector>

enum SearchSequence
{
    Positive_Sequence,          //玩家在左边的士兵使用正排序 soldier[0].x < soldier[1].x <　...
    Inverted_Sequence,          //玩家在右边的士兵使用反排序 soldier[0].x > soldier[1].x ＞ ...
};

enum SearchSortOrder
{
    SortOrder_None,             //不排序
    SortOrder_Blood,            //按血量排序
    SortOrder_X,                //按x轴排序
	SortOrder_RND,              //随机
    SortOrder_BigHatred,        //仇恨从大到小排序
    SortOrder_SmallHatred,      //仇恨从小到大排序
};

enum SearchType
{
    SearchType_None = 0,        //无
    SearchType_Attackable,      //可否单体攻击
    SearchType_Aoeable,         //可否aoe攻击
    SearchType_Buffable,        //可否给buff攻击
};

enum SearchOrderType
{
    SearchOrder_Blood,          //血量排序
    SearchOrder_XCoord,         //x轴排序
};

enum SearchRangeType
{
    SearchRange_Far = -2,       //远程范围
    SearchRange_Close = -1,     //近战范围
    SearchRange_All = 0,        //所有
};

enum SearchDirection
{
    SearchDirection_None = 0,   //无
    SearchDirection_OneWay,     //单向搜索
    SearchDirection_BothWay,    //双向搜索
};

enum SearchListType
{
    SearchListType_All = 0,             //所有
    SearchListType_Enemy,               //敌军
    SearchListType_Friend,              //友军
    SearchListType_Self,                //自己
    SearchListType_EnemySummoner,       //敌方召唤师
    SearchListType_FriendSummoner,      //己方召唤师
    SearchListType_FirstEnemy,          //指定范围内离己方最近的第一个敌人
    SearchListType_NeedTreastFriend,    //己方需要治疗
    SearchListType_NeedTreastAll,       //全阵营搜索需要治疗
    //SearchListType_FirstFriend,       //指定范围内离己方最近的第一个友军
};

class CRole;
class CGameObject;
class CBattleHelper;
class SearchConfItem;
class CTargetSearcher
{
public:
    // 判定目标是否在范围内
    static bool isTargetInRange(CRole *target, float minX, float maxX);
    // 查找范围内目标集合
    static bool searchTargets(std::vector<CRole*> &objList, float minX, float maxX, int &minIdx, int &maxIdx, int dir);
    // 查找最近目标
    static CRole *getClosestTarget(std::vector<CRole*> &objList, CRole *origin);
    // 排序列表
    static void sortList(std::vector<CRole*> &objList, int sortorder);
    // 查找目标
    static bool searchTargetsWithId(int searchid, CGameObject* searcher, std::vector<CRole *> *targets);
};

class CSearchCondition
{
public:
    // 查找所有
    static bool checkConditionAllAndPush(CRole *target, const SearchConfItem *conf, std::vector<CRole *> *targets);
    static bool checkConditionAll(CRole *target, const SearchConfItem *conf);
    // 角色是否死亡及是否能被搜索
    static bool checkDeath(CRole *target, bool canSearch);
    // ng为是否取反, 以下函数同义
	static bool checkIdentify(CRole *target, const std::vector<int> &identify);
    // 血量百分比
    static bool checkBlood(CRole *target, bool ng, float percent);
    // 职业
    static bool checkVocation(CRole *target, bool ng, const std::vector<int> &vocations);
    // 性别
    static bool checkSex(CRole *target, bool ng, int sex);
    // 是否近战
    static bool checkFireRange(CRole * target, int rangeType);
    // 种族
    static bool checkRace(CRole *target, bool ng, int race);
    // 角色星级等级
    static bool checkStarLevel(CRole *target, bool ng, int starLevel);
    // 搜索类型, 角色身上一个搜索属性
    static bool checkType(CRole *target, int type);
    // 是否拥有指定buff序列
    static bool checkBuff(CRole *target, bool ng, const std::vector<int> &buffIds);
    // 是不是某指定的角色id
    static bool checkRoleId(CRole *target, bool ng, const std::vector<int> &roleIds);
    // 是否是否在状态内
    static bool checkIsInState(CRole *target, bool ng, int state);
    // 去掉自己
    static bool checkCutMe(std::vector<CRole *> &targets, CGameObject *searcher);
    // 限制最大个数
    static bool checkLimitMaxAndSort(std::vector<CRole *> &targets, CGameObject * searcher, int max, int sortOrder);
};

#endif //__TARGET_SEARCHER_H__
