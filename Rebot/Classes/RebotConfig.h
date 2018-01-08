#ifndef _REBOT_CONFIG_H__
#define _REBOT_CONFIG_H__

#include<map>
#include<string>

#define REBOTCONFIG_PATH "..//RebotConfig//RebotConfig.csv"

class CRebotConfig
{
public:
	static CRebotConfig* getInstance();
	static void destroy();

	bool init();

	std::string GetRebotConfig(int nConfigID);

protected:
	CRebotConfig();
	~CRebotConfig();

	bool LoadRebotConfig();

private:
	static CRebotConfig*		m_pInstance;
	std::map<int,std::string>	m_MapRebotConfig;
};


#endif //_REBOT_CONFIG_H__

