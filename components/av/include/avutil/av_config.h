/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AV_CONFIG_H__
#define __AV_CONFIG_H__

// fft config
#ifndef CONFIG_FFTXER_SPEEX
#define CONFIG_FFTXER_SPEEX           (0)
#endif

#ifndef CONFIG_FFTXER_IPC
#define CONFIG_FFTXER_IPC             (0)
#endif

// resample config
#ifndef CONFIG_RESAMPLER_SILAN
#define CONFIG_RESAMPLER_SILAN        (0)
#endif

#ifndef CONFIG_RESAMPLER_SPEEX
#define CONFIG_RESAMPLER_SPEEX        (1)
#endif

#ifndef CONFIG_RESAMPLER_IPC
#define CONFIG_RESAMPLER_IPC          (0)
#endif

// sona audio affector config
#ifndef CONFIG_AEFXER_SONA
#define CONFIG_AEFXER_SONA            (1)
#endif

#ifndef CONFIG_AEFXER_IPC
#define CONFIG_AEFXER_IPC             (0)
#endif

// atempo speed play config
#ifndef CONFIG_ATEMPOER_SONIC
#define CONFIG_ATEMPOER_SONIC         (1)
#endif

#ifndef CONFIG_ATEMPOER_IPC
#define CONFIG_ATEMPOER_IPC           (0)
#endif

// audio decoder config
#ifndef CONFIG_DECODER_PCM
#define CONFIG_DECODER_PCM            (1)
#endif

#ifndef CONFIG_DECODER_PVMP3
#define CONFIG_DECODER_PVMP3          (1)
#endif

#ifndef CONFIG_DECODER_FLAC
#define CONFIG_DECODER_FLAC           (0)
#endif

#ifndef CONFIG_DECODER_ADPCM_MS
#define CONFIG_DECODER_ADPCM_MS       (0)
#endif

#ifndef CONFIG_DECODER_AMRNB
#define CONFIG_DECODER_AMRNB          (0)
#endif

#ifndef CONFIG_DECODER_AMRWB
#define CONFIG_DECODER_AMRWB          (0)
#endif

#ifndef CONFIG_DECODER_OPUS
#define CONFIG_DECODER_OPUS           (0)
#endif

#ifndef CONFIG_DECODER_SPEEX
#define CONFIG_DECODER_SPEEX          (0)
#endif

#ifndef CONFIG_DECODER_ALAW
#define CONFIG_DECODER_ALAW           (0)
#endif

#ifndef CONFIG_DECODER_MULAW
#define CONFIG_DECODER_MULAW          (0)
#endif

#ifndef CONFIG_DECODER_IPC
#define CONFIG_DECODER_IPC            (0)
#endif

// ao config
#ifndef CONFIG_AO_MIXER_SUPPORT
#define CONFIG_AO_MIXER_SUPPORT       (1)
#endif

#ifndef CONFIG_AV_ERRNO_DEBUG
#define CONFIG_AV_ERRNO_DEBUG         (0)
#endif

#ifndef CONFIG_AV_MP4_IDX_OPT
#define CONFIG_AV_MP4_IDX_OPT         (1)
#endif

// player config
#ifndef CONFIG_PLAYER_TASK_STACK_SIZE
#define CONFIG_PLAYER_TASK_STACK_SIZE (98304) ///< 96 * 1024
#endif

#endif /* __AV_CONFIG_H__ */

