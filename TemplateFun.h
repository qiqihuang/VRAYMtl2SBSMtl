/*
 * TemplateFun.h
 *
 *  Created on: Jun 1, 2015
 *      Author: huang
 */

#ifndef TEMPLATEFUN_H_
#define TEMPLATEFUN_H_

#include <sstream>
#include <iomanip>

namespace cc
{
	template<typename T>
	std::string ToString(T t)
	{
		std::stringstream stream;
		stream << std::fixed;
		//stream << std::setprecision(6) << fValue;
		stream << t;

		std::string sRet;
		stream >> sRet;

		return sRet;
	}

	template<typename T>
	int ToInteger(T t)
	{
		std::stringstream stream;
		stream << t;

		int nRet;
		stream >> nRet;

		return nRet;
	}

	template<typename T>
	unsigned long ToUInt32(T t)
	{
		std::stringstream stream;
		stream << t;

		unsigned long nRet;
		stream >> nRet;

		return nRet;
	}

	template<typename T>
	double ToDouble(T t)
	{
		std::stringstream stream;
		stream << t;

		double fRet;
		stream >> fRet;

		return fRet;
	}

	template<typename T>
	bool ToBool(T t)
	{
		std::stringstream stream;
		stream << t;

		bool bRet;
		stream >> bRet;

		return bRet;
	}

} /* namespace cc */

#endif /* TEMPLATEFUN_H_ */
