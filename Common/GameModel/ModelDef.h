#ifndef __MODEL_DEF_H__
#define __MODEL_DEF_H__

#include <string>
#include <sstream>

class ModelKey
{
public:
	static std::string UsrKey(int uid) 
	{
        return MakeKey(uid, "usr-");
	}
	static std::string BagKey(int uid)
	{
        return MakeKey(uid, "bag-");
	}
	static std::string BagInfoKey(int uid)
	{
		return MakeKey(uid, "bagInfo-");
	}
	static std::string EquipKey(int uid)
	{
        return MakeKey(uid, "eq-");
	}
	static std::string HeroKey(int uid)
	{
        return MakeKey(uid, "hero-");
	}
    static std::string HeroSkillKey(int uid)
    {
        return MakeKey(uid, "heroSki-");
    }
    static std::string HeroTalentKey(int uid)
    {
        return MakeKey(uid, "heroTal-");
    }
	static std::string HeroBagCurSizeKey(int uid)
	{
		return MakeKey(uid, "heroCurSize-");
	}
	static std::string SummonerKey(int uid)
	{
        return MakeKey(uid, "sum-");
	}
	static std::string StageStatusKey(int uid)
	{
        return MakeKey(uid, "stg-");
	}
    static std::string EliteStatusKey(int uid)
    {
        return MakeKey(uid, "elite-");
    }
	static std::string StageInfoKey(int uid)
	{
        return MakeKey(uid, "stginfo-");
	}
	static std::string StageBoxKey(int uid)
	{
		return MakeKey(uid, "stgbox-");
	}
    static std::string HeadIconKey(int uid)
    {
        return MakeKey(uid, "icon-");
    }
    static std::string TeamKey(int uid)
    {
        return MakeKey(uid, "team-");
    }
    static std::string TaskKey(int uid)
    {
        return MakeKey(uid, "task-");
    }
    static std::string AchieveKey(int uid)
    {
        return MakeKey(uid, "achieve-");
    }
	static std::string GuideKey(int uid)
	{
        return MakeKey(uid, "Guide-");
	}
	static std::string PassTeamKey()
	{
		return "passteam-key";
	}
    static std::string UnionKey(int unionId)
    {
        return MakeKey(unionId, "union-");
    }
    static std::string UnionAuditKey(int unionId)
    {
        return MakeKey(unionId, "unionAu-");
    }
    static std::string UnionLogKey(int unionId)
    {
        return MakeKey(unionId, "unionLog-");
    }
    static std::string UnionMemberKey(int unionId)
    {
        return MakeKey(unionId, "unionMb-");
    }
    static std::string ServiceCacheKey(int serviceID)
    {
        return MakeKey(serviceID, "sCache-");
    }
	static std::string ActivityInstanceKey(int uid)
	{
        return MakeKey(uid, "instance-");
	}
    //帮会ID列表Key
    static std::string UnionIDListKey(int nServerID)
    {
        return MakeKey(nServerID,"UnionIDList-");
    }
	//帮派远征KEY
	static std::string UnionExpiditionKey(int nUnionID)
	{
		return MakeKey(nUnionID, "unionExpiditionKey-");
	}
	//个人帮派数据KEY
	static std::string UserUnionKey(int nUid)
	{
		return MakeKey(nUid, "userUnionKey-");
	}
	//公会远征伤害排行Key
	static std::string UnionExpiditionDamageKey(int nUnionID)
	{
		return MakeKey(nUnionID, "ExpiditionDamageKey-");
	}
	//公会远征伤害排行数据Key
	static std::string UnionExpiditionDamageDataKey(int nUnionID)
	{
		return MakeKey(nUnionID, "ExpiditionDamageDataKey-");
	}
	//公会商店Key
	static std::string UnionShopKey(int nUnionID)
	{
		return MakeKey(nUnionID, "UnionShopKey-");
	}
	static std::string UnionMercenaryKey(int unionID)
	{
		return MakeKey(unionID, "union-mer-");
	}
	static std::string PersonDispatchMercenaryKey(int uid)
	{
		return MakeKey(uid, "usr-mer-");
	}
	static std::string PersonEmployedMercenaryKey(int uid)
	{
		return MakeKey(uid, "usr-mer-emp-");
	}
    //帮派公共属性key
    static std::string GlobalUnionKey(int nServerID)
    {
        return MakeKey(nServerID, "UnionKey-");
    }
	//个人邮件
    static std::string MailKey(int uid)
    {
        return MakeKey(uid, "mail-");
    }

	static std::string ReadWebMailKey(int uid)
    {
        return MakeKey(uid, "readWMail-");
    }
    //全局邮件
	static std::string GlobalMailKey()
    {
        return "gMail-key";
    }
	static std::string GlobalMailConextKey()
	{
		return "gMailCtx-key";
	}
	
    static std::string UnionMailKey(int uid)
    {
        return MakeKey(uid, "unionMail-");
    }
    static std::string ApplyKey(int uid)
    {
        return MakeKey(uid, "apply-");
    }
	static std::string GoldTestKey(int uid)
	{
		return MakeKey(uid, "goldTest-");
	};
	static std::string HeroTestKey(int uid)
	{
		return MakeKey(uid, "heroTest-");
	};
	static std::string TowerTestKey(int uid)
	{
		return MakeKey(uid, "towerTest-");
	};
	static std::string PvpKey(int uid)
	{
		return MakeKey(uid, "pvp-");
	}
	static std::string PvpTaskKey(int uid)
	{
		return MakeKey(uid, "pvptsk-");
	}
	static std::string PvpChestKey(int uid)
	{
		return MakeKey(uid, "pvpcst-");
	}
	static std::string PvpReportKey(int uid)
	{
		return MakeKey(uid, "pvpRep-");
	}
	static std::string PvpBattleIdKey(int uid)
	{
		return MakeKey(uid, "pvp-bid-");
	}
	static std::string PvpRankKey()
	{
		return "Pvp-RankKey";
	}
	static std::string ChampionRankKey()
	{
		return "Champion-RankKey";
	}
	static std::string PvpBakRankKey(int Day)
	{
		return MakeKey(Day, "pvp-Bakrank-");
	}

