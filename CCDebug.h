/*
 * CCDebug.h
 *
 *  Created on: 2014年5月22日
 *      Author: qiqi.huang
 */

#ifndef CC_DEBUG_H
#define CC_DEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <log4cplus/logger.h>

#include "CCString.h"

namespace cc
{

typedef enum
{
	INFO = 1,	//GREEN
	WARN,		//YELLOW
	FATAL		//RED
}LogLevel;

class CLogHelper
{
public:
	static CLogHelper& GetInstance();

	bool AddLog(LogLevel level, const char* fmt, ...);

private:
	CLogHelper();
	CLogHelper(const CLogHelper&);
	CLogHelper& operator=(const CLogHelper&);
	~CLogHelper();

private:
	//log4cplus::Logger m_instance;
};

#ifdef _WIN32
#define CCLOG(level, fmt, ...)	\
		CLogHelper::GetInstance().AddLog(level, "[%s:%d (%s)] " fmt, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#else
#define CCLOG(level, fmt, args...)	\
		CLogHelper::GetInstance().AddLog(level, "[%s:%d (%s)] " fmt, __FILE__, __LINE__, __FUNCTION__, ##args)
#endif

} // namespace

#endif /* CC_DEBUG_H */
