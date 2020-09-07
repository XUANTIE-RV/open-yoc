/*
* udcfg.h                                                   Version 2.57
*
* smxUSBD Configuration Settings.
*
* Copyright (c) 2005-2019 Micro Digital Inc.
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

#ifndef SUD_CFG_H
#define SUD_CFG_H

#define SB_MULTITASKING 1
#define SUD_SYNOPSYS   1  /* Synopsys DWC USB Full/High speed */


/* Set to 1 to support OTG feature */
#define SUD_OTG        0

/* Set to 1 to support a Composite (multi-function) device. If 1, you must
   enable multiple functions below, such as Serial and Mass Storage.
   You also need to config the logical endpoint number manually for each
   selected functions if there is any conflict.
*/
#define SUD_COMPOSITE  1

/* Device Function / Class Selection. 1 to enable. 0 to disable. */
/* Note: Enable only ONE device function if SUD_COMPOSITE is 0. */
/* Some device controllers may not support ISOC transfer mode so check
   your device controller before you enable one of these that needs it.
*/
#define SUD_AUDIO      0
#define SUD_DFU        0
#define SUD_HIDCOM     0
#define SUD_KBD        0
#define SUD_MOUSE      0
#define SUD_MSTOR      1
//#define SUD_MSTOR      0
#define SUD_MTP        0
#define SUD_RNDIS      0
//#define SUD_SERIAL     1
#define SUD_SERIAL     0
#define SUD_VIDEO      0
#define SUD_FTEMPL     0

/* Device Class Selection. Set to 1 if you implement new device functions
   of these classes.
*/
#if SUD_MOUSE || SUD_KBD || SUD_HIDCOM
#define SUD_HID        1  /* keep 1 */
#else
#define SUD_HID        0
#endif

#if SUD_RNDIS
#define SUD_CDC        1  /* keep 1 */
#else
#define SUD_CDC        0
#endif

#if SUD_MSTOR
#define SUD_COMPOSITE_PRODUCTID         0x0019
#endif

#define SUD_MIN_RAM 0

/* Logical Endpoint number */
/*
   For non-composite configuration, we define the logical endpoint numbers
   as follows:

   CTRL IN/OUT  0
   BULK IN   -- 1
   BULK OUT  -- 2
   INT  IN   -- 3
   INT  OUT  -- 4
   ISOC IN   -- 5
   ISOC OUT  -- 6
   BULK IN   -- 7
   BULK OUT  -- 8
   INT  IN   -- 9
   INT  OUT  -- 10
   ISOC IN   -- 11
   ISOC OUT  -- 12
   BULK IN   -- 13
   BULK OUT  -- 14
   INT  IN   -- 15

   If the USB device controller has some restriction, for example NXP (Philips)
   LPC3XXX, the endpoint type is fixed so you need to re-define the logical
   endpoint number and map it to the correct physical endpoint number in
   the device controller driver.

   For a composite device, you must configure the endpoint numbers manually
   to ensure the endpoint numbers are unique for different functions of
   the device.
*/

#if SUD_SYNOPSYS
#define SUD_BULK_IN_EP                 0x82
#define SUD_BULK_OUT_EP                2
#define SUD_INT_IN_EP                  0x81
#define SUD_INT_OUT_EP                 1
#define SUD_ISOC_IN_EP                 0x83
#define SUD_ISOC_OUT_EP                3
#define SUD_BULK_IN_EP1                0x85
#define SUD_BULK_OUT_EP1               5
#define SUD_INT_IN_EP1                 0x84
#define SUD_INT_OUT_EP1                4
#define SUD_ISOC_IN_EP1                0x86
#define SUD_ISOC_OUT_EP1               6
#define SUD_BULK_IN_EP2                0x88
#define SUD_BULK_OUT_EP2               8
#define SUD_INT_IN_EP2                 0x87
#define SUD_INT_OUT_EP2                7
#define SUD_ISOC_IN_EP2                0x89
#define SUD_ISOC_OUT_EP2               9
#define SUD_BULK_IN_EP3                0x8B
#define SUD_BULK_OUT_EP3               0xB
#define SUD_INT_IN_EP3                 0x8A
#define SUD_INT_OUT_EP3                0xA
#define SUD_ISOC_IN_EP3                0x8C
#define SUD_ISOC_OUT_EP3               0xC


