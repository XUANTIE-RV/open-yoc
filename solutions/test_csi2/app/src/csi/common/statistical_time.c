/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <drv/timer.h>


static csi_timer_t timer_hd;
static volatile uint32_t cell_count = 0;

static void ttimer_callback(csi_timer_t *timer, void *arg)
{
    (*(uint32_t *)arg)++; 
}

/**
 * description: 开启一个timer，并配置每隔一定时间产生一个中断
 * param {in} timer_idx: 指定初始化哪一个timer
 * param {in} time_cell: 指定每间隔多少毫秒产生一个中断
 * return 0: 成功, other: 失败
 */
int ttimer_start(uint8_t timer_idx, uint32_t time_cell)
{
    csi_error_t ret;

    ret = csi_timer_init(&timer_hd, timer_idx);
    if (ret != CSI_OK) {
        return -1;
    }

    ret = csi_timer_attach_callback(&timer_hd, ttimer_callback, (void *)&cell_count);
    if (ret != CSI_OK) {
        return -1;
    }

    cell_count = 0;

    ret = csi_timer_start(&timer_hd, time_cell*1000);
    if (ret != CSI_OK) {
        return -1;
    }

    return 0;
}


void ttimer_stop(void)
{
    csi_timer_stop(&timer_hd);
    csi_timer_detach_callback(&timer_hd);
    csi_timer_uninit(&timer_hd);

}

/**
 * description: 获取timer运行器件共产生了多少次中断
 * return 产生的中断次数
 */
uint32_t ttimer_get_cell_count(void)
{
    return cell_count;
}