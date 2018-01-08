#include "RebotConfig.h"
#include "CsvLoader.h"

using namespace std;

CRebotConfig* CRebotConfig::m_pInstance = NULL;

CRebotConfig::CRebotConfig()
{
}


CRebotConfig::~CRebotConfig()
{
}

CRebotConfig* CRebotConfig::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CRebotConfig();
	}

	return m_pInstance;
}

void CRebotConfig::destroy()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CRebotConfig::init()
{
	m_MapRebotConfig.clear();

	if (!LoadRebotConfig())
	{
		return false;
	}

	return true;
}

string CRebotConfig::GetRebotConfig(int nConfigID)
{
	map<int, string>::iterator ator = m_MapRebotConfig.find(nConfigID);

	if (ator == m_MapRebotConfig.end())
	{
		return 0;
	}

	return ator->second;
}


bool CRebotConfig::LoadRebotConfig()
{
	CCsvLoader pLoader;
	pLoader.LoadCSV(REBOTCONFIG_PATH);

	while (pLoader.NextLine())
	{
		int nCoinfigID = pLoader.NextInt();
		string szParam = pLoader.NextStr();
		m_MapRebotConfig[nCoinfigID] = szParam;
	}

	return true;
}