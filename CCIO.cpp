/*
 * CCIO.cpp
 *
 *  Created on: May 27, 2015
 *      Author: huang
 */

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#include <dirent.h>
#else
#include <windows.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>

#include "CCDefine.h"
#include "CCDebug.h"
#include "CCUtility.h"
#include "CCIO.h"

namespace cc
{

CCIO::CCIO()
{
	// TODO Auto-generated constructor stub

}

CCIO::~CCIO()
{
	// TODO Auto-generated destructor stub
}

bool CCIO::ReadFile2Buffer(const std::string& sFilePath, const std::string& sMode, std::string& sBuffer)
{
	FILE* fp = fopen(sFilePath.c_str(), sMode.c_str());
	if (NULL == fp)
	{
		CCLOG(FATAL, "fopen the file %s failed, Error:%d, Reason:%s", sFilePath.c_str(), errno, strerror(errno));
		return false;
	}

	bool bRet = false;

	do
	{
		if (0 != fseek(fp, 0, SEEK_END))
		{
			//LCOV_EXCL_START
			CCLOG(FATAL, "fseek(SEEK_END) the file %s failed, Error:%d, Reason:%s", sFilePath.c_str(), errno, strerror(errno));
			break;
			//LCOV_EXCL_STOP
		}

		long nLen = ftell(fp);
		if (-1 == nLen)
		{
			//LCOV_EXCL_START
			CCLOG(FATAL, "ftell the file %s failed, Error:%d, Reason:%s", sFilePath.c_str(), errno, strerror(errno));
			break;
			//LCOV_EXCL_STOP
		}

		if (0 != fseek(fp, 0, SEEK_SET))
		{
			//LCOV_EXCL_START
			CCLOG(FATAL, "fseek(SEEK_SET) the file %s failed, Error:%d, Reason:%s", sFilePath.c_str(), errno, strerror(errno));
			break;
			//LCOV_EXCL_STOP
		}

		char* pBuf = new char[nLen];
		memset(pBuf, '\0', nLen);

		do
		{
			size_t nRead = fread(pBuf, sizeof(char), nLen, fp);
			if (nRead != nLen)
			{
				//LCOV_EXCL_START
				CCLOG(FATAL, "fread the file %s failed, Error:%d, Reason:%s, read:%ld, size:%ld", sFilePath.c_str(), errno, strerror(errno), nRead, nLen);
				break;
				//LCOV_EXCL_STOP
			}

			sBuffer.assign(pBuf, nLen);

			bRet = true;

		} while (false);

		delete [] pBuf;
		pBuf = NULL;

	} while (false);

	fclose(fp);
	fp = NULL;

	return bRet;
}

bool CCIO::WriteBuffer2File(const std::string& sFilePath, const std::string& sMode, const std::string& sBuffer)
{
	FILE *fp = fopen(sFilePath.c_str(), sMode.c_str());
	if (NULL == fp)
	{
		CCLOG(FATAL, "fopen the file %s failed, Error:%d, Reason:%s", sFilePath.c_str(), errno, strerror(errno));
		return false;
	}

	bool bRet = false;

	do
	{
		size_t nWrite = fwrite(sBuffer.c_str(), sizeof(char), sBuffer.size(), fp);
		if (nWrite != sBuffer.size())
		{
			//LCOV_EXCL_START
			CCLOG(FATAL, "fwrite the file %s failed, Error:%d, Reason:%s, write:%ld, size:%ld", sFilePath.c_str(), errno, strerror(errno), nWrite, sBuffer.size());
			break;
			//LCOV_EXCL_STOP
		}

		bRet = true;

	} while (false);

	fclose(fp);
	fp = NULL;

	return bRet;
}

#if 0
bool CCIO::ListDir(const std::string& sDirPath, std::vector<std::string>& vFilePath)
{
	DIR *dp = opendir(sDirPath.c_str());
	if (NULL == dp)
	{
		CCLOG(INFO, "Open %s Failed, Error:%d, Reason:%s", sDirPath.c_str(), errno, strerror(errno));

		return false;
	}

	struct dirent *dirp = NULL;
	while ((dirp = readdir(dp)) != NULL)
	{
		std::string sFullPath;
		sFullPath += sDirPath;
		sFullPath += "/";
		sFullPath += dirp->d_name;

		struct stat st;
		lstat(sFullPath.c_str(), &st);
		if (S_ISDIR(st.st_mode))
		{
			//ignore hidden files
			if ('.' == dirp->d_name[0])
			{
				continue;
			}

			std::vector<std::string> vPath;
			ListDir(sFullPath, vPath);

			vFilePath.insert(vFilePath.begin(), vPath.begin(), vPath.end());
		}
		else
		{
			CCLOG(INFO, "sFullPath:%s", sFullPath.c_str());
			vFilePath.push_back(sFullPath);
		}
	}
	closedir(dp);

	return true;
}
#else
bool CCIO::ListDir(const std::string& sDirPath, std::vector<std::string>& vFilePath)
{
	if (!IsExist(sDirPath, true))
	{
		CCLOG(INFO, "The directory %s doesn't exist! \n", sDirPath.c_str());  
		return false;
	}

	char szCurPath[MAX_PATH];
	ZeroMemory(szCurPath, MAX_PATH);
	_snprintf(szCurPath, MAX_PATH, "%s\\*.*", sDirPath.c_str());

	WIN32_FIND_DATA FindFileData;
	ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATA));

	HANDLE hFile = FindFirstFile(CCString::s2ws(szCurPath).c_str(), &FindFileData);
	
	BOOL bFinded = TRUE;
	while(bFinded)
	{
		bFinded = FindNextFile(hFile, &FindFileData); //递归搜索其他的文件

		std::string sFileName = CCString::ws2s(FindFileData.cFileName);
		if( 0 == sFileName.compare(".") 
			|| 0 == sFileName.compare("..") ) //如果是"." ".."目录
		{
			continue;
		}

		std::string strFullPath;
		strFullPath += sDirPath;
		strFullPath += "\\";
		strFullPath += sFileName;
			
		if(IsDirectory(strFullPath.c_str())) //如果是目录，则递归地调用
		{
			std::vector<std::string> vPath;
			ListDir(strFullPath, vPath);

			vFilePath.insert(vFilePath.begin(), vPath.begin(), vPath.end());
		}
		else
		{
			CCLOG(INFO, "strFullPath:%s", strFullPath.c_str());
			vFilePath.push_back(strFullPath);
		}
	}

	FindClose(hFile);

	return true;  
}
#endif

