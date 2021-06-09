/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <string.h>
#include <update.h>
#include "update_diff.h"

int diff_test_fun(uint32_t oimg_addr, uint32_t oimg_size , uint32_t oimg_max_size, uint32_t diff_img_addr, uint32_t diff_img_size)
{

    SFB_LOG_I("test write\n");

    if (diff_updata_img(oimg_addr, oimg_size, oimg_max_size, diff_img_addr, diff_img_size))
        //if(diff_updata_img(0x1000bc00, 98972, 130000, 0x10031400, 29388))
    {
        SFB_LOG_I("e diff patch\n");
        return -1;
    } else {
        SFB_LOG_I("succ patch\n");
        return 0;
    }

}
