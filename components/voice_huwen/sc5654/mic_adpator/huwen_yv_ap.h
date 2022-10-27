#ifndef __YV_AP__
#define __YV_AP__

#include <yoc/mic.h>

//init
int huwen_yv_init(mic_event_t cb, mic_t *mic);
int huwen_yv_kws_enable(int flag);
int huwen_yv_pcm_enable(int flag);
int huwen_yv_wake_trigger(int flag);

//debug
void huwen_yv_debug_hook(mic_event_t hook, int dbg_level);
#endif
