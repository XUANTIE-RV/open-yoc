/*
 * silan_config.h
 */

#ifndef __SILAN_CONFIG_H__
#define __SILAN_CONFIG_H__

//#define _SUPPORT_IAP_

//#define _SUPPORT_CMD_DIRECT_

// DSP Config
//#define _SUPPORT_XOS_

// ADEV Driver User Config
//#define ADEV_USER_CONFIG

#ifdef ADEV_USER_CONFIG

#define ADEV_NOT_USE_SWAP               // DSP Don't Use 192K DRAM

#define ADEV_MAX_DMAC_CHNL      4       // DMAC Max Channle
#define ADEV_MAX_DATA_WIDTH     16      // Audio Data Width
#define ADEV_MAX_AUDIO_CHNL     2       // AUdio Channle Number
#define ADEV_MAX_MULTIPLE_48K   1       // 2(96K/88.2K)
#define ADEV_LLI_NUM            6       // DMAC LLI Max Number

#endif

#endif
