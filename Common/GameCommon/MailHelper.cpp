#include "MailHelper.h"
#include "KxMemPool.h"
#include "MailProtocol.h"
#include "MailModel.h"
#include "UserModel.h"
#include "ConfHall.h"
#include "GameDef.h"
#include "KxCommManager.h"
#include "Protocol.h"
#include "MailProtocol.h"
#include "KXServer.h"
#include "GameUserManager.h"
#include "ModelHelper.h"

using namespace KxServer;

bool CMailHelper::saveAndSendBagFullMail(int uid, std::vector<DropItemInfo> &dropItem)
{
	CMailModel *pMailModel = reinterpret_cast<CMailModel *>(CModelHelper::getModel(uid, MODELTYPE_MAIL));
	CUserModel *pUserModel = reinterpret_cast<CUserModel *>(CModelHelper::getModel(uid, MODELTYPE_USER));
    const MailItem *pMailItem = queryConfMailItem(MAIL_CONFID_FULLBAG);
    if (pMailModel && pUserModel && pMailItem)
    {
        BaseMailInfo noramlMailInfo;
        noramlMailInfo.mailConfID = MAIL_CONFID_FULLBAG;
        noramlMailInfo.sendTimeStamp = (int)time(NULL);
		int nMailID = 0;
		if (pMailModel->AddNewMail(noramlMailInfo, dropItem, nMailID))
        {
			return CMailHelper::sendNormalMail(uid, noramlMailInfo, nMailID);
        }
    }

    return false;
}

bool CMailHelper::sendNormalMail(int uid, const BaseMailInfo &noramlMailInfo, int &nMailID)
{
    int nBuffSize = sizeof(SendMailSC)+sizeof(BaseMailInfo);
    char *buff = reinterpret_cast<char*>(kxMemMgrAlocate(nBuffSize));
    SendMailSC *pSendMailSC = reinterpret_cast<SendMailSC*>(buff);
    pSendMailSC->mailType = MAIL_TYPE_NORMAL;
	pSendMailSC->nMailID = nMailID;
    BaseMailInfo *pNoramlMailInfo = reinterpret_cast<BaseMailInfo*>(pSendMailSC + 1);
    memcpy(pNoramlMailInfo, &noramlMailInfo, sizeof(BaseMailInfo));

    CKxCommManager::getInstance()->sendData(uid, CMD_MAIL, CMD_MAIL_SENDMAIL_SC, buff, nBuffSize);
    kxMemMgrRecycle(buff, nBuffSize);

    return true;
}

//PVP每日邮件
bool CMailHelper::sendPvpRankMail(int uid, std::vector<DropItemInfo> &dropItem)
{
	CMailModel *pMailModel = reinterpret_cast<CMailModel *>(CModelHelper::getModel(uid, MODELTYPE_MAIL));
	CUserModel *pUserModel = reinterpret_cast<CUserModel *>(CModelHelper::getModel(uid, MODELTYPE_USER));
	const MailItem *pMailItem = queryConfMailItem(MAIL_CONFID_PVPRANK);
	if (pMailModel && pUserModel && pMailItem)
	{
		BaseMailInfo noramlMailInfo;
		noramlMailInfo.mailConfID = MAIL_CONFID_PVPRANK;
		noramlMailInfo.sendTimeStamp = (int)time(NULL);
		int nMailID = 0;
		if (pMailModel->AddNewMail(noramlMailInfo, dropItem, nMailID))
		{
			return CMailHelper::sendNormalMail(uid, noramlMailInfo, nMailID);
		}
	}

	return false;
}

bool CMailHelper::sendUnionMail(int uid, int configID, std::string unionName)
{
    const MailItem *pMailItem = queryConfMailItem(configID);
    if (NULL == pMailItem)
    {
        KXLOGERROR("can't find mail configid %d", configID);
        return false;
    }

    BaseMailInfo noramlMailInfo;
    noramlMailInfo.mailConfID = configID;
    noramlMailInfo.sendTimeStamp = (int)time(NULL);
    strncpy(noramlMailInfo.szTitle, unionName.c_str(), strlen(unionName.c_str()));

    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    if (NULL == pGameUser)
    {
        std::vector<DropItemInfo> items;
        return CMailModel::AddOfflineNewMail(uid, noramlMailInfo, items);
    }
    else
    {
        CMailModel *pMailModel = reinterpret_cast<CMailModel *>(pGameUser->getModel(MODELTYPE_MAIL));
        if (NULL != pMailModel)
        {
            int nMailID = 0;
            if (pMailModel->AddNewMail(noramlMailInfo, nMailID))
            {
                CKxCommManager::getInstance()->forwardData(uid, CMD_MAIL, CMD_MAIL_UPDATEMAIL_SS, NULL, 0);
                return CMailHelper::sendNormalMail(uid, noramlMailInfo, nMailID);
            }
        }
    }

    return false;
}

//爬塔邮件
bool CMailHelper::sendTowerMail(int uid, std::vector<DropItemInfo> &dropItem)
{
	CMailModel *pMailModel = reinterpret_cast<CMailModel *>(CModelHelper::getModel(uid, MODELTYPE_MAIL));
	CUserModel *pUserModel = reinterpret_cast<CUserModel *>(CModelHelper::getModel(uid, MODELTYPE_USER));
	const MailItem *pMailItem = queryConfMailItem(MAIL_CONFID_TOWERRANK);
	if (pMailModel && pUserModel && pMailItem)
	{
		BaseMailInfo noramlMailInfo;
		noramlMailInfo.mailConfID = MAIL_CONFID_TOWERRANK;
		noramlMailInfo.sendTimeStamp = (int)time(NULL);
		int nMailID = 0;
		if (pMailModel->AddNewMail(noramlMailInfo, dropItem, nMailID))
		{
			return CMailHelper::sendNormalMail(uid, noramlMailInfo, nMailID);
		}
	}

	return false;
}