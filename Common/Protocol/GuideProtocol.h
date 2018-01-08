#ifndef __STORY_PROTOCOL_H__
#define __STORY_PROTOCOL_H__

#pragma pack(1)

enum GuideProtocol
{
	CMD_GUIDE_CSBEGIN,
	CMD_GUIDE_RECORD_CS,			//记录剧情点
	CMD_GUIDE_CSEND,

	CMD_GUIDE_SCBEGIN = 100,
	CMD_GUIDE_RECORD_SC,			//记录剧情点返回状况
	CMD_GUIDE_SCEND,
};

//CMD_GUIDE_RECORD_CS
struct GuideRecordCS
{
	int nEndNum;			//结束引导个数
	//引导ID 结束
};

//CMD_GUIDE_RECORD_SC
struct StoryRecordSC
{
	int nFlag;				//处理成功标识
};

#pragma pack()

#endif //__STORY_PROTOCOL_H__
