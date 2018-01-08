#include "TimeCalcTool.h"
#include "KXServer.h"
#include <string.h>

#define WEEK_DAY 7

bool CTimeCalcTool::isDayTimeOver(time_t prev, DayTime &daytime)
{
	time_t now = time(NULL);
	tm PrevTm;
	stlocaltime(&prev,&PrevTm);
	tm NowTm;
	stlocaltime(&now, &NowTm);
	int daySecond = 24 * 60 * 60;
	
	if (now - prev >= daySecond)
	{
		//如果大于1天, 返回true
		return true;
	}
	else
	{
		if (PrevTm.tm_mday != NowTm.tm_mday)
		{
			//不同一天的, now跨过指定时间即可
			bool ret1 = NowTm.tm_hour > daytime.hour;
			bool ret2 = NowTm.tm_hour == daytime.hour && NowTm.tm_min > daytime.minutes;
			bool ret3 = NowTm.tm_hour == daytime.hour && NowTm.tm_min == daytime.minutes && NowTm.tm_sec >= daytime.second;

			return ret1 || ret2 || ret3;
		}
		else
		{
			//同一天的, prev在指定时间之前
			bool ret1 = PrevTm.tm_hour < daytime.hour;
			bool ret2 = PrevTm.tm_hour == daytime.hour && PrevTm.tm_min < daytime.minutes;
			bool ret3 = PrevTm.tm_hour == daytime.hour && PrevTm.tm_min == daytime.minutes && PrevTm.tm_sec < daytime.second;
			//now在指定时间之后
			bool ret4 = NowTm.tm_hour > daytime.hour;
			bool ret5 = NowTm.tm_hour == daytime.hour && NowTm.tm_min > daytime.minutes;
			bool ret6 = NowTm.tm_hour == daytime.hour && NowTm.tm_min == daytime.minutes && NowTm.tm_sec >= daytime.second;

			return (ret1 || ret2 || ret3) && (ret4 || ret5 || ret6);
		}
	}
	return false;
}

bool CTimeCalcTool::isWeekTimeOver(time_t prev, WeekTime &weekTime)
{
	time_t now = time(NULL);
	tm PrevTm;
	stlocaltime(&prev, &PrevTm);
	tm NowTm;
	stlocaltime(&now, &NowTm);
	int weekSecond = 7 * 24 * 60 * 60;

	if (now - prev >= weekSecond)
	{
		//超过一个星期天
		return true;
	}
	else
	{ 
		//prev在指定星期天之前
		bool ret1 = PrevTm.tm_wday < weekTime.weekDay;
		bool ret2 = PrevTm.tm_wday == weekTime.weekDay
			&& PrevTm.tm_hour < weekTime.dayTime.hour;
		bool ret3 = PrevTm.tm_wday == weekTime.weekDay && PrevTm.tm_hour == weekTime.dayTime.hour
			&& PrevTm.tm_min < weekTime.dayTime.minutes;
		bool ret4 = PrevTm.tm_wday == weekTime.weekDay && PrevTm.tm_hour == weekTime.dayTime.hour
			&& PrevTm.tm_min == weekTime.dayTime.minutes && PrevTm.tm_sec < weekTime.dayTime.second;
		//now在指定星期天之后
		bool ret5 = NowTm.tm_wday > weekTime.weekDay;
		bool ret6 = NowTm.tm_wday == weekTime.weekDay
			&& NowTm.tm_hour > weekTime.dayTime.hour;
		bool ret7 = NowTm.tm_wday == weekTime.weekDay && NowTm.tm_hour == weekTime.dayTime.hour
			&& NowTm.tm_min > weekTime.dayTime.minutes;
		bool ret8 = NowTm.tm_wday == weekTime.weekDay && NowTm.tm_hour == weekTime.dayTime.hour
			&& NowTm.tm_min == weekTime.dayTime.minutes && NowTm.tm_sec >= weekTime.dayTime.second;

		return (ret1 || ret2 || ret3 || ret4) && (ret5 || ret6 || ret7 || ret8);
	}
	return false;
}

bool CTimeCalcTool::isMonthTimeOver(time_t prev, MonthTime &month)
{
	//time_t now = time(NULL);
	return true;
}

bool CTimeCalcTool::isSecondOver(time_t prev, int second)
{
	time_t now = time(NULL);
	return now - prev >= second;
}

bool CTimeCalcTool::isDayTime(DayTime &beginTime, DayTime &endTime)
{
	time_t now = time(NULL);
	tm NowTm;
	stlocaltime(&now, &NowTm);

	bool ret1 = NowTm.tm_hour > beginTime.hour;
	bool ret2 = NowTm.tm_hour == beginTime.hour && NowTm.tm_min > beginTime.minutes;
	bool ret3 = NowTm.tm_hour == beginTime.hour && NowTm.tm_min == beginTime.minutes && NowTm.tm_sec >= beginTime.second;

	bool ret4 = NowTm.tm_hour < endTime.hour;
	bool ret5 = NowTm.tm_hour == endTime.hour && NowTm.tm_min < endTime.minutes;
	bool ret6 = NowTm.tm_hour == endTime.hour && NowTm.tm_min == endTime.minutes && NowTm.tm_sec <= endTime.second;

	return (ret1 || ret2 || ret3) && (ret4 || ret5 || ret6);
}

