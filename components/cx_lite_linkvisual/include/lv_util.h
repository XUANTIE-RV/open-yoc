/**
 * @file lv_util.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef LV_UTIL_H
#define LV_UTIL_H

#include "link_visual_enum.h"
#include <cx/common/type.h>
#include <tmedia_core/common/common_inc.h>

namespace lv {
/* format convert chixiao->lv*/
lv_video_format_e       MapEncodeFormat_CX2LV(cx::VencoderType type);
int                     MapVideoFrameType_CX2LV(TMMediaInfo::PictureType type);
lv_audio_format_e       MapAudioEncodeType_CX2LV(cx::AudioEncoderType type);
lv_audio_sample_bits_e  MapAudioSampleBits_CX2LV(cx::AudioSampleBits inParam);
lv_audio_channel_e      MapAudioSampleChannels_CX2LV(cx::AudioSampleChannels inParam);
lv_audio_sample_rate_e  MapAudioSampleRate_CX2LV(int inParam);

/* format convert lv->chixiao*/
cx::AudioEncoderType    MapAudioEncodeType_LV2CX(lv_audio_format_e type);
cx::AudioSampleBits     MapAudioSampleBits_LV2CX(lv_audio_sample_bits_e inParam);
cx::AudioSampleChannels MapAudioSampleChannels_LV2CX(lv_audio_channel_e inParam);
int                     MapAudioSampleRate_LV2CX(lv_audio_sample_rate_e inParam);

}

#endif /* LV_UTIL_H */