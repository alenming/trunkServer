/*
    战斗数据转化为buffer的静态辅助方法
*/
#ifndef __BATTLE_DATA_HELPER__
#define __BATTLE_DATA_HELPER__

#include "HeroModel.h"
#include "EquipModel.h"
#include "BufferData.h"
#include "UnionMercenaryModel.h"

// 挑战房间信息，用于生成
struct BattleRoomData
{
    int battleType;
    int stageId;
    int stageLv;
    int ext1;
    int ext2;
    int summonerId;
    int mecenaryId;
    std::vector<int> heroIds;
    std::vector<int> outerBuffs;
    std::vector<BuffData> buffs;

    BattleRoomData()
        : battleType(0)
        , stageId(0)
        , stageLv(0)
        , ext1(0)
        , ext2(0)
        , summonerId(0)
        , mecenaryId(0)
    {
    }
};

class CBattleDataHelper
{
public:
    // 填充佣兵信息
    static bool mercenaryToBuffer(CUserModel *pUserModel, UnionMercenaryInfo* info, CBufferData& bufferData);

    // 填充玩家信息
    static bool playerToBuffer(int uid, const BattleRoomData& roomData, CUserModel *pUserModel, CBufferData& bufferData);

    // 将一个士兵结构填充到输出的buffer中
    // 传入英雄id，英雄模型，装备模型，输出缓冲区 + 长度
    static bool soldierToBuffer(int heroId, CEquipModel* equipModel, CHeroModel* heroModel, CBufferData& bufferData);

    // 传入房间信息，返回
    static bool roomDataToBuffer(int uid, const BattleRoomData& roomData, CBufferData& bufferData);

    // 传入用户id、召唤师id、士兵id、佣兵id，自动验证并封装进入房间包
    // static bool fillEnterRoomBuffer(int uid, int summonerId, int* heroIds, int heroCount, int mecenaryId, CBufferData& bufferData);
};

#endif //__ROOM_H__
