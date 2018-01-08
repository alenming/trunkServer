#include "MailService.h"
#include "MailProtocol.h"
#include "Protocol.h"
#include "GameUserManager.h"
#include "KxMemPool.h"
#include "GameDef.h"
#include "GlobalMailModel.h"
#include "PropUseHelper.h"
#include "ModelHelper.h"
#include "KxCommManager.h"
#include "CommOssHelper.h"

// 领取邮件奖励背包判断并添加数据
bool getAwardItems(int uid, int &remainCapacity, std::vector<ID_Num> items, std::vector<DropItemInfo> &rewardItems)
{
    for (unsigned int i = 0; i < items.size(); i++)
    {
        remainCapacity -= items[i].num;
        if (remainCapacity < 0)
        {
            return false;
        }
    }

    for (unsigned int i = 0; i < items.size(); ++i)
    {
        DropItemInfo dropInfo;
        dropInfo.id = items[i].ID;
        dropInfo.num = items[i].num;
		rewardItems.push_back(dropInfo);
    }

	bool bEmailSend = false;
	CPropUseHelper::getInstance()->AddItems(uid, rewardItems, bEmailSend, MAIL_GAIN_REASON);

    return true;
}

void CMailService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
	case CMD_MAIL_READMAIL_CS:
		processReadMailContextService(uid, buffer, len, commun);
		break;
	case CMD_MAIL_GETMAILGOODS_CS:
		processGetMailGoodsService(uid, buffer, len, commun);
        break;
    case CMD_MAIL_WEBMAIL_CS:
        processWebMailService(uid, buffer, len, commun);
        break;
    case CMD_MAIL_UPDATEMAIL_SS:
        processUpdateMailService(uid, buffer, len, commun);
        break;
	}
}

