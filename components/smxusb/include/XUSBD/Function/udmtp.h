/*
* udmtp.h                                                   Version 2.52
*
* smxUSBD MTP Function Driver.
*
* Copyright (c) 2011 Micro Digital Inc.
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

#ifndef SUD_MTP_H
#define SUD_MTP_H

/* PTP Object Format Codes */
#define SUD_MTP_OBJ_FMT_UNDEF          0x3000
#define SUD_MTP_OBJ_FMT_ASSOC          0x3001
#define SUD_MTP_OBJ_FMT_SCRIPT         0x3002
#define SUD_MTP_OBJ_FMT_EXEC           0x3003
#define SUD_MTP_OBJ_FMT_TEXT           0x3004
#define SUD_MTP_OBJ_FMT_HTML           0x3005
#define SUD_MTP_OBJ_FMT_DPOF           0x3006
#define SUD_MTP_OBJ_FMT_AIFF           0x3007
#define SUD_MTP_OBJ_FMT_WAV            0x3008
#define SUD_MTP_OBJ_FMT_MP3            0x3009
#define SUD_MTP_OBJ_FMT_AVI            0x300A
#define SUD_MTP_OBJ_FMT_MPEG           0x300B
#define SUD_MTP_OBJ_FMT_ASF            0x300C
#define SUD_MTP_OBJ_FMT_IMG_UNDEF      0x3800
#define SUD_MTP_OBJ_FMT_IMG_JPEG       0x3801
#define SUD_MTP_OBJ_FMT_IMG_TIFFEP     0x3802
#define SUD_MTP_OBJ_FMT_IMG_FLHPIX     0x3803
#define SUD_MTP_OBJ_FMT_IMG_BMP        0x3804
#define SUD_MTP_OBJ_FMT_IMG_CIFF       0x3805
#define SUD_MTP_OBJ_FMT_IMG_GIF        0x3807
#define SUD_MTP_OBJ_FMT_IMG_JFIF       0x3808
#define SUD_MTP_OBJ_FMT_IMG_PCD        0x3809
#define SUD_MTP_OBJ_FMT_IMG_PICT       0x380A
#define SUD_MTP_OBJ_FMT_IMG_PNG        0x380B
#define SUD_MTP_OBJ_FMT_IMG_TIFF       0x380D
#define SUD_MTP_OBJ_FMT_IMG_TIFFIT     0x380E
#define SUD_MTP_OBJ_FMT_IMG_JP2        0x380F
#define SUD_MTP_OBJ_FMT_IMG_JPX        0x3810
/* MTP Object Format */
#define SUD_MTP_OBJ_FMT_UNDEF_FW       0xB802
#define SUD_MTP_OBJ_FMT_WIN_BMP        0xB881
#define SUD_MTP_OBJ_FMT_WBMP           0xB803
#define SUD_MTP_OBJ_FMT_JPEG_XR        0xB804
#define SUD_MTP_OBJ_FMT_UNDEF_AUDIO    0xB900
#define SUD_MTP_OBJ_FMT_WMA            0xB901
#define SUD_MTP_OBJ_FMT_OGG            0xB902
#define SUD_MTP_OBJ_FMT_AAC            0xB903
#define SUD_MTP_OBJ_FMT_AUDIBLE        0xB904
#define SUD_MTP_OBJ_FMT_FLAC           0xB906
#define SUD_MTP_OBJ_FMT_QCELP          0xB907
#define SUD_MTP_OBJ_FMT_AMR            0xB908
#define SUD_MTP_OBJ_FMT_UNDEF_VIDEO    0xB980
#define SUD_MTP_OBJ_FMT_WMV            0xB981
#define SUD_MTP_OBJ_FMT_MP4            0xB982
#define SUD_MTP_OBJ_FMT_MP2            0xB983
#define SUD_MTP_OBJ_FMT_3GP            0xB984
#define SUD_MTP_OBJ_FMT_3G2            0xB985
#define SUD_MTP_OBJ_FMT_AVCHD          0xB986
#define SUD_MTP_OBJ_FMT_ATSC_TS        0xB987
#define SUD_MTP_OBJ_FMT_DVB_TS         0xB988
#define SUD_MTP_OBJ_FMT_UNDEF_COLL     0xBA00
#define SUD_MTP_OBJ_FMT_MM_ALBUM       0xBA01
#define SUD_MTP_OBJ_FMT_IMG_ALBUM      0xBA02
#define SUD_MTP_OBJ_FMT_AUDIO_ALBUM    0xBA03
#define SUD_MTP_OBJ_FMT_VIDEO_ALBUM    0xBA04
#define SUD_MTP_OBJ_FMT_AV_PLAYLIST    0xBA05
#define SUD_MTP_OBJ_FMT_CONTACT_GRP    0xBA06
#define SUD_MTP_OBJ_FMT_MSG_FOLDER     0xBA07
#define SUD_MTP_OBJ_FMT_CHAP_PROD      0xBA08
#define SUD_MTP_OBJ_FMT_AUDIO_PLAYLIST 0xBA09
#define SUD_MTP_OBJ_FMT_VIDEO_PLAYLIST 0xBA0A
#define SUD_MTP_OBJ_FMT_MEDIACAST      0xBA0B
#define SUD_MTP_OBJ_FMT_WPL_PLAYLIST   0xBA10
#define SUD_MTP_OBJ_FMT_M3U_PLAYLIST   0xBA11
#define SUD_MTP_OBJ_FMT_MPL_PLAYLIST   0xBA12
#define SUD_MTP_OBJ_FMT_ASX_PLAYLIST   0xBA13
#define SUD_MTP_OBJ_FMT_PLS_PLAYLIST   0xBA14
#define SUD_MTP_OBJ_FMT_UNDEF_DOC      0xBA80
#define SUD_MTP_OBJ_FMT_ABS_DOC        0xBA81
#define SUD_MTP_OBJ_FMT_XML            0xBA82
#define SUD_MTP_OBJ_FMT_WORD           0xBA83
#define SUD_MTP_OBJ_FMT_MHT_HTML       0xBA84
#define SUD_MTP_OBJ_FMT_EXCEL          0xBA85
#define SUD_MTP_OBJ_FMT_PPT            0xBA86
#define SUD_MTP_OBJ_FMT_UNDEF_MSG      0xBB00
#define SUD_MTP_OBJ_FMT_ABS_MSG        0xBB01
#define SUD_MTP_OBJ_FMT_UNDEF_BM       0xBB10
#define SUD_MTP_OBJ_FMT_ABS_BM         0xBB11
#define SUD_MTP_OBJ_FMT_UNDEF_APPOINT  0xBB20
#define SUD_MTP_OBJ_FMT_ABS_APPOINT    0xBB21
#define SUD_MTP_OBJ_FMT_VCALENDAR      0xBB22
#define SUD_MTP_OBJ_FMT_UNDEF_TASK     0xBB40
#define SUD_MTP_OBJ_FMT_ABS_TASK       0xBB41
#define SUD_MTP_OBJ_FMT_ICALENDAR      0xBB42
#define SUD_MTP_OBJ_FMT_UNDEF_NOTE     0xBB60
#define SUD_MTP_OBJ_FMT_ABS_NOTE       0xBB61
#define SUD_MTP_OBJ_FMT_UNDEF_CONTACT  0xBB80
#define SUD_MTP_OBJ_FMT_ABS_CONTACT    0xBB81
#define SUD_MTP_OBJ_FMT_VCARD2         0xBB82
#define SUD_MTP_OBJ_FMT_VCARD3         0xBB83