/* Config your VenderID and ProductID */

/* Audio related options */
#if SUD_AUDIO
#define SUD_AUDIO_PRODUCT_NAME          "smxUSBD Audio Device"
#define SUD_AUDIO_VENDORID              0x04CC  /* USER: Change to your ID; this is NXP (Philips) ID for example. */
#define SUD_AUDIO_PRODUCTID             0x0004
#define SUD_AUDIO_NUMBER_STR            "0123456789AB"
#define SUD_AUDIO_MAXPOWER              100

#define SUD_AUDIO_ISOCIN_ENDPOINT       SUD_ISOC_IN_EP
#define SUD_AUDIO_ISOCOUT_ENDPOINT      SUD_ISOC_OUT_EP
#define SUD_AUDIO_BULKIN_ENDPOINT       SUD_BULK_IN_EP
#define SUD_AUDIO_BULKOUT_ENDPOINT      SUD_BULK_OUT_EP
#define SUD_AUDIO_FEEDBACK_ENDPOINT     SUD_INT_IN_EP

/* set to '1' to enable microphone and/or speaker */
#define SUD_AUDIO_INCLUDE_MIC           1
#define SUD_AUDIO_INCLUDE_SPK           1
#define SUD_AUDIO_INCLUDE_MIDI_IN       0
#define SUD_AUDIO_INCLUDE_MIDI_OUT      0
#define SUD_AUDIO_MIDI_NUMBER           (SUD_AUDIO_INCLUDE_MIDI_IN|SUD_AUDIO_INCLUDE_MIDI_OUT) /* Audio MIDI number */
#define SUD_AUDIO_INCLUDE_FEEDBACK      0
#endif /* SUD_AUDIO */

/* DFU related options */
#if SUD_DFU
#define SUD_DFU_PRODUCT_NAME            "smxUSBD DFU Device"
#define SUD_DFU_VENDORID                0x04CC  /* USER: Change to your ID; this is NXP (Philips) ID for example. */
#define SUD_DFU_PRODUCTID               0x0009
#define SUD_DFU_NUMBER_STR              "0123456789AB"
#define SUD_DFU_MAXPOWER                100

/* which memory segment this device support */
#define SUD_DFU_INCLUDE_INTERN_FLASH    1
#define SUD_DFU_INCLUDE_SPI_FLASH       1
#define SUD_DFU_INCLUDE_NOR_FLASH       1
#define SUD_DFU_INCLUDE_NAND_FLASH      1

#define SUD_DFU_WILL_DETACH             1
#define SUD_DFU_MANIFESTATION           1
#define SUD_DFU_UPLOAD                  1
#define SUD_DFU_DNLOAD                  1

#define SUD_DFU_DETACH_TMO              500
#endif /* SUD_DFU */
#define SUD_DFU_TRANSFER_SIZE           512

/* HID related options */
#if SUD_HIDCOM
#define SUD_HID_PRODUCT_NAME            "smxUSBD HID Com"
#define SUD_HID_VENDORID                0x04CC  /* USER: Change to your ID; this is NXP (Philips) ID for example. */
#define SUD_HID_PRODUCTID               0x000A
#define SUD_HID_NUMBER_STR              "0123456789AB"
#define SUD_HID_MAXPOWER                10

#if SUD_COMPOSITE
#define SUD_HID_IN_ENDPOINT             SUD_INT_IN_EP1 /* USER: Remember to double check it */
#define SUD_HID_OUT_ENDPOINT            SUD_INT_OUT_EP1
#else
#define SUD_HID_IN_ENDPOINT             SUD_INT_IN_EP
#define SUD_HID_OUT_ENDPOINT            SUD_INT_OUT_EP
#endif

#define SUD_HID_IN_PACKET_SIZE          64 /* at most 64 bytes, this is endpoint packet size, not report size. */
#define SUD_HID_OUT_PACKET_SIZE         64 /* at most 64 bytes, this is endpoint packet size, not report size. */

#define SUD_HID_REPORT_NUM              1

