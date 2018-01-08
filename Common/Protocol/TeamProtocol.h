#ifndef __TEAM_PROTOCOL_H__
#define __TEAM_PROTOCOL_H__

#pragma pack(1)

enum TEAMPROTOCOL
{
	CMD_TEAM_CSBEGIN,
	CMD_TEAM_SETTEAM_CS, //…Ë÷√∂”ŒÈ
	CMD_TEAM_CSEND,

    CMD_TEAM_SCBEGIN = 100,
	CMD_TEAM_SETTEAM_SC, //…Ë÷√∂”ŒÈ”¶¥
	CMD_TEAM_SCEND,
};

// CMD_TEAM_SETTEAM_CS
struct TeamSetCS
{
	int teamType;       // ∂”ŒÈ¿‡–Õ
	int summonerID;     // ’ŸªΩ ¶ID
	int heroID1;        // ”¢–€ID
	int heroID2;        // ”¢–€ID
	int heroID3;        // ”¢–€ID
	int heroID4;        // ”¢–€ID
	int heroID5;        // ”¢–€ID
	int heroID6;        // ”¢–€ID
	int heroID7;        // ”¢–€ID
};

#pragma pack()

#endif //__TEAM_PROTOCOL_H__
