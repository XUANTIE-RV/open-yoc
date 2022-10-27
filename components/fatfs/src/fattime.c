/*
 * Copyright (C) 2015-2022 Alibaba Group Holding Limited
 */

#include "aos/integer.h"
#include "time.h"

DWORD get_fattime(void)
{
	DWORD res;
	time_t utime;
	struct tm *tm = NULL;

	time(&utime);
	tm = localtime(&utime);

	res =  (((DWORD)tm->tm_year + 1900 - 1980) << 25)
			| (((DWORD)tm->tm_mon + 1) << 21)
			| ((DWORD)tm->tm_mday << 16)
			| (WORD)(tm->tm_hour << 11)
			| (WORD)(tm->tm_min << 5)
			| (WORD)(tm->tm_sec >> 1);
	return res;
}
