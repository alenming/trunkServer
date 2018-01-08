#ifndef __TIMECALCTOOL_H__
#define __TIMECALCTOOL_H__

#include <time.h>

struct DayTime
{
	int hour;
	int minutes;
	int second;
};

struct MonthTime
{
	int month;
	int day;
	DayTime dayTime;
};

struct WeekTime
{
	int weekDay;
	DayTime dayTime;
};

struct YearTime
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
};

class CTimeCalcTool
{
public:
	//查询时间戳与现在时间是否跨过指定日时间
	static bool isDayTimeOver(time_t prev, DayTime &dayTime);
	//查询时间戳与现在时间是否跨过指定星期时间
	static bool isWeekTimeOver(time_t prev, WeekTime &weekTime);
	//查询时间戳与现在时间是否跨过指定月时间
	static bool isMonthTimeOver(time_t prev, MonthTime &month);
	//查询时间戳与现在时间是否大于second秒
	static bool isSecondOver(time_t prev, int second);
	//查询现在是否为指定的时间内
	static bool isDayTime(DayTime &beginTime, DayTime &endTime);
	//查询现在是否在指定日期内
	static bool isYearTime(YearTime &beginYearTime, YearTime &endYearTime);
	//查询是否到点, 并返回几次
	static int overDay(time_t prev, DayTime &dayTime);
	//查询是否到指定星期日期, 并返回几次
	static int overWeek(time_t prev, WeekTime &weekTime);
	//查询是否到指定秒数, 并返回几次
	static int overSecond(time_t prev, int second);
    //返回某个时间戳到下个几时几分的时间戳
    static int nextTimeStamp(time_t prev, int nextMin, int nextHour);
    //返回某个时间戳到周几几时几分的时间戳 wDay周1~7
    static int nextTimeStamp(time_t prev, int nextMin, int nextHour, int wDay);
	//返回prev过min分之后的时间戳
	static int nextTimeStamp(time_t prev, int min);
	//返回最近距凌晨min分钟的时间戳
	static int nextTimeStampToZero(time_t prev, int min);
	//返回当前时间戳所在当日0点0分的时间戳
	static int curTimeZero(time_t curt);
	//获取当前周第几天0点0分的时间戳
	static int curDayTimeZero(int nDay);
};

#endif //__TIMECALCTOOL_H__
