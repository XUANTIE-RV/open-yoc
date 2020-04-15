/*
* udaudio.h                                                 Version 2.51
*
* smxUSBD Audio Function Driver.
*
* Copyright (c) 2007-2011 Micro Digital Inc.
* All rights reserved. www.smxrtos.com
*
* This software is confidential and proprietary to Micro Digital Inc.
* It has been furnished under a license and may be used, copied, or
* disclosed only in accordance with the terms of that license and with
* the inclusion of this header. No title to nor ownership of this
* software is hereby transferred.
*
* Author: Yingbo Hu
*
* Portable to any ANSI compliant C compiler.
*
*****************************************************************************/

#ifndef SUD_AUDIO_H
#define SUD_AUDIO_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/* notification */
#define SUD_AUDIO_NOTIFY_ISOCDATAREADY          1 /* Received ISOC data, for audio stream */
#define SUD_AUDIO_NOTIFY_BULKDATAREADY          2 /* Received BULK data, for MIDI data */

#define SUD_AUDIO_NOTIFY_SPK_START_STOP         5
#define SUD_AUDIO_NOTIFY_MIC_START_STOP         6

/* MIDI Code Index Number, MIDI spec Table 4-1 */
#define SUD_AUDIO_MIDI_CIN_MISC                 0x0
#define SUD_AUDIO_MIDI_CIN_CABLE_EVENT          0x1
#define SUD_AUDIO_MIDI_CIN_2_SYSTEM_MSG         0x2
#define SUD_AUDIO_MIDI_CIN_3_SYSTEM_MSG         0x3
#define SUD_AUDIO_MIDI_CIN_SYSEX_START          0x4
#define SUD_AUDIO_MIDI_CIN_1_SYSTEM_MSG         0x5
#define SUD_AUDIO_MIDI_CIN_2_SYSEX_END          0x6
#define SUD_AUDIO_MIDI_CIN_3_SYSEX_END          0x7
#define SUD_AUDIO_MIDI_CIN_NOTE_OFF             0x8
#define SUD_AUDIO_MIDI_CIN_NOTE_ON              0x9
#define SUD_AUDIO_MIDI_CIN_POLY_KEY_PRESS       0xA
#define SUD_AUDIO_MIDI_CIN_CONTROL_CHANGE       0xB
#define SUD_AUDIO_MIDI_CIN_PROGRAM_CHANGE       0xC
#define SUD_AUDIO_MIDI_CIN_CHANNEL_PRESSURE     0xD
#define SUD_AUDIO_MIDI_CIN_PITCH_BEND_CHANGE    0xE
#define SUD_AUDIO_MIDI_CIN_SINGLE_BYTE          0xF

typedef void (* PAUDIOFUNC)(int port, int notification, int parameter);

typedef struct {
    uint iFormat;   /* PCM */
    uint iChannels; /* 2 or 1 */
    uint iBits;     /* 8, 16 */
    u32  dwSampleRate;/* 8000, 11025, 21050, 44100, 48000 */
    u16  wVolume[7];
    u16  wMuteEnabled[7];
} SUD_AUDIO_SETTINGS;

typedef struct {
    uint iCN;   /* Cable Number */
    uint iCIN;   /* Code Index Number */
    uint iMIDI_0;
    uint iMIDI_1;
    uint iMIDI_2;
} SUD_AUDIO_MIDI_EVENT;

/* used by smxUSBD internally */
int  sud_AudioInit(void);
void sud_AudioRelease(void);

#if SUD_COMPOSITE
/* for composite device only */
void *sud_AudioGetOps(void);
void *sud_AudioGetInterface(void);
#endif

int  sud_AudioIsConnected(int port);

/* send audio stream data */
int  sud_AudioSendAudioData(int port, u8 *pData, int iLen);
/* Get the received audio stream data */
int  sud_AudioGetAudioData(int port, u8 *pData, int iLen);
int  sud_AudioGetCurSpkSettings(int port, SUD_AUDIO_SETTINGS *pSettings);
int  sud_AudioGetCurMicSettings(int port, SUD_AUDIO_SETTINGS *pSettings);
#if SUD_AUDIO_INCLUDE_FEEDBACK
int  sud_AudioSendFeedbackData(int port, u8 *pData, int iLen);
#endif

/* send MIDI data */
int  sud_AudioSendMIDIData(int port, u8 *pData, int iLen);
/* Get the received MIDI data */
int  sud_AudioGetMIDIData(int port, u8 *pData, int iLen);
/* Pack MIDI event to a MIDI USB packet */
int  sud_AudioPackMIDIEvent(int port, u8 *pData, SUD_AUDIO_MIDI_EVENT *pEvent);
/* Unpack MIDI USB packet to a MIDI event */
int  sud_AudioUnpackMIDIEvent(int port, u8 *pData, SUD_AUDIO_MIDI_EVENT *pEvent);
/* register callback function for received INT and BULK data */
void sud_AudioRegisterNotify(int port, PAUDIOFUNC handler);

#ifdef __cplusplus
}
#endif

#endif  /* SUD_AUDIO_H */

