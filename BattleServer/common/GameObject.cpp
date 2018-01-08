#include "GameObject.h"
#include "BattleHelper.h"
#include "ComponentCache.h"

using namespace std;

static int objCount = 0;

CGameObject::CGameObject()
:m_bSafeRemove(false)
{
    //CCLOG("new GameObject count %d id %d", ++objCount, _ID);
}

CGameObject::~CGameObject()
{
    clearLogicComponents();
    //CCLOG("delete GameObject count %d id %d", --objCount, _ID);
}

bool CGameObject::init(int typeId, int ownerId, int objId, CBattleHelper* battle)
{
    CHECK_RETURN(battle);

    m_bRealDead = false;
    m_bSafeRemove = false;
    m_nTypeId = typeId;
    m_nOwnerId = ownerId;
    m_nGameObjectId = objId;
    m_pBattle = battle;
    m_pEventManager = battle->pEventManager;

    // Master在左边，非Master在右
    m_nDirection = ownerId == battle->getMasterId() ? 1 : -1;
    //LOG("================= Master UID %d OwnerID %d Dir %d", battle->getMasterId(), ownerId, m_nDirection);
#ifndef RunningInServer
    scheduleUpdate();
#endif // !RunningInServer

    return Node::init();
}

void CGameObject::onExit()
{
    Node::onExit();

    clearLogicComponents();
    removeAllChildrenWithCleanup(true);
#ifndef RunningInServer
    removeAllComponents();
#endif 
}

void CGameObject::reset()
{
    m_Position = Vec2::ZERO;
    m_IntAttributes.clear();
    m_FloatAttributes.clear();
    clearLogicComponents();
}

void CGameObject::remove()
{
    removeFromParentAndCleanup(true);
}

// 将游戏场景打包到data中
bool CGameObject::serialize(CBufferData& data)
{
	SERIALIZE_MATCH(data);

	data.writeData(m_nDirection);
	data.writeData(m_bSafeRemove);
	data.writeData(m_Position.x);
    data.writeData(m_Position.y);
    data.writeData(m_bRealDead);

	//#ifndef RunningInServer
	//	data.writeData(getLocalZOrder());
	//#endif 
	
	SERIALIZE_MATCH(data);

    return true;
}

// 从data中解压出游戏场景
bool CGameObject::unserialize(CBufferData& data)
{
	UN_SERIALIZE_MATCH(data);

	data.readData(m_nDirection);
	data.readData(m_bSafeRemove);
    float x = 0.0f, y = 0.0f;
	data.readData(x);
    data.readData(y);
    m_Position = Vec2(x, y);
    data.readData(m_bRealDead);

	//#ifndef RunningInServer
	//	int nTemp = 0;
	//	data.readData(nTemp);
	//    setLocalZOrder(nTemp);
	//#endif

	setPosition(m_Position);

#ifndef RunningInServer
	// Flipx
	//setScaleX(m_nDirection != 1 ? -1 : 1);
#endif

	UN_SERIALIZE_MATCH(data);

	return true;
}

void CGameObject::addLogicComponent(CLogicComponent* com)
{
    com->retain();
    com->setOwner(this);
    com->onEnter();
    m_LogicComponents.push_back(com);
}

void CGameObject::clearLogicComponents()
{
    for (list<CLogicComponent*>::iterator iter = m_LogicComponents.begin();
        iter != m_LogicComponents.end(); ++iter)
    {
        (*iter)->onExit();
        (*iter)->release();
        CComponentCache::getInstance()->freeComponent(*iter);
    }

    m_LogicComponents.clear();
}

void CGameObject::updateLogic(float dt)
{
    for (list<CLogicComponent*>::iterator iter = m_LogicComponents.begin();
        iter != m_LogicComponents.end(); ++iter)
    {
        (*iter)->logicUpdate(dt);
    }
}