	static std::string TowerRankBakKey()
	{
		return "TowerRankbKey";
	}

	static std::string RankRweard(int nUid, int nType)
	{
		std::string Temp = MakeKey(nType, "RankRKey-") + "-";
		return MakeKey(nUid, Temp);
	}

	static std::string ChampionBakRankKey(int Day)
	{
		return MakeKey(Day, "champion-Bakrank-");
	}
	static std::string PvpRankReward(int nUid)
	{
		return MakeKey(nUid, "pvpReward-");
	}
	static std::string ChampionRankReward(int nUid)
	{
		return MakeKey(nUid, "championReward-");
	}
	static std::string ShopGoodsKey(int uid)
	{
		return MakeKey(uid, "ShopKey-");
	}

	static std::string ShopIDKey(int uid)
	{
		return MakeKey(uid, "ShopIDKey-");
	}

	static std::string ShopInfoKey(int uid)
	{
		return MakeKey(uid, "ShopInfoKey-");
	}

	static std::string ShopDiamondDoubleKey(int uid)
	{
		return MakeKey(uid, "DoubleCash:");
	}

	static std::string RankKey(int RankType)
	{
		return MakeKey(RankType, "RankKey-");
	}

	//排行榜进入顺序key
	static std::string RankCountKey(int RankType)
	{
		return MakeKey(RankType, "RankCountK-");
	}

	static std::string StatUidKey(int nUid)
	{
		return MakeKey(nUid, "StatUidKey:");
	}

	//活动Key
	static std::string ActiveKey(int uid, int ActiveID)
	{
		std::string Temp = MakeKey(uid, "ActiveKey-") + "-";
		return MakeKey(ActiveID, Temp);
	}

	static std::string TaskActiveValueKey(int uid,int ActiveID)
	{
		std::string Temp = MakeKey(uid, "TaskActiveValueKey-") + "-";
		return MakeKey(ActiveID, Temp);
	}

	//月卡Key
	static std::string pidKey(int uid)
	{
		return MakeKey(uid, "usrPidKey-");
	}

	//QQ蓝钻活动Key
	static std::string QQActiveKey(int uid, int nActiveID)
	{
		std::string Temp = MakeKey(uid, "QQActiveIndexKey-") + "-";
		return MakeKey(nActiveID, Temp);
	}

	//平台校验key
	static std::string UserOpenKey(std::string openid,int nChannelID)
	{
		std::string Temp = MakeKey(nChannelID, "pf:") + ":";
		return Temp + openid;
	}

	//平台User自增Key
	static std::string UsrCountKey()
	{
		return "usr-count";
	}

	//游客密码key
	static std::string GuestpsKey(std::string szMobile)
	{
		szMobile = "fanhouzhs" + szMobile;
		szMobile = szMobile + "yueliushui10yi";
		return szMobile;
	}

	//充值自增key
	static std::string UsrBillNoKey()
	{
		return "usr-billno";
	}

	//头像存储key
	static std::string HeadKey(int uid)
	{
		return MakeKey(uid, "UserHead-");
	}

	//礼包key
	static std::string GiftKey(int nGiftID)
	{
		return MakeKey(nGiftID, "giftbag-");
	}

	//礼包码Key
	static std::string GiftMaskKey(std::string strCode)
	{
		std::string Temp = "cdkey-" + strCode;
		return Temp;
	}

	//礼包角色礼包使用Key
	static std::string GiftActorKey(int nUid,int nWorldID)
	{
		std::string Temp = MakeKey(nUid, "giftActor-") + "-";
		Temp = MakeKey(nWorldID, Temp);
		return Temp;
	}

	//AnySDK 验证Key
	static std::string AnySDKCheckKey(int nChannelID, std::string openid)
	{
		std::string Temp = MakeKey(nChannelID, "ck:") + ":";
		Temp = Temp + openid;
		return Temp;
	}

	//AnySDK 订单Key
	static std::string AnySDKPayKey(int nUid)
	{
		return MakeKey(nUid,"BillNo:");
	}

	//GM账号ID
	static std::string GMUserKey(std::string strAdmin)
	{
		return "GM:" + strAdmin;
	}

	//GM账号权限
	static std::string GMPerssionKey(std::string strAdmin)
	{
		return "GMPer:" + strAdmin;
	}

	//登陆验证附加信息
	static std::string ckExtraDataKey(std::string strOpenid)
	{
		return "loginInfo:qqgame:" + strOpenid;
	}

	//登陆信息附加信息与uid对应
	static std::string ckExtraDataKey(int nUid)
	{
		return MakeKey(nUid, "ckLoginInfo:");
	}

private:
    static std::string MakeKey(int num, std::string prefix)
    {
        std::string key;
        std::stringstream ss;
        ss << num;
        ss >> key;

        return prefix + key;
    }
};

#endif //__MODEL_DEF_H__
