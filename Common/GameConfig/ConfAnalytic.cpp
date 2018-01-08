#include "ConfAnalytic.h"
#include "ConfManager.h"

int CConfAnalytic::ToInt(const std::string& str)
{
	int ret = atoi(str.c_str());
	return ret;
}

bool CConfAnalytic::ToBool(const std::string& str)
{
	int ret = atoi(str.c_str());
	return ret != 0 ? true : false; 
}

float CConfAnalytic::ToFloat(const std::string& str)
{
	float ret = (float)atof(str.c_str());
	return ret;
}

float CConfAnalytic::ToPercent(const std::string& str)
{
	std::string::size_type per = str.find_last_of("%");
	if (std::string::npos != per)
	{
        float ret = static_cast<float>(atof(str.substr(0, str.size() - 1).c_str()));
		return ret / 100.0f;
	}
	return 0.0f;
}

void CConfAnalytic::ToJsonInt(const std::string& str, VecInt& ret)
{
	std::string::size_type head = str.find_first_of("[");
	std::string::size_type tail = str.find_last_of("]");
	if (std::string::npos != head && std::string::npos != tail)
	{
		std::vector<std::string> vec;
		StringSplit(str.substr(head + 1, str.size() - 2), vec, "+");
		for (std::vector<std::string>::iterator iter = vec.begin(); iter != vec.end(); ++iter)
		{
			int t = ToInt(*iter);
			ret.push_back(t);
		}
	}
}

void CConfAnalytic::ToJsonFloat(const std::string& str, VecFloat& ret)
{
	std::string::size_type head = str.find_first_of("[");
	std::string::size_type tail = str.find_last_of("]");
	if (std::string::npos != head && std::string::npos != tail)
	{
		std::vector<std::string> vec;
		StringSplit(str.substr(head + 1, str.size() - 2), vec, "+");
		for (std::vector<std::string>::iterator iter = vec.begin(); iter != vec.end(); ++iter)
		{
			float t = ToFloat(*iter);
			ret.push_back(t);
		}
	}
}

void CConfAnalytic::ToJsonStr(const std::string& str, std::vector<std::string>& ret)
{
	std::string::size_type head = str.find_first_of("[");
	std::string::size_type tail = str.find_last_of("]");
	if (std::string::npos != head && std::string::npos != tail)
	{
		std::vector<std::string> vec;
		StringSplit(str.substr(head + 1, str.size() - 2), vec, "+");
		for (std::vector<std::string>::iterator iter = vec.begin(); iter != vec.end(); ++iter)
		{
			ret.push_back(*iter);
		}
	}
}

void CConfAnalytic::ToVecMusicInfos(const std::string& str, VecMusicInfos& ret)
{
    std::vector<std::string> vecStr;
    std::vector<int> vecInt;
    StringSplit(str, vecStr, "*");
    for (unsigned int i = 0; i < vecStr.size(); ++i)
    {
        vecInt.clear();
        ToJsonInt(vecStr[i], vecInt);
        if (vecInt.size() >= 2)
        {
            MusicInfo info;
            info.MusicId = vecInt[0];
            info.MusicDelay = (vecInt[1] * 1.0f) / 1000.0f;
            if (vecInt.size() >= 3)
            {
                info.IsClose = vecInt[2] == 1;
            }
            if (vecInt.size() >= 4)
            {
                info.Volume = (vecInt[3] * 1.0f) / 1000.0f;
            }
            if (vecInt.size() >= 5)
            {
                info.Track = (vecInt[3] * 1.0f) / 1000.0f;
            }
            ret.push_back(info);
        }
    }
}

void CConfAnalytic::ToVec2(const std::string& str, Vec2& ret)
{
	VecFloat vec;
	ToJsonFloat(str, vec);
	if (vec.size() == 2)
	{
		ret.x = vec[0];
		ret.y = vec[1];
	}
}

void CConfAnalytic::ToVecVecInt(const std::string &str, VecVecInt& ret)
{
    if ("[]" == str || "" == str)
    {
        return;
    }

	std::vector<std::string> vecStr;
	std::vector<int> vecInt;
	StringSplit(str, vecStr, "*");
	for (unsigned int i = 0; i < vecStr.size(); ++i)
	{
        vecInt.clear();
		ToJsonInt(vecStr[i], vecInt);
		ret.push_back(vecInt);
	}
}

#ifndef RunningInServer
void CConfAnalytic::ToColor3B(const std::string& str, Color3B& ret)
{
	VecInt vec;
	ToJsonInt(str, vec);
	if (vec.size() == 3)
	{
		ret.r = vec[0];
		ret.g = vec[1];
		ret.b = vec[2];
	}
    else
    {
        ret = Color3B::WHITE;
    }
}
#endif 

