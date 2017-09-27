#include <string>

#pragma once

class CCString
{
private:
	CCString(void);
	virtual ~CCString(void);
	
public:
	static std::wstring s2ws(const std::string& s);
	static std::string ws2s(const std::wstring& ws);
	
	static std::wstring StringToWString(const std::string &str);
	static std::string WStringToString(const std::wstring &wstr);

	static std::string GetFileName(const std::string& sFullPath);
	static std::string GetFilePath(const std::string& sFullPath);
};

