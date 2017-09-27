/*
 * CCHash.h
 *
 *  Created on: Feb 27, 2017
 *      Author: huang
 */

#ifndef CCHASH_H_
#define CCHASH_H_

#include <string>

namespace cc
{

class CCHash
{
public:
	CCHash();
	virtual ~CCHash();

    static std::string CCSHA1(const std::string& sData);

    static std::string CCMD5(const std::string& sData);

};

} /* namespace cc */

#endif /* CCHASH_H_ */
