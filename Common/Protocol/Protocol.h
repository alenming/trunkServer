/*
* 通讯协议 主要定义主命令和包头
*
*/

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

enum NetMainCMD
{
    CMD_LOGIN = 1,      // 登录
    CMD_USER,		    // 用户数据
    CMD_SUMMONER,	    // 召唤师
    CMD_HERO,		    // 英雄
    CMD_BAG,		    // 背包
    CMD_STAGE,		    // pve场景
    CMD_PVP,		    // pvp匹配
    CMD_BATTLE,		    // pvp战斗
    CMD_TEAM,		    // 队伍
    CMD_TASK,           // 任务
    CMD_ACHIEVEMENT,    // 成就
    CMD_GUIDE,		    // 引导
    CMD_UNION,          // 公会
    CMD_MAIL,           // 邮件
    CMD_INSTANCE,	    // 活动副本
    CMD_GOLDTEST,	    // 金币试炼
    CMD_HEROTEST,	    // 英雄试炼	
    CMD_TOWERTEST,	    // 爬楼试炼
    CMD_UNIONTASK,      // 公会任务
    CMD_MATCH,		    // 匹配协议
	CMD_PVPCHEST,	    // pvp宝箱
    CMD_SHOP,           // 商店
    CMD_RANK,		    // 排行榜
    CMD_ACTIVE,		    // 活动
    CMD_ERRORCODE,	    // 错误码
    CMD_PAY,            // 充值
	CMD_UNIONEXPIDITION,// 公会远征
    CMD_NOTICE,         // 通知
    CMD_CHAT,           // 聊天
    CMD_LOOK,           // 查看数据
	CMD_GM = 50,		// GM主命令
};

inline int MakeCommand(int main, int sub)
{
    return (main << 16) | (sub & 0x0000ffff);
}

struct Head
{
    int length;
    int cmd;
    int id;

    inline void MakeCommand(int main, int sub)
    {
        cmd = (main << 16) | (sub & 0x0000ffff);
    }

    inline int SubCommand()
    {
        return cmd & 0x0000ffff;
    }

    inline int MainCommand()
    {
        return cmd >> 16;
    }

    inline void* data()
    {
        return this + 1;
    }
};

#endif
