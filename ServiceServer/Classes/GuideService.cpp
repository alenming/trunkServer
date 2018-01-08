#include "GuideService.h"
#include "Protocol.h"
#include "GuideProtocol.h"
#include "ServiceDef.h"
#include "GameUserManager.h"
#include "ConfGuide.h"
#include "CommOssHelper.h"
#include "KxCommManager.h"
#include "ModelHelper.h"

void CGuideService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	switch (subcmd)
	{
	case CMD_GUIDE_RECORD_CS:
		processRecord(uid, buffer, len, commun);
		break;
	}
}

void CGuideService::processRecord(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	CHECK_RETURN_VOID(len > sizeof(GuideRecordCS) && buffer != NULL);
	GuideRecordCS *pRecordCS = reinterpret_cast<GuideRecordCS*>(buffer);

	CGuideModel *pGuideModel = dynamic_cast<CGuideModel*>(CModelHelper::getModel(uid, MODELTYPE_GUIDE));
	CHECK_RETURN_DEBUG_VOID(NULL != pGuideModel);

	CConfGuide *pConf = dynamic_cast<CConfGuide*>(CConfManager::getInstance()->getConf(CONF_GUIDE));
	CHECK_RETURN_DEBUG_VOID(NULL != pConf);
	
	int *pGuideID = reinterpret_cast<int*>(pRecordCS + 1);
	for (int i = 0; i < pRecordCS->nEndNum; i++)
	{
		int guideId = *(int*)pGuideID;
		GuideConfItem *pGuideConf = reinterpret_cast<GuideConfItem*>(pConf->getData(*pGuideID));
		if (pGuideConf != NULL)
		{
			for (size_t j = 0; j < pGuideConf->Nexts.size(); j++)
			{
				pGuideModel->AddGuideID(pGuideConf->Nexts[j]);
			}

			for (size_t j = 0; j < pGuideConf->Closes.size(); j++)
			{
				pGuideModel->DelGuideID(pGuideConf->Closes[j]);
			}
		}
		pGuideID += 1;
	}

	StoryRecordSC SC;
	SC.nFlag = 1;
	CKxCommManager::getInstance()->sendData(uid, CMD_GUIDE, CMD_GUIDE_RECORD_SC,(char*)&SC,sizeof(SC));
}
