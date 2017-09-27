/*
 * PGUtility.h
 *
 *  Created on: 2014年5月22日
 *      Author: qiqi.huang
 */

#ifndef CCUTILITY_H_
#define CCUTILITY_H_

#include <string>
#include <vector>
#include <map>

namespace cc {

#define SAFE_DELETE(x) do {if(x) delete x, x = NULL;} while(0)
#define SAFE_DELETE_ARRAY(x) do {if(x) delete[] x, x = NULL;} while(0)
#define SAFE_FREE(x) do {if(x) free(x), x = NULL;} while(0)

class CCUtility
{
	CCUtility();
	~CCUtility();
public:
	static bool ExecuteShell(const std::string& sCommand, std::string& sResult);

	static bool Split(const std::string& sSrc, const std::string& sDelim, std::vector<std::string>& vecDes);
	
	static std::string Trim(const std::string& sSrc);
	
	static bool IsInteger(const std::string& s);

	static int Random(int nBegin, int nEnd);

	static void StripStr(const std::string& sSrc, std::string& sTgt);

	static bool GetLocalIPAddr(std::string& sLocalIP);

	static bool ExecuteShell(const std::string& sApplicationName, const std::string& sCommandLine, const std::string& sWorkDirectory);

};

} // namespace

#endif /* CCUTILITY_H_ */