void CConfAnalytic::ToFunction(const std::string& str, CDataFunction** fun)
{
	//fun([1+2]*[3.2+4.8]*5*6.7)
	CHECK_RETURN_VOID(!str.empty());
	std::string::size_type pre = str.find("(");
	std::string::size_type ext = str.find(")");
	CHECK_RETURN_VOID(pre != std::string::npos);
	CHECK_RETURN_VOID(ext != std::string::npos);
	std::string name = str.substr(0,pre);
	std::string param = str.substr(pre + 1, ext - pre - 1);
	CHECK_RETURN_VOID(!name.empty());
    int nameId = CConfManager::getInstance()->getFunID(name);
	if (-1 == nameId)
	{
		LOG("Execute False On File %s Line %d : %s", __FILE__, __LINE__, name.c_str());
	}

	*fun = new CDataFunction;
    (*fun)->NameID = nameId;
	std::string::size_type star = str.find("*");
	if(star != std::string::npos)
	{
		std::vector<std::string> vec;
		StringSplit(param,vec,"*");
		for(std::vector<std::string>::iterator iter = vec.begin(); iter != vec.end(); ++iter)
		{
			std::string::size_type head = iter->find("[");
			std::string::size_type tail = iter->find("]");
			if(head != std::string::npos && tail != std::string::npos)
			{
				std::string json = iter->substr(head + 1, tail - head - 1);
				Multi(json, *fun);
			}
			else
			{
				Singo(*iter, *fun);
			}
		}
	}
	else
	{
		std::string::size_type head = param.find("[");
		std::string::size_type tail = param.find("]");
		if(head != std::string::npos && tail != std::string::npos)
		{
			std::string json = param.substr(head + 1, tail - head - 1);
			Multi(json, *fun);
		}
		else
		{
			Singo(param, *fun);
		}
	}
}

int CConfAnalytic::JudgeDataType(const std::string& str)
{
	if(str.empty())
	{
		return CIT_UNVALID;
	}
	std::string::size_type pos = str.find(".");
	return pos != std::string::npos ? CIT_FLOAT: CIT_INT;
}

void CConfAnalytic::StringSplit(std::string src, std::vector<std::string>& ret, const std::string& split)
{
	if(src.empty())
	{
		return;
	}
	int pos = 0;
	src += split;
	int size = src.size();
	for(int i = 0; i < size; i++)
	{
		pos = src.find(split,i);                    
		if(pos < size)
		{
			std::string s = src.substr(i,pos-i);
			ret.push_back(s);
			i = pos + split.size() - 1;
		}
	}
}

void CConfAnalytic::Singo(const std::string& param, CDataFunction* data)
{
	CHECK_RETURN_VOID(!param.empty());
	int type = JudgeDataType(param);
	switch (type)
	{
	case CIT_INT:
		{
			data->ParamInt.push_back(atoi(param.c_str()));
		}
		break;
	case CIT_FLOAT:
		{
			data->ParamFloat.push_back(static_cast<float>(atof(param.c_str())));
		}
		break;
	default:
		break;
	}
}

void CConfAnalytic::Multi(const std::string& param, CDataFunction* data)
{
	CHECK_RETURN_VOID(!param.empty());
	std::vector<std::string> vec;
	CConfAnalytic::StringSplit(param, vec, "+");
	CHECK_RETURN_VOID(!vec.empty());
	int type = JudgeDataType(param);
	switch (type)
	{
	case CIT_INT:
		{
			std::vector<int> v;
			v.reserve(vec.size());
			for(std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it)
			{
				v.push_back(atoi(it->c_str()));
			}
			data->ParamIntArr.push_back(v);
		}
		break;
	case CIT_FLOAT:
		{
			std::vector<float> v;
			v.reserve(vec.size());
			for(std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it)
			{
                v.push_back(static_cast<float>(atof(it->c_str())));
			}
			data->ParamFloatArr.push_back(v);
		}
		break;
	default:
		break;
	}
}

void CConfAnalytic::ToJsonID_Num(const std::string& str, std::vector<ID_Num> &id_num)
{
    if ("[]" == str)
    {
        return;
    }

    VecVecInt temp;
    ToVecVecInt(str, temp);

    ID_Num idNum;
    VecVecInt::iterator iter = temp.begin();
    for (; iter != temp.end(); iter++)
    {
        if (2 == iter->size())
        {
            idNum.ID = (*iter)[0];
            idNum.num = (*iter)[1];

            id_num.push_back(idNum);
        }
    }
}

void CConfAnalytic::ToAward(std::string str, std::vector<AwardInfo>& vec)
{
    std::vector<std::string> vecStr;
    std::vector<std::string> vecSubStr;

    CConfAnalytic::StringSplit(str, vecStr, "*");
    for (std::vector<std::string>::iterator iter = vecStr.begin();
        iter != vecStr.end(); ++iter)
    {
        vecSubStr.clear();
        std::string temp = iter->substr(1, iter->size() - 2);
        CConfAnalytic::StringSplit(temp, vecSubStr, "+");
        if (vecSubStr.size() == 2)
        {
            AwardInfo info;
            info.ID = atoi(vecSubStr[0].c_str());
            info.Num = atoi(vecSubStr[1].c_str());
            vec.push_back(info);
        }
    }
}

void CConfAnalytic::ToTime(const std::string& str, int type, TimeInfo& info)
{
	if (str == "0" || str == "")
	{
		return;
	}
	memset(&info, 0, sizeof(info));
	std::vector<std::string> vecStr;
	CConfAnalytic::StringSplit(str.substr(1, str.size() - 2), vecStr, "+");
	switch (type)
	{
	case 2://÷‹ª÷∏¥
		if (vecStr.size() == 3)
		{
			info.Week = atoi(vecStr[0].c_str());
			info.Hour = atoi(vecStr[1].c_str());
			info.Min = atoi(vecStr[2].c_str());
		}
		break;
	case 1: //»’ª÷∏¥
		if (vecStr.size() == 2)
		{
			info.Hour = atoi(vecStr[0].c_str());
			info.Min = atoi(vecStr[1].c_str());
		}
		break;
	case -2: //√ø∏Ù∂‡…Ÿ√Îª÷∏¥
		if (vecStr.size() == 1)
		{
			info.Sec = atoi(vecStr[0].c_str());
		}
		break;
	case 0:	//≤ªª÷∏¥
	case -1: //¡¢º¥ª÷∏¥
		break;
	default:
		break;
	}
}