#include "LoginUnionService.h"
#include "UnionManager.h"
#include "GameUserManager.h"
#include "ConfGameSetting.h"
#include "UnionComm.h"
#include "UnionServer.h"
#include "KxCommManager.h"
#include "KxMemPool.h"
#include "Protocol.h"
#include "ServerProtocol.h"
#include "LoginProtocol.h"
#include "PersonMercenaryModel.h"
#include "UnionHelper.h"

using namespace std;
using namespace KxServer;

void CLoginUnionService::processService(int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
    switch (subcmd)
    {
        case CMD_LOGIN_UNION_CS:
        {
            processLoginUnionService(uid, buffer, len, commun);
        }
        break;
    }
}

// 请求登陆公会服
void CLoginUnionService::processLoginUnionService(int uid, char *buffer, int len, KxServer::IKxComm *commun)
{
	if (uid == -1)
	{
		return;
	}

    CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(uid);
    if (NULL == pGameUser)
    {
        // 可能是第一次登录公会服务器
        pGameUser = CGameUserManager::getInstance()->initGameUser(uid);
        if (NULL == pGameUser)
        {
            return;
        }
    }
    else
    {
        pGameUser->refreshModels();
    }

    CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
	CHECK_RETURN_VOID(pUserUnionModel != NULL);
    int nUnionID = 0;
    CHECK_RETURN_VOID(pUserUnionModel->getUserUnionValue(USER_UNION_ID, nUnionID));

    CUnion *pUnion = NULL;
    if (nUnionID > 0)
    {
        pUnion = CUnionManager::getInstane()->getUnion(nUnionID);
    }
    
    if (NULL == pUnion)
    {
        //检查其是否申请了公会
        map<int, int> &MapApplyUnion = pUserUnionModel->getApplyInfo();
        map<int, int>::iterator ator = MapApplyUnion.begin();
        for (; ator != MapApplyUnion.end(); ++ator)
        {
            if (CUnionManager::getInstane()->getUnion(ator->first))
            {
                //将该角色添加到链表中
                CKxCommManager::getInstance()->addKxComm(uid, commun);
                break;
            }
        }
    }
    else
    {
        //将该角色添加到链表中
        CKxCommManager::getInstance()->addKxComm(uid, commun);
        //设置路由
        CKxCommManager::getInstance()->setSessionRoute(uid, emRouteUnionKey, UNION_SERVER_ID);
        CUnionModel *pUnionModel = pUnion->getUnionModel();
		CHECK_RETURN_VOID(pUnionModel != NULL);
        int len = sizeof(LoginUnionModelInfo)+sizeof(OwnUnionInfo);
        char *buff = reinterpret_cast<char*>(KxMemManager::getInstance()->memAlocate(len));

        LoginUnionModelInfo *pLoginUnionModelInfo = reinterpret_cast<LoginUnionModelInfo *>(buff);
        pLoginUnionModelInfo->hasUnion = 1;

        OwnUnionInfo *pOwnUnionInfo = reinterpret_cast<OwnUnionInfo *>(pLoginUnionModelInfo + 1);
        if (!CUnionHelper::fillOwnUnionInfo(uid, nUnionID, *pOwnUnionInfo))
        {
            KXLOGERROR("CLoginUnionService fill own union info fail, uid %d, unionId %d", uid, nUnionID);
            return;
        }

        CKxCommManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_UNIONMODEL_SC, buff, len);
        KxMemManager::getInstance()->memRecycle(buff, len);
    }
}