#if 0
bool CCIO::Unzip2Dir(const std::string& sZipFilePath, const std::string& sDirPath)
{
	struct stat st;
	if (0 == stat(sDirPath.c_str(), &st) && S_ISDIR(st.st_mode))
	{
		CCLOG(INFO, "The directory %s already exist, remove it", sDirPath.c_str());

		if (!DelDir(sDirPath.c_str()))
		{
			CCLOG(FATAL, "Can't remove the directory %s", sDirPath.c_str());

			return false;
		}
	}

	if (0 != mkdir(sDirPath.c_str(), S_IRWXU))
	{
		CCLOG(FATAL, "Can't make the directory %s", sDirPath.c_str());

		return false;
	}

	std::string sCMD;
	sCMD += "unzip ";
	sCMD += sZipFilePath;
	sCMD += " -d ";
	sCMD += sDirPath;

	std::string sRet;
	if (!PGUtility::ExecuteShell(sCMD, sRet))
	{
		CCLOG(FATAL, "Can't unzip the file %s", sZipFilePath.c_str());

		return false;
	}

	if (0 != remove(sZipFilePath.c_str()))
	{
		CCLOG(FATAL, "Can't remove the file %s", sZipFilePath.c_str());

		return false;
	}

	return true;
}
#endif

#if 0
bool CCIO::DelDir(const std::string& sDirPath)
{
	char strpath[4096] = { 0 };
	strcpy(strpath, sDirPath.c_str());

	if (strpath[strlen(strpath) - 1] != '/')
	{
		strcat(strpath, "/");
	}

	DIR *dp = opendir(strpath);
	if (dp == NULL)
	{
		CCLOG(INFO, "Can't Open Directory: %s, Error:%d, Reason:%s", strpath, errno, strerror(errno));

		return false;
	}

	struct dirent *entry;
	while ((entry = readdir(dp)) != NULL)
	{
		char szfullpath[4096];
		sprintf(szfullpath, "%s%s", strpath, entry->d_name);

		struct stat statbuf;
		lstat(szfullpath, &statbuf);
		if (S_ISDIR(statbuf.st_mode))
		{
			if (strcmp(".", entry->d_name) == 0
					|| strcmp("..", entry->d_name) == 0)
			{
				continue;
			}

			DelDir(szfullpath);
			rmdir(szfullpath);
		}
		else
		{
			if (chmod(szfullpath, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) != 0)
			{
				CCLOG(INFO, "Modify Permissions Failed! Error:%d File:%s, Reason:%s", errno, szfullpath, strerror(errno));
			}
			remove(szfullpath);
		}
	}

	closedir(dp);
	rmdir (sDirPath.c_str());

	return true;
}
#endif

