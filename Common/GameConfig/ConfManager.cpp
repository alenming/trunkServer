#include "ConfManager.h"
#include "ConfStage.h"
#include "ConfRole.h"
#include "ConfGuide.h"
#include "ConfFight.h"
#include "ConfHall.h"
#include "ConfLanguage.h"
#include "ConfGameSetting.h"
#include "ConfMusic.h"
#include "ConfOther.h"
#include "ConfActive.h"
#include "ConfUnion.h"
#include "ConfArena.h"

#ifndef RunningInServer
#include "CsvCheck.h"
#endif
#define CHECK(success) if(!success) return false;

using namespace std;

CConfManager* CConfManager::m_Instance = NULL;

CConfBase::~CConfBase()
{
	for (map<int, void*>::iterator iter = m_Datas.begin();
		iter != m_Datas.end(); ++iter)
	{
		delete iter->second;
	}
}

CConfManager* CConfManager::getInstance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new CConfManager();
	}
	return m_Instance;
}

void CConfManager::destroy()
{
	if (NULL != m_Instance)
	{
		delete m_Instance;
		m_Instance = NULL;
	}
}

CConfManager::CConfManager()
{
	m_pFunNameToID = NULL;
	m_pStateNameToID = NULL;
	m_pRoleAttributeToID = NULL;
}


CConfManager::~CConfManager()
{
	for (map<int, CConfBase*>::iterator iter = m_Confs.begin();
		iter != m_Confs.end(); ++iter)
	{
		delete iter->second;
	}
	delete m_pFunNameToID;
	m_pFunNameToID = NULL;
	delete m_pStateNameToID;
	m_pStateNameToID = NULL;
	delete m_pRoleAttributeToID;
	m_pRoleAttributeToID = NULL;
}

