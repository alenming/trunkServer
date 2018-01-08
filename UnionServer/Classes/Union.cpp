#include "Union.h"
#include "ServiceDef.h"
#include "KxCSComm.h"
#include "ConfUnion.h"
#include "GameUserManager.h"
#include "KxCommManager.h"
#include "Protocol.h"
#include "UnionExpiditionProtocol.h"
#include "ShopProtocol.h"

using namespace std;

CUnion::CUnion()
{
	m_RewardMapID = 0;
	m_pUnionModel = NULL;
	m_pExpiditionRankModel = NULL;
	m_pUnionExpiditionModel = NULL;
	m_pMercenaryModel = NULL;
	m_pUnionShopModel = NULL;
}

CUnion::~CUnion()
{
    if (m_pUnionModel)
    {
		m_RewardMapID = 0;
        delete m_pUnionModel;
        m_pUnionModel = NULL;
    }
}

void CUnion::onTimer(const KxServer::kxTimeVal& now)
{
	CHECK_RETURN_DEBUG_VOID(m_pUnionModel != NULL && m_pUnionExpiditionModel != NULL && m_pExpiditionRankModel != NULL)
	//获取最新排行数据
	m_pExpiditionRankModel->Refresh();
	map<int, int>& MapMember = m_pUnionModel->GetMembers();
	SExpeditionMapData *pMapData = queryConfExpiditionMapData(m_RewardMapID);
	CHECK_RETURN_DEBUG_VOID(pMapData != NULL);

	//生成商店数据
	std::map<int, int> MapDropShop;
	for (int i = 0; i < pMapData->vecShopGoods.size(); i++)
	{
		CItemDrop::UnionShopDrop(pMapData->vecShopGoods[i], MapDropShop);
		m_pUnionShopModel->setUnionShop(MapDropShop);
	}
	
	int nRank = 0;
	for (map<int, int>::iterator ator = MapMember.begin(); ator != MapMember.end(); ++ator)
	{
		//商店数据刷新下发
		sendUnionShopData(ator->first);

		m_pExpiditionRankModel->getDamageRank(ator->first, nRank);
		vector<int> RewardValue;
		if (nRank == 0)
		{
			continue;
		}

		if (nRank <= 3)
		{
			RewardValue.push_back(pMapData->nGoodsID[nRank - 1]);
		}
		else if (nRank > 3 && nRank <= 10)
		{
			RewardValue.push_back(pMapData->nGoodsID[3]);
		}
		RewardValue.push_back(pMapData->nWinGoodsID);

		CGameUser *pGameUser = CGameUserManager::getInstance()->getGameUser(ator->first);
		if (pGameUser != NULL)
		{
			CUserUnionModel *pUserUnionModel = dynamic_cast<CUserUnionModel*>(pGameUser->getModel(MODELTYPE_USERUNION));
            CHECK_RETURN_VOID(NULL != pUserUnionModel);
			pUserUnionModel->setUserExpiditionValue(RewardValue);
		}
		else
		{
			CUserUnionModel::setOffLineExpiditionValue(ator->first, RewardValue);
		}

		UnionExpditionRewardFlagSC FlagSC;
		FlagSC.cFlag = 1;
		CKxCommManager::getInstance()->sendData(ator->first, CMD_UNIONEXPIDITION, CMD_UNIONEXPIDITION_REWARD_FLAG_SC,
			reinterpret_cast<char *>(&FlagSC), sizeof(UnionExpditionRewardFlagSC));
	}

	m_RewardMapID = 0;
	m_pUnionExpiditionModel->setExpiditinoFieldVale(UNION_EXPIDITION_REWARDSENDTIME, 0);
}

bool CUnion::init(int unionID, int chairmanID, std::string unionName)
{
    CUnionModel *pUnionModel = new CUnionModel();
	CUnionExpiditionModel *pExpionModel = new CUnionExpiditionModel();
	CExpiditonRankModel *pRankModel = new CExpiditonRankModel();
	CUnionMercenaryModel *pMercenaryModel = new CUnionMercenaryModel();
	CUnionShopModel *pUnionShopModel = new CUnionShopModel();
	if (pUnionModel->NewUnion(unionID, chairmanID, unionName)
		&& pUnionModel->AddMember(chairmanID, UNION_POS_CHAIRMAN)
		&& pExpionModel->init(unionID)
		&& pRankModel->init(unionID)
		&& pMercenaryModel->init(unionID)
		&& pUnionShopModel->init(unionID)
		)
    {
        m_pUnionModel = pUnionModel;
		m_pExpiditionRankModel = pRankModel;
		m_pUnionExpiditionModel = pExpionModel;
		m_pMercenaryModel = pMercenaryModel;
		m_pUnionShopModel = pUnionShopModel;
        return true;
    }

    delete pUnionModel;
	delete pExpionModel;
	delete pRankModel;
	delete pMercenaryModel;
	delete pUnionShopModel;
    return false;
}

