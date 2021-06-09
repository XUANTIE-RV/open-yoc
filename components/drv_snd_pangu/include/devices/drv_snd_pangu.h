#ifndef __DRV_SOUND_PANGU_H__
#define __DRV_SOUND_PANGU_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EMPTY_INPUT_CHANNEL     -1

#define PANGU_MAX_CHANNELS      8

typedef struct {
    int *id_list;
    int id_num;
    int dac_db_max;
    int dac_db_min;
} snd_pangu_config_t;

extern void snd_card_register(void *config);

extern void snd_card_ap_register(int vol_range);
extern void snd_card_cp_register(int vol_range);

extern void snd_card_pangu_register(void *config);
#ifdef __cplusplus
}
#endif

#endif