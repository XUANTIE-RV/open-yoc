/**
 * Copyright (C) 2022 Alibaba.inc, All rights reserved.
 */

#ifndef _AUDIO_HFP_H_
#define _AUDIO_HFP_H_

typedef struct audio_hfp_ops_s {
    int (*init)(void);
    int (*deinit)(void);
    void (*start)(void);
    void (*stop)(void);
    void (*vol)(int vol);
} audio_hfp_ops_t;

const audio_hfp_ops_t *bt_hfp_audio_get_interface(void);

#endif
