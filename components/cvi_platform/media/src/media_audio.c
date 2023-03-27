#include "media_audio.h"
#include "cv181x_snd.h"
#include "cvi_type.h"
#include <stddef.h>

static void _Snd_DriverRegister()
{
    snd_card_register(NULL);
}

static void _Snd_DriverUnRegister()
{
    snd_card_unregister(NULL);
}


int MEDIA_AUDIO_Init()
{
    _Snd_DriverRegister();
    return 0;
}

int MEDIA_AUDIO_DeInit()
{
    _Snd_DriverUnRegister();
    return 0;
}
