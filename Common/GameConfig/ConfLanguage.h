#ifndef __CONF_LANGUAGE_H__
#define __CONF_LANGUAGE_H__

#include "ConfManager.h"

class CConfLanguage : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);

	const char* getString(int stringId);

	inline const int getLanCount(){ return m_Words.size(); }

	std::map<int, std::string>& getWords()
	{
		return m_Words;
	}
private:
	std::map<int, std::string> m_Words;
};

inline const char* getLanguageString(int confId, int stringId)
{
	CConfLanguage* confLan = dynamic_cast<CConfLanguage*>(
		CConfManager::getInstance()->getConf(confId));
	if (NULL != confLan)
	{
		return confLan->getString(stringId);
	}
	return NULL;
}

#endif