/* PTP Event Code */
#define SUD_MTP_EVT_CANCEL_TRANSAC     0x4001
#define SUD_MTP_EVT_OBJ_ADDED          0x4002
#define SUD_MTP_EVT_OBJ_REMOVED        0x4003
#define SUD_MTP_EVT_STOR_ADDED         0x4004
#define SUD_MTP_EVT_STOR_REMOVED       0x4005
#define SUD_MTP_EVT_DEV_PROP_CHANGED   0x4006
#define SUD_MTP_EVT_OBJ_INFO_CHANGED   0x4007
#define SUD_MTP_EVT_DEV_INFO_CHANGED   0x4008
#define SUD_MTP_EVT_REQ_OBJ_TRANSFER   0x4009
#define SUD_MTP_EVT_STOR_FULL          0x400A
#define SUD_MTP_EVT_DEV_RESET          0x400B
#define SUD_MTP_EVT_STOR_INFO_CHANGED  0x400C
#define SUD_MTP_EVT_UNREPORTED_STATUS  0x400D
/* MTP Event Code */
#define SUD_MTP_EVT_OBJPROP_CHANGED    0xC801
#define SUD_MTP_EVT_OBJPROP_DESC_CHG   0xC802
#define SUD_MTP_EVT_OBJREF_CHANGED     0xC803

