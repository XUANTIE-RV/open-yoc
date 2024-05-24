/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_trng.c
 * @brief
 * @version
 * @date     27. April 2020
 ******************************************************************************/

#include <drv/rng.h>
#include <drv/irq.h>
#include <drv/tick.h>
#include <string.h>

/**
  \brief       Get data from the TRNG engine
  \param[out]  data  Pointer to buffer with data get from TRNG
  \param[in]   num   Number of data items,uinit in uint32
  \return      error code
*/
csi_error_t csi_rng_get_multi_word(uint32_t *data, uint32_t num)
{
    csi_error_t ret = CSI_OK;

    CSI_PARAM_CHK(data, CSI_ERROR);
    if (num == 0U) {
        ret = CSI_ERROR;
    }else{
        int i = 0;
        while (num) {
            ///< TODO：使能trng 模块

            ///< TODO：等待trng 模块有效  且未超时，超时设置ret并退出循环
            
            ///< TODO：获取trng值到data[i]
            i++;
            num--;
        }
    }
    return ret;
}

/**
  \brief       Get data from the TRNG engine
  \return      error code
*/
csi_error_t csi_rng_get_single_word(uint32_t* data)
{
    csi_error_t ret = CSI_OK;
    CSI_PARAM_CHK(data, CSI_ERROR);

    ///< TODO：使能trng 模块

    ///< TODO：等待trng 模块有效  且未超时，超时设置ret并退出循环

    ///< TODO：获取trng值到*data 

    return  ret;
}
