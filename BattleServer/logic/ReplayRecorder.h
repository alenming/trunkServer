#ifndef __REPLAY_RECORDER_H__
#define __REPLAY_RECORDER_H__

#include <string>

#include "document.h"

class CReplayRecorder
{
public:
	CReplayRecorder();
	~CReplayRecorder();

public:

	void recordBaseInfo();

	void recordPlayerInfo();

	void recordBattleCommand();

	void parseRecord(std::string &jsonStr);

	std::string recordToStr();

private:

	rapidjson::Document m_JsonReader;
};

#endif //__REPLAY_RECORDER_H__