/* object mode, used for Open */
#define SUD_MTP_OBJ_MODE_READWRITE    0
#define SUD_MTP_OBJ_MODE_READONLY     1

/* object attribute, value of attr in SUD_MTP_FILE_INFO */
#define SUD_MTP_OBJ_ATTR_DIR          1
#define SUD_MTP_OBJ_ATTR_RDONLY       2

typedef struct {
    u16 wYear;
    u16 wMonth;
    u16 wDay;
    u16 wHour;
    u16 wMinute;
    u16 wSecond;
} SUD_MTP_DATETIME;

typedef struct {
    u8               name[260];               /* full name */
    uint             attr;                    /* bitmap for attribute */
    u32              st_size_l;               /* file size, lower 32 bit */
    u32              st_size_h;               /* file size, higher 32 bit */
    SUD_MTP_DATETIME st_ctime;                /* create time */
    SUD_MTP_DATETIME st_mtime;                /* modify time */
    SUD_MTP_DATETIME st_atime;                /* access time */
    void             *pPrivate;
} SUD_MTP_FILE_INFO;

/* File System Interface */
typedef struct {
    int (*MTPDiskMounted)(uint index);
    int (*MTPStorageNum)(void);
    void    *(*MTPOpen)(uint index, char *pFileName, uint iMode);
    int (*MTPClose)(void *pFileHandle);
    int (*MTPRead)(u8 *pRAMAddr, u32 dwSize, void *pFileHandle);
    int (*MTPWrite)(u8 *pRAMAddr, u32 dwSize, void *pFileHandle);
    int (*MTPMkDir)(uint index, char *pPathName);
    int (*MTPRmDir)(uint index, char *pPathName);
    int (*MTPDelete)(uint index, char *pFileName);
    int (*MTPTotalSize)(uint index);
    int (*MTPFreeSize)(uint index);
    int (*MTPFindFirst)(uint index, char *pFindSpec, SUD_MTP_FILE_INFO *pFileInfo);
    int (*MTPFindNext)(uint iID, SUD_MTP_FILE_INFO *pFileInfo);
    int (*MTPFindClose)(SUD_MTP_FILE_INFO *pFileInfo);
    int (*MTPGetImgProp)(uint index, char *pFileName, uint *piWidth, uint *piHeight, uint *piBits, uint *piThumbSize, uint *piThumbFormat, uint *piThumbWidth, uint *piThumbHeight);
    void    *(*MTPOpenImgThumb)(uint index, char *pFileName);
    int (*MTPReadImgThumb)(u8 *pRAMAddr, u32 dwSize, void *pHandle);
    int (*MTPCloseImgThumb)(void *pHandle);
    int (*MTPGetFormat)(uint index, char *pFileName, u16 *pwFormat);
    int (*MTPInitCapture)(u32 index, u32 format);
    int (*MTPStartCapture)(void);
    int (*MTPStopCapture)(void);
} SUD_MTP_IF;

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/* used by smxUSBD internally */
int  sud_MTPInit(void);
void sud_MTPRelease(void);

#if SUD_COMPOSITE
/* for composite device only */
void *sud_MTPGetOps(void);
void *sud_MTPGetInterface(void);
#endif

int  sud_MTPIsConnected(int port);

/* used by high level application */
void sud_MTPRegisterInterface(const SUD_MTP_IF *pObjOper);

int  sud_MTPSendEvent(u32 dwEventCode, uint iNumParameter, u32 *pdwParameter);

#ifdef __cplusplus
}
#endif

#endif  /* SUD_MTP_H */

