#ifndef __DRV_SOUND_TAS5805_H__
#define __DRV_SOUND_TAS5805_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _sound_config {
    int i2c_id;
    int i2s_id;
    uint32_t pin;
} sound_config_t;

extern void tas5805_snd_card_register(void *config);

#ifdef __cplusplus
}
#endif

#endif