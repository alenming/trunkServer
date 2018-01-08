#include "GameUserManager.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ConfGameSetting.h"
#include "ConfStage.h"
#include "RankModel.h"
#include "PvpRankModel.h"
#include "ModelHelper.h"
#include "TaskAchieveHelper.h"
#include "ActiveHelper.h"
#include "GlobalMailModel.h"

CGameUserManager::CGameUserManager()
:m_pPassTeamModel(NULL)
{
	m_VectServerModel.clear();
	m_DelUserList.clear();
	m_MapDelUserList.clear();
}

CGameUserManager::~CGameUserManager()
{
	for (std::map<int, CGameUser *>::iterator iter = m_GameUsers.begin();
		iter != m_GameUsers.end(); ++iter)
	{
		delete iter->second;
	}

	m_VectServerModel.clear();
	m_DelUserList.clear();
	m_MapDelUserList.clear();
}

CGameUserManager * CGameUserManager::m_pInstance = NULL;
CGameUserManager *CGameUserManager::getInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CGameUserManager;
		m_pInstance->m_pPassTeamModel = new CPassTeamModel;
		m_pInstance->m_pPassTeamModel->init();
	}
	return m_pInstance;
}

void CGameUserManager::destroy()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
	}
}

bool CGameUserManager::init(KxServer::KxTimerManager *pTimerManager)
{
	m_TimeManager = pTimerManager;

	if (m_TimeManager == NULL)
	{
		return false;
	}

	if (!m_TimeManager->addTimer(this, CHECK_TIME_INTERVAL, KXREPEAT_FOREVER))
	{
		return false;
	}

	return true;
}

void CGameUserManager::addModelType(ModelType Type)
{
	m_VectServerModel.push_back(Type);
}

void CGameUserManager::onTimer(const KxServer::kxTimeVal& now)
{
	int nCurTime = (int)time(NULL);

	for (std::list<SDelayDelData>::iterator ator = m_DelUserList.begin();
        ator != m_DelUserList.end();)
	{
		if (nCurTime > ator->nDelayTime)
		{
			RealremoveGameUser(ator->nUid);
			m_MapDelUserList.erase(ator->nUid);
			m_DelUserList.erase(ator++);
		}
		else
		{
			break;
		}
	}
}

CGameUser* CGameUserManager::getGameUser(int uid, bool noNull)
{
	std::map<int, CGameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter != m_GameUsers.end())
	{
		return iter->second;
	}

	if (noNull)
	{
		return initGameUser(uid);
	}
	return NULL;
}

CGameUser* CGameUserManager::initGameUser(int uid)
{
	std::map<int, CGameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter == m_GameUsers.end())
	{
		CGameUser *pGameUser = new CGameUser;
		if (!pGameUser->initModels(uid))
		{
			delete pGameUser;
			return NULL;
		}

		m_GameUsers[uid] = pGameUser;
		initUserData(pGameUser);
		return pGameUser;
	}

	return NULL;
}

