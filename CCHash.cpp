/*
 * CCHash.cpp
 *
 *  Created on: May 27, 2015
 *      Author: huang
 */
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/md5.h>

#include "CCDebug.h"
#include "CCUtility.h"
#include "CCHash.h"

namespace cc
{

CCHash::CCHash()
{
	// TODO Auto-generated constructor stub

}

CCHash::~CCHash()
{
	// TODO Auto-generated destructor stub
}

std::string CCHash::CCSHA1(const std::string& sData)
{
    unsigned char digest[20] = { 0, };

    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, sData.c_str(), sData.size());
    SHA1_Final(digest, &ctx);

    std::string sSHA1;
    for (int i = 0; i < 20; i++)
    {
        char az[3] = {'\0', };
        sprintf(az, "%02X", digest[i]);
        sSHA1.append(az, 2);
    }

    return sSHA1;
}

std::string CCHash::CCMD5(const std::string& sData)
{
    unsigned char digest[16] = { 0, };

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, sData.c_str(), sData.size());
    MD5_Final(digest, &ctx);

    std::string sMD5;
    for (int i = 0; i < 16; i++)
    {
        char az[3] = {'\0', };
        sprintf(az, "%02X", digest[i]);
        sMD5.append(az, 2);
    }

    return sMD5;
}


} /* namespace cc */
