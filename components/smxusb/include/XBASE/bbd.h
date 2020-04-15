/*
* bbd.h                                                     Version 4.1.1
*
* Block Device Driver Interface
*
* Copyright (c) 2006-2015 Micro Digital Inc.
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
*****************************************************************************/

#ifndef SB_BBD_H
#define SB_BBD_H

/* Error Codes for SectorRead() and SectorWrite() */
/*
   Note:
   1. Driver should retry if reading/writing failed. Then only return
      SB_BD_MEDIA_REMOVED, SB_BD_BAD_BLOCK, SB_BD_DEVICE_ERROR, to the file
      system.
   2. If driver returns SB_BD_WRITE_PROTECT, then SB_BD_IOCTL_WRITEPROTECT
      should also return SB_PASS to tell file system that this disk is
      write protected now.
*/
#define SB_BD_OK                 0
#define SB_BD_MEDIA_REMOVED      -1   /* returned by SectorRead() and SectorWrite() */
#define SB_BD_BAD_BLOCK          -2   /* returned by SectorWrite() */
#define SB_BD_WRITE_PROTECT      -3   /* only returned by SectorWrite() */
#define SB_BD_DEVICE_ERROR       -4   /* returned by SectorRead() and SectorWrite() */

/* IOCTLs for Device Drivers */
#define SB_BD_IOCTL_INSERTED     0x00
#define SB_BD_IOCTL_REMOVED      0x01
#define SB_BD_IOCTL_CHANGED      0x02
#define SB_BD_IOCTL_WRITEPROTECT 0x03
#define SB_BD_IOCTL_GETDEVINFO   0x04
#define SB_BD_IOCTL_FLUSH        0x05
#define SB_BD_IOCTL_DELSECTOR    0x06
#define SB_BD_IOCTL_ERASE_FLASH  0x07
#define SB_BD_IOCTL_FORMAT_FLASH 0x08
#define SB_BD_IOCTL_CUSTOM       0x80
/*
Add custom IOCTLs in the driver header files like this:
#define SB_BD_IOCTL_XXX          (SB_BD_IOCTL_CUSTOM+0)
*/

/* Partition Table Constants */
#define SB_BD_NO_PARTITION       (uint)(-1)

/* Device Information */
typedef struct {
    u32      dwSectorsNum;
    uint     dwSectorSize;
    uint     wPartition;         /* set to SFS_NO_PARTITION if your device should not have any partitions */
    uint     wAutoFormat;
    uint     wFATNum;
    uint     wRootDirNum;
    uint     wRemovable;
    uint     wFATCacheSize;
    uint     wDirCacheSize;
    uint     wDataCacheSize;
    uint     wLogicalPartition;
} SB_BD_DEVINFO;

/* Block Device Driver Interface */
typedef struct {
    int (*DriverInit)(void);
    int (*DriverRelease)(void);
    int (*DiskOpen)(void);
    int (*DiskClose)(void);
    int (*SectorRead)(u8 *pRAMAddr, u32 dwStartSector, u16 wHowManySectors);
    int (*SectorWrite)(u8 *pRAMAddr, u32 dwStartSector, u16 wHowManySectors);
    int (*IOCtl)(uint dwCommand, void *pParameter);
} SB_BD_IF;

#endif /* SB_BBD_H */

