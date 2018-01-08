/*
金币试炼模型
1、保存试炼刷新时间、挑战次数、宝箱领取状态、总伤害值
2、获取/设置相关信息
*/

#ifndef __GOLDTRIALMODEL_H__
#define __GOLDTRIALMODEL_H__

#include <string>
#include <map>
#include "IDBModel.h"

enum EGoldTestField
{
	GT_FD_NONE = 0,
	GT_FD_USETIMES,         // 挑战次数
	GT_FD_RESETSTAMP,       // 时间戳
	GT_FD_DAMAGE,           // 总伤害
	GT_FD_CHESTFLAG,        // 宝箱状态
    GT_FD_END,
};

class Storage;
class CGoldTestModel : public IDBModel
{
public:
	CGoldTestModel();
	~CGoldTestModel();

public:

	bool init(int uid);

	bool Refresh();

	bool GetFieldNum(int nField, int &nValue);

	bool SetFieldNum(int nField, int nValue);
	
    bool AddFieldNum(int nField, int nValue);

    bool ResetGoldTest(int stamp);

private:
	int		                m_nUid;
    Storage *		        m_pStorage;
    std::string		        m_szKey;
    std::map<int, int>      m_mapGoldTestInfo; // <field, num>
};



#endif