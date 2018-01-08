#ifndef __BATTLEPROTOCOL_H__
#define __BATTLEPROTOCOL_H__

#include "CommStructs.h"

#pragma pack(1)

enum BattleProtocol
{
    CMD_BAT_CSBEGIN,            // CS消息命令起始
    CMD_BAT_PVPCOMMANDCS,       // 发起战斗指令请求
    CMD_BAT_PVPUPDATECS,        // 请求更新数据
    CMD_BAT_CSEND,              // CS消息命令结束

    CMD_BAT_SCBEGIN = 100,      // SC消息命令起始
    CMD_BAT_PVPCOMMANDSC,       // 回应战斗指令结果
    CMD_BAT_PVPENDSC,           // 通知客户端PVP游戏结束
    CMD_BAT_PVPUPDATESC,        // 返回更新数据
    CMD_BAT_SCEND,              // SC消息命令结束
};

//CMD_BAT_PVPCOMMANDCS
//CMD_BAT_PVPCOMMANDSC
//这两条命令转发 BattleCommandInfo 结构

//CMD_BAT_PVPENDSC
struct BattlePvpEndSC
{
    int winner;                 //胜利者
    int loser;                  //失败者
    int endType;                //结束的类型 是否需要结算数据
};

#pragma pack()

#endif 
