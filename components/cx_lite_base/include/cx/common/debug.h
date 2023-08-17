/**
 * @file debug.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_COMMON_DEBUG_H
#define CX_COMMON_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <cx/common/log.h>

#ifndef CXCHECK_PARAM
#define CXCHECK_PARAM(x, ret) \
	do { \
		if (!(x)) { \
			CX_LOGW(TAG, "param check failed %s, val=%d.\n", __FUNCTION__, x); \
			return ret; \
		}\
	} while (0)
#endif

#ifndef CXCHECK_RET_WITH_GOTO
#define CXCHECK_RET_WITH_GOTO(x, label) \
	do { \
		if (!(x)) { \
			CX_LOGE(TAG, "check ret fail %s, ret=%d\n", __FUNCTION__, x); \
			goto label; \
		}\
	} while (0)
#endif

#ifndef CXCHECK_RET_WITH_RET
#define CXCHECK_RET_WITH_RET(x, ret) \
	do { \
		if (!(x)) { \
			CX_LOGE(TAG, "check ret fail %s, ret=%d\n", __FUNCTION__, x); \
			return ret; \
		}\
	} while (0)
#endif


/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CX_COMMON_DEBUG_H */

