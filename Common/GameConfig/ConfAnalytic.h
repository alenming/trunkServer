#ifndef _SUM_CONFIG_ANALYTIC_
#define _SUM_CONFIG_ANALYTIC_

#include "KxCSComm.h"

//配置项类型
enum ConfigItemType
{
	CIT_UNVALID,		//无效
	CIT_STRING,			//字符串
	CIT_INT,			//整型
	CIT_BOOL,			//布尔
	CIT_FLOAT,			//浮点型
	CIT_PERCENT,		//百分比
	CIT_JSON,			//数组
	CIT_COUNT,			//结算
	CIT_FUNCTION		//函数
};

//函数数据
struct CDataFunction
{
	CDataFunction(){ NameID = 0; }
	int					NameID;
	VecInt				ParamInt;
	VecFloat			ParamFloat;
	VecVecInt			ParamIntArr;
	VecVecFloat			ParamFloatArr;
};

struct MusicInfo
{
    MusicInfo()
    {
        MusicDelay = 0.0f;
        MusicId = 0;
        IsClose = false;
        Volume = 1.0f;
        Track = 0.0f;
    }

    float MusicDelay;    // 声音延迟
    int MusicId;         // 声音ID
    bool IsClose;         // 循环
    float Volume;        // 音量
    float Track;         // 声道
};

typedef std::vector<MusicInfo> VecMusicInfos;

//带两个整形的结构体
struct ID_Num
{
    int                 ID;          //ID
    int                 num;         //数量
};

struct AwardInfo
{
    int	ID;			 //奖励物品ID
    int	Num;		 //奖励物品数量
};

struct TimeInfo
{
	int Week;		 //周
	int Hour;		 //时
	int Min;		 //分
	int Sec;		 //秒
};

//数据解析
class CConfAnalytic
{
public:
	static int			ToInt(const std::string& str);
	static bool			ToBool(const std::string& str);
	static float		ToFloat(const std::string& str);
	static float		ToPercent(const std::string& str);
	static void			ToJsonInt(const std::string& str, VecInt& ret);
	static void			ToJsonFloat(const std::string& str, VecFloat& ret);
	static void			ToJsonStr(const std::string& str, std::vector<std::string>& ret);
    static void         ToVecMusicInfos(const std::string& str, VecMusicInfos& ret);
	static void			ToVec2(const std::string& str, Vec2& ret);
	static void			ToVecVecInt(const std::string& str, VecVecInt& ret);

    static void         ToJsonID_Num(const std::string& str, std::vector<ID_Num> &id_num);
    static void         ToAward(std::string str, std::vector<AwardInfo>& vec);
	static void			ToTime(const std::string& str, int type, TimeInfo& info);

#ifndef RunningInServer
	static void			ToColor3B(const std::string& str, Color3B& ret);
#endif 
	static void			ToFunction(const std::string& str, CDataFunction** fun);

public:
	static int			JudgeDataType(const std::string& str);
	static void			StringSplit(std::string src, std::vector<std::string>& ret, const std::string& split);
	static void			Singo(const std::string& param, CDataFunction* data);
	static void			Multi(const std::string& param, CDataFunction* data);
};

#endif