/*
 * CCJsonHelper.h
 *
 *  Created on: 2015年10月14日
 *      Author: qiqi.huang
 */

#ifndef CCJSONHELPER_H_
#define CCJSONHELPER_H_

#include <string>
#include <vector>
#include <map>

#include "json/json.h"

namespace cc {

class CCJsonHelper
{
	CCJsonHelper();
	~CCJsonHelper();
public:
	static bool ParseFromString(const std::string& sDoc, Json::Value& root);
	static std::string SerializeToString(const Json::Value& root);
	static int JsonValue2Int(const Json::Value& value);
	static unsigned int JsonValue2UInt(const Json::Value& value);
	static double JsonValue2Double(const Json::Value& value);
	static bool JsonValue2Bool(const Json::Value& value);
	static std::string JsonValue2String(const Json::Value& value);
};

} // namespace

#endif /* CCJSONHELPER_H_ */
