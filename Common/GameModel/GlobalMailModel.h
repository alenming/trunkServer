#ifndef __GLOBAL_MAIL_MODEL_H__
#define __GLOBAL_MAIL_MODEL_H__

#include <map>
#include <vector>
#include <string>

#include "IDBModel.h"
#include "CommStructs.h"

struct DropItemInfo;
class CRedisStorer;
class CGlobalMailModel : public IDBModel
{
private:
	CGlobalMailModel();
	~CGlobalMailModel();

public:

	static CGlobalMailModel *getInstance();
	static void destroy();
	
public:

	bool init();

	bool Refresh();
	// 移除全局邮件
	bool RemoveGlobalMail(int globalMailId);
	// 获得对应的指定全局邮件基本信息
	bool GetGlobalMailBase(int globalMailId, BaseMailInfo& mailBase);
	// 获得指定全局邮件文字内容
	bool GetGlobalMailContext(int globalMailId, std::string& mailCtx);
	// 获得指定全局邮件的道具内容
	bool GetGlobalMailItems(int globalMailId, std::vector<DropItemInfo> &items);

	// 获得所有全局邮件
	std::map<int, BaseMailInfo>& GetGlobalMailBase() { return m_GlobalMailBase; }
	// 获得所有全局邮件文件内容
	std::map<int, std::string>& GetGlobalMailContexts() { return m_GlobalMailContexts; }
	// 获得所有全局邮件道具内容
	std::map<int, std::vector<DropItemInfo> >& GetGlobalMailItems() { return m_GlobalMailItems; }
	bool getLoadingState() { return m_IsLoadIng; }
	void setLoadingState(bool bState) { m_IsLoadIng = bState;}

private:

	static CGlobalMailModel *					m_pInstance;			
	CRedisStorer *								m_pRedisStorer;			// 数据库节点
	std::string									m_szGlobalMailKey;		// 全局邮件key
	std::string									m_szGlobalMailCtxKey;	// 全局邮件内容key
	std::map<int, BaseMailInfo>					m_GlobalMailBase;		// <全局邮件id, 邮件基础信息>
	std::map<int, std::string>					m_GlobalMailContexts;	// <全局邮件id, 内容>
	std::map<int, std::vector<DropItemInfo> >	m_GlobalMailItems;		// <全局邮件id, 邮件道具信息>
	bool										m_IsLoadIng;			// 正在重新加载
};

#endif //__GOLBAL_MAIL_MODEL_H__
