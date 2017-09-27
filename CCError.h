/*
 * CCError.h
 *
 *  Created on: 2016年8月22日
 *      Author: qiqi.huang
 */

#ifndef CCERROR_H_
#define CCERROR_H_

#include <string>
#include <map>

namespace cc {

enum CCErrno {
	NO_ERR              = 200, //OK
	ERR_NULL_POINTER    = 11,

	ERR_FILE_ACCESS    = 60, //System: can't access the file
	ERR_DIR_ACCESS     = 61, //System: can't access the directory

	ERR_FILE_READ      = 70, //System: read file failed
	ERR_FILE_WRITE     = 71, //System: write file failed

	ERR_JSON_DESERIALIZE        = 80, //SDK: deserialize json failed
	ERR_JSON_SERIALIZE          = 81, //SDK: serialize json failed

	ERR_UPLOAD_DISALLOW    = 100, //SDK: server disallowed
	ERR_UPLOAD_AUTH        = 101, //SDK: upload auth failed
	ERR_UPLOAD_TRANSFER    = 102, //SDK: upload transfer failed

	ERR_PARAM            = 401, //System: invalid param
	ERR_NO_LOGIN         = 420, //System: no login
	ERR_SYSTEM           = 500, //System: system error
	ERR_SIGN             = 10500, //System: invalid sign
};

typedef unsigned int Errno;

} // namespace

#endif /* CCERROR_H_ */
