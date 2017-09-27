/*
 * CCTime.h
 *
 *  Created on: 2016年1月7日
 *      Author: qiqi.huang
 */

#ifndef CCTIME_H_
#define CCTIME_H_

#include <string>
#include <vector>

namespace cc
{

class CCTime
{
	CCTime();
	~CCTime();
public:
	static void MSleep(unsigned long fmsec);
	static double GetTimeofDay(void);
	static void GetGMTTime(std::string& sTime);
};

} // namespace

#endif /* CCTIME_H_ */
