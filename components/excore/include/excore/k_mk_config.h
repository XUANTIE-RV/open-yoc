/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#ifndef K_MK_CONFIG_H
#define K_MK_CONFIG_H

#if 0
#include "belog.h"

/* config kernel print */
#define OS_PRINT_SHOW(format, ...)      be_trace(format, ## __VA_ARGS__)
#define OS_PRINT_DBG(format, ...)       be_debug("[Knl Dbg][%s %d]: "format, __func__, __LINE__, ## __VA_ARGS__)
#define OS_PRINT_ERR(format, ...)       be_error("[Knl Err][%s %d]: "format, __func__, __LINE__, ## __VA_ARGS__)
#else
/* config kernel print */
#define OS_PRINT_SHOW(format, ...)
#define OS_PRINT_DBG(format, ...)
#define OS_PRINT_ERR(format, ...)

#endif

/* Maximum number of processes plus one, should be less than 256
   when == 1: always in kernel mode
   when > 1 : kernel + user mode
*/
#ifndef CONFIG_EXCORE_PROCESS_MAX
#define CONFIG_EXCORE_PROCESS_MAX           1
#endif

/* Whether the secondary page table is supported */
#ifndef MK_CONFIG_MMU_LVL2
#define MK_CONFIG_MMU_LVL2              1
#endif

#if (MK_CONFIG_MMU_LVL2 > 0)
/* Number of level2 page tables,
   1 table takes 1KB ram, cover 1MB memory */
#ifndef MK_CONFIG_MMU_LVL2_TABLES
#define MK_CONFIG_MMU_LVL2_TABLES       256
#endif
#endif



#endif /* K_MK_CONFIG_H */

