/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name: isp_debug.h
 * Description:
 *
 */

#ifndef _ISP_DEBUG_H_
#define _ISP_DEBUG_H_

#include <stdio.h>
#include "isp_comm_inc.h"
#ifndef ARCH_RTOS_MARS
#include "cvi_debug.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef ARCH_RTOS_MARS
#define LOG_EMERG	0	/* system is unusable */
#define LOG_ALERT	1	/* action must be taken immediately */
#define LOG_CRIT		2	/* critical conditions */
#define LOG_ERR		3	/* error conditions */
#define LOG_WARNING	4	/* warning conditions */
#define LOG_NOTICE	5	/* normal but significant condition */
#define LOG_INFO		6	/* informational */
#define LOG_DEBUG	7	/* debug-level messages */
#endif

extern int gDebugLevel;
extern int gExportToStdout;

void CVI_DEBUG_SetDebugLevel(int level);
CVI_S32 isp_dbg_dumpFrameRawInfoToBuffer(VI_PIPE ViPipe, CVI_U8 *buf, CVI_S32 buf_size);

//-----------------------------------------------------------------------------
// isp_debug.h
//-----------------------------------------------------------------------------
static const char ISP_DEBUG_LEVEL_SYMBOL[] = {'e', 'A', 'C', 'E', 'W', 'N', 'I', 'D'};
#ifdef ARCH_RTOS_MARS
#define ISP_DEBUG(level, fmt, ...) do { \
	if (level <= 5) { \
		printf("#[%c] %s::%d : " fmt, \
			ISP_DEBUG_LEVEL_SYMBOL[level], __func__, __LINE__, ##__VA_ARGS__); \
	} \
} while (0)
#else
#define ISP_DEBUG(level, fmt, ...) do { \
	if (level <= gDebugLevel) { \
		if (level <= LOG_NOTICE) { \
			CVI_TRACE(level, CVI_ID_ISP, "[%c] %s::%d : " fmt, \
				ISP_DEBUG_LEVEL_SYMBOL[level], __func__, __LINE__, ##__VA_ARGS__); \
		} else { \
			CVI_TRACE(level, CVI_ID_ISP, "%s : " fmt, __func__, ##__VA_ARGS__); \
		} \
		if (gExportToStdout) { \
			printf("[%c] %s::%d : " fmt, \
				ISP_DEBUG_LEVEL_SYMBOL[level], __func__, __LINE__, ##__VA_ARGS__); \
		} \
	} \
} while (0)
#endif

#define ISP_LOG_EMERG(fmt, ...)		ISP_DEBUG(LOG_EMERG, fmt, ##__VA_ARGS__)
#define ISP_LOG_ALERT(fmt, ...)		ISP_DEBUG(LOG_ALERT, fmt, ##__VA_ARGS__)
#define ISP_LOG_CRIT(fmt, ...)		ISP_DEBUG(LOG_CRIT, fmt, ##__VA_ARGS__)

#ifndef IGNORE_LOG_ERR
#define ISP_LOG_ERR(fmt, ...)		ISP_DEBUG(LOG_ERR, fmt, ##__VA_ARGS__)
#else
#define ISP_LOG_ERR(fmt, ...)
#endif // IGNORE_LOG_ERR

#ifndef IGNORE_LOG_WARNING
#define ISP_LOG_WARNING(fmt, ...)	ISP_DEBUG(LOG_WARNING, fmt, ##__VA_ARGS__)
#else
#define ISP_LOG_WARNING(fmt, ...)
#endif // IGNORE_LOG_WARNING

#ifndef IGNORE_LOG_NOTICE
#define ISP_LOG_NOTICE(fmt, ...)	ISP_DEBUG(LOG_NOTICE, fmt, ##__VA_ARGS__)
#else
#define ISP_LOG_NOTICE(fmt, ...)
#endif // IGNORE_LOG_NOTICE

#ifndef IGNORE_LOG_INFO
#define ISP_LOG_INFO(fmt, ...)		ISP_DEBUG(LOG_INFO, fmt, ##__VA_ARGS__)
#else
#define ISP_LOG_INFO(fmt, ...)
#endif // IGNORE_LOG_INFO

#ifndef IGNORE_LOG_DEBUG
#define ISP_LOG_DEBUG(fmt, ...)		ISP_DEBUG(LOG_DEBUG, fmt, ##__VA_ARGS__)
#else
#define ISP_LOG_DEBUG(fmt, ...)
#endif // IGNORE_LOG_DEBUG

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif //_ISP_DEBUG_H_