#define SUD_HID_IN_REPORT1_SIZE         64 /* at most 64 bytes, payload data is only SUD_HID_IN_REPORT1_SIZE - 1 */
#define SUD_HID_OUT_REPORT1_SIZE        64 /* at most 64 bytes, payload data is only SUD_HID_OUT_REPORT1_SIZE - 1 */

#define SUD_HID_IN_REPORT2_SIZE         64 /* at most 64 bytes, payload data is only SUD_HID_IN_REPORT2_SIZE - 1 */
#define SUD_HID_OUT_REPORT2_SIZE        64 /* at most 64 bytes, payload data is only SUD_HID_OUT_REPORT2_SIZE - 1 */

#define SUD_HID_IN_REPORT3_SIZE         64 /* at most 64 bytes, payload data is only SUD_HID_IN_REPORT3_SIZE - 1 */
#define SUD_HID_OUT_REPORT3_SIZE        64 /* at most 64 bytes, payload data is only SUD_HID_OUT_REPORT3_SIZE - 1 */

#define SUD_HID_IN_REPORT4_SIZE         64 /* at most 64 bytes, payload data is only SUD_HID_IN_REPORT4_SIZE - 1 */
#define SUD_HID_OUT_REPORT4_SIZE        64 /* at most 64 bytes, payload data is only SUD_HID_OUT_REPORT4_SIZE - 1 */

#if SUD_HID_REPORT_NUM > 4
#error Add more HIDCOM report size here
#endif

#define SUD_HID_IN_INTERVAL             8 /* Unit ms */
#define SUD_HID_OUT_INTERVAL            8 /* Unit ms */

#endif /* SUD_HIDCOM */

/* Keyboard related options */
#if SUD_KBD
#define SUD_KBD_PRODUCT_NAME            "smxUSBD Keyboard"
#define SUD_KBD_VENDORID                0x04CC  /* USER: Change to your ID; this is NXP (Philips) ID for example. */
#define SUD_KBD_PRODUCTID               0x000B
#define SUD_KBD_NUMBER_STR              "0123456789AB"
#define SUD_KBD_MAXPOWER                10
#define SUD_KBD_INTERVAL                16 /* Unit ms */

#if SUD_COMPOSITE
#define SUD_KBD_INT_ENDPOINT            1 /* USER: Remember to double check it */
#else
#define SUD_KBD_INT_ENDPOINT            SUD_INT_IN_EP
#endif
#endif /* SUD_KBD */

/* Mouse related options */
#if SUD_MOUSE
#define SUD_MOUSE_PRODUCT_NAME          "smxUSBD Mouse"
#define SUD_MOUSE_VENDORID              0x04CC  /* USER: Change to your ID; this is NXP (Philips) ID for example. */
#define SUD_MOUSE_PRODUCTID             0x0001
#define SUD_MOUSE_NUMBER_STR            "0123456789AB"
#define SUD_MOUSE_MAXPOWER              10
#define SUD_MOUSE_INT_INTERVAL          8 /* Unit ms */

#if SUD_COMPOSITE && SUD_SERIAL
#define SUD_MOUSE_INT_ENDPOINT          SUD_INT_IN_EP1
#else
#define SUD_MOUSE_INT_ENDPOINT          SUD_INT_IN_EP
#endif
#endif /* SUD_MOUSE */

/* Mass Storage related options */
#if SUD_MSTOR
#define SUD_MSTOR_PRODUCT_NAME          "smxUSBD Disk" /* at most 15 bytes */
#define SUD_MSTOR_VENDORID              0x04CC  /* USER: Change to your ID; this is NXP (Philips) ID for example. */
#define SUD_MSTOR_PRODUCTID             0x0002
#define SUD_MSTOR_NUMBER_STR            "0123456789AB"
#define SUD_MSTOR_MAXPOWER              10

/* Async data access, separate task to handle the media data read/write */
#define SUD_MSTOR_ASYNC_ACCESS          (SB_MULTITASKING && 0)
#define SUD_MSTOR_TASK_STACK            1000

