#include "ItemDrop.h"
#include "ConfHall.h"
#include "ConfGameSetting.h"
#include "GameDef.h"
#include "CommonHelper.h"
#include "RandGenerator.h"
#include "KxLog.h"
#include "ConfUnion.h"

using namespace std;

void CItemDrop::Drop(int id, std::vector<DropItemInfo> &vecDropItem, bool isExtra)
{
    const DropPropItem *pDropItem = queryConfDropProp(id);
    if (NULL == pDropItem)
    {
        LOG("DropPropItem is null : %d", id);
        return;
    }
    DropItemInfo Info;
    //货币掉落
    for (std::vector<DropCurrencyData>::const_iterator iter = pDropItem->DropCurrencys.begin();
        iter != pDropItem->DropCurrencys.end(); ++iter)
    {
        Info.id = iter->CurrencyId;
        Info.num = g_RandGenerator.MakeRandNum(iter->LowerLimit, iter->UpperLimit);
        if (Info.num != 0)
        {
            vecDropItem.push_back(Info);
        }
    }

    //如果不是额外的才会进行额外掉落, 保证递归只会一次
    if (!isExtra)
    {
        for (std::vector<int>::const_iterator iter = pDropItem->ExtraDropRuleID.begin();
            iter != pDropItem->ExtraDropRuleID.end(); ++iter)
        {
            Drop(*iter, vecDropItem, true);
        }
    }

    int kinds = 1;
    int totalRate = 0;
    int randRate = 0;
    if (pDropItem->MeanwhileDropNum.size() >= 2)
    {
        kinds = g_RandGenerator.MakeRandNum(pDropItem->MeanwhileDropNum[0], pDropItem->MeanwhileDropNum[1]);
    }

    // 当要掉落的总类等于掉落总类，且不重复，直接掉落这些东西
    if (kinds == static_cast<int>(pDropItem->DropIDs.size()) && pDropItem->IsRepeat == 0)
    {
        for (int i = 0; i < static_cast<int>(pDropItem->DropIDs.size()); ++i)
        {
            DropItemInfo dropItem;
            dropItem.id = pDropItem->DropIDs[i].DropID;
            dropItem.num = g_RandGenerator.MakeRandNum(
                pDropItem->DropIDs[i].DropNum[0], pDropItem->DropIDs[i].DropNum[1]);
            vecDropItem.push_back(dropItem);
        }
    }
    else
    {
        // 计算总概率
        for (std::vector<DropIdData>::const_iterator iter = pDropItem->DropIDs.begin();
            iter != pDropItem->DropIDs.end(); ++iter)
        {
            const DropIdData &item = *iter;
            const PropItem *pPropConf = queryConfProp(item.DropID);
            if (NULL == pPropConf)
            {
                LOG("queryConfProp NULL : %d", item.DropID);
                return;
            }
            totalRate += static_cast<int>(item.DropRate * pPropConf->Ratio);
        }
        if (totalRate <= 0)
        {
            LOG("drop id %d totalRate is %d", id, totalRate);
            return;
        }

        // 根据重复和不重复进行掉落
        if (pDropItem->IsRepeat == 0)
        {
            // 如果不重复
            vector<DropIdData> dropDatas;
            dropDatas.assign(pDropItem->DropIDs.begin(), pDropItem->DropIDs.end());
            for (int i = 0; i < kinds; ++i)
            {
                int accumRate = 0;
                // 随机概率
                randRate = rand() % totalRate;
                // 判断随机到哪一个
                for (vector<DropIdData>::iterator iterDrop = dropDatas.begin();
                    iterDrop != dropDatas.end(); ++iterDrop)
                {
                    // 前面计算totalRate时已经做过了判断
                    int itemRate = static_cast<int>(iterDrop->DropRate * queryConfProp(iterDrop->DropID)->Ratio);
                    accumRate += itemRate;
                    if (accumRate >= randRate)
                    {
                        // 掉落了该物品
                        DropItemInfo dropItem;
                        dropItem.id = iterDrop->DropID;
                        dropItem.num = g_RandGenerator.MakeRandNum(
                            iterDrop->DropNum[0], iterDrop->DropNum[1]);
                        vecDropItem.push_back(dropItem);
                        // 修正概率，并将该物品从掉落列表中移除
                        totalRate -= itemRate;
                        dropDatas.erase(iterDrop);
                        break;
                    }
                }

                // 避免要随机的总数大于掉落池总数，或掉率错误等问题
                if (dropDatas.size() == 0 || totalRate <= 0)
                {
                    break;
                }
            }
        }
        else
        {
            // 如果允许重复
            for (int i = 0; i < kinds; ++i)
            {
                int accumRate = 0;
                // 随机概率
                randRate = rand() % totalRate;
                // 判断随机到哪一个
                for (vector<DropIdData>::const_iterator iterDrop = pDropItem->DropIDs.begin();
                    iterDrop != pDropItem->DropIDs.end(); ++iterDrop)
                {
                    // 前面计算totalRate时已经做过了判断
                    int itemRate = iterDrop->DropRate * queryConfProp(iterDrop->DropID)->Ratio;
                    accumRate += itemRate;
                    if (accumRate >= randRate)
                    {
                        // 掉落了该物品
                        DropItemInfo dropItem;
                        dropItem.id = iterDrop->DropID;
                        dropItem.num = g_RandGenerator.MakeRandNum(
                            iterDrop->DropNum[0], iterDrop->DropNum[1]);
                        vecDropItem.push_back(dropItem);
                        break;
                    }
                }
            }
        }
    }
}

