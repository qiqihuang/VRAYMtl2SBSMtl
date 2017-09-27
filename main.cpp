#include <stdio.h>
#include <string.h>
#include <fstream>
#include <windows.h>

#include <boost/locale.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "tinyxml.h"

#include "TemplateFun.h"
#include "CCDefine.h"
#include "CCDebug.h"
#include "CCUtility.h"
#include "CCIO.h"
#include "CCTime.h"
#include "CCHash.h"
#include "CCJsonHelper.h"

using namespace cc;

void PrintMenu()
{
	printf("\n==========================\n");
	printf("Server ... \n");
	printf("--------------------------\n");
	printf("[X]: stop\n");
	printf("==========================\n");
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

/**
 1 jpg
 2 bmp
 3 png
 4 gif
 */
int GetPicFormat(const std::string& sImgBuf)
{
	if (8 > sImgBuf.size())
	{
		CCLOG(FATAL, " wrong image file, size < 8 bytes");
		return -1;
	}

	unsigned short pic[4] = { 0 };
	memcpy(pic, sImgBuf.c_str(), 8);

	unsigned short BMP = 0x4D42;
	unsigned short JPG = 0xD8FF;
	unsigned short PNG[4] = { 0x5089, 0x474E, 0x0A0D, 0x0A1A };
	unsigned short GIF[3] = { 0x4947, 0x3846, 0x6139 };

	int flag = 0;
	if (JPG == pic[0])
	{
		flag = 1;
		//CCLOG(INFO, "it is a jpg");
	}
	else if (BMP == pic[0])
	{
		flag = 2;
		//CCLOG(INFO, "it is a bmp");
	}
	else if (PNG[0] == pic[0] && PNG[1] == pic[1] && PNG[2] == pic[2] && PNG[3] == pic[3])
	{
		flag = 3;
		//CCLOG(INFO, "it is a png");
	}
	else if (GIF[0] == pic[0] && GIF[1] == pic[1] && GIF[2] == pic[2])
	{
		flag = 4;
		//CCLOG(INFO, "it is a gif");
	}
	else
	{
		flag = 0;
		CCLOG(INFO, "Can't identify the format");
	}

	return flag;
}

bool JPEGCompress(const std::string& sJpegFilePath)
{
	//
	std::string sApplicationName = "C:/Program Files/ImageProc/jpegoptim-1.4.4-win64-binary/jpegoptim-64.exe";

	//
	std::string sArgs;
	sArgs += " -m80 -o --force --strip-all --strip-iptc --strip-icc --all-progressive ";
	sArgs += "\"" + sJpegFilePath + "\"";

	//
	std::string sWorkspacePath;

	bool bRet = CCUtility::ExecuteShell(sApplicationName, sArgs, sWorkspacePath);

	return bRet;
}

bool PNGCompress(const std::string& sPngFilePath)
{
	//
	std::string sApplicationName = "C:/Program Files/ImageProc/pngquant/pngquant.exe";

	//
	std::string sArgs;
	sArgs += " --force --verbose --ordered --speed=1 --quality=50-90 ";
	sArgs += "\"" + sPngFilePath + "\"";
	sArgs += " --output ";
	sArgs += "\"" + sPngFilePath + "\"";


	//
	std::string sWorkspacePath;

	bool bRet = CCUtility::ExecuteShell(sApplicationName, sArgs, sWorkspacePath);

	return bRet;
}

bool CompressImage(const std::string& sImageFilePath)
{
	std::string sImageBuffer;
	if (!CCIO::ReadFile2Buffer(sImageFilePath, "rb", sImageBuffer))
	{
		return false;
	}

	int nType = GetPicFormat(sImageBuffer);

	//JPG
	if (1 == nType)
	{
		JPEGCompress(sImageFilePath);
	}
	else if (3 == nType)
	{
		PNGCompress(sImageFilePath);
	}

	return true;
}

bool SBS2SBSAR(const std::string& sSBSFilePath, const std::string& sOutputPath)
{
	//sbscooker 全路径
	std::string sApplicationName = "C:/Program Files/Allegorithmic/Substance Designer/sbscooker.exe";

	//执行参数
	std::string sArgs;
	sArgs += "--inputs ";
	sArgs += "\"" + sSBSFilePath + "\"";
	sArgs += " --output-path ";
	sArgs += "\"" + sOutputPath + "\"";

	//sbscooker 工作目录
	std::string sWorkspacePath;

	bool bRet = CCUtility::ExecuteShell(sApplicationName, sArgs, sWorkspacePath);

	return bRet;
}

typedef struct _Resource
{
	std::string sIdentifier;
	std::string sUID;
	std::string sType;
	std::string sFormat;
	std::string sFilePath;
	std::string sLastModif;
	std::string sCheckSum;
	
	typedef struct _Source
	{
		typedef struct _ExternalCopy
		{
			std::string sFileName;
			std::string sLastModif;
			std::string sCheckSum;
		}stExternalCopy;
		stExternalCopy externalCopy;

	}stSource;
	stSource source;

}stResource;

TiXmlElement* ResourceToXML(stResource* pResource)
{
	TiXmlElement* elemResource = new TiXmlElement("resource");

	//
	std::string sUTF8Identifier = boost::locale::conv::between(pResource->sIdentifier, "UTF-8", "GB 2312");
	TiXmlElement* elemIdentifier = new TiXmlElement("identifier");
	elemIdentifier->SetAttribute("v", sUTF8Identifier.c_str());
	elemResource->LinkEndChild(elemIdentifier);

	TiXmlElement* elemUID = new TiXmlElement("uid");
	elemUID->SetAttribute("v", pResource->sUID.c_str());
	elemResource->LinkEndChild(elemUID);

	TiXmlElement* elemType = new TiXmlElement("type");
	elemType->SetAttribute("v", pResource->sType.c_str());
	elemResource->LinkEndChild(elemType);

	TiXmlElement* elemFormat = new TiXmlElement("format");
	elemFormat->SetAttribute("v", pResource->sFormat.c_str());
	elemResource->LinkEndChild(elemFormat);

	std::string sUTF8FilePath = boost::locale::conv::between(pResource->sFilePath, "UTF-8", "GB 2312");
	TiXmlElement* elemFilePath = new TiXmlElement("filepath");
	elemFilePath->SetAttribute("v", sUTF8FilePath.c_str());
	elemResource->LinkEndChild(elemFilePath);

	TiXmlElement* elemLastModif = new TiXmlElement("lastModif");
	elemLastModif->SetAttribute("v", pResource->sLastModif.c_str());
	elemResource->LinkEndChild(elemLastModif);

	TiXmlElement* elemCheckSum = new TiXmlElement("checksum");
	elemCheckSum->SetAttribute("v", pResource->sCheckSum.c_str());
	elemResource->LinkEndChild(elemCheckSum);

	//
	TiXmlElement* elemSource = new TiXmlElement("source");
	{
		TiXmlElement* elemExternalcopy = new TiXmlElement("externalcopy");
		{
			std::string sUTF8FileName = boost::locale::conv::between(pResource->source.externalCopy.sFileName, "UTF-8", "GB 2312");
			TiXmlElement* elemFilename = new TiXmlElement("filename");
			elemFilename->SetAttribute("v", sUTF8FileName.c_str());
			elemExternalcopy->LinkEndChild(elemFilename);

			TiXmlElement* elemLastModif = new TiXmlElement("lastModif");
			elemLastModif->SetAttribute("v", pResource->source.externalCopy.sLastModif.c_str());
			elemExternalcopy->LinkEndChild(elemLastModif);

			TiXmlElement* elemCheckSum = new TiXmlElement("checksum");
			elemCheckSum->SetAttribute("v", pResource->source.externalCopy.sCheckSum.c_str());
			elemExternalcopy->LinkEndChild(elemCheckSum);
		}
		elemSource->LinkEndChild(elemExternalcopy);
	}
	elemResource->LinkEndChild(elemSource);

	return elemResource;
}

typedef struct _CompFilter
{
	std::string sFilter;

	typedef struct _Parameter
	{
		std::string sName;
		std::string sRelativeTo;

		typedef struct _ParamValue
		{
			std::string sType;
			std::string sValue;
		}stParamValue;
		stParamValue value;

	}stParameter;

	std::vector<stParameter*> vecpParameters;
}stCompFilter;

TiXmlElement* CompFilterToXML(stCompFilter* pCompFilter)
{
	TiXmlElement* elemCompFilter = new TiXmlElement("compFilter");
	
	//
	TiXmlElement* elemFilter = new TiXmlElement("filter");
	elemFilter->SetAttribute("v", pCompFilter->sFilter.c_str());
	elemCompFilter->LinkEndChild(elemFilter);

	//
	TiXmlElement* elemParameters = new TiXmlElement("parameters");

	std::vector<stCompFilter::stParameter*>::iterator it;
	for (it = pCompFilter->vecpParameters.begin(); it != pCompFilter->vecpParameters.end(); ++it)
	{
		stCompFilter::stParameter* pParameter = (*it);
		if (NULL == pParameter)
		{
			continue;
		}

		TiXmlElement* elemParameter = new TiXmlElement("parameter");
		
		//
		TiXmlElement* elemName = new TiXmlElement("name");
		elemName->SetAttribute("v", pParameter->sName.c_str());
		elemParameter->LinkEndChild(elemName);

		//
		TiXmlElement* elemRelativeTo = new TiXmlElement("relativeTo");
		elemRelativeTo->SetAttribute("v", pParameter->sRelativeTo.c_str());
		elemParameter->LinkEndChild(elemRelativeTo);

		//
		TiXmlElement* elemParamValue = new TiXmlElement("paramValue");
		{
			TiXmlElement* elemConstantValueString = new TiXmlElement(pParameter->value.sType.c_str());
			{
				std::string sUTF8Value = boost::locale::conv::between(pParameter->value.sValue, "UTF-8", "GB 2312");
				TiXmlElement* elemValue = new TiXmlElement("value");
				elemValue->SetAttribute("v", sUTF8Value.c_str());

				elemConstantValueString->LinkEndChild(elemValue);
			}
			elemParamValue->LinkEndChild(elemConstantValueString);
		}
		elemParameter->LinkEndChild(elemParamValue);

		elemParameters->LinkEndChild(elemParameter);
	}
	elemCompFilter->LinkEndChild(elemParameters);

	return elemCompFilter;
}

bool ProcessGraph(TiXmlNode* &nodeGraph, const std::string& sName, std::map<std::string, stCompFilter*> mapCompFilter)
{
	for(TiXmlElement* node = nodeGraph->FirstChildElement(); node; node = node->NextSiblingElement())  
	{  
		std::string sNodeName = node->Value();
		if (sNodeName == "identifier")
		{
			std::string sUTF8Name = boost::locale::conv::between(sName, "UTF-8", "GB 2312");
			node->SetAttribute("v", sUTF8Name.c_str());
			continue;
		}

		if (sNodeName != "compNodes")
		{
			continue;
		}

		for(TiXmlElement* compNode = node->FirstChildElement(); compNode;)  
		{
			TiXmlElement* elem = compNode->FirstChildElement();
			std::string elemName = elem->Value();
			if (elemName != "g_identifier")
			{
				//
				compNode = compNode->NextSiblingElement();
				continue;
			}

			std::string elemValue = elem->Attribute("v");

			std::map<std::string, stCompFilter*>::iterator it = mapCompFilter.find(elemValue);
			if (it == mapCompFilter.end())
			{
				//
				TiXmlElement* compNodeNext = compNode->NextSiblingElement();
				//移除节点
				node->RemoveChild(compNode);
				//
				compNode = compNodeNext;
				continue;
			}

			stCompFilter* pCompFilter = it->second;

			TiXmlElement* elemCompFilter = CompFilterToXML(pCompFilter);

			TiXmlElement* elemCompImplementation = new TiXmlElement("compImplementation");
			elemCompImplementation->LinkEndChild(elemCompFilter);

			compNode->LinkEndChild(elemCompImplementation);

			//
			compNode = compNode->NextSiblingElement();
		}

	}

	return true;
}

bool ProcessGroup(TiXmlNode* &nodeGroup, std::vector<stResource*> vecpResource)
{
	TiXmlNode* nodeContent = nodeGroup->FirstChild("content");
	
	std::vector<stResource*>::iterator it;
	for (it = vecpResource.begin(); it != vecpResource.end(); ++it)
	{
		stResource* pResource = (*it);
		if (NULL == pResource)
		{
			continue;
		}

		TiXmlElement* pElemResource = ResourceToXML(pResource);
		if (NULL == pElemResource)
		{
			continue;
		}

		nodeContent->LinkEndChild(pElemResource);
	}

	return true;
}

typedef struct _InputParameter
{
	std::string sName;

	typedef struct _basecolor
	{
		std::string sColor;
		std::string sBitmap;
	}stBaseColor;
	stBaseColor basecolor;

	typedef struct _normal
	{
		std::string sBitmap;
		std::string sIntensity;
	}stNormal;
	stNormal normal;

	typedef struct _roughness
	{
		std::string sColor;
	}stRoughness;
	stRoughness roughness;

	typedef struct _metallic
	{
		std::string sColor;
	}stMetallic;
	stMetallic metallic;

}stInputParameter;

int main(int argc, char* argv[])
{
	if (argc < 3) 
	{
		printf("Usage: VRAY2SBS template.sbs input.json \n");
		return -1;
	}

	if (!CCIO::IsExist(argv[1], false))
	{
		CCLOG(WARN, "template.sbs doesn't Exist !!!");
		return -1;
	}
	std::string sSBSTemplateFilePath = argv[1];

	if (!CCIO::IsExist(argv[2], false))
	{
		CCLOG(WARN, "input.json doesn't exist !!!");
		return -1;
	}
	std::string sInputJsonFilePath = argv[2];
	
	std::string sInputJsonFileData;
	CCIO::ReadFile2Buffer(sInputJsonFilePath, "rb", sInputJsonFileData);
	Json::Value root;
	if (!CCJsonHelper::ParseFromString(sInputJsonFileData, root))
	{
		return -1;
	}

	stInputParameter* pInputParameter = new stInputParameter;
	pInputParameter->sName = CCJsonHelper::JsonValue2String(root["name"]);
	pInputParameter->basecolor.sBitmap = CCJsonHelper::JsonValue2String(root["basecolor"]["bitmap"]);
	pInputParameter->basecolor.sColor = CCJsonHelper::JsonValue2String(root["basecolor"]["color"]);
	pInputParameter->normal.sBitmap = CCJsonHelper::JsonValue2String(root["normal"]["bitmap"]);
	pInputParameter->normal.sIntensity = CCJsonHelper::JsonValue2String(root["normal"]["intensity"]);
	pInputParameter->roughness.sColor = CCJsonHelper::JsonValue2String(root["roughness"]["color"]);
	pInputParameter->metallic.sColor = CCJsonHelper::JsonValue2String(root["metallic"]["color"]);

	/*
	wchar_t szPath[MAX_PATH] = {'\0',};
	GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath));
	(wcsrchr(szPath, ('\\')))[1] = 0;

	boost::uuids::uuid aUUID = boost::uuids::random_generator()();

	std::string sResourcePath;
	sResourcePath += CCString::ws2s(szPath);
	sResourcePath += boost::uuids::to_string(aUUID) + ".resource";
	if (!CCIO::MakeDir(sResourcePath))
	{
		CCLOG(FATAL, "MakeDir \"%s\" Failed!", sResourcePath.c_str());
		return false;
	}
	if (!CCIO::IsExist(sResourcePath, true))
	{
		CCLOG(WARN, "% doesn't Exist !!!", sResourcePath.c_str());
		return -1;
	}
	*/

	TiXmlDocument doc(sSBSTemplateFilePath.c_str());
	if (!doc.LoadFile())
	{
		CCLOG(WARN, "Failed to Load template.sbs File !!!");
		return -1;
	}

	TiXmlElement* package = doc.RootElement();
	TiXmlNode* contentNode = package->FirstChild("content");
	TiXmlNode* graphNode = contentNode->FirstChild("graph");
	TiXmlNode* groupNode = contentNode->FirstChild("group");

	std::vector<stResource*> vecpResource;
	{
		//压缩图片
		//CompressImage(pInputParameter->basecolor.sBitmap);

		CCIO::CCFileInfo fileInfo;
		CCIO::GetFileInfo(pInputParameter->basecolor.sBitmap, fileInfo);

		stResource* pResource = new stResource;
		pResource->sIdentifier = CCString::GetFileName(pInputParameter->basecolor.sBitmap);
		pResource->sUID = "1294561004";
		pResource->sType = "bitmap";
		pResource->sFormat = "PNG";
		pResource->sFilePath = "../../AppData/Local/Temp/Allegorithmic/Substance Designer/resources/" + CCString::GetFileName(pInputParameter->basecolor.sBitmap);
		pResource->sLastModif = ToString(fileInfo.st_mtime);
		pResource->sCheckSum = CCHash::CCMD5(pInputParameter->basecolor.sBitmap);
		pResource->source.externalCopy.sFileName = pInputParameter->basecolor.sBitmap;
		pResource->source.externalCopy.sLastModif = "";
		pResource->source.externalCopy.sCheckSum = "";

		vecpResource.push_back(pResource);
	}
	{
		//压缩图片
		//CompressImage(pInputParameter->normal.sBitmap);

		CCIO::CCFileInfo fileInfo;
		CCIO::GetFileInfo(pInputParameter->normal.sBitmap, fileInfo);
		
		stResource* pResource = new stResource;
		pResource->sIdentifier = CCString::GetFileName(pInputParameter->normal.sBitmap);
		pResource->sUID = "1294561758";
		pResource->sType = "bitmap";
		pResource->sFormat = "PNG";
		pResource->sFilePath = "../../AppData/Local/Temp/Allegorithmic/Substance Designer/resources/" + CCString::GetFileName(pInputParameter->normal.sBitmap);
		pResource->sLastModif = ToString(fileInfo.st_mtime);
		pResource->sCheckSum = CCHash::CCMD5(pInputParameter->normal.sBitmap);
		pResource->source.externalCopy.sFileName = pInputParameter->normal.sBitmap;
		pResource->source.externalCopy.sLastModif = "";
		pResource->source.externalCopy.sCheckSum = "";

		vecpResource.push_back(pResource);
	}
	ProcessGroup(groupNode, vecpResource);

	std::map<std::string, stCompFilter*> mapCompFilter;
	{
		if (CCIO::IsExist(pInputParameter->basecolor.sBitmap, false))
		{
			//优先使用贴图
			stCompFilter* pCompFilter = new stCompFilter;
			pCompFilter->sFilter = "bitmap";
			{
				stCompFilter::stParameter* pParameter = new stCompFilter::stParameter;
				pParameter->sName = "bitmapresourcepath";
				pParameter->sRelativeTo = "0";
				pParameter->value.sType = "constantValueString";
				pParameter->value.sValue = "pkg:///Resources/"+ CCString::GetFileName(pInputParameter->basecolor.sBitmap) + "?dependency=1294560918";
				pCompFilter->vecpParameters.push_back(pParameter);
			}
			{
				stCompFilter::stParameter* pParameter = new stCompFilter::stParameter;
				pParameter->sName = "outputsize";
				pParameter->sRelativeTo = "0";
				pParameter->value.sType = "constantValueInt2";
				pParameter->value.sValue = "9 9";
				pCompFilter->vecpParameters.push_back(pParameter);
			}
			mapCompFilter.insert(std::make_pair("basecolor", pCompFilter));
		}
		else if (!pInputParameter->basecolor.sColor.empty())
		{
			//
			stCompFilter* pCompFilter = new stCompFilter;
			pCompFilter->sFilter = "uniform";
			{
				stCompFilter::stParameter* pParameter = new stCompFilter::stParameter;
				pParameter->sName = "colorswitch";
				pParameter->sRelativeTo = "0";
				pParameter->value.sType = "constantValueBool";
				pParameter->value.sValue = "0";
				pCompFilter->vecpParameters.push_back(pParameter);
			}
			{
				stCompFilter::stParameter* pParameter = new stCompFilter::stParameter;
				pParameter->sName = "outputcolor";
				pParameter->sRelativeTo = "0";
				pParameter->value.sType = "constantValueFloat4";
				pParameter->value.sValue = pInputParameter->basecolor.sColor;
				pCompFilter->vecpParameters.push_back(pParameter);
			}
			mapCompFilter.insert(std::make_pair("basecolor", pCompFilter));
		}
		else
		{
			//
		}
		
	}
	{
		if (CCIO::IsExist(pInputParameter->normal.sBitmap, false))
		{
			stCompFilter* pCompFilter = new stCompFilter;
			pCompFilter->sFilter = "bitmap";
			{
				stCompFilter::stParameter* pParameter = new stCompFilter::stParameter;
				pParameter->sName = "bitmapresourcepath";
				pParameter->sRelativeTo = "0";
				pParameter->value.sType = "constantValueString";
				pParameter->value.sValue = "pkg:///Resources/" + CCString::GetFileName(pInputParameter->normal.sBitmap) + "?dependency=1294560918";
				pCompFilter->vecpParameters.push_back(pParameter);
			}
			{
				stCompFilter::stParameter* pParameter = new stCompFilter::stParameter;
				pParameter->sName = "outputsize";
				pParameter->sRelativeTo = "0";
				pParameter->value.sType = "constantValueInt2";
				pParameter->value.sValue = "9 9";
				pCompFilter->vecpParameters.push_back(pParameter);
			}
			mapCompFilter.insert(std::make_pair("normal", pCompFilter));
		}
		else
		{
			//
		}
	}
	{
		stCompFilter* pCompFilter = new stCompFilter;
		pCompFilter->sFilter = "normal";
		{
			stCompFilter::stParameter* pParameter = new stCompFilter::stParameter;
			pParameter->sName = "intensity";
			pParameter->sRelativeTo = "0";
			pParameter->value.sType = "constantValueFloat1";
			pParameter->value.sValue = pInputParameter->normal.sIntensity;
			pCompFilter->vecpParameters.push_back(pParameter);
		}
		mapCompFilter.insert(std::make_pair("normal_intensity", pCompFilter));
	}
	{
		stCompFilter* pCompFilter = new stCompFilter;
		pCompFilter->sFilter = "uniform";
		{
			stCompFilter::stParameter* pParameter = new stCompFilter::stParameter;
			pParameter->sName = "colorswitch";
			pParameter->sRelativeTo = "0";
			pParameter->value.sType = "constantValueBool";
			pParameter->value.sValue = "0";
			pCompFilter->vecpParameters.push_back(pParameter);
		}
		{
			stCompFilter::stParameter* pParameter = new stCompFilter::stParameter;
			pParameter->sName = "outputcolor";
			pParameter->sRelativeTo = "0";
			pParameter->value.sType = "constantValueFloat4";
			pParameter->value.sValue = pInputParameter->roughness.sColor;
			pCompFilter->vecpParameters.push_back(pParameter);
		}
		mapCompFilter.insert(std::make_pair("roughness", pCompFilter));
	}
	{
		stCompFilter* pCompFilter = new stCompFilter;
		pCompFilter->sFilter = "uniform";
		{
			stCompFilter::stParameter* pParameter = new stCompFilter::stParameter;
			pParameter->sName = "colorswitch";
			pParameter->sRelativeTo = "0";
			pParameter->value.sType = "constantValueBool";
			pParameter->value.sValue = "0";
			pCompFilter->vecpParameters.push_back(pParameter);
		}
		{
			stCompFilter::stParameter* pParameter = new stCompFilter::stParameter;
			pParameter->sName = "outputcolor";
			pParameter->sRelativeTo = "0";
			pParameter->value.sType = "constantValueFloat4";
			pParameter->value.sValue = pInputParameter->metallic.sColor;
			pCompFilter->vecpParameters.push_back(pParameter);
		}
		mapCompFilter.insert(std::make_pair("metallic", pCompFilter));
	}
	ProcessGraph(graphNode, pInputParameter->sName, mapCompFilter);

	std::string sOutputPath = CCString::GetFilePath(sInputJsonFilePath);
	if (!CCIO::IsExist(sOutputPath, true))
	{
		CCLOG(WARN, "Output Path doesn't exist !!!");
		return -1;
	}

	std::string sSBSFilePath;
	sSBSFilePath += sOutputPath;
	sSBSFilePath += pInputParameter->sName + ".sbs";
	doc.SaveFile(sSBSFilePath.c_str());

	if (!SBS2SBSAR(sSBSFilePath, sOutputPath))
	{
		CCLOG(WARN, "SBSCooker Failed !!!");
		return -1;
	}
	CCIO::DelFile(sSBSFilePath);
	CCIO::DelFile(sInputJsonFilePath);

	return 0;
}