bool CTimeCalcTool::isYearTime(YearTime &beginYearTime, YearTime &endYearTime)
{
	tm beginTm;
	tm endTm;
	memset(&beginTm, 0, sizeof(beginTm));
	memset(&endTm, 0, sizeof(endTm));

	time_t now = time(NULL);
	time_t beginTime;
	time_t endTime;

	beginTm.tm_year = beginYearTime.year;
	beginTm.tm_mon = beginYearTime.month;
	beginTm.tm_mday = beginYearTime.day;
	beginTm.tm_hour = beginYearTime.hour;
	beginTm.tm_min = beginYearTime.minute;
	beginTm.tm_sec = beginYearTime.second;
	
	endTm.tm_year = endYearTime.year;
	endTm.tm_mon = endYearTime.month;
	endTm.tm_mday = endYearTime.day;
	endTm.tm_hour = endYearTime.hour;
	endTm.tm_min = endYearTime.minute;
	endTm.tm_sec = endYearTime.second;

	beginTime = mktime(&beginTm);
	endTime = mktime(&beginTm);

	return beginTime <= now && endTime >= now;
}

int CTimeCalcTool::overDay(time_t prev, DayTime &dayTime)
{
	time_t now = time(NULL);
	tm PrevTm;
	stlocaltime(&prev, &PrevTm);
	tm NowTm;
	stlocaltime(&now, &NowTm);

	int times = 0;
	int d = int(now - prev);
	int ds = 24 * 60 * 60;
	// 先算到指定时间需要的秒数
	int seconds = 0;
	if (PrevTm.tm_hour <= dayTime.hour)
	{
		seconds = (dayTime.hour - PrevTm.tm_hour) * 60 * 60;
	}
	else
	{
		seconds = ((24 - PrevTm.tm_hour) + dayTime.hour) * 60 * 60;
	}

	seconds = (seconds + dayTime.minutes * 60 + dayTime.second) - (PrevTm.tm_min * 60 + PrevTm.tm_sec);
	if (d >= seconds)
	{
		times += 1;
		// 剩下的满1天+1
		d -= seconds;
		times += static_cast<int>(d / ds);
		return times;
	}
	return 0;
}

int CTimeCalcTool::overWeek(time_t prev, WeekTime &weekTime)
{
	time_t now = time(NULL);
	tm PrevTm;
	stlocaltime(&prev, &PrevTm);
	tm NowTm;
	stlocaltime(&now, &NowTm);

	int times = 0;
	int d = int(now - prev);
	int ds = 24 * 60 * 60;
	int ws = 7 * ds;

	int seconds = 0;
	if (PrevTm.tm_wday <= weekTime.weekDay)
	{
		seconds = (weekTime.weekDay - PrevTm.tm_wday) * ds;
	}
	else
	{
		seconds = ((7 - PrevTm.tm_wday) + weekTime.weekDay) * ds;
	}

	seconds = (seconds + weekTime.dayTime.hour * 60 * 60 + weekTime.dayTime.minutes * 60 + weekTime.dayTime.second)
		- (PrevTm.tm_hour * 60 * 60 + PrevTm.tm_min * 60 + PrevTm.tm_sec);

	if (d >= seconds)
	{
		times += 1;
		d -= seconds;
		times += static_cast<int>(d / ws);
		return times;
	}
	return 0;
}

int CTimeCalcTool::overSecond(time_t prev, int second)
{
	time_t now = time(NULL);
	int d = (int)(now - prev);
	return static_cast<int>(d/second);
}

int CTimeCalcTool::nextTimeStamp(time_t prev, int nextMin, int nextHour)
{
	tm PrevTm;
	stlocaltime(&prev, &PrevTm);

	int n = (nextHour - PrevTm.tm_hour) * 3600 + (nextMin - PrevTm.tm_min) * 60 - PrevTm.tm_sec;
    if (n <= 0)
    {
        n += 24 * 3600 + int(prev);
    }
    else
    {
        n += int(prev);
    }

    return n;
}

int CTimeCalcTool::nextTimeStamp(time_t prev, int nextMin, int nextHour, int wDay)
{
    wDay %= WEEK_DAY; // 注：周7为0
	tm PrevTm;
	stlocaltime(&prev, &PrevTm);

    int w = wDay - PrevTm.tm_wday;
    int n =  w*86400 + (nextHour - PrevTm.tm_hour) * 3600 + (nextMin - PrevTm.tm_min) * 60 - PrevTm.tm_sec;
    if (n < 0)
    {
		n = n + WEEK_DAY * 68400;
    }

    return n + (int)prev;
}

int CTimeCalcTool::nextTimeStamp(time_t prev, int min)
{
	return (int)prev + min * 60;
}

int CTimeCalcTool::nextTimeStampToZero(time_t prev, int min)
{
	int nextHour = 0;
	int nextMin = 0;
	//不允许超过一天
	min = min % 1440;
	nextHour = min / 60;
	nextMin = min % 60;
	return nextTimeStamp(prev, nextMin, nextHour);
}

//返回当前时间戳所在当日0点0分的时间戳
int CTimeCalcTool::curTimeZero(time_t curt)
{
	tm PrevTm;
	stlocaltime(&curt, &PrevTm);
	PrevTm.tm_hour = 0;
	PrevTm.tm_min = 0;
	PrevTm.tm_sec = 0;
	curt = mktime(&PrevTm);

	return curt;
}

//获取当前周第几天0点0分的时间戳
int CTimeCalcTool::curDayTimeZero(int nDay)
{
	tm PrevTm;
	time_t tTime = time(NULL);
	int nInterval = 0;
	stlocaltime(&tTime, &PrevTm);
	PrevTm.tm_hour = 0;
	PrevTm.tm_min = 0;
	PrevTm.tm_sec = 0;
	if (PrevTm.tm_wday == 0)
	{
		nInterval = (nDay - 7)*86400;
	}
	else
	{
		nInterval = (nDay - PrevTm.tm_wday) * 86400;
	}
	
	tTime = mktime(&PrevTm) + nInterval;
	
	return tTime;
}
