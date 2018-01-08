#ifndef _REBOT_OP_CONFIG_H__
#define _REBOT_OP_CONFIG_H__

#include<vector>
#include<string>

#define REBOTOPCONFIG_PATH "..//RebotConfig//RebotOpConfig.csv"

struct SOpParamData
{
	int nParamType;				//参数类型
	int nLen;					//参数长度
	std::string szParam;		//参数数据

	SOpParamData()
	{
		nParamType = 0;
		nLen = 0;
		szParam.clear();
	}
};

struct SOpData
{
	int OpId;								//操作ID
	int nMainCmd;							//主消息
	int nSubCmd;							//子消息
	std::vector<SOpParamData> VectParm;		//操作参数
};

class CRebotOpConfig
{
public:
	static CRebotOpConfig* getInstance();
	static void destroy();

	bool init();

	std::vector<SOpData>&	GetRebotOp();
protected:
	CRebotOpConfig();
	~CRebotOpConfig();

	bool LoadRebotOpConfig();

private:
	static CRebotOpConfig*	m_pInstance;
	std::vector<SOpData>	m_VecRebotOp;				//机器人操作列表
};


#endif //_REBOT_OP_CONFIG_H__