void CMailService::processReadMailContextService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	ReadMailCS *pReadMailCS = reinterpret_cast<ReadMailCS*>(buffer);
	CHECK_RETURN_DEBUG_VOID(len == sizeof(ReadMailCS));
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN_DEBUG_VOID(NULL != pGameUser);
	CMailModel *pMailModel = dynamic_cast<CMailModel*>(pGameUser->getModel(MODELTYPE_MAIL));
	CHECK_RETURN_DEBUG_VOID(NULL != pMailModel);

	if (pReadMailCS->mailType == MAIL_TYPE_WEB)
	{
		CGlobalMailModel *pGlobalMailModel = CGlobalMailModel::getInstance();
		int mailId = pReadMailCS->mailID;
		std::map<int, BaseMailInfo>& mailBaseMap = pGlobalMailModel->GetGlobalMailBase();
		CHECK_RETURN_DEBUG_VOID(mailBaseMap.find(mailId) != mailBaseMap.end());
		//该邮件是否为已读
		CHECK_RETURN_DEBUG_VOID(!pMailModel->IsReadWebMail(mailId));

		ReadMailSC *pMailSC = NULL;
		int len = sizeof(ReadMailSC);
		char *buff = NULL;
		int nContextLen = 0;
		std::map<int, std::vector<DropItemInfo> >& mailItemsMap = pGlobalMailModel->GetGlobalMailItems();
		std::map<int, std::vector<DropItemInfo> >::iterator itemsIter = mailItemsMap.find(mailId);
		if (itemsIter == mailItemsMap.end())
		{
			//没有物品, 加入已读列表
			CHECK_RETURN_DEBUG_VOID(pMailModel->AddWebMail(mailId, MAIL_STATUS_READ));

			std::map<int, std::string>& mailCtxMap = pGlobalMailModel->GetGlobalMailContexts();
			std::map<int, std::string>::iterator iter = mailCtxMap.find(mailId);
			if (iter != mailCtxMap.end())
			{
				len += iter->second.length() + 1;
				nContextLen = iter->second.length() + 1;
			}

			buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(len));
			pMailSC = reinterpret_cast<ReadMailSC*>(buff);
			pMailSC->mailType = MAIL_TYPE_WEB;
			pMailSC->mailID = mailId;
			pMailSC->nCount = 0;
			pMailSC->nContextLen = nContextLen;
			char *pContext = reinterpret_cast<char*>(pMailSC + 1);
			if (iter != mailCtxMap.end())
			{
				strncpy(pContext, iter->second.c_str(),iter->second.length() + 1);
			}
		}
		else
		{
			std::map<int, std::string>& mailCtxMap = pGlobalMailModel->GetGlobalMailContexts();
			std::map<int, std::string>::iterator iter = mailCtxMap.find(mailId);
			if (iter != mailCtxMap.end())
			{
				len += iter->second.length()+1;
				nContextLen = iter->second.length()+1;
			}

			len += sizeof(DropItemInfo)* itemsIter->second.size();
			buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(len));
			pMailSC = reinterpret_cast<ReadMailSC*>(buff);
			pMailSC->mailType = MAIL_TYPE_WEB;
			pMailSC->mailID = mailId;
			pMailSC->nCount = itemsIter->second.size();
			pMailSC->nContextLen = nContextLen;
			DropItemInfo *pItemInfo = reinterpret_cast<DropItemInfo*>(pMailSC + 1);
			for (int i = 0; i < itemsIter->second.size(); i++)
			{
				memcpy(pItemInfo, &itemsIter->second[i], sizeof(DropItemInfo));
				pItemInfo++;
			}

			char *pContext = reinterpret_cast<char*>(pItemInfo);
			if (iter != mailCtxMap.end())
			{
				strncpy(pContext, iter->second.c_str(), iter->second.length() + 1);
			}
		}

		CKxCommManager::getInstance()->sendData(uid, CMD_MAIL, CMD_MAIL_READMAIL_SC, buff, len);
		KxServer::kxMemMgrRecycle(buff,len);
	}
	else if (pReadMailCS->mailType == MAIL_TYPE_NORMAL)
	{
		CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
		CHECK_RETURN_DEBUG_VOID(NULL != pGameUser);

		CMailModel *pMailModel = dynamic_cast<CMailModel*>(pGameUser->getModel(MODELTYPE_MAIL));
		CHECK_RETURN_DEBUG_VOID(NULL != pMailModel);

		std::map<int, BaseMailInfo> normalMailMap = pMailModel->GetMailBasicInfo();
		std::map<int, std::vector<DropItemInfo> > mailItemMap = pMailModel->GetMailItemInfo();
		std::map<int, std::vector<DropItemInfo> >::iterator itemIter = mailItemMap.find(pReadMailCS->mailID);
		//没有这个邮件, 或者为配置邮件
		std::map<int, BaseMailInfo>::iterator iter = normalMailMap.find(pReadMailCS->mailID);
		if (iter == normalMailMap.end())
		{
			return;
		}

		ReadMailSC *pMailSC = NULL;
		int len = sizeof(ReadMailSC);
		char *buff = NULL;
		int nContextLen = 0;
		//没有物品
		if (itemIter == mailItemMap.end())
		{
			std::map<int, std::string> mailContextMap = pMailModel->GetMailContextInfo();
			std::map<int, std::string>::iterator ator = mailContextMap.find(pReadMailCS->mailID);
			if (ator != mailContextMap.end())
			{
				len += ator->second.length() + 1;
				nContextLen = ator->second.length() + 1;
			}

			buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(len));
			pMailSC = reinterpret_cast<ReadMailSC*>(buff);
			pMailSC->mailType = MAIL_TYPE_NORMAL;
			pMailSC->mailID = pReadMailCS->mailID;
			pMailSC->nCount = 0;
			pMailSC->nContextLen = nContextLen;
			char *pContext = reinterpret_cast<char*>(pMailSC + 1);
			if (ator != mailContextMap.end())
			{
				strncpy(pContext, ator->second.c_str(), ator->second.length() + 1);
			}

			//邮件没有物品，删除邮件
			pMailModel->RemoveMail(pReadMailCS->mailID);
		}
		else
		{
			std::map<int, std::string> mailContextMap = pMailModel->GetMailContextInfo();
			std::map<int, std::string>::iterator ator = mailContextMap.find(pReadMailCS->mailID);
			nContextLen = 0;
			if (ator != mailContextMap.end())
			{
				len += ator->second.length() + 1;
				nContextLen = ator->second.length() + 1;
			}

			len += sizeof(DropItemInfo)* itemIter->second.size();
			buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(len));
			pMailSC = reinterpret_cast<ReadMailSC*>(buff);
			pMailSC->mailType = MAIL_TYPE_NORMAL;
			pMailSC->mailID = pReadMailCS->mailID;
			pMailSC->nCount = itemIter->second.size();
			pMailSC->nContextLen = nContextLen;
			DropItemInfo *pItemInfo = reinterpret_cast<DropItemInfo*>(pMailSC + 1);
			for (int i = 0; i < itemIter->second.size(); i++)
			{
				memcpy(pItemInfo, &itemIter->second[i], sizeof(DropItemInfo));
				pItemInfo++;
			}

			char *pContext = reinterpret_cast<char*>(pItemInfo);
			if (ator != mailContextMap.end())
			{
				strncpy(pContext, ator->second.c_str(), ator->second.length() + 1);
			}
		}

		CKxCommManager::getInstance()->sendData(uid, CMD_MAIL, CMD_MAIL_READMAIL_SC, buff, len);
		KxServer::kxMemMgrRecycle(buff, len);
	}
}