CGameUser *CGameUserManager::newGameUser(int uid)
{
	// 新用户
	CGameUser *pGameUser = new CGameUser;
	int nCreateTime = KxServer::KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
	// 创建每个模型
	CUserModel *pUserModel = new CUserModel;
	CBagModel *pBagModel = new CBagModel;
	CEquipModel *pEquipModel = new CEquipModel;
	CStageModel *pStageModel = new CStageModel;
	CSummonModel *pSummonModel = new CSummonModel;
	CHeroModel *pHeroModel = new CHeroModel;
	CTeamModel *pTeamModel = new CTeamModel;
	CTaskModel *pTaskModel = new CTaskModel;
	CAchievementModel *pAchieveModel = new CAchievementModel;
	CGuideModel *pGuideModel = new CGuideModel;
	CInstanceModel*pInstanceModel = new CInstanceModel;
	CMailModel *pMailModel = new CMailModel;
	CGoldTestModel * pGoldTestModel = new CGoldTestModel;
	CHeroTestModel * pHeroTestModel = new CHeroTestModel;
	CTowerTestModel *pTowerTestModel = new CTowerTestModel;
	CPvpModel *pPvpModel = new CPvpModel;
	CShopModel *pShopModel = new CShopModel;
	CUserActiveModel *pActiveModel = new CUserActiveModel();
	CHeadModel *pHeadModel = new CHeadModel();
	CUserUnionModel *pUserUnionModel = new CUserUnionModel;
	CPersonMercenaryModel *pPersonMercenaryModel = new CPersonMercenaryModel;
	CBlueDiamondModel *pBDModel = new CBlueDiamondModel;
    CPvpChestModel *pPvpChestModel = new CPvpChestModel;

	pGameUser->setModel(MODELTYPE_USER, pUserModel);
	pGameUser->setModel(MODELTYPE_BAG, pBagModel);
	pGameUser->setModel(MODELTYPE_EQUIP, pEquipModel);
	pGameUser->setModel(MODELTYPE_STAGE, pStageModel);
	pGameUser->setModel(MODELTYPE_SUMMONER, pSummonModel);
	pGameUser->setModel(MODELTYPE_HERO, pHeroModel);
	pGameUser->setModel(MODELTYPE_TEAM, pTeamModel);
	pGameUser->setModel(MODELTYPE_TASK, pTaskModel);
	pGameUser->setModel(MODELTYPE_ACHIEVEMENT, pAchieveModel);
	pGameUser->setModel(MODELTYPE_GUIDE, pGuideModel);
	pGameUser->setModel(MODELTYPE_INSTANCE, pInstanceModel);
	pGameUser->setModel(MODELTYPE_MAIL, pMailModel);
	pGameUser->setModel(MODELTYPE_GOLDTEST, pGoldTestModel);
	pGameUser->setModel(MODELTYPE_HEROTEST, pHeroTestModel);
	pGameUser->setModel(MODELTYPE_TOWERTEST, pTowerTestModel);
	pGameUser->setModel(MODELTYPE_PVP, pPvpModel);
	pGameUser->setModel(MODELTYPE_SHOP, pShopModel);
	pGameUser->setModel(MODELTYPE_ACTIVE, pActiveModel);
	pGameUser->setModel(MODELTYPE_HEAD, pHeadModel);
	pGameUser->setModel(MODELTYPE_USERUNION, pUserUnionModel);
	pGameUser->setModel(MODELTYPE_MERCENARY, pPersonMercenaryModel);
	pGameUser->setModel(MODELTYPE_BDACTIVE, pBDModel);
    pGameUser->setModel(MODELTYPE_PVPCHEST, pPvpChestModel);
	pGameUser->setUid(uid);

	// 添加到玩家管理器
	addGameUser(uid, pGameUser);

	// 读取配表, 添加新玩家物品及属性数据	
	const NewPlayerItem *pNewConf = queryConfNewPlayerItem(1);
	// 用户信息模型初始化
	char name[32] = {};
	sprintf(name, "%s", pNewConf->UserName.c_str());
	// 用户基本信息
	std::map<int, int> attrs;
	for (int i = USR_FD_USERID; i < USR_FD_END; i++)
	{
		// 所有属性默认为0
		attrs[i] = 0;
	}

	attrs[USR_FD_USERID] = uid;
	attrs[USR_FD_USERLV] = pNewConf->UserLv;
	attrs[USR_FD_EXP] = pNewConf->UserExp;
	attrs[USR_FD_GOLD] = pNewConf->Gold;
	attrs[USR_FD_DIAMOND] = pNewConf->Diamond;
	attrs[USR_FD_FREEHEROTIMES] = 1;
	attrs[USR_FD_BAGCAPACITY] = pNewConf->InitBagCapacity;
	attrs[USR_FD_EQUIPREF] = DEFAULT_EQUIPREF;
	attrs[USR_FD_RESETTIMESTAMP] = 0;
	attrs[USR_FD_CREATETIME] = nCreateTime;

	if (!pNewConf->Heads.empty())
	{
		if (!pHeadModel->init(uid))
		{
			return NULL;
		}

		for (size_t i = 0; i < pNewConf->Heads.size(); i++)
		{
			const std::map<int, SSystemHeadIconItem>& mapAllHeadIcon = queryConfHeadIcon();

			if (mapAllHeadIcon.find(pNewConf->Heads[i]) != mapAllHeadIcon.end())
			{
				pHeadModel->AddHeadID(pNewConf->Heads[i]);

				if (attrs[USR_FD_HEADICON] == 0)
				{
					attrs[USR_FD_HEADICON] = pNewConf->Heads[i];
				}
			}
		}
	}

	if (!pUserModel->NewUser(uid, name, attrs))
	{
		// 用户数据初始化失败
		return NULL;
	}

	//将角色等级排行榜中
	CRankModel::getInstance()->AddRankData(LEVEL_RANK_TYPE, uid, pNewConf->UserLv);

	// 背包模型数据 & 装备数据
	if (pBagModel->init(uid) && pEquipModel->init(uid))
	{
		// 消耗品
		for (std::map<int, int>::const_iterator iter = pNewConf->Items.begin();
			iter != pNewConf->Items.end(); ++iter)
		{
			const PropItem* pPropItem = queryConfProp(iter->first);
			if (pPropItem)
			{
				pBagModel->AddItem(iter->first, iter->second);
			}
			else
			{
				LOG("Execute False On File %s Line %d : %d", __FILE__, __LINE__, iter->first);
			}
		}
		// 装备
		if (pNewConf->Equips.size() > 0)
		{
			std::map<int, int>::const_iterator iterEquip = pNewConf->Equips.begin();
			for (; iterEquip != pNewConf->Equips.end(); ++iterEquip)
			{
				for (int i = 0; i < iterEquip->second; i++)
				{
					SEquipInfo EquipInfo;
					if (!CModelHelper::addEquip(uid, iterEquip->first, EquipInfo))
					{
						continue;
					}
				}
			}
		}
	}
	// 召唤师模型数据
	if (pSummonModel->init(uid))
	{
		// 召唤师
		for (std::vector<int>::const_iterator iter = pNewConf->SummonerIds.begin();
			iter != pNewConf->SummonerIds.end(); ++iter)
		{
			pSummonModel->AddSummon(*iter);
		}
	}
	// 英雄模型数据
	if (pHeroModel->init(uid))
	{
		// 英雄
		if (pNewConf->Heros.size() > 0)
		{
			for (std::vector<NewPlayerHeroInfo>::const_iterator iter = pNewConf->Heros.begin();
				iter != pNewConf->Heros.end(); ++iter)
			{
				const NewPlayerHeroInfo &heroInfo = *iter;
				CModelHelper::addHero(uid, heroInfo.heroId, heroInfo.heroStarLv, 1);
			}
		}
	}
	// 关卡模型
	if (pStageModel->init(uid))
	{
		if (pNewConf->Chapters.size() > 0)
		{
			for (std::vector<int>::const_iterator iter = pNewConf->Chapters.begin();
				iter != pNewConf->Chapters.end(); ++iter)
			{
				int chapterId = *iter;
				const ChapterConfItem *pChapterConf = queryConfChapter(chapterId);
				if (NULL != pChapterConf)
				{
					if (pChapterConf->Type == CHAPTERTYPE_NORMAL)
					{
						//解锁普通章节
						pStageModel->SetChapterStatus(chapterId, CHAPTERSTATUS_UNLOCK);
						pStageModel->SetCurStage(pChapterConf->ExInfo.FirstStageID);
					}
					else
					{
						//解锁精英章节
						pStageModel->SetEliteChapterStatus(chapterId, CHAPTERSTATUS_UNLOCK);
						pStageModel->SetCurElite(pChapterConf->ExInfo.FirstStageID);
					}
				}
			}
		}
	}
	// 队伍模型
	if (pTeamModel->init(uid))
	{
		if (pNewConf->SummonerInGroup > 0)
		{
			TeamInfo teamInfo;
			memset(&teamInfo, 0, sizeof(teamInfo));
			teamInfo.summonerId = pNewConf->SummonerInGroup;
			if (!pTeamModel->SetTeamInfo(ETT_PASE, teamInfo))
			{
				KXLOGDEBUG("userRegister SetTeamInfo failed");
			}
		}
	}
	// 任务模型
	if (pTaskModel->init(uid))
	{
	}
	// 成就模型
	if (pAchieveModel->init(uid))
	{
	}
	// 剧情模型
	if (pGuideModel->init(uid))
	{
		if (!pNewConf->Guides.empty())
		{
			for (size_t i = 0; i < pNewConf->Guides.size(); i++)
			{
				pGuideModel->AddGuideID(pNewConf->Guides[i]);
			}
		}
	}
	// 活动副本模型
	if (!pInstanceModel->init(uid))
	{
		KXLOGERROR("CInstanceModel init fail!!!");
	}
	// 邮件模型
	if (!pMailModel->init(uid))
	{
		KXLOGERROR("CMailModel init fail!!!");
	}
	// 金币试炼模型
	if (!pGoldTestModel->init(uid))
	{
		KXLOGERROR("CGoldTestModel init fail!!!");
	}
	// 英雄试炼模型
	if (!pHeroTestModel->init(uid))
	{
		KXLOGERROR("CHeroTestModel init fail!!!");
	}
	// 爬塔试练
	if (pTowerTestModel->init(uid))
	{
		//初始化爬塔数据
		pTowerTestModel->ResetTowerTest();
		CModelHelper::updateTowerTestInfo(uid);
	}
	// pvp模型
	if (pPvpModel->init(uid))
	{
		std::map<int, int> mapValues;
		for (int i = PVP_FD_MMR; i < PVP_FD_MAX; ++i)
		{
			mapValues[i] = 0;
		}
		
		mapValues[PVP_FD_MMR] = 750;
		
		if (!pPvpModel->SetPvpField(mapValues))
		{
			KXLOGERROR("Init New Player Pvp Model Data Error!");
		}
	}
	// pvp任务宝箱
	if (pPvpChestModel->init(uid))
	{
		int maxChest = 5;
		int index = 0;
		for (std::vector<int>::const_iterator iter = pNewConf->ChestIds.begin();
			iter != pNewConf->ChestIds.end(); ++iter)
		{
			if (++index <= maxChest)
			{
				CModelHelper::addPvpChest(uid, *iter);
			}
		}
		// 宝箱不满, 记录最后生成时间
		if (maxChest > index)
		{
			pPvpModel->SetPvpField(PVPCOMM_FD_LASTCHESTGENTIME, nCreateTime);
		}
	}

	// 商店模型
	if (!pShopModel->init(uid))
	{
		KXLOGERROR("CShopModel init fail!!!");
	}

	// 个人活动数据模型
	if (!pActiveModel->init(uid))
	{
		KXLOGERROR("CActiveModel init fail!!!");
	}

	if (!pUserUnionModel->init(uid))
	{
		KXLOGERROR("CUserUnionModel init fail!!!");
	}

	if (!pPersonMercenaryModel->init(uid))
	{
		KXLOGERROR("CPersonMecenaryModel init fail!!!");
	}

	// 任务成就最后才创建,可能需要用到其他模型
	if (!pNewConf->Tasks.empty())
	{
		for (size_t i = 0; i < pNewConf->Tasks.size(); ++i)
		{
			CTaskAchieveHelper::addTask(uid, pNewConf->Tasks[i]);
		}
	}

	if (!pNewConf->Achieves.empty())
	{
		for (size_t i = 0; i < pNewConf->Achieves.size(); ++i)
		{
			CTaskAchieveHelper::addAchieve(uid, pNewConf->Achieves[i]);
		}
	}

	if (pBDModel->init(uid))
	{

	}

    //pvp宝箱模型


	initUserData(pGameUser);
	return pGameUser;
}

