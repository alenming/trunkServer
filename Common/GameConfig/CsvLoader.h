/*
* CSV 解析辅助工具
* 加载一个CSV，提供从CSV中按照指定的格式解析数据的接口
*
* 2014-12-16 By 宝爷
*/
#ifndef __CSV_LOADER_H__
#define __CSV_LOADER_H__

#include <string>
#include <vector>

#if KX_TARGET_PLATFORM == KX_PLATFORM_LINUX
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>
#endif

class CCsvLoader
{
public:
	CCsvLoader();
	virtual ~CCsvLoader();

	void Release();

	// 加载csv文件，传入相对路径，加载失败会返回失败
	// 加载成功后自动跳转至第一行
	bool LoadCSV(const char* fileName);

	// NextLine会跳转至下一行
	// 返回false表示已经到了尾部，返回true表示读到了新的一行数据
	bool NextLine();

	// 将当前行下一个字段解析为float并返回
	// 如果解析失败返回0.0f
	float NextFloat();

	// 将当前行下一个字段解析为int并返回
	// 如果解析失败返回0
	int NextInt();

	// 将当前行下一个字段解析为string并返回
	// 如果解析失败返回""
	std::string NextStr();
	// 左右两边去除空行
	std::string Trim(std::string str);
	// 返回当前行剩余的字符串
	std::string LeftStr();

	bool isCurrentLineNull();	//跳过空行(暂时只是跳过第一个为空的字段)

	int ColCount();

	// 将一个字符串以分隔符 sep 分隔开，转换为字符串，填入vector容器中
	bool SplitStrToVector(const std::string &str, char sep, std::vector<std::string>& out);

	// 将一个字符串以分隔符 sep 分隔开，转换为float，填入vector容器中
	bool SplitFloatToVector(const std::string &str, char sep, std::vector<float>& out);

	// 将一个字符串以分隔符 sep 分隔开，转换为int，填入vector容器中
	bool SplitIntToVector(const std::string &str, char sep, std::vector<int>& out);

private:

	std::string getStringFromFile(const std::string &path);

private:

	std::string m_CurLine;          //当前在解析的列
	std::string m_Document;         //CSV文档内容
	std::string::size_type m_nDocOffset;
	std::string::size_type m_nDocStep;        //文档的截取下标
	std::string::size_type m_nLineOffset;     //行的下标偏移值
	std::string::size_type m_nLineStep;       //行的截取下标
};

#endif