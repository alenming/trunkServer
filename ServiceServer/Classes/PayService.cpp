#include "PayService.h"
#include "PayProtocol.h"
#include "GameUserManager.h"
#include "ServiceDef.h"
#include "ConfGameSetting.h"
#include "TimeCalcTool.h"
#include "ModelHelper.h"
#include "CommOssHelper.h"
#include "ErrorCodeProtocol.h"
#include "KxCommManager.h"
#include "HttpHelper.h"
#include "CommTools.h"
#include "StorageManager.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "ConfAnalytic.h"
#include "Protocol.h"

using namespace KxServer;
using namespace std;

void CPayService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
    case CMD_PAY_YSDK_CS:
		processPayService(uid, buffer, len, commun);
        break;

    default:
        break;
	}
}

void CPayService::processPayService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(CModelHelper::getModel(uid, MODELTYPE_USER));
	CUserActiveModel *pUserActiveModel = dynamic_cast<CUserActiveModel*>(CModelHelper::getModel(uid, MODELTYPE_ACTIVE));
	CHECK_RETURN_DEBUG_VOID(NULL != pUserModel && NULL != pUserActiveModel);

	Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGE_PAY);
	CHECK_RETURN_DEBUG_VOID(pStorage != NULL);
	CRedisStorer *pRedisStorer = static_cast<CRedisStorer*>(pStorage->GetStorer(uid));
	CHECK_RETURN_DEBUG_VOID(pRedisStorer != NULL);

	string strBillKey = ModelKey::AnySDKPayKey(uid);
	vector<string> vecBill;
	PayYSDKSC SDKSC;
	
	if (SUCCESS != pRedisStorer->ExistKey(strBillKey))
	{
		KXLOGDEBUG("uid %d strBillKey key %s is not exist!", uid, strBillKey.c_str());
	}
	else
	{
		CHECK_RETURN_DEBUG_VOID(SUCCESS == pRedisStorer->ListRange(strBillKey, vecBill));
		for (size_t i = 0; i < vecBill.size(); i++)
		{
			VecInt vec;
			CConfAnalytic::ToJsonInt(vecBill[i], vec);
			CHECK_CONTINUE(vec.size() >= 6);
			SDKSC.nVipLv = vec[0];
			SDKSC.nVipNum = vec[1];
			SDKSC.result = vec[2];

			pUserModel->GetUserFieldVal(USR_FD_DIAMOND, SDKSC.nDiamond, true);

			//充值成功
			if (SDKSC.result == PAYBACK_SUCESS_TYPE)
			{
				int nPayNum = vec[3];
				int nGetDiamond = vec[4];
				SDKSC.nPid = vec[5];
				//月卡拿时间戳
				if (SDKSC.nPid == MONTH_PID)
				{
					//月卡时为时间戳
					SDKSC.nDiamond = nGetDiamond;
					KXLOGDEBUG("uid %d Pay Success PID %d", uid, SDKSC.nPid);
				}
				else if (SDKSC.nPid == FOND_PID)
				{
					CModelHelper::DispatchActionEvent(uid, ELA_FUND, NULL, 0);
				}
				else
				{
					CModelHelper::DispatchActionEvent(uid, ELA_PAYDIAMOND, &nGetDiamond, sizeof(nGetDiamond));
				}

				CModelHelper::DispatchActionEvent(uid, ELA_RMVBCOST, &nPayNum, sizeof(nPayNum));
			}

			CKxCommManager::getInstance()->sendData(uid, CMD_PAY, CMD_PAY_YSDK_SC, (char*)&SDKSC, sizeof(SDKSC));
			KXLOGDEBUG("uid %d Pay Sucess PID %d SendData", uid, SDKSC.nPid);
		}

		if (SUCCESS != pRedisStorer->DelKey(strBillKey))
		{
			KXLOGDEBUG("uid %d DelKey %s failed", uid, strBillKey);
			return;
		}
	}
}
