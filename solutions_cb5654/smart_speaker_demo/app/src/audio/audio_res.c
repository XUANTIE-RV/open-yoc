#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <media.h>

#include "audio_res.h"

typedef struct _local_audio_info {
    char *path;
}local_audio_info_t;

static local_audio_info_t audio_res[]= {
    AUDIO_RES_ARRAY
};

int local_audio_play_imp(local_audio_name_t name, int resume)
{
    char local_url[64];
    char *addr;

    addr = audio_res[name].path;

    snprintf(local_url, sizeof(local_url), "file://%s", addr);

    return aui_player_play(MEDIA_SYSTEM, local_url, resume);
}

int local_audio_play(local_audio_name_t name)
{
    if (name == LOCAL_AUDIO_HELLO) {
        return local_audio_play_imp(name, 0);
    } else {
        return local_audio_play_imp(name, 1);
    }
}
