/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     main.c
 * @brief    CSI Source File for main
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <yoc_config.h>

#include <aos/log.h>
#include <aos/kv.h>
#include <yoc/sysinfo.h>
#include <yoc/yoc.h>

#define INIT_TASK_STACK_SIZE 8192

extern void main();
static void application_task_entry(void *arg)
{
    main();

    aos_task_exit(0);
}

//
void base_init()
{

}

int entry_c(void)
{
    /* kernel init */
    aos_init();

    /* init task */
    aos_task_t task_handle;
    aos_task_new_ext(&task_handle, "app_task", application_task_entry,
                     NULL, INIT_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI);

    /* kernel start */
    aos_start();

    return 0;
}
