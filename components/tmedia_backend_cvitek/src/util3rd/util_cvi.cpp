/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <iostream>

#include <tmedia_backend_cvitek/cvi_util/util_cvi.h>

using namespace std;

bool gCviOpenedEncodeChn[ENCODE_MAX_CHN_NUM] = {false};

PIXEL_FORMAT_E TMUtilCvi::MapPixelFormat(TMImageInfo::PixelFormat inParam)
{
    PIXEL_FORMAT_E format_out;
    switch (inParam)
    {
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P:
        format_out = PIXEL_FORMAT_YUV_PLANAR_420;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV422P:
        format_out = PIXEL_FORMAT_YUV_PLANAR_422;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12:
        format_out = PIXEL_FORMAT_NV12;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21:
        format_out = PIXEL_FORMAT_NV21;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888:
        format_out = PIXEL_FORMAT_BGR_888;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888P:
        format_out = PIXEL_FORMAT_BGR_888_PLANAR;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888:
        format_out = PIXEL_FORMAT_RGB_888;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888P:
        format_out = PIXEL_FORMAT_RGB_888_PLANAR;
        break;
    default:
        format_out = PIXEL_FORMAT_MAX;
    }
    return format_out;
}

int GetAvaliableEncodeChannel(int &chnID)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(gCviOpenedEncodeChn); i++)
    {
        if (gCviOpenedEncodeChn[i] == false)
        {
            chnID = i;
            return TMResult::TM_OK;
        }
    }
    return TMResult::TM_EBUSY;
}

int CheckVideoIsIframe(const VENC_STREAM_S *stStream, TMMediaInfo::CodecID codecID)
{
	CVI_U32 i;
	for(i = 0;i < stStream->u32PackCount;i++) {
        if(codecID == TMMediaInfo::CodecID::H264) {
            H264E_NALU_TYPE_E type = stStream->pstPack[i].DataType.enH264EType;
            if(type == H264E_NALU_ISLICE || type == H264E_NALU_SPS || type == H264E_NALU_IDRSLICE) {
                return 1;
            }
        } else if(codecID == TMMediaInfo::CodecID::H265) {
            H265E_NALU_TYPE_E type = stStream->pstPack[i].DataType.enH265EType;
			if(type == H265E_NALU_ISLICE || type == H265E_NALU_SPS || type == H265E_NALU_IDRSLICE) {
				return 1;
			}
        } else {
            cout << "wrong codec id" << endl;
            return -1;
        }
	}
	return 0;
}

int TMUtilCvi::MapAudioSampleBits(AudioSampleBits_e inParam)
{
    int outParam = -1;
    switch (inParam)
    {
    case AUDIO_SAMPLE_BITS_8BIT:
        outParam = 8;
        break;
    case AUDIO_SAMPLE_BITS_16BIT:
        outParam = 16;
        break;
    default:
        break;
    }
    return outParam;
}

aos_pcm_access_t TMUtilCvi::MapAudioPcmDataType(AudioPcmDataType_e inParam)
{
    aos_pcm_access_t outParam = AOS_PCM_ACCESS_RW_INTERLEAVED;
    switch (inParam)
    {
    case AUDIO_PCM_ACCESS_RW_INTERLEAVED:
        outParam = AOS_PCM_ACCESS_RW_INTERLEAVED;
        break;
    case AUDIO_PCM_ACCESS_RW_NONINTERLEAVED:
        outParam = AOS_PCM_ACCESS_RW_NONINTERLEAVED;
        break;
    default:
        break;
    }
    return outParam;
}