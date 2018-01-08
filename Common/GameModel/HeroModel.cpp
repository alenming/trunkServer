#include "HeroModel.h"
#include "RedisStorer.h"
#include "ModelDef.h"
#include "StorageManager.h"
#include "KxMemPool.h"
#include "KxCore.h"
#include "ConfAnalytic.h"
#include "HeroModel.h"

using namespace std;

CHeroModel::CHeroModel()
: m_pRedisStorer(NULL)
, m_nUid(0)
{
}

CHeroModel::~CHeroModel()
{
}

bool CHeroModel::init(int uid)
{
	Storage *pStorage = StorageManager::getInstance()->GetStorage(STORAGEID_HERO);
	if (NULL == pStorage)
	{
		return false;
	}

	m_pRedisStorer = dynamic_cast<CRedisStorer*>(pStorage->GetStorer(uid));
	if (NULL == m_pRedisStorer)
	{
		return false;
	}

	m_nUid = uid;
	m_strHeroKey = ModelKey::HeroKey(uid);
	Refresh();
	return true;
}

bool CHeroModel::Refresh()
{
	if (m_pRedisStorer == NULL)
	{
		return false;
	}

	std::map<int, std::string> mapAttrStr;
	if (SUCCESS != m_pRedisStorer->GetHash(m_strHeroKey, mapAttrStr))
	{
		return false;
	}

	m_mapHeros.clear();
	
	for (std::map<int, std::string>::iterator iter = mapAttrStr.begin();
		iter != mapAttrStr.end(); ++iter)
	{
		DBHeroAttr heroAttr;
		if (StringToAttr(iter->second, heroAttr))
		{
			m_mapHeros[iter->first] = heroAttr;
		}
	}
	return true;
}

bool CHeroModel::SetHero(int heroId, DBHeroAttr &heroAttr)
{
	if (m_pRedisStorer == NULL)
	{
		return false;
	}
	std::string attrStr = "";
	if (!AttrToString(heroAttr, attrStr))
	{
		KXLOGERROR("heroModel add hero error %d", heroId);
		return false;
	}
	if (SUCCESS != m_pRedisStorer->SetHashByField(m_strHeroKey, heroId, attrStr))
	{
		return false;
	}
	m_mapHeros[heroId] = heroAttr;
	return true;
}

bool CHeroModel::SetHero(std::map<int, DBHeroAttr> &heros)
{
	if (m_pRedisStorer == NULL)
	{
		return false;
	}
	std::map<int, std::string> mapAttrs;
	for (std::map<int, DBHeroAttr>::iterator iter = heros.begin();
		iter != heros.end(); ++iter)
	{
		if (!AttrToString(iter->second, mapAttrs[iter->first]))
		{
			KXLOGERROR("heroModel add hero error %d", iter->first);
			return false;
		}
		m_mapHeros[iter->first] = iter->second;
	}
	return SUCCESS == m_pRedisStorer->SetHash(m_strHeroKey, mapAttrs);
}

bool CHeroModel::UpdateHero(int heroId, DBHeroAttr &heroAttr)
{
	if (m_pRedisStorer == NULL)
	{
		return false;
	}
	std::map<int, DBHeroAttr>::iterator iter = m_mapHeros.find(heroId);
	if (iter == m_mapHeros.end())
	{
		return false;
	}
	m_mapHeros[heroId] = heroAttr;
	std::string attrStr;
	if (!AttrToString(heroAttr, attrStr))
	{
		KXLOGERROR("heroModel UpdateHero error %d", iter->first);
		return false;
	}
	return SUCCESS == m_pRedisStorer->SetHashByField(m_strHeroKey, heroId, attrStr);
}

bool CHeroModel::UpdateHeroFragment(int heroId, int frag)
{
	if (m_pRedisStorer == NULL)
	{
		return false;
	}
	DBHeroAttr heroAttr;
	std::map<int, DBHeroAttr>::iterator iter = m_mapHeros.find(heroId);
	if (iter == m_mapHeros.end())
	{
		m_mapHeros[heroId] = heroAttr;
	}

	m_mapHeros[heroId].fragment += frag;
	std::string attrStr;
	if (!AttrToString(m_mapHeros[heroId], attrStr))
	{
		KXLOGERROR("heroModel UpdateHeroFragment error %d", iter->first);
		return false;
	}
	return SUCCESS == m_pRedisStorer->SetHashByField(m_strHeroKey, heroId, attrStr);
}

bool CHeroModel::UpdateHeroTalent(int heroId, char* talent, int len)
{
	if (m_pRedisStorer == NULL)
	{
		return false;
	}
	std::map<int, DBHeroAttr>::iterator iter = m_mapHeros.find(heroId);
	if (iter == m_mapHeros.end())
	{
		return false;
	}

    if (len != TALENT_COUNT * sizeof(char))
    {
        return false;
    }
    
    memcpy(iter->second.talent, talent, len);
	std::string attrStr;
	if (!AttrToString(iter->second, attrStr))
	{
		KXLOGERROR("heroModel UpdateHeroFragment error %d", iter->first);
		return false;
	}
	return SUCCESS == m_pRedisStorer->SetHashByField(m_strHeroKey, heroId, attrStr);
}