bool CUnion::init(int unionID)
{
    CUnionModel *pUnionModel = new CUnionModel();
	CUnionExpiditionModel *pExpionModel = new CUnionExpiditionModel();
	CExpiditonRankModel *pRankModel = new CExpiditonRankModel();
	CUnionMercenaryModel *pMercenaryModel = new CUnionMercenaryModel();
	CUnionShopModel *pUnionShopModel = new CUnionShopModel();
    if (!pUnionModel->init(unionID)
		|| !pExpionModel->init(unionID)
		|| !pRankModel->init(unionID)
		|| !pMercenaryModel->init(unionID)
		|| !pUnionShopModel->init(unionID)
		)
    {
        delete pUnionModel;
		delete pExpionModel;
		delete pRankModel;
		delete pMercenaryModel;
		delete pUnionShopModel;
        return false;
    }

    m_pUnionModel = pUnionModel;
	m_pExpiditionRankModel = pRankModel;
	m_pUnionExpiditionModel = pExpionModel;
	m_pMercenaryModel = pMercenaryModel;
	m_pUnionShopModel = pUnionShopModel;
    return true;
}

std::string CUnion::getUnionName()
{
    if (m_pUnionModel)
    {
        return m_pUnionModel->GetUnionName();
    }

    return "";
}

bool CUnion::removeUnion()
{
    if (m_pUnionModel)
    {
        delete m_pUnionModel;
        m_pUnionModel = NULL;
    }

    return true;
}

bool CUnion::deleteUnion()
{
    if (m_pUnionModel)
    {
        m_pUnionModel->DeleteUnion();
        
        delete m_pUnionModel;
        m_pUnionModel = NULL;
    }

    return true;
}

bool CUnion::sendUnionShopData(int nUid)
{
    CHECK_RETURN(NULL != m_pUnionShopModel);
	//获取最新商店数据
	m_pUnionShopModel->Refresh();
	std::map<int, int> MapShop = m_pUnionShopModel->getUnionShopMap();
	int nSendLength = sizeof(SUnionShopFreshSC)+sizeof(SUnionShopGoodsData)*MapShop.size();
	char *pSendData = reinterpret_cast<char *>(KxServer::kxMemMgrAlocate(nSendLength));
	int nOffset = 0;

	SUnionShopFreshSC *pUnionShop = (SUnionShopFreshSC*)pSendData;
	pUnionShop->sNum = MapShop.size();
	int nValue = 0;
	m_pUnionExpiditionModel->getExpiditionFieldValue(UNION_EXPIDITION_FINISHTIME, nValue);
	pUnionShop->cFlag = nValue;
	SUnionShopGoodsData *pUnionGoodsData = reinterpret_cast<SUnionShopGoodsData *>(pUnionShop + 1);
	nOffset += sizeof(SUnionShopFreshSC);
	for (map<int, int>::iterator ator = MapShop.begin(); ator != MapShop.end(); ++ator)
	{
		const ShopGoodsConfigData *pTemplateGoods = queryConfShopData(ator->first);
		if (pTemplateGoods == NULL)
		{
			pUnionShop->sNum--;
			continue;
		}

		pUnionGoodsData->sGoodsShopID = ator->first;
		pUnionGoodsData->cGoodsNum = static_cast<int>(ator->second);
		pUnionGoodsData->nCoinNum = pTemplateGoods->nCoinNum;
		pUnionGoodsData->cCoinType = pTemplateGoods->nCoinType;
		pUnionGoodsData->nGoodsID = pTemplateGoods->nGoodsID;
		pUnionGoodsData->cSale = pTemplateGoods->nSale;
		pUnionGoodsData++;
		nOffset += sizeof(SUnionShopGoodsData);
	}

	CKxCommManager::getInstance()->sendData(nUid, CMD_SHOP, CMD_UNIONSHOP_FRESHSC, pSendData,nOffset);
	return true;
}