bool CCIO::MakeDir(const std::string& sDirPath)
{
	if (IsExist(sDirPath, true))
	{
		return true;
	}

	std::vector<std::string> vecSubDir;
	if (!CCUtility::Split(sDirPath, "\\", vecSubDir))
	{
		CCLOG(INFO, "Split the \"%s\" Failed!", sDirPath.c_str());
		return false;
	}

	std::string sCurDirPath;
	sCurDirPath += vecSubDir[0];
	sCurDirPath += "\\";

	for (size_t i=1; i<vecSubDir.size(); ++i)
	{
		if ("" == vecSubDir[i])
		{
			break;
		}

		sCurDirPath += vecSubDir[i];
		sCurDirPath += "\\";

		if (!IsExist(sCurDirPath, true))
		{
			CreateDirectory(CCString::s2ws(sCurDirPath).c_str(), NULL);
		}

		if (!IsExist(sCurDirPath, true))
		{
			CCLOG(INFO, "CreateDirectory the \"%s\" Failed!", sCurDirPath.c_str());
			return false;
		}
	}

    return true;
}

bool CCIO::IsDirectory(const std::string& sDir)
{
	char szCurPath[MAX_PATH];
	ZeroMemory(szCurPath, MAX_PATH);
	sprintf_s(szCurPath, MAX_PATH, "%s\\*", sDir.c_str());

	WIN32_FIND_DATA FindFileData;        
	ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATA));

	bool bRet = false;
	
	HANDLE hFile = FindFirstFile(CCString::s2ws(szCurPath).c_str(), &FindFileData); /**< find first file by given path. */
	if(INVALID_HANDLE_VALUE == hFile)   
	{  
		bRet = false;
	}
	else
	{
		bRet = true;
	}

	FindClose(hFile);

	return bRet;
} 

