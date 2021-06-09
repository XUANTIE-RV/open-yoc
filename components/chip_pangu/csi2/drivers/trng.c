/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     osr_trng.c
 * @brief
 * @version
 * @date     2020-03-03
 ******************************************************************************/
#ifndef CONFIG_TEE_CA
#include <string.h>
#include <drv/tick.h>
#include <drv/rng.h>
#include "osr_trng_ll.h"

#define DEFAULT_OSR_TRNG_TIMEOUT 1000U

typedef enum{
    STATUS_FIRST_ENTRY = 0,
    STATUS_UNFIRST_ENTRY = 0,
}first_entry_status;
/**
  \brief       Get data from the TRNG engine
  \param[out]  data  Pointer to buffer with data get from TRNG
  \param[in]   num   Number of data items,uinit in uint32
  \return      error code
*/
csi_error_t csi_rng_get_multi_word(uint32_t *data, uint32_t num)
{
    uint32_t timestart;
    csi_error_t ret = CSI_OK;
    static first_entry_status ststus = STATUS_FIRST_ENTRY;
    osr_trng_regs_t *trng_base = (osr_trng_regs_t *)OSR_RNG_BASE;

    CSI_PARAM_CHK(data, CSI_ERROR);
    if (num == 0U) {
        ret = CSI_ERROR;
    }else{
        if(ststus == STATUS_FIRST_ENTRY){
            ststus = STATUS_UNFIRST_ENTRY;
            csr_trng_disable_rbg(trng_base);
        }
        csr_trng_register_sr(trng_base);
        csr_trng_enable_rbg(trng_base);
        timestart = csi_tick_get();
        while (num) {
            if( ((csr_trng_get_status(trng_base) & OSR_TRNG_RBG_SR_DRDY) == OSR_TRNG_RBG_SR_DRDY) \
            && (csr_trng_get_trbg_fifo_tfestate(trng_base) != OSR_TRNG_RBG_FIFO_SR_TFE) ){
                data[(num-1U)] = csr_trng_get_random_data(trng_base);
                num--;
            } else {
                if ((csi_tick_get() - timestart) > DEFAULT_OSR_TRNG_TIMEOUT) {
                    ret = CSI_TIMEOUT;
                    break;
                }

            }
        }
        csr_trng_disable_rbg(trng_base);
    }
    return ret;
}

/**
  \brief       Get data from the TRNG engine
  \return      error code
*/
csi_error_t csi_rng_get_single_word(uint32_t* data)
{
    uint32_t timestart;
    first_entry_status ststus = STATUS_FIRST_ENTRY;
    csi_error_t ret = CSI_OK;
    static osr_trng_regs_t *trng_base = (osr_trng_regs_t *)OSR_RNG_BASE;
    CSI_PARAM_CHK(data, CSI_ERROR);
    if(ststus == STATUS_FIRST_ENTRY){
        ststus = STATUS_UNFIRST_ENTRY;
        csr_trng_disable_rbg(trng_base);
    }
    csr_trng_register_sr(trng_base);
    csr_trng_enable_rbg(trng_base);
    timestart = csi_tick_get();
    while(1){
         if( ((csr_trng_get_status(trng_base) & OSR_TRNG_RBG_SR_DRDY) == OSR_TRNG_RBG_SR_DRDY) \
            && (csr_trng_get_trbg_fifo_tfestate(trng_base) != OSR_TRNG_RBG_FIFO_SR_TFE) ){
            *data = csr_trng_get_random_data(trng_base);
            ret = CSI_OK;
            break;
        } else {
            if ((csi_tick_get() - timestart) > DEFAULT_OSR_TRNG_TIMEOUT) {
                ret = CSI_TIMEOUT;
                break;
            }

        }
    }
    csr_trng_disable_rbg(trng_base);
    return  ret;
}
#endif