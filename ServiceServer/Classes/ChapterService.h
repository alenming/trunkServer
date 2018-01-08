#ifndef __CHAPTER_SERVICE_H__
#define __CHAPTER_SERVICE_H__

#include "KxCommInterfaces.h"

class CChapterService
{
public:
	// 挑战业务处理
	static void processService(int maincmd, int subcmd, int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 关卡挑战
	static void processChallenge(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 关卡挑战结束
	static void processFinish(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 扫荡业务
	static void processSweep(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 攻略业务
	static void processStrategy(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 章节奖励
	static void processChapterReward(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 购买挑战次数
	static void processBuyTimes(int uid, char *buffer, int len, KxServer::IKxComm *commun);
	// 购买章节
	static void processBuyChapter(int uid, char *buffer, int len, KxServer::IKxComm *commun);
};

#endif //__CHAPTER_SERVICE_H__
