#include <comdef.h> 

#include "CCString.h"


CCString::CCString(void)
{
}


CCString::~CCString(void)
{
}

std::wstring CCString::s2ws(const std::string& s)
{
	_bstr_t t = s.c_str();
	wchar_t* pwchar = (wchar_t*)t;
	std::wstring result = pwchar;
	return result;
}

std::string CCString::ws2s(const std::wstring& ws)
{
	_bstr_t t = ws.c_str();
	char* pchar = (char*)t;
	std::string result = pchar;
	return result;
}

std::wstring CCString::StringToWString(const std::string &str)
{
	std::wstring wstr(str.length(),L' ');
	std::copy(str.begin(), str.end(), wstr.begin());
	return wstr; 
}

std::string CCString::WStringToString(const std::wstring &wstr)
{
	std::string str(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), str.begin());
	return str; 
}

std::string CCString::GetFileName(const std::string& sFullPath)
{
	std::size_t nPos = sFullPath.find_last_of("/\\");
	std::string sFileName = sFullPath.substr(nPos + 1);
	return sFileName;
}

std::string CCString::GetFilePath(const std::string& sFullPath)
{
	std::size_t nPos = sFullPath.find_last_of("/\\");
	std::string sFilePath = sFullPath.substr(0, nPos + 1);
	return sFilePath;
}


