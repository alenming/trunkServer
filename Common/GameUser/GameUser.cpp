#include "GameUser.h"
#include "GameUserManager.h"
#include "PersonMercenaryModel.h"

CGameUser::CGameUser() 
: m_nUid(0)
{
}

CGameUser::~CGameUser()
{
}

bool CGameUser::initModels(int userId)
{
	std::vector<ModelType> VectModelType = CGameUserManager::getInstance()->getModelType();
	for (std::vector<ModelType>::iterator iter = VectModelType.begin();
		iter != VectModelType.end(); ++iter)
	{
		int type = *iter;
		IDBModel *model = NULL;
		switch (type)
		{
		case MODELTYPE_USER:
			model = new CUserModel;
			break;
		case MODELTYPE_BAG:
			model = new CBagModel;
			break;
		case MODELTYPE_EQUIP:
			model = new CEquipModel;
			break;
		case MODELTYPE_HERO:
			model = new CHeroModel;
			break;
		case MODELTYPE_SUMMONER:
			model = new CSummonModel;
			break;
		case MODELTYPE_STAGE:
			model = new CStageModel;
			break;
		case MODELTYPE_TEAM:
			model = new CTeamModel;
			break;
		case MODELTYPE_TASK:
			model = new CTaskModel;
			break;
		case MODELTYPE_ACHIEVEMENT:
			model = new CAchievementModel;
			break;
		case MODELTYPE_GUIDE:
			model = new CGuideModel;
			break;
		case MODELTYPE_MAIL:
			model = new CMailModel;
			break;
		case MODELTYPE_INSTANCE:
			model = new CInstanceModel;
			break;
		case MODELTYPE_HEROTEST:
			model = new CHeroTestModel;
			break;
		case MODELTYPE_GOLDTEST:
			model = new CGoldTestModel;
			break;
		case MODELTYPE_TOWERTEST:
			model = new CTowerTestModel;
			break;
		case MODELTYPE_PVP:
			model = new CPvpModel;
			break;
		case MODELTYPE_SHOP:
			model = new CShopModel;
			break;
		case MODELTYPE_ACTIVE:
			model = new CUserActiveModel;
			break;
		case MODELTYPE_HEAD:
			model = new CHeadModel;
			break;
		case MODELTYPE_USERUNION:
			model = new CUserUnionModel;
			break;
		case MODELTYPE_MERCENARY:
			model = new CPersonMercenaryModel;
			break;
        case MODELTYPE_PVPCHEST:
			model = new CPvpChestModel;
			break;
		case MODELTYPE_BDACTIVE:
			model = new CBlueDiamondModel;
		default:
			break;
		}

		if (!model->init(userId))
        {
            KXLOGERROR("uid=%d init model fail ! modelType=%d", userId, type);
            if (MODELTYPE_USER == type)
            {
                delete model;
                return false;
            }
            else
            {
                delete model;
                continue;
            }
		}
		m_mapModels[type] = model;
	}

	m_nUid = userId;
	return true;
}

void CGameUser::refreshModels()
{
	for (std::map<int, IDBModel*>::iterator iter = m_mapModels.begin();
		iter != m_mapModels.end(); ++iter)
	{
		iter->second->Refresh();
	}
}

void CGameUser::refreshModel(int modelType)
{
	if (m_mapModels.find(modelType) != m_mapModels.end())
	{
		m_mapModels[modelType]->Refresh();
	}
}

IDBModel* CGameUser::getModel(int modelType)
{
	if (m_mapModels.find(modelType) != m_mapModels.end())
	{
		return m_mapModels[modelType];
	}
	return NULL;
}

void CGameUser::setModel(int modelType, IDBModel *model)
{
	if (m_mapModels.find(modelType) != m_mapModels.end())
	{
		delete m_mapModels[modelType];
	}

	m_mapModels[modelType] = model;
}
