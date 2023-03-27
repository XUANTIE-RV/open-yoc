#ifndef __UVC_FUNC_H__
#define __UVC_FUNC_H__

#include <stdio.h>
#include "cvi_param.h"
#include "cvi_comm_venc.h"


//#define MEDIABUG_PRINTF(fmt,...) printf(fmt,##__VA_ARGS__)
#define MEDIABUG_PRINTF(fmt,...)

#define MEDIA_CHECK_RET(actual, fmt, arg...)                                   \
	do {																		 \
		if ((actual) != 0) {													 \
			MEDIABUG_PRINTF("[%d]:%s() \n" fmt, __LINE__, __func__, ## arg); \
			return -1;																\
		}																		 \
	} while (0)

int MEDIA_UVC_Init();
int MEDIA_UVC_DEInit();

#endif
