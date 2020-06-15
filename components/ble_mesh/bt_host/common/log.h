/** @file
 *  @brief Bluetooth subsystem logging helpers.
 */

/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_LOG_H
#define __BT_LOG_H

#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(BT_DBG_ENABLED)
#define BT_DBG_ENABLED 1
#endif

#if defined(CONFIG_BT_DEBUG_LOG)

#if !defined(SYS_LOG_DOMAIN)
#define SYS_LOG_DOMAIN "bt"
#endif
#define SYS_LOG_LEVEL SYS_LOG_LEVEL_DEBUG

#define BT_DBG(fmt, ...) \
	if (BT_DBG_ENABLED) { \
		SYS_LOG_DBG("(%p) " fmt, k_current_get(), \
			    ##__VA_ARGS__); \
	}

#define BT_ERR(fmt, ...) SYS_LOG_ERR(fmt, ##__VA_ARGS__)
#define BT_WARN(fmt, ...) SYS_LOG_WRN(fmt, ##__VA_ARGS__)
#define BT_INFO(fmt, ...) SYS_LOG_INF(fmt, ##__VA_ARGS__)

/* Enabling debug increases stack size requirement considerably */
#define BT_STACK_DEBUG_EXTRA	300

#else
#define BT_DBG(fmt, ...)
#define BT_ERR BT_DBG
#define BT_WARN BT_DBG
#define BT_INFO BT_DBG

#define BT_STACK_DEBUG_EXTRA	0

#endif

#define BT_ASSERT(cond) if (!(cond)) { \
				BT_ERR("assert: '" #cond "' failed"); \
				k_oops(); \
			}

/* This helper is only available when BT_DEBUG is enabled */
const char *bt_hex(const void *buf, size_t len);

/* These helpers are only safe to be called from internal threads as they're
 * not multi-threading safe
 */
const char *bt_addr_str(const bt_addr_t *addr);
const char *bt_addr_le_str(const bt_addr_le_t *addr);

u8_t stringtohex(char *str, u8_t *out, u8_t count);

#ifdef __cplusplus
}
#endif

#endif /* __BT_LOG_H */

