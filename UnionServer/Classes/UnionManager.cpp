#include "UnionManager.h"
#include "GameUserManager.h"
#include "ConfGameSetting.h"
#include "ConfUnion.h"
#include "UnionServer.h"
#include "TimeCalcTool.h"
#include "ModelHelper.h"
#include "ServiceDef.h"
#include "MailHelper.h"
#include "UnionHelper.h"

using namespace std;

#define SEARCH_INTERVAL 600 // 十分钟

bool UnionCompare(int union1, int union2)
{
    CUnion *pUnion1 = CUnionManager::getInstane()->getUnion(union1);
    CUnion *pUnion2 = CUnionManager::getInstane()->getUnion(union2);
    if (pUnion1 && pUnion2)
    {
        CUnionModel *pUnionModel1 = pUnion1->getUnionModel();
        CUnionModel *pUnionModel2 = pUnion2->getUnionModel();
        
        int nUnionLv1 = 0;
        int nUnionLv2 = 0;
        if (pUnionModel1->GetUnionFieldVal(UNION_FIELD_LV, nUnionLv1)
            && pUnionModel2->GetUnionFieldVal(UNION_FIELD_LV, nUnionLv2))
        {
            if (nUnionLv1 > nUnionLv2)
            {
                return true;
            }
            else if (nUnionLv1 == nUnionLv2)
            {
                if (pUnionModel1->GetMembers().size() > pUnionModel2->GetMembers().size())
                {
                    return true;
                }
                else if (pUnionModel1->GetMembers().size() == pUnionModel2->GetMembers().size())
                {
                    if (pUnionModel1->GetUnionID() > pUnionModel2->GetUnionID())
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

CUnionManager::CUnionManager():m_nSearchTime(0)
{
    m_UnionSort.clear();
    m_UnionMap.clear();
}

CUnionManager::~CUnionManager()
{
    std::map<int, CUnion*>::iterator iter = m_UnionMap.begin();
    for (; iter != m_UnionMap.end(); ++iter)
    {
        if (NULL != iter->second)
        {
            delete iter->second;
        }
    }
}

CUnionManager *CUnionManager::m_Instance = NULL;
CUnionManager* CUnionManager::getInstane()
{
    if (NULL == m_Instance)
    {
        m_Instance = new CUnionManager;
    }

    return m_Instance;
}

void CUnionManager::destroy()
{
    if (NULL != m_Instance)
    {
        CUnionModel::CloseUnionList(1);
        delete m_Instance;
    }
}

bool CUnionManager::init()
{
    if (!CUnionModel::CreateUnionList(UNION_SERVER_ID))
    {
        KXLOGERROR("CUnionManager::init(), CreateUnionList fail");
        return false;
    }

    m_Instance->initUnions(UNION_SERVER_ID);

    int nDayResetStamp = 0;
    CHECK_RETURN(CUnionModel::getUnionField(UNION_SERVER_ID, SRV_FD_DAYRESETSTAMP, nDayResetStamp));

    int CurTime = (int)time(NULL);
    int nCurDayResetStamp = CTimeCalcTool::nextTimeStamp(CurTime, 0, 0);
    if (nDayResetStamp <= 0)
    {
        CHECK_RETURN(CUnionModel::setUnionField(UNION_SERVER_ID, SRV_FD_DAYRESETSTAMP, nCurDayResetStamp));
    }
    else if (nDayResetStamp < CurTime)
    {
        CHECK_RETURN(CUnionModel::setUnionField(UNION_SERVER_ID, SRV_FD_DAYRESETSTAMP, nCurDayResetStamp));
        // 处理重启服务器的时候过了公会日重置数据
        resetUnionsDayData();
    }

    setTimer(nCurDayResetStamp - CurTime, KXREPEAT_FOREVER);

    return true;
}

bool CUnionManager::createUnion(int unionID, int chairmanID, std::string unionName)
{
    if (m_UnionMap.find(unionID) != m_UnionMap.end())
    {
        return false;
    }

    CUnion *pUnion = new CUnion();
    if (pUnion->init(unionID, chairmanID, unionName))
    {
        m_UnionMap[unionID] = pUnion;
        m_UnionSort.push_back(unionID);
        return true;
    }

    delete pUnion;

    return false;
}

bool CUnionManager::removeUnion(int unionID)
{
    std::map<int, CUnion*>::iterator iter = m_UnionMap.find(unionID);
    if (iter != m_UnionMap.end())
    {
        CUnion* pUnion = iter->second;
        if (NULL != pUnion)
        {
            // 移除公会模型
            pUnion->removeUnion();

            delete pUnion;
            pUnion = NULL;
            
            removeFromList(unionID);

            return true;
        }
    }

    return false;
}

bool CUnionManager::deleteUnion(int unionID)
{
    std::map<int, CUnion*>::iterator iter = m_UnionMap.find(unionID);
    if (iter != m_UnionMap.end())
    {
        CUnion* pUnion = iter->second;
        if (NULL != pUnion)
        {
            if (CUnionModel::deleteUnion(UNION_SERVER_ID, unionID))
            {
                // 删除公会模型数据
                pUnion->deleteUnion();
            }
            else
            {
                KXLOGERROR("delete union error! unionid:%d", unionID);
            }

            delete pUnion;
            pUnion = NULL;

            removeFromList(unionID);
            return true;
        }
    }

    return false;
}

CUnion* CUnionManager::getUnion(int unionID)
{
    std::map<int, CUnion*>::iterator iter = m_UnionMap.find(unionID);
    if (iter != m_UnionMap.end())
    {
        return iter->second;
    }

    return NULL;
}

std::map<int, CUnion*> CUnionManager::getAllUnion()
{
    return m_UnionMap;
}

std::vector<int>& CUnionManager::getUnionSort()
{
    int nCurTime = int(time(NULL));
    if (nCurTime >= m_nSearchTime)
    {
        m_nSearchTime = nCurTime + SEARCH_INTERVAL;
        // 排序
        sort(m_UnionSort.begin(), m_UnionSort.end(), UnionCompare);
    }

    return m_UnionSort;
}

void CUnionManager::initUnions(int nServerID)
{
    CGameUserManager::getInstance();

    vector<int> &VectUnion = CUnionModel::GetUnionIDList();

    for (unsigned int i = 0; i < VectUnion.size(); ++i)
    {
        addUnion(VectUnion[i]);
    }
}

bool CUnionManager::addUnion(int unionID)
{
    std::map<int, CUnion*>::iterator iter = m_UnionMap.find(unionID);
    if (iter != m_UnionMap.end())
    {
        return false;
    }

    // 初始化公会
    CUnion *pUnion = new CUnion;
    if (!pUnion->init(unionID))
    {
        delete pUnion;
        pUnion = NULL;
        return false;
    }

    m_UnionMap[unionID] = pUnion;
    m_UnionSort.push_back(unionID);

    return true;
}

CUnionModel * CUnionManager::getUnionModel(int unionID)
{
    std::map<int, CUnion*>::iterator iter = m_UnionMap.find(unionID);
    if (iter != m_UnionMap.end())
    {
        CUnion* pUnion = iter->second;
        if (pUnion)
        {
            return pUnion->getUnionModel();
        }
    }

    return NULL;
}

CUnionMercenaryModel * CUnionManager::getUnionMercenaryModel(int unionID)
{
	std::map<int, CUnion*>::iterator iter = m_UnionMap.find(unionID);
	if (iter != m_UnionMap.end())
	{
		CUnion* pUnion = iter->second;
		if (pUnion)
		{
			return pUnion->getMercenaryModel();
		}
	}

	return NULL;
}

void CUnionManager::onTimer(const KxServer::kxTimeVal& now)
{
    stop();
    daily();
    // 添加下一轮计时器
    int CurTime = (int)time(NULL);
    int nIterval = CTimeCalcTool::nextTimeStamp(CurTime, 0, 0) - CurTime;
    if (nIterval < 60)
    {
        nIterval = nIterval + 24 * 3600;
    }

    // 重置公会刷新时间
    CHECK_RETURN_VOID(CUnionModel::setUnionField(UNION_SERVER_ID, SRV_FD_DAYRESETSTAMP, CurTime + 24 * 3600));
    setTimer(nIterval, KXREPEAT_FOREVER);
}

void CUnionManager::setTimer(int sec, int nNum /*= 0*/)
{
    CUnionServer::getInstance()->getTimerManager()->addTimer(this, sec, nNum);
}

void CUnionManager::removeFromList(int unionID)
{
    std::map<int, CUnion*>::iterator iterMap = m_UnionMap.find(unionID);
    if (iterMap != m_UnionMap.end())
    {
        m_UnionMap.erase(iterMap);
    }

    std::vector<int>::iterator iterVec = m_UnionSort.begin();
    for (; iterVec != m_UnionSort.end(); )
    {
        if (*iterVec == unionID)
        {
            iterVec = m_UnionSort.erase(iterVec);
        }
        else
        {
            ++iterVec;
        }
    }
}

void CUnionManager::daily()
{
    std::list<int> dismissList;
    std::map<int, CUnion*>::iterator iter = m_UnionMap.begin();
    for (; iter != m_UnionMap.end(); ++iter)
    {
        CUnion *pUnion = iter->second;
        if (NULL == pUnion)
        {
            KXLOGERROR("union list find an union object is null, %d", iter->first);
            continue;
        }

        CUnionModel* pUnionModel = pUnion->getUnionModel();
		CHECK_CONTINUE(pUnionModel != NULL);
        int nUnionLv = 0;
        pUnionModel->GetUnionFieldVal(UNION_FIELD_LV, nUnionLv);
        const UnionLevelItem* pUnionLevelItem = queryConfUnionLevel(nUnionLv);
        if (NULL == pUnionLevelItem)
        {
            KXLOGERROR("queryConfUnionLevel fail, union lv %d", nUnionLv);
            continue;
        }

        int nLiveness = 0, nReputation = 0, nDangerousTag = 0;
        pUnionModel->GetUnionFieldVal(UNION_FIELD_LIVENESS, nLiveness);
        pUnionModel->GetUnionFieldVal(UNION_FIELD_REPUTATION, nReputation);
        pUnionModel->GetUnionFieldVal(UNION_FIELD_DANGEROUSTAG, nDangerousTag);

        std::map<int, int> mapInfo;
        // 1、记录0活跃天数
        if (nLiveness <= 0)
        {
            nDangerousTag += 1;
            mapInfo[UNION_FIELD_DANGEROUSTAG] = nDangerousTag;
        }
        else
        {
            if (nDangerousTag > 0)
            {
                nDangerousTag = 0;
                mapInfo[UNION_FIELD_DANGEROUSTAG] = 0;
            }
        }

        // 2、计算声望是否降级
        if (nLiveness < pUnionLevelItem->ActiveMin)
        {
            nReputation -= pUnionLevelItem->UnActiveReduce;
            nReputation = nReputation < 0 ? 0 : nReputation;
            mapInfo[UNION_FIELD_REPUTATION] = nReputation;
        }

        if (nReputation < pUnionLevelItem->DownLevelCost)
        {
            if (nUnionLv > 1)
            {
                nUnionLv -= 1;
                mapInfo[UNION_FIELD_LV] = nUnionLv;
                mapInfo[UNION_FIELD_ORIGINLV] = nUnionLv;
            }
        }

        // 3、公会等级<=3且连续15天活跃天数解散
        if (nUnionLv <= 3 && nDangerousTag >= 15)
        {
            dismissList.push_back(iter->first);
            continue;
        }

        // 4、会长自动转让
        assignChairman(iter->first);

        // 5、重置今日活跃度
        mapInfo[UNION_FIELD_LIVENESS] = 0;

        // 6、保存数据
        if (!pUnionModel->SetUnionFieldVals(mapInfo))
        {
            KXLOGERROR("daily unionid %d setdata fail!!!", iter->first);
        }

		// 7、清除召回时间超过24小时的佣兵
		pUnion->getMercenaryModel()->ClearRecalled24HoursMercenary();

		// 8、重置公会每个成员的雇佣列表
		std::map<int, int> mapMerbers = pUnionModel->GetMembers();
		for (std::map<int, int>::iterator mapit = mapMerbers.begin(); mapit != mapMerbers.end(); ++mapit)
		{
			CPersonMercenaryModel *pPersonMercenaryModel = dynamic_cast<CPersonMercenaryModel *>(CModelHelper::getModel(mapit->first, MODELTYPE_MERCENARY));
			if (NULL != pPersonMercenaryModel)
			{
				pPersonMercenaryModel->ClearAllEmployedMercenaryInfo();
			}
		}
    }

    // 统一处理所有要解散的公会
    dismissUnion(dismissList);
}

void CUnionManager::dismissUnion(std::list<int> unions)
{
    if (unions.empty())
    {
        return;
    }

    std::list<int>::iterator iter = unions.begin();
    for (; iter != unions.end(); ++iter)
    {
        CUnionModel* pUnionModel = getUnionModel(*iter);
		CHECK_CONTINUE(pUnionModel != NULL);
        std::map<int, int> mapMembers = pUnionModel->GetMembers();
        std::map<int, int>::iterator iterMembers = mapMembers.begin();
        // 处理公会成员
        for (; iterMembers != mapMembers.end(); ++iterMembers)
        {
            CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(iterMembers->first, false);
            if (NULL == pGameUser)
            {
                CUserUnionModel::setOffLineUserUnionValue(iterMembers->first, USER_UNION_ID, 0);
            }
            else
            {
                // 设置公会id为0
                CUserUnionModel *pUserUnionModel = reinterpret_cast<CUserUnionModel *>(pGameUser->getModel(MODELTYPE_USERUNION));
				CHECK_CONTINUE(pUserUnionModel != NULL);
                pUserUnionModel->setUserUnionValue(USER_UNION_ID, 0);
                CUnionHelper::sendBeFunctionInfo(iterMembers->first, *iter, UNION_FUNC_DISMISS);
            }

            // 发送解散邮件
            CMailHelper::sendUnionMail(iterMembers->first, MAIL_CONFID_UNIONDISMISS, pUnionModel->GetUnionName());
        }

        if (!deleteUnion(*iter))
        {
            KXLOGERROR("delete unionid %d fail!!!", *iter);
        }
    }
}

bool CUnionManager::assignChairman(int unionId)
{
    CUnionModel* pUnionModel = getUnionModel(unionId);
    if (NULL == pUnionModel)
    {
        return false;
    }

    int nChairManID = 0;
    pUnionModel->GetUnionFieldVal(UNION_FIELD_CHAIRMAN, nChairManID);

    UserBasicInfo chairmanInfo;
    if (!CModelHelper::getUserInfo(nChairManID, chairmanInfo))
    {
        KXLOGERROR("getUserInfo fail!!userid %d", nChairManID);
        return false;
    }

    // 连续5天没有登录
    if (((int)time(NULL) - chairmanInfo.lastLoginTime) / 86400 < 5)
    {
        return true;
    }

    std::map<int, int> mapMembers = pUnionModel->GetMembers();
    // 只有会长不用管
    if (mapMembers.size() <= 1)
    {
        return true;
    }

    // 优先级:副会长>等级最高有今日贡献>最后登录时间
    int nNewChairManID = 0, nPos = 0, nMaxLv = 0, nTodayLiveness = 0, nLastLogin = 0;
    UserBasicInfo tempUserInfo;
    UserUnionBasicInfo tempUserUnionInfo;
    std::map<int, int>::iterator iter = mapMembers.begin();
    for (; iter != mapMembers.end(); ++iter)
    {
        if (UNION_POS_CHAIRMAN == iter->second)
        {
            continue;
        }

        if (!CModelHelper::getUserInfo(iter->first, tempUserInfo)
            || !CModelHelper::getUserUnionInfo(iter->first, tempUserUnionInfo))
        {
            KXLOGERROR("getUserInfo fail!!userid %d", iter->first);
            continue;
        }

        int nTempTodayLiveness = tempUserUnionInfo.todayPvpLiveness + tempUserUnionInfo.todayStageLiveness;
        if (0 == nNewChairManID || iter->second > nPos)
        {
            nPos = iter->second;
            nNewChairManID = iter->first;
            nMaxLv = tempUserInfo.userLv;
            nTodayLiveness = nTempTodayLiveness;
            nLastLogin = tempUserInfo.lastLoginTime;
        }
        else if (iter->second == nPos)
        {
            if (tempUserInfo.userLv > nMaxLv)
            {
                nNewChairManID = iter->first;
                nMaxLv = tempUserInfo.userLv;
                nTodayLiveness = nTempTodayLiveness;
                nLastLogin = tempUserInfo.lastLoginTime;
            }
            else if (tempUserInfo.userLv == nMaxLv)
            {
                if (nTempTodayLiveness > nTodayLiveness)
                {
                    nNewChairManID = iter->first;
                    nTodayLiveness = nTempTodayLiveness;
                    nLastLogin = tempUserInfo.lastLoginTime;
                }
                else if (nTempTodayLiveness == nTodayLiveness)
                {
                    if (tempUserInfo.lastLoginTime > nLastLogin)
                    {
                        nNewChairManID = iter->first;
                        nLastLogin = tempUserInfo.lastLoginTime;
                    }
                }
            }
        }
    }

    // 有职位、今日有活跃、最后登录时间比会长近都可以转让
    if ((nPos != 0 || nTodayLiveness > 0 || nLastLogin > chairmanInfo.lastLoginTime)
        && nChairManID != nNewChairManID)
    {
        // 替换会长
        if (!pUnionModel->SetMemberPos(nChairManID, UNION_POS_NORMAL)
            || !pUnionModel->SetMemberPos(nNewChairManID, UNION_POS_CHAIRMAN)
            || !pUnionModel->SetUnionFieldVal(UNION_FIELD_CHAIRMAN, nNewChairManID))
        {
            KXLOGERROR("change chairman fail!!! nChairManID %d, nNewChairManID %d",
                nChairManID, nNewChairManID);
            return false;
        }
    }

    return true;
}

void CUnionManager::resetUnionsDayData()
{
    std::map<int, CUnion*>::iterator iter = m_UnionMap.begin();
    for (; iter != m_UnionMap.end(); ++iter)
    {
        CUnion *pUnion = iter->second;
        if (NULL == pUnion)
        {
            KXLOGERROR("union list find an union object is null, %d", iter->first);
            continue;
        }

        CUnionModel* pUnionModel = pUnion->getUnionModel();
        int nLv = 0;
        pUnionModel->GetUnionFieldVal(UNION_FIELD_LV, nLv);
        if (!pUnionModel->SetUnionFieldVal(UNION_FIELD_LIVENESS, 0)
            || pUnionModel->SetUnionFieldVal(UNION_FIELD_ORIGINLV, nLv))
        {
            KXLOGERROR("setUnionFieldVal, unionid, %d", iter->first);
        }
    }
}
