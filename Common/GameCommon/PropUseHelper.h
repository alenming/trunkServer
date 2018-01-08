#ifndef __PROP_USE_HELPER_H__
#define __PROP_USE_HELPER_H__

#include <vector>
#include <map>
#include "ItemDrop.h"
#include "EquipComm.h"
#include "BufferData.h"

struct AwardInfo;
class IDBModel;
class CPropUseHelper
{
private:
	CPropUseHelper();
	~CPropUseHelper();

public:

	static CPropUseHelper *getInstance();
	static void destroy();

public:
	// 添加多个掉落物品并决定发送还是发邮件
	bool AddItems(int uid, std::vector<DropItemInfo> &dropItem, bool bSendEmail = true,int nOssActionID = 0);
	// 添加单个掉落
	bool AddItems(int uid, DropItemInfo Info, bool bSendEmail = true, int nOssActionID = 0);
	// 背包使用道具
	bool bagUseProp(int uid, int propId, int count, std::vector<DropItemInfo> &newItems);
    // 添加资源(金币、公会币、经验...)辅助接口
    void AddResource(int resourceID, int val, std::vector<DropItemInfo> &dropItem);

    void AddItems(const std::vector<AwardInfo> &items, std::vector<DropItemInfo> &dropItem);

private:

	//实际上将物品添加到包中
	bool RealAddItem(int uid,std::vector<DropItemInfo> &dropItem);
	//处理添加物品数据拼装
    bool ProcessMapItemAdd(int uid, std::vector<DropItemInfo> &dropItem, CBufferData* bufferData);
	//发送物品数据
	bool ProcessMapItemSend(int uid, std::vector<SItemBase*> &sendItems);
	
private:

	static CPropUseHelper *m_pInstance;
};

#endif //__PROP_USE_HELPER_H__
