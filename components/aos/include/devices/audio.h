#ifndef __DEV_AUDIO_H__
#define __DEV_AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

enum {
    WRITE_CON,
    READ_CON,
    TYPE_END
};

typedef struct {
    int type;
    int sample_rate;
    int sample_width;
    int sound_track;
} audio_config_t;


#define audio_open(name) device_open(name)
#define audio_open_id(name, id) device_open_id(name, id)
#define audio_close(dev) device_close(dev)

int audio_send(aos_dev_t *dev, const void *data, uint32_t size);
int audio_recv(aos_dev_t *dev, void *data, uint32_t size, unsigned int timeout_ms);
void audio_set_event(aos_dev_t *dev, void (*event)(aos_dev_t *dev, int event_id, void *priv), void *priv);
int audio_get_len(aos_dev_t *dev, int type);
int audio_config(aos_dev_t *dev, audio_config_t *config);
void audio_config_default(audio_config_t *config);
int audio_set_gain(aos_dev_t *dev, int l, int r);
int audio_start(aos_dev_t *dev);
int audio_pause(aos_dev_t *dev);


#ifdef __cplusplus
}
#endif

#endif
