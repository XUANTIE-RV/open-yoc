/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef UTIL_CVI_H
#define UTIL_CVI_H

#include <tmedia_core/common/common_inc.h>
#include <cvi_common.h>
#include <cvi_defines.h>
#include <cvi_vpss.h>
#include "cvi_venc.h"
#include <alsa/pcm.h>

#ifdef CONFIG_TMEDIA_CVI_LOG
#define TMEDIA_PRINTF(fmt,...) printf(fmt,##__VA_ARGS__)
#else
#define TMEDIA_PRINTF(fmt,...)
#endif

#define ENCODE_MAX_CHN_NUM 8

extern bool gCviOpenedEncodeChn[ENCODE_MAX_CHN_NUM];
#define UTIL_CVI_CHECK_RET(actual)                                   \
	do {																		 \
		if ((actual) != 0) {													 \
            cout << __func__ << __LINE__ << "ret:" << actual << endl; \
			return;																\
		}																		 \
	} while (0)

#define UTIL_CVI_CHECK_RET_WITH_VALUE(actual)                                   \
	do {																		 \
		if ((actual) != 0) {													 \
			cout << __func__ << __LINE__ << "ret:" << actual << endl; \
			return actual;														 \
		}																		 \
	} while (0)

typedef enum {
	VPSS_GROUP_ID = 100,
	VPSS_CHANNEL_ID,
	VPSS_DEVICE_ID,
	VPSS_ONLINE_MODE_ID,
} cvi_channel_attr_t;


typedef struct {
	int deviceID;
	int channelID;
	VIDEO_FRAME_INFO_S pstFrameInfo;
} CviNativeFrameCtx;

class TMUtilCvi
{
public:
    static PIXEL_FORMAT_E MapPixelFormat(TMImageInfo::PixelFormat inParam);
	static int MapAudioSampleBits(AudioSampleBits_e inParam);  //tmedia to minialsa
	static aos_pcm_access_t MapAudioPcmDataType(AudioPcmDataType_e inParam);
};

int GetAvaliableEncodeChannel(int &chnID);
int CheckVideoIsIframe(const VENC_STREAM_S *stStream, TMMediaInfo::CodecID codecID);
#endif  // UTIL_CVI_H