//更新用户需要重置的数据
void CGameUserManager::reSetGameUserData(int uid, bool bLogin)
{
	CGameUser *pGameUser = getGameUser(uid);
	if (pGameUser == NULL)
	{
		return;
	}
    initUserData(pGameUser);
    updateGameUserData(pGameUser, bLogin);
}

void CGameUserManager::updateGameUserData(CGameUser* gameUsr, bool bLogin)
{
    if (gameUsr == NULL)
    {
        return;
    }
    int uid = gameUsr->getUid();
    //更新金币购买次数
    CModelHelper::updateUserModelReset(uid, bLogin);
    //重置金币副本信息
    CModelHelper::updateGoldTestInfo(uid);
    //重置英雄试炼信息
    CModelHelper::updateHeroTestInfo(uid);
    //登录时先更新爬塔试练的数据
    CModelHelper::updateTowerTestInfo(uid);
    //重置PVP信息
    CModelHelper::updatePvpInfo(uid);
    //重置商店信息
    CModelHelper::updateShopData(uid);
    CModelHelper::checkShopFreshTimes(uid);
    // 先处理掉过期的邮件
    CModelHelper::removeExpireGlobalMail();
    CModelHelper::removeExpireMail(uid);
    // 任务重置数据
    CModelHelper::updateTaskInfo(uid);
    // 个人公会信息
    CModelHelper::updateUserUnionInfo(uid, false);
}

