/*
 * CCTime.cpp
 *
 *  Created on: 2016年1月7日
 *      Author: qiqi.huang
 */

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#include <sys/timeb.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif
#include <stdint.h>
#include <ctype.h>

#include "CCDebug.h"
#include "CCTime.h"

namespace cc
{

CCTime::CCTime()
{
	// TODO Auto-generated constructor stub
}

CCTime::~CCTime()
{
	// TODO Auto-generated destructor stub
}

void CCTime::MSleep(unsigned long fmsec)
{
#ifdef _WIN32
	Sleep(fmsec);
#else
	usleep(1000*fmsec);
#endif
}

double CCTime::GetTimeofDay(void)
{
#ifdef _WIN32
    struct _timeb tb;
    _ftime(&tb);
    return tb.time + tb.millitm / 1000;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
#endif
}

void CCTime::GetGMTTime(std::string& sTime)
{
	const char *g_s_wday[] = 
	{
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};

	const char *g_s_mon[] = 
	{
		"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};

	const char g_s_gmt_format[] = "%s, %.2d %s %.4d %.2d:%.2d:%.2d GMT";

	time_t t = time(0);
	struct tm *p = gmtime(&t);
	//char s[80];
	//strftime(s, 80, "%Y-%m-%d %H:%M:%S::%Z", p);

	char date[128];
	memset(date, '\0', 128);
	_snprintf(date, 128, g_s_gmt_format, g_s_wday[p->tm_wday], p->tm_mday, g_s_mon[p->tm_mon], 1900 + p->tm_year, p->tm_hour, p->tm_min, p->tm_sec);

	sTime.assign(date);
}

} // namespace