void CMailService::processGetMailGoodsService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	GetMailGoodsCS *pReadMailCS = reinterpret_cast<GetMailGoodsCS*>(buffer);
	int nNeedLen = sizeof(GetMailGoodsCS)+(pReadMailCS->normalMailCount + pReadMailCS->webMailCount) * sizeof(int);
	CHECK_RETURN_DEBUG_VOID(len == nNeedLen);
	CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
	CHECK_RETURN_DEBUG_VOID(NULL != pGameUser);
	CMailModel *pMailModel = dynamic_cast<CMailModel*>(pGameUser->getModel(MODELTYPE_MAIL));
	CUserModel *pUserModel = dynamic_cast<CUserModel*>(pGameUser->getModel(MODELTYPE_USER));
	CGlobalMailModel *pGlobalMailModel = CGlobalMailModel::getInstance();
	CHECK_RETURN_DEBUG_VOID(pMailModel != NULL);
	CHECK_RETURN_DEBUG_VOID(pUserModel != NULL);
	CHECK_RETURN_DEBUG_VOID(pGlobalMailModel != NULL);

	// 读取(领取奖励)的邮件
	std::list<int> readNorMails;
	std::list<int> readWebMails;

	// 获取奖励列表, 道具(英雄卡、装备、消耗品等)
	std::vector<DropItemInfo> rewardItems;
	std::vector<DropItemInfo> tempMailItems;

	CPropUseHelper *pPropUseHelper = CPropUseHelper::getInstance();
	// 按顺序读取,直到一封邮件的东西发不下背包
	int *pMailID = reinterpret_cast<int*>(pReadMailCS + 1);
	// 1.处理普通邮件
	if (pReadMailCS->normalMailCount > 0)
	{
		BaseMailInfo normalMailInfo;
		for (int i = 0; i < pReadMailCS->normalMailCount; i++)
		{
			int mailId = *pMailID;
			tempMailItems.clear();
			if (pMailModel->GetMailInfo(mailId, normalMailInfo, tempMailItems))
			{
				if (tempMailItems.size() == 0)
				{
					pMailID += 1;
					continue;
				}

				//检查
				if (!CModelHelper::checkAllCapacity(uid, tempMailItems))
				{
					break;	
				}

				if (pPropUseHelper->AddItems(uid, tempMailItems, false, MAIL_GAIN_REASON))
				{
					rewardItems.insert(rewardItems.end(), tempMailItems.begin(), tempMailItems.end());
					readNorMails.push_back(mailId);
				}

				for (int i = 0; i < tempMailItems.size(); i++)
				{

					if (tempMailItems[i].id == RESOURCEID_DIAMOND)
					{
						CCommOssHelper::userDiamondGetOss(uid, tempMailItems[i].num, MAIL_DIAMOND_GET_REASON);
					}
					//else if (tempMailItems[i].id == RESOURCEID_ENERGY)
					//{
					//	CCommOssHelper::userTokenShopOss(uid, TASK_PAYLER_TYPE, 0, NONE_PAY_TYPE, tempMailItems[i].num, iterItems->ID);
					//}
				}
			}
			pMailID += 1;
			pMailModel->RemoveMail(mailId);
		}
	}
	// 2.处理web邮件
	if (pReadMailCS->webMailCount > 0) // 普通邮件读完并且有web邮件
	{
		for (int i = 0; i < pReadMailCS->webMailCount; ++i)
		{
			int mailId = *pMailID;
			tempMailItems.clear();
			BaseMailInfo gMailBase;
			if (pGlobalMailModel->GetGlobalMailBase(mailId, gMailBase))
			{
				if (!pGlobalMailModel->GetGlobalMailItems(mailId, tempMailItems)
					|| pMailModel->IsReadWebMail(mailId))
				{
					pMailID += 1;
					continue;
				}

				if (tempMailItems.size() == 0)
				{
					pMailID += 1;
					continue;
				}

				int nUserCreatTime = 0;
				pUserModel->GetUserFieldVal(USR_FD_CREATETIME, nUserCreatTime);
				if (nUserCreatTime > gMailBase.sendTimeStamp)
				{
					pMailID += 1;
					continue;
				}

				//检查
				if (!CModelHelper::checkAllCapacity(uid, tempMailItems))
				{
					break;
				}

				if (pPropUseHelper->AddItems(uid, tempMailItems, false, MAIL_GAIN_REASON))
				{
					rewardItems.insert(rewardItems.end(), tempMailItems.begin(), tempMailItems.end());
					readWebMails.push_back(mailId);

					for (int i = 0; i < tempMailItems.size(); i++)
					{

						if (tempMailItems[i].id == RESOURCEID_DIAMOND)
						{
							CCommOssHelper::userDiamondGetOss(uid, tempMailItems[i].num, MAIL_DIAMOND_GET_REASON);
						}
						//else if (tempMailItems[i].id == RESOURCEID_ENERGY)
						//{
						//	CCommOssHelper::userTokenShopOss(uid, TASK_PAYLER_TYPE, 0, NONE_PAY_TYPE, tempMailItems[i].num, iterItems->ID);
						//}
					}
				}
			}

			pMailID += 1;
			CHECK_RETURN_DEBUG_VOID(pMailModel->AddWebMail(mailId,MAIL_STATUS_READ));
		}
	}

	int nBuffSize = sizeof(GetMailGoodsSC)+(readNorMails.size() + readWebMails.size()) * sizeof(int);
	if (!rewardItems.empty())
	{
		nBuffSize += sizeof(DropItemInfo)* rewardItems.size();
	}

	char *buff = reinterpret_cast<char*>(KxServer::kxMemMgrAlocate(nBuffSize));
	GetMailGoodsSC *pReadMailSC = reinterpret_cast<GetMailGoodsSC*>(buff);
	pReadMailSC->normalMailCount = readNorMails.size();
	pReadMailSC->webMailCount = readWebMails.size();
	pReadMailSC->itemCount = rewardItems.size();

	pMailID = reinterpret_cast<int*>(pReadMailSC + 1);

	for (std::list<int>::iterator iterNorMail = readNorMails.begin();
		iterNorMail != readNorMails.end(); iterNorMail++)
	{
		pMailModel->ReadMail(*iterNorMail);
		*pMailID = *iterNorMail;
		pMailID += 1;
	}

	for (std::list<int>::iterator iterWebMail = readWebMails.begin();
		iterWebMail != readWebMails.end(); iterWebMail++)
	{
		pMailModel->AddWebMail(*iterWebMail, MAIL_STATUS_READ);
		*pMailID = *iterWebMail;
		pMailID += 1;
	}

	// 奖励封包
	DropItemInfo *pRewardInfo = reinterpret_cast<DropItemInfo*>(pMailID);
	for (unsigned int j = 0; j < rewardItems.size(); ++j)
	{
		memcpy(pRewardInfo, &(rewardItems[j]), sizeof(DropItemInfo));
		pRewardInfo += 1;
	}

	CKxCommManager::getInstance()->sendData(uid, CMD_MAIL, CMD_MAIL_GETMAILGOODS_SC, buff, nBuffSize);
	KxServer::kxMemMgrRecycle(buff, nBuffSize);
}

void CMailService::processWebMailService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CGlobalMailModel *pGlobalMailModel = CGlobalMailModel::getInstance();
	if (!pGlobalMailModel->Refresh())
	{
		return;
	}
}

void CMailService::processUpdateMailService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    CHECK_RETURN_DEBUG_VOID(NULL != pGameUser);
    CMailModel *pMailModel = reinterpret_cast<CMailModel*>(pGameUser->getModel(MODELTYPE_MAIL));
	CHECK_RETURN_DEBUG_VOID(NULL != pMailModel);

    if (!pMailModel->Refresh())
    {
        KXLOGERROR("processUpdateMailService pMailModel->Refresh() fail!!!");
    }
}
