/*
 * CCIO.h
 *
 *  Created on: May 27, 2015
 *      Author: huang
 */

#ifndef CCIO_H_
#define CCIO_H_

#include <string>
#include <vector>
#include <sys/stat.h>

namespace cc
{

class CCIO
{
public:
	CCIO();
	virtual ~CCIO();

	static bool ReadFile2Buffer(const std::string& sFilePath, const std::string& sMode, std::string& sBuffer);
	static bool WriteBuffer2File(const std::string& sFilePath, const std::string& sMode, const std::string& sBuffer);

	static bool ListDir(const std::string& sDirPath, std::vector<std::string>& vFilePath);
	//static bool Unzip2Dir(const std::string& sZipFilePath, const std::string& sDirPath);
	//static bool DelDir(const std::string& sDirPath);

	static bool IsDirectory(const std::string& sDir);
	static bool DelDir(const std::string& sDirName);
	static bool MakeDir(const std::string& sDirPath);
	static bool CopyDir(const std::string& sSrcDir, const std::string& sTgtDir);

	static bool DelFile(const std::string& sFilePath);

	typedef struct stat CCFileInfo;
	static bool GetFileInfo(const std::string& sFilePath, CCFileInfo& fileInfo);

	static bool IsExist(const std::string& sPath, bool isDir);

};

} /* namespace cc */

#endif /* CCIO_H_ */