bool CConfManager::init()
{
	do
	{
#ifndef RunningInServer
		m_szConfRoot = "config/";
#else
		m_szConfRoot = "../config/";
#endif
		//////////////////////////////Role 目录////////////////////////////////////////////
		// 先加载转换表
		m_pFunNameToID = new CConfStrToID;
		CHECK(m_pFunNameToID->LoadCSV(m_szConfRoot + "Role/NameToId/FunNameToID.csv"));
		m_pStateNameToID = new CConfStrToID;
		CHECK(m_pStateNameToID->LoadCSV(m_szConfRoot + "Role/NameToId/StateNameToID.csv"));
		m_pRoleAttributeToID = new CConfStrToID;
		CHECK(m_pRoleAttributeToID->LoadCSV(m_szConfRoot + "Role/NameToId/RoleAttributeToID.csv"));
		CHECK(addConf(CONF_STATUS, m_szConfRoot + "Role/AIAndStatus.csv", new CConfStatus()));
		CHECK(addConf(CONF_AIDATA, m_szConfRoot + "Role/AIAndStatus.csv", new CConfAI()));
		// 一个角色对象对应两张表(logic,numerical)
		CHECK(addConf(CONF_BOSS, m_szConfRoot + "Role/Boss", new CConfBoss()));
		CHECK(addConf(CONF_CALL, m_szConfRoot + "Role/Call", new CConfCall()));
		CHECK(addConf(CONF_HERO, m_szConfRoot + "Role/Hero", new CConfHero()));
		CHECK(addConf(CONF_MONSTER, m_szConfRoot + "Role/Monster", new CConfMonster()));
		CHECK(addConf(CONF_SOLDIER, m_szConfRoot + "Role/Soldier", new CConfSoldier()));

		//////////////////////////////Fight 目录////////////////////////////////////////////
#ifndef RunningInServer
		CHECK(addConf(CONF_EFFECT, m_szConfRoot + "Fight/Animation.csv", new CConfEffect()));
		CHECK(addConf(CONF_UI_EFFECT, m_szConfRoot + "Fight/UI_Effect.csv", new CConfUIEffect()));
#endif 
		CHECK(addConf(CONF_BUFF, m_szConfRoot + "Fight/Buff.csv", new CConfBuff()));
		CHECK(addConf(CONF_BULLET, m_szConfRoot + "Fight/Bullet.csv", new CConfBullet()));
		CHECK(addConf(CONF_CARD_COUNT, m_szConfRoot + "Fight/Card_Count.csv", new CConfCardCount()));
		CHECK(addConf(CONF_COUNT, m_szConfRoot + "Fight/Count.csv", new CConfCount()));
		CHECK(addConf(CONF_CRYSTAL, m_szConfRoot + "Fight/Crystal.csv", new CConfCrystal()));
		CHECK(addConf(CONF_SEARCH, m_szConfRoot + "Fight/Search.csv", new CConfSearch()));
		CHECK(addConf(CONF_SKILL, m_szConfRoot + "Fight/Skill.csv", new CConfSkill()));

		//////////////////////////////////Hall 目录////////////////////////////////////////
		CHECK(addConf(CONF_EQUIPMENT, m_szConfRoot + "Hall/Equipment.csv", new CConfEquipment()));
		CHECK(addConf(CONF_SUIT, m_szConfRoot + "Hall/Suit.csv", new CConfSuit()));
		CHECK(addConf(CONF_OUTTER_BONUS, m_szConfRoot + "Hall/OutterBonus.csv", new CConfOutterBonus()));
        CHECK(addConf(CONF_TALENT, m_szConfRoot + "Hall/Talent.csv", new CConfTalent()));
        CHECK(addConf(CONF_TALENT_ARRANGE, m_szConfRoot + "Hall/TalentArrangement.csv", new CConfTalentArrange()));
		
#ifdef RunningInServer
		
		CHECK(addConf(CONF_ACHIEVE, m_szConfRoot + "Hall/Achievement.csv", new CConfAchieve()));
		CHECK(addConf(CONF_ACTIVITY_INSTANCE, m_szConfRoot + "Hall/ActivityInstance.csv", new CConfActivityInstance()));

		CHECK(addConf(CONF_ITEM, m_szConfRoot + "Hall/Item.csv", new CConfProp()));
		CHECK(addConf(CONF_MAIL, m_szConfRoot + "Hall/Mail.csv", new CConfMail()));
		CHECK(addConf(CONF_SOLDIER_UP_RATE, m_szConfRoot + "Hall/SoldierUpRate.csv", new CConfSoldierUpRate()));

		CHECK(addConf(CONF_SALESUMMONER, m_szConfRoot + "Hall/Summoner.csv", new CConfSaleSummoner()));
		CHECK(addConf(CONF_TASK, m_szConfRoot + "Hall/Task.csv", new CConfTask()));

		CHECK(addConf(CONF_GOLD_TEST, m_szConfRoot + "Activity/GoldTest.csv", new CConfGoldTest()));
		CHECK(addConf(CONF_GOLD_TEST_CHEST, m_szConfRoot + "Activity/GoldTestChest.csv", new CConfGoldTestChest()));
		CHECK(addConf(CONF_HERO_TEST, m_szConfRoot + "Activity/HeroTest.csv", new CConfHeroTest()));

		CHECK(addConf(CONF_TOWER_FLOOR, m_szConfRoot + "Activity/TowerFloor.csv", new CConfTowerFloor()));
		//CHECK(addConf(CONF_TOWER_BUFF, m_szConfRoot + "Activity/TowerBuff.csv", new CConfTowerBuff()));
		CHECK(addConf(CONF_TOWER_RANK, m_szConfRoot + "Activity/TowerRank.csv", new CConfTowerRank()));

		CHECK(addConf(CONF_SHOP_DIAMOND, m_szConfRoot + "Hall/DiamondShop.csv", new CConfDiamondShop()));
		CHECK(addConf(CONF_ACTIVE_TIME, m_szConfRoot + "Hall/GameOp.csv", new CConfActiveTime()));
		CHECK(addConf(CONF_ACTIVE_SHOP, m_szConfRoot + "Hall/GameOp1.csv", new CConfActiveShop()));
		CHECK(addConf(CONF_ACTIVE_DROP, m_szConfRoot + "Hall/GameOp2.csv", new CConfActiveExtraAdd()));
		CHECK(addConf(CONF_ACTIVE_TASK, m_szConfRoot + "Hall/GameOp3.csv", new CConfActiveTask()));
		CHECK(addConf(CONF_MONTH_ACTIVE, m_szConfRoot + "Hall/GameOp4.csv", new CConfMonthActive()));
		CHECK(addConf(CONF_EXCHANGE_ACTIVE, m_szConfRoot + "Hall/GameOp5.csv", new CConfExChangeActive()));
		CHECK(addConf(CONF_7DAY_ACTIVE, m_szConfRoot + "Hall/SevenDay.csv", new CConf7DayActive()));
		CHECK(addConf(CONF_BLUEDIAMOND_ACTIVE, m_szConfRoot + "Hall/BlueDiamend.csv", new CConfBDActive()));
		CHECK(addConf(CONF_SHOP_GOODS, m_szConfRoot + "Hall/Goods.csv", new CShopGoodsData()));
		CHECK(addConf(CONF_SHOP, m_szConfRoot + "Hall/Shop.csv", new CShopData()));
		CHECK(addConf(CONF_ITEMDROP, m_szConfRoot + "Hall/ItemDrop.csv", new CConfDropProp()));
		CHECK(addConf(CONF_EQUIP_CREATE, m_szConfRoot + "Hall/EquipmentCreat.csv", new CConfEqupmentCreate()));
		CHECK(addConf(CONF_DAYSIGN, m_szConfRoot + "Hall/Checkin.csv", new CConfDaySign()));
		CHECK(addConf(CONF_CONDAYSIGN, m_szConfRoot + "Hall/ConCheckin.csv", new CConfConDaySign()));

		CHECK(addConf(CONF_FIRSTPAY_SETING, m_szConfRoot + "Hall/FirstPay.csv", new CConfFirstPay()));
		CHECK(addConf(CONF_CARD_GAMBLE, m_szConfRoot + "Hall/CardGamble.csv", new CConfCardGamble()));

        CHECK(addConf(CONF_PVP_SHARE, m_szConfRoot + "Hall/PVPShare.csv", new CConfPVPShare()));
        CHECK(addConf(CONF_PVP_UPLOAD, m_szConfRoot + "Hall/PVPUpload.csv", new CConfPVPUpload()));
#endif
		//////////////////////////////// Arena目录 //////////////////////////////////////////

		CHECK(addConf(CONF_ARENA_CHEST, m_szConfRoot + "Arena/Arena_Chest.csv", new CConfArenaChest()));
		CHECK(addConf(CONF_ARENA_LEVEL, m_szConfRoot + "Arena/Arena_Level.csv", new CConfArenaLevel()));
		CHECK(addConf(CONF_ARENA_RANK, m_szConfRoot + "Arena/Arena_Rank.csv", new CConfArenaRank()));
		CHECK(addConf(CONF_ARENA_ROBOT, m_szConfRoot + "Arena/Arena_Robot.csv", new CConfArenaRobot()));
		CHECK(addConf(CONF_ARENA_ROBOTNAME, m_szConfRoot + "Arena/Arena_RobotName.csv", new CConfArenaRobotName()));
		CHECK(addConf(CONF_ARENA_SCORLLBAR, m_szConfRoot + "Arena/Arena_ScrollBar.csv", new CConfArenaScrollBar()));
		CHECK(addConf(CONF_ARENA_SETTING, m_szConfRoot + "Arena/Arena_Settings.csv", new CConfArenaSetting()));
		CHECK(addConf(CONF_ARENA_TRAINING, m_szConfRoot + "Arena/Arena_Training.csv", new CConfArenaTraining()));
		
		CHECK(addConf(CONF_PVP_GRADING_CHANGE, m_szConfRoot + "GameSetting/PVP_GradingChange.csv", new CConfPvpGradingChange()));
		CHECK(addConf(CONF_PVP_GRADING_INITIAL, m_szConfRoot + "GameSetting/PVP_GradingInitialization.csv", new CConfPvpGradingInitial()));
		CHECK(addConf(CONF_PVP_SETTING, m_szConfRoot + "GameSetting/PVP_PVPSetting.csv", new CConfPvpSetting()));

		///////////////////////////////Language 目录///////////////////////////////////////////
#ifndef RunningInServer
		CHECK(addConf(CONF_UI_LAN, m_szConfRoot + "Language/cn/uiLan.csv", new CConfLanguage()));
		CHECK(addConf(CONF_BMC_LAN, m_szConfRoot + "Language/cn/BossMonsterCall.csv", new CConfLanguage()));
		CHECK(addConf(CONF_HS_LAN, m_szConfRoot + "Language/cn/HeroSoldier.csv", new CConfLanguage()));
		CHECK(addConf(CONF_HS_SKILL_LAN, m_szConfRoot + "Language/cn/HeroSoldier_Skill.csv", new CConfLanguage()));
		CHECK(addConf(CONF_ERROR_CODE_LAN, m_szConfRoot + "Language/cn/ErrorLan.csv", new CConfLanguage()));

		//CHECK(addConf(CONF_PROP_LAN, m_szConfRoot + "Language/cn/ItemLan.csv", new CConfLanguage()));
		//CHECK(addConf(CONF_STAGE_LAN, m_szConfRoot + "Language/cn/StageLan.csv", new CConfLanguage()));
		//CHECK(addConf(CONF_BMC_SKILL_LAN, m_szConfRoot + "Language/cn/BossMonsterCall_Skill.csv", new CConfLanguage()));
		//CHECK(addConf(CONF_STORY_LAN, m_szConfRoot + "Language/cn/GuildLan.csv", new CConfLanguage()));
		//CHECK(addConf(CONF_TASK_LAN, m_szConfRoot + "Language/cn/TaskLan.csv", new CConfLanguage()));
		//CHECK(addConf(CONF_ACHIEVE_LAN, m_szConfRoot + "Language/cn/AchievementLan.csv", new CConfLanguage()));
		//CHECK(addConf(CONF_ROLE_ATTRIBUT_LAN, m_szConfRoot + "Language/cn/RoleAttributeLan.csv", new CConfLanguage()));
		//CHECK(addConf(CONF_LOADING_TIPS_LAN, m_szConfRoot + "Language/cn/BattleTips.csv", new CConfLanguage()));
#endif
		//////////////////////////////Stage 目录////////////////////////////////////////////

		CHECK(addConf(CONF_STAGE, m_szConfRoot + "Stage/Stage.csv", new CConfStage()));
		CHECK(addConf(CONF_STAGE_SCENE, m_szConfRoot + "Stage/StageScene.csv", new CConfStageScene()));

#ifdef RunningInServer
		CHECK(addConf(CONF_CHAPTER, m_szConfRoot + "Stage/Chapter.csv", new CConfChapter()));
		CHECK(addConf(CONF_MAP, m_szConfRoot + "Stage/ChapterMap.csv", new CConfMap()));
		CHECK(addConf(CONF_PVPSCENE, m_szConfRoot + "Stage/PvPScene.csv", new CConfPvpScene()));
#endif
		///////////////////////////////Guide 目录///////////////////////////////////////////

#ifdef RunningInServer
		CHECK(addConf(CONF_GUIDE, m_szConfRoot + "Guide/GuideConfig.csv", new CConfGuide()));
		CHECK(addConf(CONF_GUIDE_STEP, m_szConfRoot + "Guide/GuideStep.csv", new CConfGuideStep()));
#else
		CHECK(addConf(CONF_CAMERA, m_szConfRoot + "Guide/Camera.csv", new CConfCamera()));

		//CHECK(addConf(CONF_UI_NODE, m_szConfRoot + "Guide/UINode.csv", new CConfUINode()));
		//CHECK(addConf(CONF_UI_STATUS, m_szConfRoot + "Guide/UIStatus.csv", new CConfUIStatus()));
		//CHECK(addConf(CONF_GUIDE_BATTLE, m_szConfRoot + "Stage/FirstStage.csv", new CConfGuideBattle()));
#endif

		///////////////////////////////////GameSetting 目录///////////////////////////////////////

		CHECK(addConf(CONF_ICON_SETTING, m_szConfRoot + "GameSetting/IconSetting.csv", new CConfIconSetting()));
		CHECK(addConf(CONF_SOLDIER_STAR_SETTING, m_szConfRoot + "GameSetting/SoldierStarSetting.csv", new CConfSoldierStarSetting()));
		CHECK(addConf(CONF_SOLDIER_RARE_SETTING, m_szConfRoot + "GameSetting/SoldierRareSetting.csv", new CConfSoldierRareSetting()));
		CHECK(addConf(CONF_OUTTER_BONUS_SETTING, m_szConfRoot + "GameSetting/OutterBonusSetting.csv", new CConfOutterBonusSetting()));

#ifdef RunningInServer

		CHECK(addConf(CONF_CARD_GAMBLE_SETTING, m_szConfRoot + "GameSetting/CardGambleSetting.csv", new CConfCardGambleSetting()));
		CHECK(addConf(CONF_SKILL_UP_RATE_SETTING, m_szConfRoot + "GameSetting/SkillUpRateSetting.csv", new CConfSkillUpRateSetting()));
		CHECK(addConf(CONF_SOLDIER_LEVEL_SETTING, m_szConfRoot + "GameSetting/SoldierLevelSetting.csv", new CConfSoldierLevelSetting()));	
		CHECK(addConf(CONF_TASK_ACHIEVE_SETTING, m_szConfRoot + "GameSetting/TaskAndAchievementSetting.csv", new CConfTaskAchieveSetting()));
		CHECK(addConf(CONF_USER_LEVEL_SETTING, m_szConfRoot + "GameSetting/UserLevelSetting.csv", new CConfUserLevelSetting()));
		CHECK(addConf(CONF_TIMERECOVER, m_szConfRoot + "GameSetting/TimeRecover.csv", new CConfTimeRecover()));
		CHECK(addConf(CONF_TOWER_SETTING, m_szConfRoot + "Activity/TowerSetting.csv", new CConfTowerTestSetting()));
		//CHECK(addConf(CONF_CHAPTER_SETTING, m_szConfRoot+"GameSetting/ChapterSetting.csv", new CConfChapterSetting()));
		CHECK(addConf(CONF_INCREASE_PAY, m_szConfRoot + "GameSetting/IncreasePaymentPriceSetting.csv", new CConfIncreasePaymentPrice()));
		CHECK(addConf(CONF_SYSTEM_HEAD_ICON, m_szConfRoot + "GameSetting/SystemHeadIcon.csv", new CConfSystemHeadIcon()));
		CHECK(addConf(CONF_NEW_PLAYER, m_szConfRoot + "GameSetting/NewPlayerSetting.csv", new CConfNewPlayerSetting()));
		CHECK(addConf(CONF_EQUIP_QUALITY, m_szConfRoot + "GameSetting/EquipmentSetting.csv", new CEquipSet()));
		CHECK(addConf(CONF_UNION_MERCENARY, m_szConfRoot + "GameSetting/Mercenary.csv", new CConfUnionMercenaryPrize()));
        CHECK(addConf(CONF_EQUIP_CAST, m_szConfRoot + "GameSetting/EquipmentForCast.csv", new CConfEquipmentForCast()));
		CHECK(addConf(CONF_ITEM_CRIT, m_szConfRoot + "GameSetting/ItemCritSetting.csv", new CConfItemCrit()));
#else
		CHECK(addConf(CONF_SOUND_EFFECT_SETTING, m_szConfRoot + "GameSetting/MusicSetting.csv", new CConfSoundEffectSetting()));
		//CHECK(addConf(CONF_CHAT_SETTING, m_szConfRoot + "GameSetting/ChatSetting.csv", new CConfChatSetting()));
		//CHECK(addConf(CONF_STAGE_SETTING, m_szConfRoot + "GameSetting/StageSetting.csv", new CConfStageSetting()));
		//CHECK(addConf(CONF_ITEM_LEVEL_SETTING, m_szConfRoot + "GameSetting/ItemLevelSetting.csv", new CConfItemLevelSetting()));
        //CHECK(addConf(CONF_SYSAUTONAME, m_szConfRoot + "GameSetting/SysAutoName.csv", new CConfSysAutoName()));
#endif

        ///////////////////////////////////Union 目录///////////////////////////////////////
#ifdef RunningInServer

        CHECK(addConf(CONF_UNION_LEVEL, m_szConfRoot + "Union/UnionLevelSetting.csv", new CConfUnionLevel()));
        CHECK(addConf(CONF_UNION, m_szConfRoot + "Union/UnionSetting.csv", new CConfUnion()));
        CHECK(addConf(CONF_UNIONEXPIDTION, m_szConfRoot + "Union/Expedition.csv", new CConfExpeditionWorld()));
        CHECK(addConf(CONF_UNIONEXPIDTION_MAP, m_szConfRoot + "Union/ExpeditionMap.csv", new CConfExpeditionMap()));
        CHECK(addConf(CONF_UNIONBADGE, m_szConfRoot + "Union/UnionBadge.csv", new CConfUnionBadge()));
		CHECK(addConf(CONF_UNIONSHOPDROP, m_szConfRoot + "Union/UnionItemDrop.csv", new CConfUnionDrop()));
#endif

		///////////////////////////////////Music 目录///////////////////////////////////////
#ifndef RunningInServer
		CHECK(addConf(CONF_SOUND_EFFECT, m_szConfRoot + "Music/UIEffect.csv", new CConfUISoundEffect()));
		CHECK(addConf(CONF_AUDIO, m_szConfRoot + "Music/Music.csv", new CConfAudio()));
		CHECK(addConf(CONF_BG_MUSIC, m_szConfRoot + "Music/BackGroundMusic.csv", new CConfBgMusic()));
		CHECK(addConf(CONF_BG_MUSIC_SETTING, m_szConfRoot + "Music/BGMSetting.csv", new CConfBgMusicSetting()));
		CHECK(addConf(CONF_BUTTON_EFFECT, m_szConfRoot + "Music/UIButtonEffect.csv", new CConfUIButtonEffect()));
#endif 

		///////////////////////////////////其他///////////////////////////////////////
#ifndef RunningInServer
		CHECK(addConf(CONF_RESPATH, m_szConfRoot + "Res.csv", new CConfAnimationRes()));
		CHECK(addConf(CONF_ROLERES, m_szConfRoot + "ResPreload.csv", new CConfRoleResPreload()));
#endif

#ifndef RunningInServer
#ifdef CheckCsvTest
		CHECK(addConf(CONF_ITEMDROP, m_szConfRoot + "Hall/ItemDrop.csv", new CConfDropProp()));
#endif
#endif

	} while (0);

	return true;
}

bool CConfManager::addConf(int confId, const std::string& csvFile, CConfBase* conf)
{
	// 如果重复，则直接更新
	if (m_Confs.find(confId) != m_Confs.end())
	{
		delete m_Confs[confId];
	}

	if (NULL == conf || !conf->LoadCSV(csvFile))
	{
#ifndef RunningInServer
		cocos2d::MessageBox(("Load " + csvFile + " Error!!").c_str(), "Load CSV Error");
#else
		KXLOGDEBUG("Load %s Error", csvFile.c_str());
#endif 
		return false;
	}

	m_Confs[confId] = conf;
	return true;
}

int CConfManager::getFunID(const std::string& str)
{
	return m_pFunNameToID ? m_pFunNameToID->getIDByName(str) : 0;
}

int CConfManager::getStateID(const std::string& str)
{
	return m_pStateNameToID ? m_pStateNameToID->getIDByName(str) : 0;
}

int CConfManager::getRoleAttributeID(const std::string& str)
{
	return m_pRoleAttributeToID ? m_pRoleAttributeToID->getIDByName(str) : 0;
}
