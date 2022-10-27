/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <stdio.h>
#include <sys_clk.h>
#include "app_main.h"

int main(int argc, char *argv[])
{
    board_yoc_init();
    
    printf("\r\napp start core clock %d........\r\n", soc_get_cur_cpu_freq());
    
    codec_output_init();
    codec_input_init();
    codec_loop_init();
    return 0;
}
