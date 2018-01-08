#include "ServiceModule.h"
#include "LoginService.h"
#include "UserService.h"
#include "BagService.h"
#include "HeroService.h"
#include "SummonerService.h"
#include "TeamService.h"
#include "TaskService.h"
#include "GuideService.h"
#include "AchieveService.h"
#include "MailService.h"
#include "ServerService.h"
#include "ChapterService.h"
#include "InstanceService.h"
#include "GoldTrialService.h"
#include "HeroTrialService.h"
#include "TowerTrialService.h"
#include "ShopService.h"
#include "RankService.h"
#include "ActiveService.h"
#include "PayProtocol.h"
#include "ModelHelper.h"
#include "PayService.h"
#include "UnionRewardService.h"
#include "PvpChestService.h"
#include "PvpProtocol.h"
#include "Protocol.h"
#include "ServerProtocol.h"
#include "PvpBattleService.h"
#include "LookService.h"
#include "KxLog.h"

CServiceModule::CServiceModule()
{
}

CServiceModule::~CServiceModule(void)
{
}

int CServiceModule::processLength(char* buffer, unsigned int len)
{
    if (len < sizeof(Head))
    {
        return sizeof(Head);
    }

	// 返回一个包的长度, 由包头解析, 解析的长度包括包头
    return reinterpret_cast<Head*>(buffer)->length;
}

void CServiceModule::processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target)
{
	Head *head = reinterpret_cast<Head *>(buffer);
	int cmd = head->MainCommand();
	int subcmd = head->SubCommand();
	int uid = head->id;
	char *data = reinterpret_cast<char *>(head->data());
	int length = head->length - sizeof(Head);

	//KXLOGDEBUG("ServiceServer maincmd %d, subcmd %d, len %d, uid %d", cmd, subcmd, len, uid);
	switch (cmd)
	{
	case CMD_LOGIN:
		CLoginService::processService(subcmd, uid, data, length, target);
		break;

	case CMD_USER:
		CUserService::processService(subcmd, uid, data, length, target);
		break;
    case CMD_PVP:
        CPvpBattleService::processService(subcmd, uid, data, length, target);
        break;
	case CMD_SUMMONER:
		CSummonerService::processService(subcmd, uid, data, length, target);
		break;

	case CMD_HERO:
		CHeroService::processService(subcmd, uid, data, length, target);
		break;

	case CMD_BAG:
		CBagService::processService(subcmd, uid, data, length, target);
		break;

	case CMD_TEAM:
		CTeamService::processService(subcmd, uid, data, length, target);
		break;

    case CMD_TASK:
        CTaskService::processService(subcmd, uid, data, length, target);
        break;

    case CMD_ACHIEVEMENT:
        CAchieveService::processService(subcmd, uid, data, length, target);
        break;
	
	case CMD_GUIDE:
		CGuideService::processService(subcmd, uid, data, length, target);
		break;

    case CMD_MAIL:
        CMailService::processService(subcmd, uid, data, length, target);
        break;

	case CMD_PVPCHEST:
		CPvpChestService::processService(subcmd, uid, data, length, target);
		break;

	case CMD_STAGE:
        CChapterService::processService(cmd, subcmd, uid, data, length, target);
		break;

	case CMD_INSTANCE:
        CInstanceService::processService(subcmd, uid, data, length, target);
		break;

	case CMD_GOLDTEST:
        CGoldTrialService::ProcessService(cmd, subcmd, uid, data, length, target);
		break;

	case CMD_HEROTEST:
        CHeroTrialService::ProcessService(cmd, subcmd, uid, data, length, target);
		break;

	case CMD_TOWERTEST:
        CTowerTrialService::processService(cmd, subcmd, uid, data, length, target);
		break;

	case CMD_SHOP:
		CShopService::processService(subcmd, uid, data, length, target);
		break;
	case CMD_RANK:
		//CRankService::ProcessService(subcmd, uid, data, length, target);
		break;
	case CMD_ACTIVE:
		CActiveService::processService(subcmd, uid, data, length, target);
		break;
    case CMD_PAY:
        CPayService::processService(subcmd, uid, data, length, target);
        break;
	case CMD_UNIONEXPIDITION:
        CUnionExpiditionRewardService::processService(subcmd, uid, data, length, target);
		break;
    case CMD_UNION:
        CUnionRewardService::processService(subcmd, uid, data, length, target);
        break;
    case CMD_LOOK:
        CLookService::processService(subcmd, uid, data, length, target);
        break;
	case ServerProtocol::ServerMain:
		CServerService::processService(subcmd, uid, data, length, target);
		break;

	default:
		break;
	}
}

void CServiceModule::processError(KxServer::IKxComm *target)
{

}