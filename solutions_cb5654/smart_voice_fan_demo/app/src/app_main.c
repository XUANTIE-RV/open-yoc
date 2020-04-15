/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "silan_pic.h"
#include "app_main.h"
#include "app_printf.h"

#define TAG "app"

#define __REG32(addr)             (*(volatile uint32_t*)(addr))

#define CXC_MBOX(x)                 __REG32(0x42020400+x*4)
#define CXC_RAW_INT_STATUS          __REG32(0x42020000+0x800)
#define CXC_RAW_INT_CLR             __REG32(0x42020000+0x808)
#define CXC_RISC_INT_MASK           __REG32(0x42020000+0x80C)
#define CXC_RISC_INT_STATUS         __REG32(0x42020000+0x818)
#define CXC_INT_MASK                CXC_RISC_INT_MASK
#define CXC_INT_STATUS              CXC_RISC_INT_STATUS

#define VCUP_CXC_KEYWORD            6                   // CXC中断位定义
#define VCUP_MBOX_KEYWORD           CXC_MBOX(0x10/4)    // 唤醒词
#define VCUP_MBOX_RKEYWORD          CXC_MBOX(0x14/4)    // 播报

int vcup_keyword_vld = 0;
int vcup_keyword;

static void vcup_keyword_handler(uint32_t idx)
{
    int32_t sts = CXC_INT_STATUS;

    if (sts & (1 << VCUP_CXC_KEYWORD)) {
        vcup_keyword = VCUP_MBOX_KEYWORD;
        /* play audio */
        VCUP_MBOX_RKEYWORD = vcup_keyword;
        vcup_keyword_vld = 1;
        VCUP_MBOX_KEYWORD = 0;
        CXC_RAW_INT_CLR |= (1 << VCUP_CXC_KEYWORD);
    }
}


static void run_dsp(void)
{
    __REG32(0x42010300) &= ~(0x01<<1);/*DSP_BOOT_FLASH*/
    __REG32(0x42010030) |= (0x1<<3);//silan_dsp_cclk_onoff(CLK_ON);
    __REG32(0x4201002C) |= (0x1<<2);//silan_dsp_unreset();

    // 选择中断方案
    silan_pic_request(PIC_IRQID_CXC, 0, vcup_keyword_handler);
    CXC_INT_MASK &= ~(1 << VCUP_CXC_KEYWORD);
}

static int is_wakeup(void)
{
    if (vcup_keyword_vld) {
        return 1;
    }
    return 0;
}


/*
*************************************************************
函数 : get_keyword
功能 : 获取语音关键词
输入 : 
输出 : 关键词
注意 : 
*************************************************************
*/
static int get_keyword(void)
{
    return vcup_keyword;
}

/*
*************************************************************
函数 : clear_keyword
功能 : 清除关键词，可以接收下一个词
输入 : 
输出 : 
注意 : 
*************************************************************
*/
void clear_keyword(void)
{
    vcup_keyword_vld = 0;
}


int get_voice_info(int *key)
{
    /* read dsp info by cxc */
    if (is_wakeup()) {
        *key = get_keyword();
        return 1;
    }

    return 0;
}


static int wakeup_cnt = 0;

int get_wakeup_cnt(void)
{
    return wakeup_cnt;
}

/*************************************************
 * YoC入口函数
 *************************************************/
int main(void)
{
    int key = 0;
    char send_data[16];
    int data_len = 0;

    run_dsp();
    usart_init(1);

    shell_init();
    shell_reg_cmd_gpio();
    shell_reg_cmd_pwm();
    shell_reg_cmd_timer();
    shell_reg_cmd_iic();

    while(1) {
        /* read dsp info by cxc */
        if (get_voice_info(&key)) {
            clear_keyword();
            if(key == 1 || key == 2){
                //统计一级唤醒词唤醒次数
                wakeup_cnt += 1;
            }

            /* has data, send cmd by uart */
            memset(send_data, 0x00, 16);
            strcpy(send_data, "AT+CTRL=");
            data_len = strlen(send_data);
            if(key < 10){
                send_data[data_len] = key + '0';
                data_len += 1;
            }else
            {
                send_data[data_len] = key / 10 + '0';
                send_data[data_len + 1] = key % 10 + '0';
                data_len += 2;
            }

            send_data[data_len] = '\r';
            if (usart_send(send_data, data_len + 1) < 0) {
                LOGE("uart send error\r\n");
                return -1;
            }
            LOGD("\r\nusart_send_async: %s\r\n", send_data);
        }
         shell_main();
    }

    return 0;
}
