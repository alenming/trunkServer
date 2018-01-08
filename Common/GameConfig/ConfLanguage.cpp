#include "ConfLanguage.h"
#include "CommTools.h"
using namespace std;

string& replace_all(string& str, const string& old_value, const string& new_value)
{
	while (true)
	{
		string::size_type pos(0);
		if ((pos = str.find(old_value)) != string::npos)
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}
	return   str;
}

bool CConfLanguage::LoadCSV(const std::string& str)
{
	CCsvLoader pLoader;
	CHECK_RETURN(loadCsv(pLoader, str, 3));
	m_Words.clear();
	//如果有数据
	while (pLoader.NextLine())
	{
		int lanId = pLoader.NextInt();
		pLoader.NextStr();
		m_Words[lanId] = pLoader.NextStr();
		replace_all(m_Words[lanId], "\\n", "\n");
		replace_all(m_Words[lanId], "&", ",");
	}
	return true;
}

const char* CConfLanguage::getString(int stringId)
{
	if (stringId == 0)
	{
		return "";
	}

	map<int, string>::iterator iter = m_Words.find(stringId);
	if (iter != m_Words.end())
	{
		return iter->second.c_str();
	}
	return toolToStr(stringId).c_str();
}