void CItemDrop::UnionShopDrop(int id, std::map<int, int> &MapDropShop)
{
    const UnionShopDropData *pDropItem = queryConfUnionDropProp(id);
    CHECK_RETURN_DEBUG_VOID(pDropItem != NULL);
    int kinds = 1;
    int totalRate = 0;
    int randRate = 0;
    if (pDropItem->MeanwhileDropNum.size() >= 2)
    {
        kinds = g_RandGenerator.MakeRandNum(pDropItem->MeanwhileDropNum[0], pDropItem->MeanwhileDropNum[1]);
    }

    //总概率
    for (std::vector<UnionDropIdData>::const_iterator iter = pDropItem->DropIDs.begin();
        iter != pDropItem->DropIDs.end(); ++iter)
    {
        const UnionDropIdData &item = *iter;
        const ShopGoodsConfigData *pPropConf = queryConfShopData(item.DropID);
        if (NULL == pPropConf)
        {
            LOG("Execute False On File %s Line %d : %d", __FILE__, __LINE__, item.DropID);
            continue;
        }
        totalRate += item.DropRate;
    }

    //种类
    std::map<int, int> tempMap;
    std::map<int, int>::iterator ator;
    for (int i = 0; i < kinds; ++i)
    {
        //是否再随机一次(当配置不可重复而随机出重复物品时)
        bool bNext = false;
        do
        {
            bNext = false;
            int accumRate = 0;
            int critRate = 0;
            if (totalRate > 0)
                randRate = rand() % totalRate;

            for (std::vector<UnionDropIdData>::const_iterator iter = pDropItem->DropIDs.begin();
                iter != pDropItem->DropIDs.end(); ++iter)
            {
                const UnionDropIdData &item = *iter;
                const ShopGoodsConfigData *pPropConf = queryConfShopData(item.DropID);
                if (NULL == pPropConf)
                {
                    LOG("Execute False On File %s Line %d : %d", __FILE__, __LINE__, item.DropID);
                    continue;
                }

                accumRate += item.DropRate;
                if (randRate <= accumRate)
                {
                    ator = tempMap.find(item.DropID);
                    if (ator != tempMap.end())
                    {
                        // 重复再随机一次
                        bNext = true;
                        break;
                    }

                    int nNum = g_RandGenerator.MakeRandNum(item.DropNum[0], item.DropNum[1]);
                    tempMap[item.DropID] = nNum;
                    break;
                }
            }
        } while (bNext);
    }

    // 添加到容器最后
    MapDropShop.insert(tempMap.begin(), tempMap.end());
}

CardGambleItem * CItemDrop::RandHeroCard(int randType)
{
    const CardGambleSettingItem &cardGambleConf = queryConfCardGambleSetting();
    int totalRatio = 0;
    // 先处理稀有度列表
    std::map<int, int> rarityPool;
    for (std::map<int, Prob>::const_iterator iter = cardGambleConf.RareRatios.begin();
        iter != cardGambleConf.RareRatios.end(); ++iter)
    {
        if (randType == 0)
        {
            rarityPool[iter->first] = (int)(iter->second.Probability * iter->second.Ratio);
            totalRatio += rarityPool[iter->first];
        }
        else
        {
            if (iter->first >= RARITY_BLUE)
            {
                rarityPool[iter->first] = (int)(iter->second.Probability * iter->second.Ratio);
                totalRatio += rarityPool[iter->first];
            }
        }
    }
    //随机稀有度
    int rarity = 0;
    int tempRatio = 0;
    int randRatio = 0;
    bool again = false;
    //稀有度对应卡牌列表
    std::vector<CardGambleItem*> gambleCards;
    do
    {
        if (totalRatio <= 0)
        {
            KXLOGERROR("config is error!!! totalRatio %d", totalRatio);
            return NULL;
        }

        randRatio = rand() % totalRatio;
        for (std::map<int, int>::iterator iter = rarityPool.begin();
            iter != rarityPool.end(); ++iter)
        {
            tempRatio += iter->second;
            if (tempRatio > randRatio)
            {
                rarity = iter->first;
                break;
            }
        }

        again = !queryConfCardGamble(rarity, gambleCards);
        if (again)
        {
            LOG("card gamble haven't rarity=%d", rarity);
        }
    } while (again);

    // 计算列表总概率
    totalRatio = 0;
    for (std::vector<CardGambleItem*>::iterator iter = gambleCards.begin();
        iter != gambleCards.end(); ++iter)
    {
        totalRatio += (*iter)->Rate * (*iter)->Ratio;
    }
    // 随机一张卡片
    if (totalRatio <= 0)
    {
        KXLOGERROR("config is error!!! totalRatio %d", totalRatio);
        return NULL;
    }
    randRatio = rand() % totalRatio;
    tempRatio = 0;

    for (std::vector<CardGambleItem*>::iterator iter = gambleCards.begin();
        iter != gambleCards.end(); ++iter)
    {
        tempRatio += (*iter)->Rate * (*iter)->Ratio;
        if (tempRatio >= randRatio)
        {
            return (*iter);
        }
    }

    return NULL;
}