bool CCIO::DelDir(const std::string& sDirName)
{
	if (!IsExist(sDirName, true))
	{
		CCLOG(INFO, "The directory %s doesn't exist! \n", sDirName.c_str());  
		return false;
	}

	char szCurPath[MAX_PATH];
	ZeroMemory(szCurPath, MAX_PATH);
	_snprintf(szCurPath, MAX_PATH, "%s\\*.*", sDirName.c_str());

	WIN32_FIND_DATA FindFileData;
	ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATA));


	HANDLE hFile = FindFirstFile(CCString::s2ws(szCurPath).c_str(), &FindFileData);
	
	BOOL bFinded = TRUE;
	while(bFinded)
	{
		bFinded = FindNextFile(hFile, &FindFileData); //递归搜索其他的文件

		std::string sFileName = CCString::ws2s(FindFileData.cFileName);
		if( 0 != sFileName.compare(".") 
			&& 0 != sFileName.compare("..") ) //如果不是"." ".."目录
		{
			std::string strFileName;
			strFileName += sDirName;
			strFileName += "\\";
			strFileName += sFileName;
			
			if(IsDirectory(strFileName.c_str())) //如果是目录，则递归地调用
			{
				DelDir(strFileName);
			}
			else
			{
				DeleteFile(CCString::s2ws(strFileName).c_str());
			}
		}
	}

	FindClose(hFile);

	if (0 == RemoveDirectory(CCString::s2ws(sDirName).c_str()))
	{  
		CCLOG(INFO, "RemoveDirectory %s Failed! \n", sDirName.c_str());  
		return false;
	}

	return true;  
}

bool CCIO::CopyDir(const std::string& sSrcDir, const std::string& sTgtDir)
{
	std::wstring wsOldPath = CCString::s2ws(sSrcDir);
	wsOldPath.append(1, '\0');//需要double '\0'

	std::wstring wsNewPath = CCString::s2ws(sTgtDir);
	wsNewPath.append(1, '\0');//需要double '\0'

	SHFILEOPSTRUCT fileOp;
	ZeroMemory(&fileOp, sizeof(SHFILEOPSTRUCT));

	fileOp.hwnd = NULL;
	fileOp.wFunc = FO_COPY;
	fileOp.pFrom = wsOldPath.c_str();
	fileOp.pTo = wsNewPath.c_str();
	fileOp.fFlags = FOF_NO_UI;
	fileOp.hNameMappings = NULL;
	fileOp.lpszProgressTitle = NULL;

	return SHFileOperation(&fileOp) == 0;
}

bool CCIO::DelFile(const std::string& sFilePath)
{
	if (0 != remove(sFilePath.c_str()))
	{
		CCLOG(FATAL, "remove the file %s failed, Error:%d, Reason:%s", sFilePath.c_str(), errno, strerror(errno));
		return false;
	}

	return true;
}

bool CCIO::GetFileInfo(const std::string& sFilePath, CCFileInfo& fileInfo)
{
	FILE* fp = fopen(sFilePath.c_str(), "r");
	if (NULL == fp)
	{
		CCLOG(FATAL, "fopen the file %s failed, Error:%d, Reason:%s", sFilePath.c_str(), errno, strerror(errno));
		return false;
	}

#ifdef _WIN32
	fstat(_fileno(fp), &fileInfo);
#else
	fstat(fileno(fp), &fileInfo);
#endif

	fclose(fp);
	fp = NULL;

	return true;
}

bool CCIO::IsExist(const std::string& sPath, bool isDir)
{
	bool bRet = false;
	struct stat st;

	if (isDir)
	{
		//check accessable of the directory
#ifdef _WIN32
		std::string sTempPath = sPath.substr(0, sPath.size() - std::string(PATH_SEPARATOR).size());
		if (0 == stat(sTempPath.c_str(), &st) && (st.st_mode & _S_IFDIR))
#else
		if (0 == stat(sPath.c_str(), &st) && S_ISDIR(st.st_mode))
#endif
		{
			bRet = true;
		}
		else
		{
			CCLOG(FATAL, "Can't access directory %s, Error:%d, Reason :%s", sPath.c_str(), errno, strerror(errno));
		}
	}
	else
	{
		//check accessable of the source file
#ifdef _WIN32
		if (0 == stat(sPath.c_str(), &st) && (st.st_mode & _S_IFREG))
#else
		if (0 == stat(sPath.c_str(), &st) && S_ISREG(st.st_mode))
#endif
		{
			bRet = true;
		}
		else
		{
			CCLOG(FATAL, "Can't access source file %s, Error:%d, Reason :%s", sPath.c_str(), errno, strerror(errno));
		}
	}

	return bRet;
}

} /* namespace cc */
