/*
 * CCDefine.h
 *
 *  Created on: 2016年8月5日
 *      Author: qiqi.huang
 */

#define MAX_WAITING_TIME 120

#define REQUEST_TIMEOUT 5
#define CONNECT_TIMEOUT 3
#define DOWNLOAD_TIMEOUT 30
#define UPLOAD_TIMEOUT 30
#define MAX_RETRY_COUNTS 3
#define MAX_RETRY_INTERVAL 120

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#ifdef UNIT_TEST
#define private public
#endif

