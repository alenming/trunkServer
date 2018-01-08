#ifndef _STAT_MODEL_H__
#define _STAT_MODEL_H__

#include <string>
#include "Storage.h"
#include "IDBModel.h"
#include "RedisStorer.h"

class CStatModel : public IDBModel
{
public:
	static CStatModel *getInstance();
	static void		  destroy();

	virtual bool init();

	bool MessageStart(int nUid, std::string szType);
	//Type标识是否为最后一条数据，0标识不是，1-标识是
	bool MessageDataAdd(std::string szDataKey, std::string szDataValue, int Type = 0);
	bool MessageEnd(std::string szTime);

private:
	CStatModel();
	~CStatModel();
	bool MessageTypeAdd(std::string szType);
private:
	std::string		m_MessageData;			//统计数据块
	int				m_ModNum;				//取模值
	static CStatModel*		m_pInstance;	//统计模型实例
	std::string		m_StatUidKey;			//统计玩家相关Key
	CRedisStorer *	m_pRedisStorer;
};


#endif //_STAT_MODEL_H__
