#ifndef __DEV_GNSS_H__
#define __DEV_GNSS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <devices/driver.h>

#define gnss_open(name) device_open(name)
#define gnss_open_id(name, id) device_open_id(name, id)
#define gnss_close(dev) device_close(dev)

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
} gnss_info_t ;

int gnss_getinfo(aos_dev_t *dev, gnss_info_t *info);

#ifdef __cplusplus
}
#endif

#endif
