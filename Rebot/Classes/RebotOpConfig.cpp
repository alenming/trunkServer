#include "RebotOpConfig.h"
#include "CsvLoader.h"
#include "ConfAnalytic.h"
#include "RebotComm.h"

using namespace std;

CRebotOpConfig*	CRebotOpConfig::m_pInstance = NULL;

CRebotOpConfig::CRebotOpConfig()
{
}


CRebotOpConfig::~CRebotOpConfig()
{
}

CRebotOpConfig* CRebotOpConfig::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CRebotOpConfig();
	}

	return m_pInstance;
}

void CRebotOpConfig::destroy()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CRebotOpConfig::init()
{
	m_VecRebotOp.clear();

	if (!LoadRebotOpConfig())
	{
		return false;
	}

	return true;
}

std::vector<SOpData>&	CRebotOpConfig::GetRebotOp()
{
	return m_VecRebotOp;
}

bool CRebotOpConfig::LoadRebotOpConfig()
{
	CCsvLoader pLoader;
	pLoader.LoadCSV(REBOTOPCONFIG_PATH);

	while (pLoader.NextLine())
	{
		SOpData OpData;
		OpData.OpId = pLoader.NextInt();
		OpData.nMainCmd = pLoader.NextInt();
		OpData.nSubCmd = pLoader.NextInt();

		for (int i = 0; i < 18; i++)
		{
			string szParam = pLoader.NextStr();
			vector<string> VectValue;
			CConfAnalytic::ToJsonStr(szParam,VectValue);

			if (VectValue.size() == 0)
			{
				break;
			}

			if (VectValue.size() != 3)
			{
				return false;
			}

			SOpParamData ParamData;

			ParamData.nParamType = atoi(VectValue[0].c_str());
			ParamData.nLen = atoi(VectValue[1].c_str());
			ParamData.szParam = VectValue[2].c_str();

			OpData.VectParm.push_back(ParamData);
		}

		m_VecRebotOp.push_back(OpData);
	}

	return true;
}