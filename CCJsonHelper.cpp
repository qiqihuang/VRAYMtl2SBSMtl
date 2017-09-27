/*
 * CCJsonHelper.cpp
 *
 *  Created on: 2015年10月14日
 *      Author: qiqi.huang
 */

#include "CCDebug.h"
#include "CCJsonHelper.h"

namespace cc {


CCJsonHelper::CCJsonHelper()
{
	// TODO Auto-generated constructor stub
}

CCJsonHelper::~CCJsonHelper()
{
	// TODO Auto-generated destructor stub
}

bool CCJsonHelper::ParseFromString(const std::string& sDoc, Json::Value& root)
{
	if (sDoc.empty())
	{
		CCLOG(WARN, "Empty Json String!!!");
		return false;
	}

	Json::Reader reader(Json::Features::strictMode());
	if (!reader.parse(sDoc, root, false))
	{
		CCLOG(FATAL, "Parse %s failed!!!", sDoc.c_str());
		return false;
	}

	return true;
}

std::string CCJsonHelper::SerializeToString(const Json::Value& root)
{
	std::string sJson;

	if (root.empty())
	{
		CCLOG(WARN, "Empty Json Value!!!");
		return sJson;
	}

	Json::FastWriter writer;
	return writer.write(root);
}

int CCJsonHelper::JsonValue2Int(const Json::Value& value)
{
	if (value.empty())
	{
		CCLOG(WARN, "Empty Json Value!!!");
		return 0;
	}

	if (!value.isInt())
	{
		CCLOG(WARN, "Json Value Type is not Integer!!!");
		return 0;
	}

	return value.asInt();
}

unsigned int CCJsonHelper::JsonValue2UInt(const Json::Value& value)
{
	if (value.empty())
	{
		CCLOG(WARN, "Empty Json Value!!!");
		return 0;
	}

	if (!value.isUInt())
	{
		CCLOG(WARN, "Json Value Type is not Unsigned Integer!!!");
		return 0;
	}

	return value.asUInt();
}

double CCJsonHelper::JsonValue2Double(const Json::Value& value)
{
	if (value.empty())
	{
		CCLOG(WARN, "Empty Json Value!!!");
		return 0.0;
	}

	if (!value.isDouble())
	{
		CCLOG(WARN, "Json Value Type is not Double!!!");
		return 0.0;
	}

	return value.asDouble();
}

bool CCJsonHelper::JsonValue2Bool(const Json::Value& value)
{
	if (value.empty())
	{
		CCLOG(WARN, "Empty Json Value!!!");
		return false;
	}

	if (!value.isBool())
	{
		CCLOG(WARN, "Json Value Type is not Boolean!!!");
		return false;
	}

	return value.asBool();
}

std::string CCJsonHelper::JsonValue2String(const Json::Value& value)
{
	std::string s;

	if (value.empty())
	{
		CCLOG(WARN, "Empty Json Value!!!");
		return s;
	}

	if (!value.isString())
	{
		CCLOG(WARN, "Json Value Type is not String!!!");
		return s;
	}

	return value.asString();
}

} // namespace
