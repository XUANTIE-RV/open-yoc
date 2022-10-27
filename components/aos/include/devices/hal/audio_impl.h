/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_AUDIO_IMPL_H
#define HAL_AUDIO_IMPL_H

#include <stdint.h>
#include <devices/driver.h>
#include <devices/audio.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    WRITE_CON,
    READ_CON,

    TYPE_END
};

typedef void (*audio_event)(aos_dev_t *dev, int event_id, void *priv);

typedef struct {
    driver_t drv;
    int (*config)(aos_dev_t *dev, audio_config_t *config);
    int (*get_buffer_size)(aos_dev_t *dev, int type);
    int (*send)(aos_dev_t *dev, const void *data, uint32_t size);
    int (*recv)(aos_dev_t *dev, void *data, uint32_t size);
    void (*set_event)(aos_dev_t *dev, audio_event evt_cb, void *priv);
    int (*set_gain)(aos_dev_t *dev, int l, int r);

    /* only for output */
    int (*start)(aos_dev_t *dev);
    int (*pause)(aos_dev_t *dev);
} audio_driver_t;


#ifdef __cplusplus
}
#endif

#endif
