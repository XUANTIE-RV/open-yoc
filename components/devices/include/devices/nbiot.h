/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

#ifndef _DEVICE_NBIOT_MODULE_H
#define _DEVICE_NBIOT_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

#define NBIOT_IMEI_LEN          15
#define NBIOT_IESI_LEN          15
#define NBIOT_ICCID_LEN         20
#define NBIOT_MCC_LEN           3
#define NBIOT_MNC_LEN           3

typedef struct {
    char imei[NBIOT_IMEI_LEN + 1];
} rvm_hal_nbiot_imei_t;

typedef struct {
    char imsi[NBIOT_IESI_LEN + 1];
} rvm_hal_nbiot_imsi_t;

typedef struct {
    char iccid[NBIOT_ICCID_LEN + 1];
} rvm_hal_nbiot_iccid_t;

typedef struct {
    char mcc[NBIOT_MCC_LEN + 1];
    char mnc[NBIOT_MNC_LEN + 1];
    int ue_category;
    unsigned short cellId;
    unsigned int lacId;
    int rsrp;
    short rsrq;
    short snr;
    unsigned char band;
    unsigned int arfcn;
    unsigned short pci;
} rvm_hal_nbiot_cell_info_t;

typedef enum {
    NBIOT_STATUS_DISCONNECTED = 0,
    NBIOT_STATUS_CONNECTED,
} rvm_hal_nbiot_status_t;

#define rvm_hal_nbiot_open(name) rvm_hal_device_open(name)
#define rvm_hal_nbiot_close(dev) rvm_hal_device_close(dev)

int rvm_hal_nbiot_start(rvm_dev_t *dev);
int rvm_hal_nbiot_stop(rvm_dev_t *dev);

int rvm_hal_nbiot_get_status(rvm_dev_t *dev, rvm_hal_nbiot_status_t *link_status);

int rvm_hal_nbiot_get_imsi(rvm_dev_t *dev, rvm_hal_nbiot_imsi_t *imsi);
int rvm_hal_nbiot_get_imei(rvm_dev_t *dev, rvm_hal_nbiot_imei_t *imei);

int rvm_hal_nbiot_get_csq(rvm_dev_t *dev, int *csq);
int rvm_hal_nbiot_get_simcard_info(rvm_dev_t *dev, rvm_hal_nbiot_iccid_t *iccid, int *insert);

int rvm_hal_nbiot_get_cell_info(rvm_dev_t *dev, rvm_hal_nbiot_cell_info_t *cellinfo);
int rvm_hal_nbiot_set_status_ind(rvm_dev_t *dev, int status);
int rvm_hal_nbiot_set_signal_strength_ind(rvm_dev_t *dev, int status);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_nbiot.h>
#endif

#ifdef __cplusplus
}
#endif

#endif /*_DEVICE_NBIOT_MODULE_H*/
