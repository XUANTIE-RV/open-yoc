/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 * Created by ethan.lcz@alibaba-inc.com.
 */

#ifndef _U_LD_H_
#define _U_LD_H_

typedef int (*OS_DRIVER_ENTRY)(void);

#ifdef DRV_ENTRY_DEBUG
#define TRACE_DRV_ENTRY() printk("%s\r\n", __func__)
#else
#define TRACE_DRV_ENTRY()
#endif

#if defined (__CC_ARM) || defined (__GNUC__)
#define OS_CORE_DRIVER_ATT    __attribute__((used, section(".core_driver_entry")))
#define OS_BUS_DRIVER_ATT     __attribute__((used, section(".bus_driver_entry")))
#define OS_VFS_DRIVER_ATT     __attribute__((used, section(".vfs_driver_entry")))
#define OS_EARLY_DRIVER_ATT   __attribute__((used, section(".early_driver_entry")))
#define OS_LEVEL0_DRIVER_ATT  __attribute__((used, section(".level0_driver_entry")))
#define OS_LEVEL1_DRIVER_ATT  __attribute__((used, section(".level1_driver_entry")))
#define OS_LEVEL2_DRIVER_ATT  __attribute__((used, section(".level2_driver_entry")))
#define OS_LEVEL3_DRIVER_ATT  __attribute__((used, section(".level3_driver_entry")))
#define OS_POST_DRIVER_ATT    __attribute__((used, section(".post_driver_entry")))
#define OS_POST_LOAD_LEVEL0_DRIVER_ATT    __attribute__((used, section(".post_load_level0_driver_entry")))
#define OS_POST_LOAD_LEVEL1_DRIVER_ATT    __attribute__((used, section(".post_load_level1_driver_entry")))
#define OS_PRE_APP_DRIVER_ATT __attribute__((used, section(".pre_app_driver_entry")))
#elif defined (__ICCARM__)
#pragma OS_CORE_DRIVER_ATT   = ".core_driver_entry"
#pragma OS_BUS_DRIVER_ATT    = ".bus_driver_entry"
#pragma OS_VFS_DRIVER_ATT    = ".vfs_driver_entry"
#pragma OS_EARLY_DRIVER_ATT  = ".early_driver_entry"
#pragma OS_LEVEL0_DRIVER_ATT = ".level0_driver_entry"
#pragma OS_LEVEL1_DRIVER_ATT = ".level1_driver_entry"
#pragma OS_LEVEL2_DRIVER_ATT = ".level2_driver_entry"
#pragma OS_LEVEL3_DRIVER_ATT = ".level3_driver_entry"
#pragma OS_POST_DRIVER_ATT   = ".post_driver_entry"
#pragma OS_POST_LOAD_LEVEL0_DRIVER_ATT = ".post_load_level0_driver_entry"
#pragma OS_POST_LOAD_LEVEL1_DRIVER_ATT = ".post_load_level1_driver_entry"
#pragma OS_PRE_APP_DRIVER_ATT = ".pre_app_driver_entry"
#endif

enum {
   PRIO_CORE_DRIVER = 0,
   PRIO_BUS_DRIVER = 1,
   PRIO_VFS_DRIVER,
   PRIO_EARLY_DRIVER,
   PRIO_LEVEL0_DRIVER,
   PRIO_LEVEL1_DRIVER,
   PRIO_LEVEL2_DRIVER,
   PRIO_LEVEL3_DRIVER,
   PRIO_POST_DRIVER_DRIVER,
   PRIO_POST_LOAD_LEVEL0_DRIVER,
   PRIO_POST_LOAD_LEVEL1_DRIVER,
   PRIO_MAX_DRIVER,
};

#define CORE_DRIVER_ENTRY(x) int core_drv_##x(void) { \
    TRACE_DRV_ENTRY(); \
    return x(); \
} \
OS_CORE_DRIVER_ATT OS_DRIVER_ENTRY core_##x = core_drv_##x;

#define BUS_DRIVER_ENTRY(x) int bus_drv_##x(void) { \
    TRACE_DRV_ENTRY(); \
    return x(); \
} \
OS_BUS_DRIVER_ATT OS_DRIVER_ENTRY bus_##x = bus_drv_##x;

#define VFS_DRIVER_ENTRY(x) int vfs_drv_##x(void) { \
    TRACE_DRV_ENTRY(); \
    return x(); \
} \
OS_VFS_DRIVER_ATT OS_DRIVER_ENTRY vfs_##x = vfs_drv_##x;

#define EARLY_DRIVER_ENTRY(x) int early_drv_##x(void) { \
    TRACE_DRV_ENTRY(); \
    return x(); \
} \
OS_EARLY_DRIVER_ATT OS_DRIVER_ENTRY early_##x = early_drv_##x;

#define LEVEL0_DRIVER_ENTRY(x) int level0_drv_##x(void) { \
    TRACE_DRV_ENTRY(); \
    return x(); \
} \
OS_LEVEL0_DRIVER_ATT OS_DRIVER_ENTRY level0_##x = level0_drv_##x;

#define LEVEL1_DRIVER_ENTRY(x) int level1_drv_##x(void) { \
    TRACE_DRV_ENTRY(); \
    return x(); \
} \
OS_LEVEL1_DRIVER_ATT OS_DRIVER_ENTRY level1_##x = level1_drv_##x;

#define LEVEL2_DRIVER_ENTRY(x) int level2_drv_##x(void) { \
    TRACE_DRV_ENTRY(); \
    return x(); \
} \
OS_LEVEL2_DRIVER_ATT OS_DRIVER_ENTRY level2_##x = level2_drv_##x;

#define LEVEL3_DRIVER_ENTRY(x) int level3_drv_##x(void) { \
    TRACE_DRV_ENTRY(); \
    return x(); \
} \
OS_LEVEL3_DRIVER_ATT OS_DRIVER_ENTRY level3_##x = level3_drv_##x;

#define POST_DRIVER_ENTRY(x) int post_drv_##x(void) { \
    TRACE_DRV_ENTRY(); \
    return x(); \
} \
OS_POST_DRIVER_ATT OS_DRIVER_ENTRY post_##x = post_drv_##x;

#define POST_LOAD_LEVEL0_DRIVER_ENTRY(x) int post_load_level0_drv_##x(void) { \
    TRACE_DRV_ENTRY(); \
    return x(); \
} \
OS_POST_LOAD_LEVEL0_DRIVER_ATT OS_DRIVER_ENTRY post_load_level0_##x = post_load_level0_drv_##x;

#define POST_LOAD_LEVEL1_DRIVER_ENTRY(x) int post_load_level1_drv_##x(void) { \
    TRACE_DRV_ENTRY(); \
    return x(); \
} \
OS_POST_LOAD_LEVEL1_DRIVER_ATT OS_DRIVER_ENTRY post_load_level1_##x = post_load_level1_drv_##x;

#define PRE_APP_DRIVER_ENTRY(x) int pre_app_drv_##x(void) { \
    TRACE_DRV_ENTRY(); \
    return x(); \
} \
OS_PRE_APP_DRIVER_ATT OS_DRIVER_ENTRY pre_app_##x = pre_app_drv_##x;

int _os_driver_entry_get_init_func_num(unsigned char prio);

void* _os_driver_entry_get_init_func(unsigned char prio, unsigned char index);

#endif //_U_LD_H_
