#ifndef DISK_SD_H
#define DISK_SD_H

#include "cvi_type.h"

typedef enum {
    STORAGE_NOEXIT,
    STORAGE_NORMAL,
    STORAGE_UNFORMAT,
    STORAGE_FORMAT,
}STORAGE_STATUS_E;

STORAGE_STATUS_E DISK_SdGetStatus(CVI_BOOL isCheck);
CVI_S32 DISK_SdFormat();
CVI_S32 DISK_SdGetCapacity(CVI_DOUBLE *Total,CVI_DOUBLE *Free);
int8_t app_sd_detect_check(void);
int sd_hotplugRegisterCb(int (*cb)(int));


#endif