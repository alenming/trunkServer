#include "PropUseHelper.h"
#include "GameDef.h"
#include "ModelHelper.h"
#include "ConfHall.h"
#include "KxCommManager.h"
#include "Protocol.h"
#include "BagProtocol.h"
#include "CommonHelper.h"
#include "RandGenerator.h"
#include "MailHelper.h"
#include "ErrorCodeProtocol.h"
#include "ConfGameSetting.h"
#include "UserActionListener.h"
#include "BufferData.h"
#include "CommOssHelper.h"

using namespace KxServer;
using namespace std;

CPropUseHelper::CPropUseHelper()
{
}

CPropUseHelper::~CPropUseHelper()
{
}

CPropUseHelper *CPropUseHelper::m_pInstance = NULL;
CPropUseHelper *CPropUseHelper::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CPropUseHelper;
	}
	return m_pInstance;
}

void CPropUseHelper::destroy()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CPropUseHelper::AddItems(int uid, std::vector<DropItemInfo> &dropItem, bool bSendEmail, int nOssActionID)
{
    if (dropItem.size() == 0)
    {
        KXLOGINFO("notice AddItems to %d but dropItem is Empty", uid);
        return true;
    }

    // 先整理好掉落列表，去除暴击和重复

    // 判断背包不足
    if (!CModelHelper::checkBagCapacity(uid, dropItem))
    {
        if (bSendEmail)
        {
            // bSendEmail为true则发送邮件结束
            return CMailHelper::saveAndSendBagFullMail(uid, dropItem);
        }
        else
        {
            ErrorCodeData CodeData;
            CodeData.nCode = ERROR_BAG_ISNOTENOUGH;
            CKxCommManager::getInstance()->sendData(uid, CMD_ERRORCODE, ERRORCODE_PROTOCOL,
                (char*)&CodeData, sizeof(CodeData));
            return false;
        }
    }

	if (!RealAddItem(uid, dropItem))
	{
		return false;
	}

	for (int i = 0; i < dropItem.size(); i++)
	{
		CCommOssHelper::userGoodsGainOss(uid, nOssActionID,dropItem[i].id,dropItem[i].num);
	}
	
    return true;
}

bool CPropUseHelper::AddItems(int uid, DropItemInfo Info, bool bSendEmail, int nOssActionID)
{
	std::vector<DropItemInfo> dropItem;
	dropItem.push_back(Info);
	return AddItems(uid, dropItem, bSendEmail,nOssActionID);
}

bool CPropUseHelper::bagUseProp(int uid, int propId, int count, std::vector<DropItemInfo> &newItems)
{
	const PropItem *pPropConf = queryConfProp(propId);
	if (NULL == pPropConf)
	{
		return false;
	}

	for (int i = 0; i < count; ++i)
	{
		switch (pPropConf->Type)
		{
		case PROPTYPE_GOLDBAG:
			//配置为随机个数
			if (pPropConf->TypeParam.size() >= 2)
			{
				int value = g_RandGenerator.MakeRandNum(pPropConf->TypeParam[0], pPropConf->TypeParam[1]);
				DropItemInfo info;
				info.id = RESOURCEID_GOLD; //金币
				info.num = value;
				newItems.push_back(info);
			}
			break;

		case PROPTYPE_ENERGYBAG:
			//配置为随机个数
			if (pPropConf->TypeParam.size() >= 2)
			{
				int value = g_RandGenerator.MakeRandNum(pPropConf->TypeParam[0], pPropConf->TypeParam[1]);
				DropItemInfo info;
				info.id = RESOURCEID_ENERGY; //体力
				info.num = value;
				newItems.push_back(info);
			}
			break;

		case PROPTYPE_EXPBAG:
			//配置为随机个数
			if (pPropConf->TypeParam.size() >= 2)
			{
				int value = g_RandGenerator.MakeRandNum(pPropConf->TypeParam[0], pPropConf->TypeParam[1]);
				DropItemInfo info;
				info.id = RESOURCEID_EXP; //经验
				info.num = value;
				newItems.push_back(info);
			}
			break;

		case PROPTYPE_DIAMONDBAG:
			//配置为随机个数
			if (pPropConf->TypeParam.size() >= 2)
			{
				int value = g_RandGenerator.MakeRandNum(pPropConf->TypeParam[0], pPropConf->TypeParam[1]);
				DropItemInfo info;
				info.id = RESOURCEID_DIAMOND; //钻石
				info.num = value;
				newItems.push_back(info);
			}
			break;

		case PROPTYPE_TREASURE:
			//配置参数为掉落id
			if (pPropConf->TypeParam.size() >= 1)
			{
				int value = pPropConf->TypeParam[0];
				CItemDrop::Drop(value,newItems);
			}
			break;

		default:
			return false;
			break;
		}
	}

	if (!AddItems(uid, newItems))
	{
		return false;
	}

	for (int i = 0; i < newItems.size(); i++)
	{
		if (newItems[i].id == RESOURCEID_DIAMOND)
		{
			CCommOssHelper::userDiamondGetOss(uid, newItems[i].num, ITEMUSE_DIAMOND_GET_REASON);
		}

	}
	return true;
}

