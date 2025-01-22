/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

#ifndef _DEVICE_NBIOT_MODULE_IMPL_H
#define _DEVICE_NBIOT_MODULE_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/impl/net_impl.h>
#include <devices/nbiot.h>

typedef struct nbiot_driver {
    int (*start_nbiot)(rvm_dev_t *dev);
    int (*stop_nbiot)(rvm_dev_t *dev);
    int (*reset_nbiot)(rvm_dev_t *dev);
    int (*get_nbiot_status)(rvm_dev_t *dev, rvm_hal_nbiot_status_t *status);
    int (*get_imsi)(rvm_dev_t *dev, rvm_hal_nbiot_imsi_t *imsi);
    int (*get_imei)(rvm_dev_t *dev, rvm_hal_nbiot_imei_t *imei);
    int (*get_csq)(rvm_dev_t *dev, int *csq);
    int (*get_simcard_info)(rvm_dev_t *dev, rvm_hal_nbiot_iccid_t *iccid, int *insert);
    int (*get_cell_info)(rvm_dev_t *dev, rvm_hal_nbiot_cell_info_t *cellinfo);
    int (*set_nbiot_status_ind)(rvm_dev_t *dev, int status);
    int (*set_nbiot_signal_strength_ind)(rvm_dev_t *dev, int status);
} nbiot_driver_t;

#ifdef __cplusplus
}
#endif

#endif /*_DEVICE_NBIOT_MODULE_IMPL_H*/