void CGameUserManager::addGameUser(int uid, CGameUser* gameUsr)
{
	std::map<int, CGameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter == m_GameUsers.end())
	{
		m_GameUsers[uid] = gameUsr;
	}
}

//检测用户在该服务器是否存在
bool CGameUserManager::checkUserIsExist(int uid)
{
	std::map<int, CGameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter == m_GameUsers.end())
	{
		return false;
	}
	return true;
}

void CGameUserManager::removeGameUser(int uid)
{
	if (m_TimeManager == NULL)
	{
		return;
	}

	std::list<SDelayDelData>::iterator ator;
	SDelayDelData DelData;

	DelData.nDelayTime = (int)time(NULL) + DELAY_TIME_NUM;
	DelData.nUid = uid;
	ator = m_DelUserList.insert(m_DelUserList.end(), DelData);
	m_MapDelUserList[uid] = ator;
}

//删除移除用户数据
void CGameUserManager::donotDeleteUser(int uid)
{
	std::map<int, std::list<SDelayDelData>::iterator>::iterator ator = m_MapDelUserList.find(uid);
	if (ator != m_MapDelUserList.end())
	{
		std::list<SDelayDelData>::iterator iter = ator->second;
		m_DelUserList.erase(iter);
		m_MapDelUserList.erase(ator);
	}
}

bool CGameUserManager::initUserData(CGameUser* gameUsr)
{
	if (gameUsr == NULL)
	{
		return false;
	}
	//清除监听
	gameUsr->getUserActionManager()->CleanAction();
	//添加监听
	CTaskAchieveHelper::initTask(gameUsr->getUid());
	CTaskAchieveHelper::initAchieve(gameUsr->getUid());
	CActiveHelper::getInstance()->initActiveTask(gameUsr->getUid());
	return true;
}

//真正删除用户
void CGameUserManager::RealremoveGameUser(int uid)
{
	std::map<int, CGameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter != m_GameUsers.end())
	{
		delete iter->second;
		m_GameUsers.erase(iter);
	}
}