/* Mass storage internal buffer size */
#if SUD_MIN_RAM
#define SUD_MSTOR_PACKET_SIZE           512
#else
#define SUD_MSTOR_PACKET_SIZE           2048
#endif
/* Number of mass storage devices (LUN's). Change to support multiple disks. */
#define SUD_MSTOR_MAX_NUM               1

#if SUD_COMPOSITE
#if SUD_AUDIO && SUD_CF5329
/* config it for composite case */
#define SUD_MSTOR_IN_ENDPOINT           SUD_INT_IN_EP
#define SUD_MSTOR_OUT_ENDPOINT          SUD_INT_OUT_EP
#elif SUD_AUDIO || SUD_SERIAL
#define SUD_MSTOR_IN_ENDPOINT           SUD_BULK_IN_EP1 /* modify according to the number of serial ports, EP1 if one port, EP2 if two */
#define SUD_MSTOR_OUT_ENDPOINT          SUD_BULK_OUT_EP1
#else
/* common case, do not change */
#define SUD_MSTOR_IN_ENDPOINT           SUD_BULK_IN_EP
#define SUD_MSTOR_OUT_ENDPOINT          SUD_BULK_OUT_EP
#endif
#else
/* common case, do not change */
#define SUD_MSTOR_IN_ENDPOINT           SUD_BULK_IN_EP
#define SUD_MSTOR_OUT_ENDPOINT          SUD_BULK_OUT_EP
#endif /* SUD_COMPOSITE */

#endif /* SUD_MSTOR */

/* MTP related options */
#if SUD_MTP
#define SUD_MTP_PRODUCT_NAME            "smxUSBD MTP Device"
#define SUD_MTP_VENDORID                0x04CC  /* USER: Change to your ID; this is NXP (Philips) ID for example. */
#define SUD_MTP_PRODUCTID               0x0008
#define SUD_MTP_NUMBER_STR              "0123456789AB"
#define SUD_MTP_MAXPOWER                100

/* Number of MTP devices. Currently only supports one. */
#define SUD_MTP_MAX_NUM                 1

#define SUD_MTP_INTIN_ENDPOINT          SUD_INT_IN_EP
#define SUD_MTP_BULKIN_ENDPOINT         SUD_BULK_IN_EP
#define SUD_MTP_BULKOUT_ENDPOINT        SUD_BULK_OUT_EP

#define SUD_MTP_PACKET_SIZE             2048

#endif

/* RNDIS related options */
#if SUD_RNDIS
#define SUD_RNDIS_PRODUCT_NAME          "smxUSBD RNDIS Network"
#define SUD_RNDIS_VENDORID              0x04CC  /* USER: Change to your ID; this is NXP (Philips) ID for example. */
#define SUD_RNDIS_PRODUCTID             0x0003
#define SUD_RNDIS_NUMBER_STR            "0123456789AB"
#define SUD_RNDIS_MAXPOWER              10

/* Number of RNDIS devices. Currently only supports one. */
#define SUD_RNDIS_MAX_NUM               1

#if SUD_COMPOSITE
#define SUD_RNDIS_IN_ENDPOINT           SUD_BULK_IN_EP1  /* USER: Remember to double check it */
#define SUD_RNDIS_OUT_ENDPOINT          SUD_BULK_OUT_EP1
#define SUD_RNDIS_INT_ENDPOINT          SUD_INT_IN_EP
#else
#define SUD_RNDIS_IN_ENDPOINT           SUD_BULK_IN_EP
#define SUD_RNDIS_OUT_ENDPOINT          SUD_BULK_OUT_EP
#define SUD_RNDIS_INT_ENDPOINT          SUD_INT_IN_EP
#endif
#endif /* SUD_RNDIS */

/* Serial Port related options */
#if SUD_SERIAL

/* Number of serial ports. We can support up to 4 ports. */
#define SUD_SERIAL_MAX_NUM              1
#define SUD_SERIAL_USE_INT_ENDPOINT     1
#define SUD_SERIAL_SUPPORT_ACM          0  /* ACM mode is needed for linux/osx */

#if SUD_SERIAL_SUPPORT_ACM
#undef SUD_CDC
#define SUD_CDC                         1
#endif

