#ifndef _REBOT_COMM_H__
#define _REBOT_COMM_H__

//机器人操作
enum emRebotOpParamType
{
	emRebotParmNone,
	emRebotParamInt,					//整形
	emRebotParamStr,					//字符串
};

enum emRebotSpecialParam
{
	emRebotNoneSParam,
	emRebotUidSParam,					//uid
	emRebotBattleSParam,				//BattleID
	emRebotRoomSParam,					//roomID
};

//机器人配置
enum emRebotConfig
{
	emRebotNone,
	emRebotOpInterval,			//机器人操作间隔
	emRebotOpIp,				//机器人连接IP
	emRebotOpPort,				//机器人连接端口
	emRebotMinUid,				//机器人最小UID
	emRebotMaxUid,				//机器人最大UID
};


#endif //_REBOT_COMM_H__