void CPropUseHelper::AddResource(int resourceID, int val, std::vector<DropItemInfo> &dropItem)
{
    if (val <= 0)
    {
        return;
    }

    DropItemInfo info;
    info.id = resourceID;
    info.num = val;

    dropItem.push_back(info);
}

void CPropUseHelper::AddItems(const std::vector<AwardInfo> &items, std::vector<DropItemInfo> &dropItem)
{
    if (items.empty())
    {
        return;
    }

    DropItemInfo itemInfo;
    for (unsigned int i = 0; i < items.size(); ++i)
    {
        itemInfo.id = items[i].ID;
        itemInfo.num = items[i].Num;

        dropItem.push_back(itemInfo);
    }
}

//实际上将物品添加到包中
bool CPropUseHelper::RealAddItem(int uid, std::vector<DropItemInfo> &dropItem)
{
    CBufferData bufferData;
    if (!bufferData.init(512))
    {
        KXLOGERROR("RealAddItem buffer data init faile");
        return false;
    }
    if (!ProcessMapItemAdd(uid, dropItem, &bufferData))
	{
        KXLOGERROR("RealAddItem ProcessMapItemAdd faile");
		return false;
	}

    if (!CKxCommManager::getInstance()->sendData(uid,
        CMD_BAG, CMD_BAD_ITEM_ADD_SC, bufferData.getBuffer(), bufferData.getDataLength()))
	{
        KXLOGERROR("RealAddItem SendData To %d data length %d faile", uid, bufferData.getDataLength());
		return false;
	}

	return true;
}

