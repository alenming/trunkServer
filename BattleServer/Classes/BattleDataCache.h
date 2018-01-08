#ifndef __BATTLE_DATA_CACHE__
#define __BATTLE_DATA_CACHE__

#include <list>
#include <map>

#include "PvpProtocol.h"
#include "BufferData.h"

struct PvpCacheData
{
    int Tick;
    CBufferData* BufferData;
};

class CBattleDataCache
{
public:
    static CBattleDataCache* getInstance();
    static void destory();

    CBattleDataCache();
    virtual ~CBattleDataCache();

    void pushBackBattleData(int uid, CBufferData* data);
    void autoReleaseCache();

    inline PvpCacheData* getBattleData(int uid)
    {
        std::map<int, PvpCacheData>::iterator iter = m_DataMap.find(uid);
        if (iter != m_DataMap.end())
        {
            return &iter->second;
        }
        return NULL;
    }

private:
    std::map<int, PvpCacheData> m_DataMap;
    static CBattleDataCache* m_Instance;
};


#endif // !__BATTLE_DATA_CACHE__
