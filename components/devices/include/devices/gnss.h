#ifndef __DEV_GNSS_H__
#define __DEV_GNSS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

#define rvm_hal_gnss_open(name) rvm_hal_device_open(name)
#define rvm_hal_gnss_close(dev) rvm_hal_device_close(dev)

#define POSITION_VALID      'A'
#define POSITION_INVALID    'V'


typedef struct gnss_info {
    double   longitude;
    double   latitude;
    double   altitude;
    double   speed;
    double   orientation;
    float    hdop;
    char     longitude_indicator;
    char     latitude_indocator;
    uint8_t  star_num;
    char     positon_indicator;
    double   time_utc;
    uint32_t data;
} rvm_hal_gnss_info_t ;

int rvm_hal_gnss_getinfo(rvm_dev_t *dev, rvm_hal_gnss_info_t *info);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_gnss.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