#define SUD_SERIAL_PRODUCT_NAME         "smxUSBD Serial Emulator"
#define SUD_SERIAL_VENDORID             0x04CC  /* USER: Change to your ID; this is NXP (Philips) ID for example. */
#if SUD_SERIAL_MAX_NUM == 1
#if SUD_SERIAL_USE_INT_ENDPOINT
#define SUD_SERIAL_PRODUCTID            0x0000
#else
#define SUD_SERIAL_PRODUCTID            0x0006  /* single port but without INT endpoint */
#endif
#else
#define SUD_SERIAL_PRODUCTID            0x0005  /* Multiple Port Serial Emulator, we need a windows driver */
#endif
#define SUD_SERIAL_NUMBER_STR           "0123456789AB"
#define SUD_SERIAL_MAXPOWER             10

/* Serial port internal receive ring buffer size */
#if SUD_MIN_RAM
#define SUD_SERIAL_MTU                  256
#else
#define SUD_SERIAL_MTU                  2048
#endif

#define SUD_SERIAL_IN_ENDPOINT          SUD_BULK_IN_EP
#define SUD_SERIAL_OUT_ENDPOINT         SUD_BULK_OUT_EP
#define SUD_SERIAL_INT_ENDPOINT         SUD_INT_IN_EP
#define SUD_SERIAL_IN_ENDPOINT1         SUD_BULK_IN_EP1
#define SUD_SERIAL_OUT_ENDPOINT1        SUD_BULK_OUT_EP1
#define SUD_SERIAL_INT_ENDPOINT1        SUD_INT_IN_EP1
#define SUD_SERIAL_IN_ENDPOINT2         SUD_BULK_IN_EP2
#define SUD_SERIAL_OUT_ENDPOINT2        SUD_BULK_OUT_EP2
#define SUD_SERIAL_INT_ENDPOINT2        SUD_INT_IN_EP2
#define SUD_SERIAL_IN_ENDPOINT3         SUD_BULK_IN_EP3
#define SUD_SERIAL_OUT_ENDPOINT3        SUD_BULK_OUT_EP3
#define SUD_SERIAL_INT_ENDPOINT3        SUD_INT_IN_EP3

#endif /* SUD_SERIAL */

/* Video related options */
#if SUD_VIDEO

#define SUD_VIDEO_PRODUCT_NAME          "smxUSBD Video Device"
#define SUD_VIDEO_VENDORID              0x04CC  /* USER: Change to your ID; this is NXP (Philips) ID for example. */
#define SUD_VIDEO_PRODUCTID             0x0007
#define SUD_VIDEO_NUMBER_STR            "0123456789AB"
#define SUD_VIDEO_MAXPOWER              250

#define SUD_VIDEO_ISOCIN_ENDPOINT       SUD_ISOC_IN_EP
#define SUD_VIDEO_ISOCOUT_ENDPOINT      SUD_ISOC_OUT_EP
#define SUD_VIDEO_INTIN_ENDPOINT        SUD_INT_IN_EP

/* use UVC 1.1 or 1.0, windows XP only support v1.0 */
#define SUD_VIDEO_USE_11                0

/* set to '1' to enable camera and/or display */
#define SUD_VIDEO_INCLUDE_IN            1
#define SUD_VIDEO_INCLUDE_OUT           0  /* not tested */

/* if we support still image */
#define SUD_VIDEO_IN_STILL_IMAGE        0

/* Uncompressed for YUV422 or YUV420 */
#define SUD_VIDEO_USE_YUV422            1

/* Video Streaming IN Format, enable at least one */
#define SUD_VIDEO_IN_FORMAT_UNCOMPRESSED   1
#define SUD_VIDEO_IN_FORMAT_MJPEG          0
#define SUD_VIDEO_IN_FORMAT_MPEG2TS        0

/* Video Streaming OUT Format, enable at least one */
#define SUD_VIDEO_OUT_FORMAT_UNCOMPRESSED  1
#define SUD_VIDEO_OUT_FORMAT_MJPEG         0
#define SUD_VIDEO_OUT_FORMAT_MPEG2TS       0

#endif /* SUD_VIDEO */

