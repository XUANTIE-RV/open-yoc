#ifndef __CV181X_SND_H__
#define __CV181X_SND_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void snd_card_register(void *config);
extern void snd_card_unregister(void *config);
#ifdef __cplusplus
}
#endif

#endif