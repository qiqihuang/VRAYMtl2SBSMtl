/*
 * CCUtility.cpp
 *
 *  Created on: 2014年5月22日
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
#include <stdint.h>
#include <ctype.h>
#include <map>
#include <random>

#include <time.h>
#ifdef _WIN32
#include <windows.h>
#include <sys/timeb.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include "CCDebug.h"
#include "CCUtility.h"

namespace cc {


CCUtility::CCUtility()
{
	// TODO Auto-generated constructor stub
}

CCUtility::~CCUtility()
{
	// TODO Auto-generated destructor stub
}

bool CCUtility::ExecuteShell(const std::string& sCommand, std::string& sResult)
{
#ifdef _WIN32
	FILE *fp = _popen(sCommand.c_str(), "r");
#else
    FILE *fp = popen(sCommand.c_str(), "r");
#endif
    if (NULL == fp)
    {
        CCLOG(FATAL, "popen %s error\n", sCommand.c_str());
        return false;
    }

    char buf[1024];
    while(fgets(buf, 1024, fp)!=NULL)
    {
        sResult += buf;
    }

#ifdef _WIN32
	_pclose(fp);
#else
    pclose(fp);
#endif
    fp = NULL;

    return true;
}

bool CCUtility::Split(const std::string& sSrc, const std::string& sDelim, std::vector<std::string>& vecDes)
{
	if (sSrc.empty())
	{
		return false;
	}

	size_t oldPos = 0;
	size_t newPos = 0;

	std::string sTemp;
	while (1)
	{
		newPos = sSrc.find(sDelim, oldPos);
		if (std::string::npos != newPos)
		{
			sTemp = sSrc.substr(oldPos, newPos - oldPos);

			vecDes.push_back(sTemp);

			oldPos = newPos + sDelim.size();

			continue;
		}
		else if (oldPos <= sSrc.size())
		{
			sTemp = sSrc.substr(oldPos);

			vecDes.push_back(sTemp);

			break;
		}
		else
		{
			break;
		}
	}

	return true;
}

std::string CCUtility::Trim(const std::string& sSrc)
{
	std::string sTgt = sSrc;
	std::string::size_type pos = sTgt.find_last_not_of(' ');
	if (pos != std::string::npos)
	{
		sTgt.erase(pos + 1);
		pos = sTgt.find_first_not_of(' ');
		if(std::string::npos != pos)
		{
			sTgt.erase(0, pos);
		}
	}
	else
	{
		sTgt.erase(sTgt.begin(), sTgt.end());
	}

	return sTgt;
}

bool CCUtility::IsInteger(const std::string& s)
{
	size_t nSize = s.size();
	for (size_t i=0; i<nSize; ++i)
	{
		if (0 == isdigit(s[i]))
		{
			return false;
		}
	}

	return true;
}

int CCUtility::Random(int nBegin, int nEnd)
{
	std::default_random_engine generator(time(NULL));
	std::uniform_int_distribution<int> dis(nBegin, nEnd);

	return dis(generator);
}

void CCUtility::StripStr(const std::string& sSrc, std::string& sTgt)
{
	const char *data = sSrc.c_str();
	int len = sSrc.size();
	int offset = 0;

	if (len == 0)
	{
		return;
	}

	while (len > 0 && (data[len - 1] == ' ' || data[len - 1] == '\t'))
	{
		--len;
	}

	for (; offset < len && (data[offset] == ' ' || data[offset] == '\t'); ++offset)
	{
		// empty;
	}

	sTgt.assign(data + offset, len - offset);
}

bool CCUtility::GetLocalIPAddr(std::string& sLocalIP)  
{  
	WORD wVersionRequested = MAKEWORD(2, 2);  

	WSADATA wsaData;  
	if (0 != WSAStartup(wVersionRequested, &wsaData))
	{
		return false;
	}

	char local[255] = {0};
	gethostname(local, sizeof(local));  
	hostent* ph = gethostbyname(local);  
	if (NULL == ph)
	{
		return false;
	}

	in_addr addr;
	memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); //这里仅获取第一个ip  

	sLocalIP.assign(inet_ntoa(addr));  

	WSACleanup();

	return true;  
}

void PrintErrorString(void)
{
	DWORD dwErrno = GetLastError();
	LPTSTR lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |  FORMAT_MESSAGE_FROM_SYSTEM |  FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrno,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);

	CCLOG(FATAL, "%s", CCString::ws2s((LPCTSTR)lpMsgBuf).c_str());
	LocalFree(lpMsgBuf);
}

bool CCUtility::ExecuteShell(const std::string& sApplicationName, const std::string& sCommandLine, const std::string& sWorkDirectory)
{
	wchar_t* pApplicationName = NULL;
	std::wstring wsApplicationName = CCString::s2ws(sApplicationName);
	if (!wsApplicationName.empty())
	{
		pApplicationName = const_cast<wchar_t *>(wsApplicationName.c_str());
	}

	wchar_t* pCommandLine = NULL;
	std::wstring wsCommandLine = CCString::s2ws(sCommandLine);
	if (!wsCommandLine.empty())
	{
		pCommandLine = const_cast<wchar_t *>(wsCommandLine.c_str());
	}

	wchar_t* pWorkDirectory = NULL;
	std::wstring wsWorkDirectory = CCString::s2ws(sWorkDirectory);
	if (!wsWorkDirectory.empty())
	{
		pWorkDirectory = const_cast<wchar_t *>(wsWorkDirectory.c_str());
	}

	STARTUPINFO si = {sizeof(si)};
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION pi;
	if(FALSE == ::CreateProcess(pApplicationName, pCommandLine, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, pWorkDirectory, &si, &pi))
	{
		PrintErrorString();
		return false;
	}

	CCLOG(INFO, "Rebuild ProcessID: %d, ThreadID: %d\n", pi.dwProcessId, pi.dwThreadId);
	CCLOG(INFO, "Application:%s", sApplicationName.c_str());
	CCLOG(INFO, "CommandLine:%s", sCommandLine.c_str());
	CCLOG(INFO, "WorkDirectory:%s", sWorkDirectory.c_str());

	bool bRet = false;

	DWORD dwRet = WaitForSingleObject(pi.hProcess, 120 * 1000);
	switch(dwRet)
	{
	case WAIT_OBJECT_0:
		CCLOG(INFO, "Rebuild Process Terminated Normally");
		bRet = true;
		break;
	case WAIT_TIMEOUT:
		CCLOG(INFO, "Rebuild Process Time Out");
		TerminateProcess(pi.hProcess, -1);
		break;
	case WAIT_FAILED:
		// Bad call to function (invalid handle?)
		break;
	}

	::CloseHandle(pi.hThread);
	::CloseHandle(pi.hProcess);

	return bRet;
}

} // namespace cc
