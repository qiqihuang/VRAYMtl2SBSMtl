/*
 * CCDebug.cpp
 *
 *  Created on: 2014年5月22日
 *      Author: qiqi.huang
 */

#include <time.h>
#include <string.h>
#include <sstream>
#include <errno.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/loggingmacros.h>

#include "CCDefine.h"

#include "CCDebug.h"

namespace cc
{

CLogHelper::CLogHelper()
{
#if 0
	log4cplus::initialize();
	log4cplus::SharedAppenderPtr spAppender(new log4cplus::DailyRollingFileAppender(CCString::s2ws("log"), log4cplus::DAILY));

	std::string sPattern = "%D{%y/%m/%d %H:%M:%S} [%p] [RebuildManager] %m %n";
	std::auto_ptr < log4cplus::Layout > apLayout(new log4cplus::PatternLayout(CCString::s2ws(sPattern)));
	spAppender->setLayout(apLayout);

	auto logger = log4cplus::Logger::getInstance(CCString::s2ws("test"));
	m_instance = logger;
	m_instance.addAppender(spAppender);
#endif

#if 0
#include <ctime>
#include <sys/timeb.h>
	struct _timeb tb;
	_ftime(&tb);
	time_t t = tb.time;
	struct tm *gm_t = localtime(&t);

	size_t nSize = 16;
	char* pBuf = new char[nSize + 1];
	memset(pBuf, '\0', nSize + 1);

	_snprintf(pBuf, nSize, "%04d%02d%02d_%02d%02d%02d", gm_t->tm_year + 1900, gm_t->tm_mon + 1, gm_t->tm_mday, gm_t->tm_hour, gm_t->tm_min, gm_t->tm_sec);

	std::string sJobName;
	sJobName.assign(pBuf, nSize);
	delete [] pBuf;
#endif
}

CLogHelper::~CLogHelper()
{

}

CLogHelper& CLogHelper::GetInstance()
{
	static CLogHelper inst;
	return inst;
}

bool CLogHelper::AddLog(LogLevel level, const char* fmt, ...)
{
	if (INFO > level || FATAL < level)
	{
		return false;
	}

#ifdef _LOG_FATAL
	if (ERROR > level)
	{
		return false;
	}
#elif _LOG_WARN
	if (WARN > level)
	{
		return false;
	}
#elif _LOG_INFO
	if (INFO > level)
	{
		return false;
	}
#endif

	//get string size
	va_list args;
	va_start(args, fmt);
	int size = vsnprintf(NULL, 0, fmt, args);
	va_end(args);

	char* pBuf = new char[size + 1];
	memset(pBuf, '\0', size + 1);

	//get string
	va_start(args, fmt);
	vsnprintf(pBuf, size + 1, fmt, args);
	va_end(args);

	do
	{
		if (INFO == level)
		{
			//LOG4CPLUS_INFO(m_instance, pBuf);
			printf("%s\n", pBuf);
			break;
		}

		if (WARN == level)
		{
			//LOG4CPLUS_WARN(m_instance, pBuf);
			printf("%s\n", pBuf);
			break;
		}

		if (FATAL == level)
		{
			//LOG4CPLUS_ERROR(m_instance, pBuf);
			printf("%s\n", pBuf);
			break;
		}

	} while (false);

	delete[] pBuf;
	pBuf = NULL;

	return true;
}

} /* namespace cc */

