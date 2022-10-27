/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

#ifndef _DEVICE_NBIOT_MODULE_H
#define _DEVICE_NBIOT_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/driver.h>

#define NBIOT_IMEI_LEN          15
#define NBIOT_IESI_LEN          15
#define NBIOT_ICCID_LEN         20
#define NBIOT_MCC_LEN           3
#define NBIOT_MNC_LEN           3

typedef struct {
    char imei[NBIOT_IMEI_LEN + 1];
} nbiot_imei_t;

typedef struct {
    char imsi[NBIOT_IESI_LEN + 1];
} nbiot_imsi_t;

typedef struct {
    char iccid[NBIOT_ICCID_LEN + 1];
} nbiot_iccid_t;

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
} nbiot_cell_info_t;

typedef enum {
    NBIOT_STATUS_DISCONNECTED = 0,
    NBIOT_STATUS_CONNECTED,
} nbiot_status_t;

#define hal_nbiot_open(name) device_open(name)
#define hal_nbiot_close(dev) device_close(dev)

int hal_nbiot_start(aos_dev_t *dev);
int hal_nbiot_stop(aos_dev_t *dev);

int hal_nbiot_get_status(aos_dev_t *dev, nbiot_status_t *link_status);

int hal_nbiot_get_imsi(aos_dev_t *dev, nbiot_imsi_t *imsi);
int hal_nbiot_get_imei(aos_dev_t *dev, nbiot_imei_t *imei);

int hal_nbiot_get_csq(aos_dev_t *dev, int *csq);
int hal_nbiot_get_simcard_info(aos_dev_t *dev, nbiot_iccid_t *iccid, int *insert);

int hal_nbiot_get_cell_info(aos_dev_t *dev, nbiot_cell_info_t *cellinfo);
int hal_nbiot_set_status_ind(aos_dev_t *dev, int status);
int hal_nbiot_set_signal_strength_ind(aos_dev_t *dev, int status);

#ifdef __cplusplus
}
#endif

#endif /*_DEVICE_NBIOT_MODULE_H*/
