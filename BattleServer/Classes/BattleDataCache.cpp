#include "BattleDataCache.h"
#include "KXServer.h"
#include "BattleServer.h"

CBattleDataCache* CBattleDataCache::m_Instance = NULL;

using namespace std;
using namespace KxServer;

CBattleDataCache* CBattleDataCache::getInstance()
{
    if (m_Instance == NULL)
    {
        m_Instance = new CBattleDataCache();
    }
    return m_Instance;
}

void CBattleDataCache::destory()
{
    if (m_Instance != NULL)
    {
        delete m_Instance;
        m_Instance = NULL;
    }
}

CBattleDataCache::CBattleDataCache()
{

}

CBattleDataCache::~CBattleDataCache()
{
    std::map<int, PvpCacheData>::iterator iter = m_DataMap.begin();
    for (; iter != m_DataMap.end();)
    {
        delete (iter->second.BufferData);
        m_DataMap.erase(iter++);
    }
}

void CBattleDataCache::pushBackBattleData(int uid, CBufferData* data)
{
    PvpCacheData cacheData;
    cacheData.Tick = CBattleServer::getInstance()->getTimerManager()->getTimestamp();
    cacheData.BufferData = data;

    m_DataMap[uid] = cacheData;
    if (m_DataMap.size() > 256)
    {
        autoReleaseCache();
    }
}

void CBattleDataCache::autoReleaseCache()
{
    int nowTick = CBattleServer::getInstance()->getTimerManager()->getTimestamp();
    std::map<int, PvpCacheData>::iterator iter = m_DataMap.begin();
    for (;  iter != m_DataMap.end();)
    {
        // 2 小时前的可以剔除掉
        if (iter->second.Tick < nowTick - 7200)
        {
            delete (iter->second.BufferData);
            m_DataMap.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}
