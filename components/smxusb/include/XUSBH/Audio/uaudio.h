/*
* uaudio.h                                                  Version 2.55
*
* smxUSBH Audio Class Driver.
*
* Copyright (c) 2007-2016 Micro Digital Inc.
* All rights reserved. www.smxrtos.com
*
* This software is confidential and proprietary to Micro Digital Inc.
* It has been furnished under a license and may be used, copied, or
* disclosed only in accordance with the terms of that license and with
* the inclusion of this header. No title to nor ownership of this
* software is hereby transferred.
*
* Author: Yingbo Hu and XiaoYi Chen
*
* Portable to any ANSI compliant C compiler.
*
*****************************************************************************/

#ifndef SU_AUDIO_H
#define SU_AUDIO_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

#define SU_AUDIO_MUTE_PRESENT                 (0x01 << 0)
#define SU_AUDIO_VOLUME_PRESENT               (0x01 << 1)
#define SU_AUDIO_BASS_PRESENT                 (0x01 << 2)
#define SU_AUDIO_MID_PRESENT                  (0x01 << 3)
#define SU_AUDIO_TREBLE_PRESENT               (0x01 << 4)
#define SU_AUDIO_GRAPHIC_EQUALIZER_PRESENT    (0x01 << 5)
#define SU_AUDIO_AUTOMATIC_GAIN_PRESENT       (0x01 << 6)
#define SU_AUDIO_DELAY_PRESENT                (0x01 << 7)
#define SU_AUDIO_BASS_BOOST_PRESENT           (0x01 << 8)
#define SU_AUDIO_LOUDNESS_PRESENT             (0x01 << 9)

/* Format we supported */
#define SU_AUDIO_FORMAT_PCM            0x0001
#define SU_AUDIO_FORMAT_PCM8           0x0002
#define SU_AUDIO_FORMAT_IEEE_FLOAT     0x0003
#define SU_AUDIO_FORMAT_ALAW           0x0004
#define SU_AUDIO_FORMAT_MULAW          0x0005

#define SU_AUDIO_MAX_FREQ              16

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct
{
    uint iID;
    uint iFeature;
    char szName[32];
}SU_AUDIO_CHAN_INFO;

typedef struct
{
    u16  wFormatTag;                            
    u8   bChannels;                             
    u8   bBits;                                 
    uint iSamRateNum;                           
    uint iContinuous;
    u32  SamRate[SU_AUDIO_MAX_FREQ];
}SU_AUDIO_FORMAT_INFO;

int  su_AudioInit(void);
void su_AudioRelease(void);
void su_AudioRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

int  su_AudioRecGetChannelNum(uint iID);
int  su_AudioRecGetChannelInfo(uint iID, uint iChannel, SU_AUDIO_CHAN_INFO *pInfo);
int  su_AudioRecGetCurChannel(uint iID, uint *piChannel);
int  su_AudioRecGetCurVolume(uint iID, uint iChannel, int *piVolume, int *piMax, int *piMin, int *piRes);
int  su_AudioRecGetCurMute(uint iID, uint iChannel, uint *piMute);
int  su_AudioRecSelectChannel(uint iID, uint iChannel);
int  su_AudioRecSetVolume(uint iID, uint iChannel, int iVolume);
int  su_AudioRecSetMute(uint iID, uint iChannel, uint iMute);
int  su_AudioRecGetFormatNum(uint iID);
int  su_AudioRecGetFormat(uint iID, uint iIndex, SU_AUDIO_FORMAT_INFO *pFormat);

int  su_AudioRecSetFormat(uint iID, uint wFormat, u32 dwSamRate, uint iBits, uint Channel);
int  su_AudioRecOpen(uint iID);
int  su_AudioRecClose(uint iID);
int  su_AudioRecRead(uint iID, u8 *pData, uint iLen);

int  su_AudioPlaybackGetChannelNum(uint iID);
int  su_AudioPlaybackGetChannelInfo(uint iID, uint iChannel, SU_AUDIO_CHAN_INFO *pInfo);
int  su_AudioPlaybackGetCurVolume(uint iID, uint iChannel, int *piVolume, int *piMax, int *piMin, int *piRes);
int  su_AudioPlaybackGetCurMute(uint iID, uint iChannel, uint *piMute);
int  su_AudioPlaybackSetVolume(uint iID, uint iChannel, int iVolume);
int  su_AudioPlaybackSetMute(uint iID, uint iChannel, uint iMute);
int  su_AudioPlaybackGetFormatNum(uint iID);
int  su_AudioPlaybackGetFormat(uint iID, uint iIndex, SU_AUDIO_FORMAT_INFO *pFormat);

int  su_AudioPlaybackSetFormat(uint iID, uint wFormat, u32 dwSamRate, uint iBits, uint Channel);
int  su_AudioPlaybackOpen(uint iID);
int  su_AudioPlaybackClose(uint iID);
int  su_AudioPlaybackWrite(uint iID, u8 *pData, uint iLen);

BOOLEAN  su_AudioInserted(uint iID);

/* MIDI interface API */
typedef struct
{
    u8 iCN;         /* Cable Number */
    u8 iCIN;        /* Code Index Number */
    u8 iMIDI_0;
    u8 iMIDI_1;
    u8 iMIDI_2;
}SU_MIDI_EVENT;

BOOLEAN su_MIDIInserted(uint iID);
int  su_MIDIPackEvent(u8 *pData, SU_MIDI_EVENT *pEvent);
int  su_MIDIUnpackEvent(u8 *pData, SU_MIDI_EVENT *pEvent);
int  su_MIDIRead(uint iID, u8 *pData, uint iLen, uint iTimeout);
int  su_MIDIWrite(uint iID, u8 *pData, uint iLen);
int  su_MIDIOutCableNum(uint iID);
#if SB_MULTITASKING
typedef void (*MIDIDATAREADYFUNC)(uint iID, u8 *pData, uint iLen);
void su_MIDISetDataReadyNotify(MIDIDATAREADYFUNC handler);
int  su_MIDIStartReadTask(uint iID);
int  su_MIDIStopReadTask(uint iID);
BOOLEAN su_MIDIReadTaskStarted(uint iID);
#endif

#if defined(__cplusplus)
}
#endif


/*==========================================================================*/
#endif /* SU_AUDIO_H */
