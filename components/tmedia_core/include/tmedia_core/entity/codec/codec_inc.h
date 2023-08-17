/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_CODEC_INC_H
#define TM_CODEC_INC_H

#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <tmedia_core/entity/codec/codec.h>
#include <tmedia_core/entity/codec/codec_factory.h>
#include <tmedia_core/entity/codec/codec_params.h>

#include <tmedia_core/entity/codec/decoder.h>
#include <tmedia_core/entity/codec/encoder.h>

#include <tmedia_core/entity/codec/h264_decoder.h>
#include <tmedia_core/entity/codec/h264_encoder.h>
#include <tmedia_core/entity/codec/h265_encoder.h>
#include <tmedia_core/entity/codec/h265_decoder.h>
#include <tmedia_core/entity/codec/g711_encoder.h>
#include <tmedia_core/entity/codec/g711_decoder.h>
#include <tmedia_core/entity/codec/jpeg_decoder.h>
#include <tmedia_core/entity/codec/jpeg_encoder.h>
#include <tmedia_core/entity/codec/vp9_decoder.h>

#endif  /* TM_CODEC_INC_H */
