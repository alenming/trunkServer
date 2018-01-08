/*
* 组件缓存池
*
* 1.负责战斗中组件的获取和释放
*/
#include "LogicComponent.h"

#include <map>
#include <list>

class CComponentCache
{
public:
    static CComponentCache* getInstance();
    static void destory();

    CLogicComponent* getComponent(EComponentType type);
    void freeComponent(CLogicComponent* pComponent);

private:
    CComponentCache();
    static CComponentCache* m_pInstance;

    std::map<EComponentType, std::list<CLogicComponent*> > m_ComponentCache;
};

template <typename T>
T* createComponent(EComponentType type)
{
    return dynamic_cast<T*>(CComponentCache::getInstance(
        )->getComponent(type));
}
