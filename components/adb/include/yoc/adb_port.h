#ifndef ADB_PORT_H
#define ADB_PORT_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADB_CHANNEL_EVENT_READ  (1UL << 0)
#define ADB_CHANNEL_EVENT_WRITE (1UL << 1)
#define ADB_CHANNEL_OVERFLOW    (1UL << 2)

typedef void (*channel_event_t)(int event_id, void *priv);

typedef struct {
    void *(*init)(void *config, int port);
    void (*deinit)(void *hdl);
    int (*send)(void *hdl, const char *data, int size); /* send success return 0*/
    int (*set_event)(void *hdl, channel_event_t evt_cb, void *priv);
    int (*recv)(void *hdl, const char *data, int size, int timeout); /* return real recv size */
} adb_channel_t;

#ifdef __cplusplus
}
#endif

#endif