bool CPropUseHelper::ProcessMapItemAdd(int uid, std::vector<DropItemInfo> &dropItem, CBufferData* bufferData)
{
    // 占位
    int sendCount = 0;
    bufferData->writeData(sendCount);
	for (std::vector<DropItemInfo>::iterator iter = dropItem.begin(); 
		iter != dropItem.end(); ++iter)
	{
		DropItemInfo &item = *iter;
        const PropItem *pPropConf = queryConfProp(item.id);
        if (NULL == pPropConf || item.num == 0)
		{
            KXLOGDEBUG("item %d is null or num %d is 0", item.id, item.num);
			continue;
		}

		switch (pPropConf->Type)
		{
			case PROPTYPE_EQUIP:
			case PROPTYPE_EQUIP_CREATE:
				if (pPropConf->TypeParam.size() == 0)
				{
					KXLOGDEBUG("%s pPropConf->TypeParam.size() == 0 PROPTYPE_EQUIP", __FUNCTION__);
                    continue;
				}
                for (int i = 0; i < item.num; ++i)
				{
					//获取装备生成ID
					int createEquipID = pPropConf->TypeParam[0];
                    SEquipInfo eqiupInfo;
					eqiupInfo.nType = pPropConf->Type;
					if (!CModelHelper::addEquip(uid, createEquipID, eqiupInfo))
					{
                        KXLOGERROR("add Equip faile create id %d", createEquipID);
						continue;
					}
                    ++sendCount;
                    bufferData->writeData(&eqiupInfo, sizeof(eqiupInfo));
				}
			    break;
			case PROPTYPE_HEROCARD:
				if (pPropConf->TypeParam.size() < 2)
				{
					KXLOGDEBUG("%s pPropConf->TypeParam.size() == 0 PROPTYPE_HEROCARD", __FUNCTION__);
                    continue;
				}
                for (int i = 0; i < item.num; ++i)
				{
					if (!CModelHelper::addHero(uid, pPropConf->TypeParam[0], pPropConf->TypeParam[1], 1))
					{
                        KXLOGERROR("user %d add Hero %d faile", uid, pPropConf->TypeParam[0]);
                        continue;
					}
                    SHeroInfo heroInfo;
                    heroInfo.nItemID = item.id;
                    heroInfo.nCardID = pPropConf->TypeParam[0];
                    heroInfo.nStart = pPropConf->TypeParam[1];
                    heroInfo.nType = pPropConf->Type;
                    bufferData->writeData(&heroInfo, sizeof(heroInfo)); 
                    ++sendCount;
                }
			    break;
            case PROPTYPE_SUMMONERCARD:
                {
                    if (pPropConf->TypeParam.size() < 1)
                    {
                        KXLOGDEBUG("%s pPropConf->TypeParam.size() == 0 PROPTYPE_SUMMONERCARD", __FUNCTION__);
                        continue;
                    }
				    if (!CModelHelper::addSummoner(uid, pPropConf->TypeParam[0]))
				    {
                        KXLOGERROR("user %d add Summoner %d faile", uid, pPropConf->TypeParam[0]);
                        continue;
				    }

                    SItemBase itemBase;
                    itemBase.nItemID = pPropConf->TypeParam[0];
                    itemBase.nType = pPropConf->Type;
                    bufferData->writeData(&itemBase, sizeof(itemBase));
                    ++sendCount;

                    const SaleSummonerConfItem *pSaleSumConf = queryConfSaleSummoner(pPropConf->TypeParam[0]);
                    CHECK_CONTINUE(NULL != pSaleSumConf);
                    CHECK_CONTINUE(queryConfHasHeadIcon(pSaleSumConf->HeadID));

                    //同时添加头像
                    CModelHelper::AddHeadID(uid, pSaleSumConf->HeadID);
                    CModelHelper::DispatchActionEvent(uid, ELA_OWN_SUMMONER_COUNT, NULL, 0);
                }
			    break;
			case PROPTYPE_EQUIPMATERIAL:
			case PROPTYPE_EXPBOOK:
			case PROPTYPE_SKILLBOOK:
			case PROPTYPE_GOLDBAG:
			case PROPTYPE_ENERGYBAG:
			case PROPTYPE_EXPBAG:
			case PROPTYPE_DIAMONDBAG:
			case PROPTYPE_TREASURE:
            case PROPTYPE_MATERIAL:
                {
					//普通物品
					if (!CModelHelper::addItem(uid, item.id, item.num))
					{
                        KXLOGERROR("user %d add item %d faile", uid, item.id);
                        continue;
                    }
                    SItemInfo itemBase;
                    itemBase.nItemID = item.id;
                    itemBase.nType = pPropConf->Type;
                    itemBase.nCount = item.num;
                    bufferData->writeData(&itemBase, sizeof(itemBase));
                    ++sendCount;
                }
                break;
			case PROPTYPE_RESOURCE:
				{
					bool bAdd = false;
                    switch (item.id)
					{
					case RESOURCEID_GOLD:					//金币
						bAdd = CModelHelper::addGold(uid, item.num);
						break;
					//case RESOURCEID_ENERGY:					//体力
					//	bAdd = CModelHelper::addEnergy(uid, item.num);
					//	break;
					case RESOURCEID_EXP:					//经验
						bAdd = CModelHelper::addExp(uid, item.num);
						break;
					case RESOURCEID_DIAMOND:				//钻石
						bAdd = CModelHelper::addDiamond(uid, item.num);
						break;
					case RESOURCEID_TOWER:					//爬塔积分
						bAdd = CModelHelper::addTowerCoin(uid, item.num);
						break;
					case RESOURCEID_UNION:					//公会积分
						bAdd = CModelHelper::addGuildContrib(uid, item.num);
						break;
					case RESOURCEID_PVPCOIN:				//竞技币
						bAdd = CModelHelper::addPvpCoin(uid, item.num);
						break;
					case RESOURCEID_FLASHCARD:				//卡券
						bAdd = CModelHelper::addFlashCard(uid, item.num);
						break;
					case RESOURCEID_FLASHCARD10:				//10连卡券
						bAdd = CModelHelper::addFlashCard10(uid, item.num);
						break;

					default:
						continue;
						break;
					}

					if (bAdd)
					{
						SItemInfo itemBase;
						itemBase.nItemID = item.id;
						itemBase.nType = pPropConf->Type;
						itemBase.nCount = item.num;
                        bufferData->writeData(&itemBase, sizeof(itemBase));
                        ++sendCount;
					}
                    else
                    {
                        KXLOGERROR("user %d add item resource %d faile", uid, item.id);
                    }
				}
				break;
			case PROPTYPE_HEROFRAGMENT:
				{
					if (pPropConf->TypeParam.size() == 0)
					{
						KXLOGDEBUG("%s pPropConf->TypeParam.size() == 0 PROPTYPE_HEAD", __FUNCTION__);
						continue;
					}

					if (!CModelHelper::addHeroFragment(uid, pPropConf->TypeParam[0], item.num))
					{
                        KXLOGERROR("user %d add HeroFragment %d faile", uid, pPropConf->TypeParam[0]);
                        continue;
					}

                    SItemFrag itemBase;
                    itemBase.nItemID = item.id;
                    itemBase.nType = pPropConf->Type;
                    itemBase.nHeroId = pPropConf->TypeParam[0];
                    itemBase.nCount = item.num;
                    bufferData->writeData(&itemBase, sizeof(itemBase));
                    ++sendCount;
				}
				break;
			case PROPTYPE_HEAD:
				{
					if (pPropConf->TypeParam.size() == 0)
					{
						KXLOGDEBUG("%s pPropConf->TypeParam.size() == 0 PROPTYPE_HEAD", __FUNCTION__);
						continue;
					}

					if (!CModelHelper::AddHeadID(uid, pPropConf->TypeParam[0]))
					{
                        KXLOGERROR("user %d add Head %d faile", uid, pPropConf->TypeParam[0]);
                        continue;
                    }

                    SItemInfo itemBase;
                    itemBase.nItemID = item.id;
                    itemBase.nType = pPropConf->Type;
                    itemBase.nCount = item.num;
                    bufferData->writeData(&itemBase, sizeof(itemBase));
                    ++sendCount;
				}
			break;
		default:
			break;
		}
	}
    // 修改数量字段
    *(reinterpret_cast<int*>(bufferData->getBuffer())) = sendCount;
	return true;
}
