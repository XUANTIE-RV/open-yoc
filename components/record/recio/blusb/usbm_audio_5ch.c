/**
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include <stdint.h>
#include <usbd_core.h>
#include <usbd_audio.h>

#define IN_CHANNEL_NUM 5

#define USBD_VID           0xffff
#define USBD_PID           0xffff
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#ifdef CONFIG_USB_HS
#define EP_INTERVAL 0x04
#else
#define EP_INTERVAL 0x01
#endif

#define AUDIO_IN_EP 0x81

/* AUDIO Class Config */
/* FIXME：采样率从16K调整到32K，临时解决录音软件无法读完设备端缓冲的问题 */
#define AUDIO_FREQ 32000U

#if IN_CHANNEL_NUM == 1
#define INPUT_CTRL      0x03
#define INPUT_CH_ENABLE 0x0001
#elif IN_CHANNEL_NUM == 2
#define INPUT_CTRL      0x03, 0x03
#define INPUT_CH_ENABLE 0x0003
#elif IN_CHANNEL_NUM == 3
#define INPUT_CTRL      0x03, 0x03, 0x03
#define INPUT_CH_ENABLE 0x0007
#elif IN_CHANNEL_NUM == 4
#define INPUT_CTRL      0x03, 0x03, 0x03, 0x03
#define INPUT_CH_ENABLE 0x000f
#elif IN_CHANNEL_NUM == 5
#define INPUT_CTRL      0x03, 0x03, 0x03, 0x03, 0x03
#define INPUT_CH_ENABLE 0x001f
#elif IN_CHANNEL_NUM == 6
#define INPUT_CTRL      0x03, 0x03, 0x03, 0x03, 0x03, 0x03
#define INPUT_CH_ENABLE 0x003F
#elif IN_CHANNEL_NUM == 7
#define INPUT_CTRL      0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
#define INPUT_CH_ENABLE 0x007f
#elif IN_CHANNEL_NUM == 8
#define INPUT_CTRL      0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
#define INPUT_CH_ENABLE 0x00ff
#endif

/* AudioFreq * DataSize (2 bytes) * NumChannels (Stereo: 1) */
/* 16bit(2 Bytes) 单声道(Mono:1) */
#define AUDIO_IN_PACKET ((uint32_t)((AUDIO_FREQ * 2 * IN_CHANNEL_NUM) / 1000))

#define USB_AUDIO_CONFIG_DESC_SIZ                                                                                      \
    (unsigned long)(9 + AUDIO_AC_DESCRIPTOR_INIT_LEN(1) + AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC                          \
                    + AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(1, IN_CHANNEL_NUM) + AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC      \
                    + AUDIO_AS_DESCRIPTOR_INIT_LEN(1))

#define AUDIO_AC_SIZ                                                                                                   \
    (AUDIO_SIZEOF_AC_HEADER_DESC(1) + AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC                                              \
     + AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(1, IN_CHANNEL_NUM) + AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC)

const uint8_t audio_descriptor_5ch[]
    = { USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0001, 0x01),
        USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
        AUDIO_AC_DESCRIPTOR_INIT(0x00, 0x02, AUDIO_AC_SIZ, 0x00, 0x01),
        AUDIO_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x01, AUDIO_INTERM_MIC, IN_CHANNEL_NUM, INPUT_CH_ENABLE),
        AUDIO_AC_FEATURE_UNIT_DESCRIPTOR_INIT(0x02, 0x01, 0x01, INPUT_CTRL),
        AUDIO_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x03, AUDIO_TERMINAL_STREAMING, 0x02),
        AUDIO_AS_DESCRIPTOR_INIT(0x01, 0x03, IN_CHANNEL_NUM, 0x81, AUDIO_IN_PACKET, EP_INTERVAL,
                                 AUDIO_SAMPLE_FREQ_3B(AUDIO_FREQ)),
        ///////////////////////////////////////
        /// string0 descriptor
        ///////////////////////////////////////
        USB_LANGID_INIT(USBD_LANGID_STRING),
        ///////////////////////////////////////
        /// string1 descriptor
        ///////////////////////////////////////
        0x12,                       /* bLength */
        USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
        'M',
        0x00, /* wcChar0 */
        'I',
        0x00, /* wcChar1 */
        'C',
        0x00, /* wcChar2 */
        '0',
        0x00, /* wcChar3 */
        (0x30 + IN_CHANNEL_NUM),
        0x00, /* wcChar4 */
        'S',
        0x00, /* wcChar5 */
        '0',
        0x00, /* wcChar6 */
        '0',
        0x00, /* wcChar7 */
        ///////////////////////////////////////
        /// string2 descriptor
        ///////////////////////////////////////
        0x28,                       /* bLength */
        USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
        'B',
        0x00, /* wcChar0 */
        'L',
        0x00, /* wcChar1 */
        ' ',
        0x00, /* wcChar2 */
        'M',
        0x00, /* wcChar3 */
        'I',
        0x00, /* wcChar4 */
        'C',
        0x00, /* wcChar5 */
        '0',
        0x00, /* wcChar6 */
        (0x30 + IN_CHANNEL_NUM),
        0x00, /* wcChar7 */
        'S',
        0x00, /* wcChar8 */
        'P',
        0x00, /* wcChar9 */
        'E',
        0x00, /* wcChar10 */
        'A',
        0x00, /* wcChar11 */
        'K',
        0x00, /* wcChar12 */
        '0',
        0x00, /* wcChar13 */
        (0x30 + 0),
        0x00, /* wcChar14 */
        'D',
        0x00, /* wcChar15 */
        'E',
        0x00, /* wcChar16 */
        'M',
        0x00, /* wcChar17 */
        'O',
        0x00, /* wcChar18 */
        ///////////////////////////////////////
        /// string3 descriptor
        ///////////////////////////////////////
        0x16,                       /* bLength */
        USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
        '2',
        0x00, /* wcChar0 */
        '0',
        0x00, /* wcChar1 */
        '2',
        0x00, /* wcChar2 */
        '2',
        0x00, /* wcChar3 */
        '0',
        0x00, /* wcChar4 */
        '4',
        0x00, /* wcChar5 */
        '2',
        0x00, /* wcChar6 */
        '1',
        0x00, /* wcChar7 */
        '0',
        0x00, /* wcChar8 */
        '0',
        (0x30 + AUDIO_FREQ / 8000), 0x00, /* wcChar9 */
#ifdef CONFIG_USB_HS
        ///////////////////////////////////////
        /// device qualifier descriptor
        ///////////////////////////////////////
        0x0a,
        USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
        0x00,
        0x02,
        0x00,
        0x00,
        0x00,
        0x40,
        0x01,
        0x00,
#endif
        0x00 };