#if SUD_COMPOSITE
#define SUD_COMPOSITE_PRODUCT_NAME      "smxUSBD Composite Device"
#define SUD_COMPOSITE_VENDORID          0x04CC  /* USER: Change to your ID; this is NXP (Philips) ID for example. */

#if SUD_SERIAL && !SUD_DFU
#if SUD_AUDIO
#define SUD_COMPOSITE_PRODUCTID         (0x0020 + SUD_SERIAL_MAX_NUM - 1)
#elif SUD_VIDEO
#define SUD_COMPOSITE_PRODUCTID         (0x0024 + SUD_SERIAL_MAX_NUM - 1)
#else
#if SUD_SERIAL_MAX_NUM > 1
#define SUD_COMPOSITE_PRODUCTID         (0x0030 + SUD_SERIAL_MAX_NUM - 1)
#else
#define SUD_COMPOSITE_PRODUCTID         (0x0010 + SUD_SERIAL_MAX_NUM - 1)
#endif
#endif
#elif SUD_RNDIS && !SUD_DFU
#if SUD_AUDIO
#define SUD_COMPOSITE_PRODUCTID         0x001E
#elif SUD_VIDEO
#define SUD_COMPOSITE_PRODUCTID         0x001D
#elif SUD_SERIAL
#define SUD_COMPOSITE_PRODUCTID         0x001C
#else
#define SUD_COMPOSITE_PRODUCTID         0x001B
#endif
#elif SUD_DFU
#if SUD_HIDCOM
#define SUD_COMPOSITE_PRODUCTID         0x001A
#elif SUD_MSTOR
#define SUD_COMPOSITE_PRODUCTID         0x0019
#else
#define SUD_COMPOSITE_PRODUCTID         0x0018
#endif
#elif SUD_VIDEO
#define SUD_COMPOSITE_PRODUCTID         0x0017
#elif SUD_AUDIO
#define SUD_COMPOSITE_PRODUCTID         0x0015
#elif SUD_HID
#define SUD_COMPOSITE_PRODUCTID         0x0016
#endif


#define SUD_COMPOSITE_NUMBER_STR        "0123456789AB"
#define SUD_COMPOSITE_MAXPOWER          10
#endif /* SUD_COMPOSITE */


/* if need to process the class descriptor */
#if SUD_AUDIO || SUD_CDC || SUD_HID || SUD_VIDEO
#define SUD_CLASS_DESC                  1
#else
#define SUD_CLASS_DESC                  0
#endif

#if SUD_HIDCOM
#define SUD_HID_BUFFER_SIZE             (64 + (SUD_HID_REPORT_NUM*12))
#else
#define SUD_HID_BUFFER_SIZE             64
#endif

#if SUD_SERIAL
#define SUD_SERIAL_BUFFER_SIZE          (SUD_SERIAL_MAX_NUM*(64 + SUD_SERIAL_SUPPORT_ACM*64))
#else
#define SUD_SERIAL_BUFFER_SIZE          0
#endif

/* Data buffer length for EP0. Depends on which function driver is included. */
#define SUD_EP0_BUFFER_SIZE (SUD_AUDIO*384 + SUD_DFU*SUD_DFU_TRANSFER_SIZE + SUD_HID*SUD_HID_BUFFER_SIZE + SUD_MSTOR*64 + SUD_RNDIS*256 + SUD_SERIAL*SUD_SERIAL_BUFFER_SIZE + SUD_VIDEO*1200 + SUD_MTP*64 + SUD_FTEMPL*64)

/* Set your manufacturer and product name */
#define SUD_MANUFACTURER_NAME           "MDI"
//#define SUD_SELFPOWERED                 1
#define SUD_SELFPOWERED                 0

/* Debug Level */
/*   0  disables all debug output and debug statements are null macros */
/*   1  only output fatal error information */
/*   2  output additional warning information */
/*   3  output additional status information */
/*   4  output additional device change information */
/*   5  output additional data transfer information */
/*   6  output interrupt information */

//#define SUD_DEBUG_LEVEL                 6
#define SUD_DEBUG_LEVEL                 0
//#define SUD_DEBUG_LEVEL                 7


#endif /* SUD_CFG_H */
#endif /* SUD_CFG_H */

