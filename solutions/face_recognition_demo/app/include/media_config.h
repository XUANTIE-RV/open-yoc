/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef APP_MEDIA_CONFIG_H
#define APP_MEDIA_CONFIG_H

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef CONFIG_RGBIR_SENSOR_SWITCH
int MediaVideoRGBInit();
int MediaVideoRGBDeInit();
int MediaVideoIRInit();
int MediaVideoIRDeInit();
#define SENSOR_TOGGLE_LOCK(lock)        pthread_mutex_lock(lock)
#define SENSOR_TOGGLE_UNLOCK(lock)      pthread_mutex_unlock(lock)
#else
int MediaVideoInit();
#define SENSOR_TOGGLE_LOCK(lock)
#define SENSOR_TOGGLE_UNLOCK(lock)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* APP_MEDIA_CONFIG_H */