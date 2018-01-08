/*
 * 公会管理类
 * 1.公会列表(实例)
 * 2.添加(删除)新公会
 * 3.获取某个公会ID的公会实例
 */

#ifndef __UNION_MANAGER_H__
#define __UNION_MANAGER_H__

#include <map>
#include "Union.h"
#include "KXServer.h"

class CUnionManager : public KxServer::KxTimerObject
{
private:
    CUnionManager();
    ~CUnionManager();

public:
    static CUnionManager* getInstane();
    static void destroy();
    
    bool init();
    // 创建新公会
    bool createUnion(int unionID, int chairmanID, std::string unionName);
    // 移除公会(不删除数据库数据)
    bool removeUnion(int unionID);
    // 删除公会(删除数据库数据)
    bool deleteUnion(int unionID);
    // 根据ID获取公会
    CUnion* getUnion(int unionID);
    // 获取所有公会
    std::map<int, CUnion*> getAllUnion();
    // 获取排序列表
    std::vector<int>& getUnionSort();
    // 获取对应的模型
    CUnionModel *getUnionModel(int unionID);
	//获取对应的公会佣兵模型
	CUnionMercenaryModel *getUnionMercenaryModel(int unionID);

    virtual void onTimer(const KxServer::kxTimeVal& now);

    virtual void setTimer(int sec, int nNum = 0);

private:
    //初始化所有公会信息
    void initUnions(int nServerID);
    // 数据库的公会
    bool addUnion(int unionID);
    // 从列表移除
    void removeFromList(int unionID);
    // 公会每日处理
    void daily();
    // 解散公会
    void dismissUnion(std::list<int> unions);
    // 转让会长
    bool assignChairman(int unionId);
    // 关服务器过了日重置处理
    void resetUnionsDayData();

private:
    std::map<int, CUnion*>    m_UnionMap;    // 公会列表<公会id, 公会>
    std::vector<int>          m_UnionSort;   // 排序列表(等级>人数>ID)
    int                       m_nSearchTime; // 记录查询时间戳

    
    static CUnionManager *    m_Instance;
};

#endif