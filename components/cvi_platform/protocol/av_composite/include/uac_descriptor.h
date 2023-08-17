#ifndef __UAC_DESCRIPTOR_H__
#define __UAC_DESCRIPTOR_H__

#include <stdint.h>

#ifdef CONFIG_USB_HS
#define EP_INTERVAL 0x04
#else
#define EP_INTERVAL 0x01
#endif

/* AUDIO Class Config */
#define AUDIO_FREQ 8000U

#define AUDIO_SAMPLE_FREQ(frq) (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))

/* Audio Channel Number */
#define AUDIO_CHANNEL_NUM 1

/* AudioFreq * DataSize (2 bytes) * NumChannels (Stereo: 2) */
#define AUDIO_OUT_PACKET ((uint32_t)((AUDIO_FREQ * 2 * AUDIO_CHANNEL_NUM) / 1000))
/* 16bit(2 Bytes) 双声道(Mono:2) */
#define AUDIO_IN_PACKET ((uint32_t)((AUDIO_FREQ * 2 * AUDIO_CHANNEL_NUM) / 1000))

/* AUDIO ep address*/
#define AUDIO_IN_EP  0x83
#define AUDIO_OUT_EP 0x02

#define AUDIO_FIRST_INTERFACE 2
#define AUDIO_INTERFACE_COUNT 3
#define AUDIO_STREAM_INTF_COUNT 2

uint8_t *uac_build_descriptor(uint32_t *len);
void uac_destroy_descriptor(uint8_t *desc);

#endif