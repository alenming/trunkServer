#ifndef _GIFT_MODEL_H__
#define _GIFT_MODEL_H__

#include "IDBModel.h"
#include "Storage.h"
#include <string>

enum eGiftType			//在生成KEY的时候增加限制类型
{
	eNoneGiftType,				//无限制
	eSpecPlatfType,				//指定平台
	eSpecChannelType,			//指定渠道
	eSpecServerType,			//指定服务器
};

struct SGoodsData
{
	int nGoodsID;
	int nGoodsNum;
};

struct SGiftData
{
	std::vector<SGoodsData> vectGoods;
};

class CGiftModel : public IDBModel
{
public:
	static CGiftModel* getInstance();

	static void destroy();

	bool init(int nWorldID);

	bool Refresh();

	bool GetGiftData(int nUid, std::string strGift, int &nType, int &nParam, int &nGiftID, SGiftData &Data);

	bool DelGiftData(int nUid, std::string strGift);

	bool modifyUsrPs(int nUid, int nChannelID, const char *strPs,int PsLen);

private:
	CGiftModel();
	~CGiftModel();

	//检查该礼包是否使用过
	bool CheckCodeIsUseFull(int nUid, int nCode);

	//标记某个礼包码为使用状态
	bool SetCodeUseState(int nUid, int nCode);

private:
	Storage             *m_pStorage;    // 数据库

	static CGiftModel *							m_pInstance;
	int					m_nWorldID;		//游戏服务器ID
};

#endif //_GIFT_MODEL_H__

