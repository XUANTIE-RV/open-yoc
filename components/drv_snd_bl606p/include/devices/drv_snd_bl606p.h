#ifndef __DRV_SOUND_BL606P_H__
#define __DRV_SOUND_BL606P_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int audio_in_gain_list[3];
    int audio_out_gain_list[2];
} snd_bl606p_config_t;

extern void snd_card_bl606p_register(void *config);

#ifdef __cplusplus
}
#endif

#endif