bool CHeroModel::UpdateHeroEquip(int heroId, int part, int equipId)
{
	if (m_pRedisStorer == NULL)
	{
		return false;
	}
	std::map<int, DBHeroAttr>::iterator iter = m_mapHeros.find(heroId);
	if (iter == m_mapHeros.end())
	{
		return false;
	}

	DBHeroAttr &heroAttr = m_mapHeros[heroId];
	heroAttr.equipId[part] = equipId;
	return UpdateHero(heroId, heroAttr);
}

bool CHeroModel::UpdateHeroEquip(int heroId, std::map<int, int> &equipIds)
{
	if (m_pRedisStorer == NULL)
	{
		return false;
	}

	std::map<int, DBHeroAttr>::iterator iter = m_mapHeros.find(heroId);
	if (iter == m_mapHeros.end())
	{
		return false;
	}

	DBHeroAttr &heroAttr = m_mapHeros[heroId];
	for (std::map<int, int>::iterator iter = equipIds.begin();
		iter != equipIds.end(); ++iter)
	{
		heroAttr.equipId[iter->first] = iter->second;
	}
	return UpdateHero(heroId, heroAttr);
}

bool CHeroModel::RemoveHero(int heroId)
{
	if (m_pRedisStorer == NULL)
	{
		return false;
	}
	std::map<int, DBHeroAttr>::iterator iter = m_mapHeros.find(heroId);
	if (iter == m_mapHeros.end())
	{
		return false;
	}
	if (SUCCESS == m_pRedisStorer->DelHashByField(m_strHeroKey, heroId))
	{
		m_mapHeros.erase(iter);
	}
	return true;
}

bool CHeroModel::ExsitHero(int heroId)
{
	return m_mapHeros.find(heroId) != m_mapHeros.end();
}

bool CHeroModel::GetHero(int heroId, DBHeroAttr& heroAttr, bool fromDB)
{
	if (fromDB)
	{
		std::string attrStr = "";
		if (SUCCESS != m_pRedisStorer->GetHashByField(m_strHeroKey, heroId, attrStr)
			|| !StringToAttr(attrStr, heroAttr))
		{
			return false;
		}

		m_mapHeros[heroId] = heroAttr;
		return true;
	}
	
	if (m_mapHeros.find(heroId) == m_mapHeros.end())
	{
		return false;
	}

	heroAttr = m_mapHeros[heroId];
	return true;
}

std::map<int, DBHeroAttr>& CHeroModel::GetHeros(bool fromDB)
{
	if (fromDB)
	{
		Refresh();
	}
	return m_mapHeros;
}

bool CHeroModel::AttrToString(DBHeroAttr &heroAttr, std::string &attrStr)
{
	char temp[32] = {};
	attrStr = "";
	snprintf(temp, sizeof(temp), "%d", heroAttr.fragment);
	attrStr += temp;
	snprintf(temp, sizeof(temp), "+%d", heroAttr.star);
	attrStr += temp;
	snprintf(temp, sizeof(temp), "+%d", heroAttr.level);
	attrStr += temp;
	snprintf(temp, sizeof(temp), "+%d", heroAttr.exp);
	attrStr += temp;
	// Ìì¸³ÁÐ±í
    for (int i = 0; i < TALENT_COUNT; ++i)
    {
        snprintf(temp, sizeof(temp), "+%d", heroAttr.talent[i]);
        attrStr += temp;
    }

	if (heroAttr.equipId.size() >= 6)
	{
		for (std::map<int, int>::iterator iter = heroAttr.equipId.begin(); 
			iter != heroAttr.equipId.end(); ++iter)
		{
			snprintf(temp, sizeof(temp), "+%d", iter->second);
			attrStr += temp;
		}
	}
	else
	{
		snprintf(temp, sizeof(temp), "+%d", 0);
		for (int i = 0; i < 6; ++i)
		{
			attrStr += temp;
		}
	}

	return true;
}

bool CHeroModel::StringToAttr(std::string attrStr, DBHeroAttr & heroAttr)
{
	std::vector<int> attrs;
	attrStr = "[" + attrStr + "]";
	CConfAnalytic::ToJsonInt(attrStr, attrs);

	if (attrs.size() < 18)
	{
		return false;
	}
	
    int index = 0;
    heroAttr.fragment = attrs[index++];
    heroAttr.star = attrs[index++];
    heroAttr.level = attrs[index++];
    heroAttr.exp = attrs[index++];
    for (int i = 0; i < TALENT_COUNT; ++i)
    {
        heroAttr.talent[i] = attrs[index++];
    }

    for (int k = 1; k <= TREASURE; ++k)
	{
        heroAttr.equipId[k] = attrs[index++];
	}

	return true;
}
