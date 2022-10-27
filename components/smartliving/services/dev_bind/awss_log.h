/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_LOG_H__
#define __AWSS_LOG_H__

#include "iotx_log.h"
#include <ulog/ulog.h>

#if 0
#define awss_flow(...)      log_flow("awss", __VA_ARGS__)
#define awss_debug(...)     log_debug("awss", __VA_ARGS__)
#define awss_info(...)      log_info("awss", __VA_ARGS__)
#define awss_warn(...)      log_warning("awss", __VA_ARGS__)
#define awss_err(...)       log_err("awss", __VA_ARGS__)
#define awss_crit(...)      log_crit("awss", __VA_ARGS__)
#define awss_emerg(...)     log_emerg("awss", __VA_ARGS__)
#define awss_trace(...)     log_crit("awss", __VA_ARGS__)
#endif


#if (CONFIG_BLDTIME_MUTE_DBGLOG)
#define awss_flow(...)
#define awss_info(...)
#define awss_debug(...)
#else
#define awss_flow(...)      ulog(AOS_LL_DEBUG, "awss", ULOG_TAG, __VA_ARGS__)
#define awss_debug(...)     ulog(AOS_LL_DEBUG, "awss", ULOG_TAG, __VA_ARGS__)
#define awss_info(...)      ulog(AOS_LL_INFO, "awss", ULOG_TAG, __VA_ARGS__)
#endif
#define awss_warn(...)      ulog(AOS_LL_WARN, "awss", ULOG_TAG, __VA_ARGS__)
#define awss_err(...)       ulog(AOS_LL_ERROR, "awss", ULOG_TAG, __VA_ARGS__)
#define awss_crit(...)      ulog(AOS_LL_FATAL, "awss", ULOG_TAG, __VA_ARGS__)
#define awss_emerg(...)     ulog(AOS_LL_FATAL, "awss", ULOG_TAG, __VA_ARGS__)
#define awss_trace(...)     ulog(AOS_LL_INFO, "awss", ULOG_TAG, __VA_ARGS__)


#endif
