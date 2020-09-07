/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_LOG_H__
#define __AWSS_LOG_H__

#include "iotx_log.h"
#include <aos/log.h>

// #define awss_flow(...)      log_flow("awss", __VA_ARGS__)
// #define awss_debug(...)     log_debug("awss", __VA_ARGS__)
// #define awss_info(...)      log_info("awss", __VA_ARGS__)
// #define awss_warn(...)      log_warning("awss", __VA_ARGS__)
// #define awss_err(...)       log_err("awss", __VA_ARGS__)
// #define awss_crit(...)      log_crit("awss", __VA_ARGS__)
// #define awss_emerg(...)     log_emerg("awss", __VA_ARGS__)
// #define awss_trace(...)     log_crit("awss", __VA_ARGS__)
#if (CONFIG_BLDTIME_MUTE_DBGLOG)
#define awss_flow(...)
#define awss_info(...)
#define awss_debug(...)
#else
#define awss_flow(...)      aos_log_tag("awss", AOS_LL_DEBUG, __VA_ARGS__)
#define awss_debug(...)     aos_log_tag("awss", AOS_LL_DEBUG, __VA_ARGS__)
#define awss_info(...)      aos_log_tag("awss", AOS_LL_INFO, __VA_ARGS__)
#endif
#define awss_warn(...)      aos_log_tag("awss", AOS_LL_WARN, __VA_ARGS__)
#define awss_err(...)       aos_log_tag("awss", AOS_LL_ERROR, __VA_ARGS__)
#define awss_crit(...)      aos_log_tag("awss", AOS_LL_FATAL, __VA_ARGS__)
#define awss_emerg(...)     aos_log_tag("awss", AOS_LL_FATAL, __VA_ARGS__)
#define awss_trace(...)     aos_log_tag("awss", AOS_LL_INFO, __VA_ARGS__)

#endif
