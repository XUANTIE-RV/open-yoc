#ifndef __APP_INIT_H
#define __APP_INIT_H

#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif

void board_yoc_init();
void speaker_init();
void speaker_uninit();

#ifdef __cplusplus
}
#endif

#endif
