#include "LogicComponent.h"
#include "KxLog.h"

//static int refCount = 0;

CLogicComponent::CLogicComponent()
: m_pOwner(NULL)
, m_eComponentType(ELogicComponent)
{
    //++refCount;
    //KXLOGDEBUG("RefCount %d", refCount);
}

CLogicComponent::~CLogicComponent()
{
    //--refCount;
    //KXLOGDEBUG("RefCount %d", refCount);
}

void CLogicComponent::onExit()
{
    m_pOwner = NULL;
}

void CLogicComponent::logicUpdate(float delta)
{

}

bool CLogicComponent::serialize(CBufferData& data)
{
	return true;
}

bool CLogicComponent::unserialize(CBufferData& data)
{
	return true;
}
