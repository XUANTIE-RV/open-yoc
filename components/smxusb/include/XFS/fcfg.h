/*
* fcfg.h                                                    Version 2.25
*
* smxFS Configuration Settings.
*
* Copyright (c) 2004-2017 Micro Digital Inc.
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

#ifndef SFS_FCFG_H
#define SFS_FCFG_H

/* Built-In Driver Selection. Set to 1 to enable. */

#if !defined(SB_OS_WIN32)  /* Normal Case (not Win32) */
#define SFS_DRV_ATA                     0
#define SFS_DRV_CF                      0
#define SFS_DRV_DOC                     0
#define SFS_DRV_MMCSD                   0
#define SFS_DRV_NANDFLASH               0
#define SFS_DRV_NORFLASH                0
#define SFS_DRV_RAMDISK                 0    /* needs a lot of RAM */
#define SFS_DRV_USB                     1
#define SFS_DRV_WINDISK                 0    /* 0 */
#else
#define SFS_DRV_ATA                     0    /* keep all 0 except WINDISK */
#define SFS_DRV_CF                      0
#define SFS_DRV_DOC                     0
#define SFS_DRV_MMCSD                   0
#define SFS_DRV_NANDFLASH               0
#define SFS_DRV_NORFLASH                0
#define SFS_DRV_RAMDISK                 0
#define SFS_DRV_USB                     0
#define SFS_DRV_WINDISK                 1    /* 1 */
#endif


/* Set to 1 to disable all code and APIs to modify the contents of disk,
   such as create file and directory.
*/
#define SFS_READONLY                    0

/* Set to 1 to enable extended API calls. Set to 0 for smxFS Lite. */
#define SFS_FULL_FEATURES_SUPPORT       1

#if SFS_FULL_FEATURES_SUPPORT

/* Set to 1 to support FAT32 */
#define SFS_FAT32_SUPPORT               1

/* Set to 1 to support long file names compatible with Windows. */
/*
   Note: VFAT is patented by Microsoft. US Patent #5,758,352.
         Microsoft may require a license fee to use it. Setting
         SFS_VFAT_SUPPORT to 0 will avoid potential patent
         infringement problems.
*/
#define SFS_VFAT_SUPPORT                0

/* Set to 0 to disable current working directory functions such as
   sfs_setcwd(), sfs_getcwd().
*/
#define SFS_CWD_SUPPORT                 1

/* Set to 0 to disable wildcard find file functions 
   sfs_findfirst(), sfs_findnext() and sfs_findclose().
*/
#define SFS_FINDFIRST_SUPPORT           1

/* Set to 0 to disable directory functions 
   sfs_mkdir() and sfs_rmdir().
*/
#define SFS_MKDIR_SUPPORT               1

/* Set to 0 to disable volume functions such as
   sfs_setvolname() and sfs_getvolname().
*/
#define SFS_VOLUME_SUPPORT              1

/* Set to 0 to disable file properties functions such as
   sfs_timestamp() and sfs_stat().
*/
#define SFS_PROPERTY_SUPPORT            1

/* Set to 0 to disable sfs_chkdsk() function. */
#define SFS_CHKDSK_SUPPORT              1

/* Set to 0 to disable sfs_rename() function. */
#define SFS_RENAME_SUPPORT              1

/* Language support for Long File Names. Only enable one.
   SFS_VFAT_SUPPORT must also be enabled.
*/
#define SFS_BIG5_SUPPORT                0
#define SFS_GB2312_SUPPORT              0

/* Set to 1 to always generate the long file name entry even for short
   (8.3) file names. The advantage is the name will be stored in the
   specified upper/lower case (e.g. TheFile.txt not THEFILE.TXT).
   The disadvantage is it uses 2 directory entries instead of 1, so for
   FAT16 this reduces the maximum size of the root directory by half.
*/
#define SFS_VFAT_ALWAYS_USE_LFN         1

/* Set to 1 enable extra code to do safety checks; 0 to disable. */
#define SFS_SAFETY_CHECKS               1

/* Set to 1 to handle bad data sectors. When writing and a bad sector is
   encountered, smxFS will try to replace the cluster with a new one.
*/
#define SFS_HANDLE_BAD_SECTOR           0

/* Set to 1 to support not clean shutdown flag so file system will know 
   if it is not shutdown properly.
   NOTE: We have not fully studied/tested this option to ensure it is
   reliable for power loss at every point in the code.
*/
#define SFS_CLN_SHUTDOWN_SUPPORT        0

/* Set to 1 to support FSInfo sector for FAT12/16, so the file system
   does not need to scan the whole FAT to get the free disk size. However,
   it can be wrong. See the discussion of SFS_USE_FAT32_FSINFO below.
   This is ignored when formatting removable media because it is not part
   of the FAT12/16 spec, and other FAT file system implementations do not
   support it, so they may modify the disk but not FSInfo and then smxFS
   relies on wrong information.
*/
#define SFS_FAT_FSINFO_SUPPORT          0

/* Set to 1 to use the FAT32 FSInfo sector to store the number of free
   clusters and next free cluster information. If 1, sfs_freekb() will
   read the values from the FSInfo sector; otherwise, it will scan the
   whole FAT the first time, which can be very slow. The FSInfo sector
   can be wrong, though, causing sfs_freekb() to report the wrong size.
   For example, Windows chkdsk does not update this sector when lost
   chains are recovered. Also, with this option on, the FSInfo data
   is updated as the disk changes, which reduces performance.
*/
#define SFS_USE_FAT32_FSINFO            1

/* Set to 1 to write FAT nodes to the second FAT. smxFS does not currently
   use the second FAT for fixing a disk, so we recommend leaving this option
   disabled. Note that Windows does not use the second FAT to repair the
   first one either.
*/
#define SFS_2NDFAT_SUPPORT              0

/* Set to 1 to enable free cluster caching. This feature needs more RAM.
   Enable it only if your application will store some very large files
   and you don't want a long delay for the next fwrite operation. If the
   file system has a lot of fragments, this feature may decrease performance.
   When this feature is enabled:
   1. Whenever smxFS attempts to allocate some free clusters, it will 
      try to find more if the free cluster number in the cache is less 
      than SFS_FREECLUS_TRIGGER. Each time it will scan at most
      SFS_FREECLUS_SCAN_CACHE_SIZE FAT area.
   2. You can call sfs_scanfreeclus() to force it to cache more free clusters
      so the next fwrite operation won't wait a long time.
   3. This feature is disabled for FAT12 disks.
*/
#define SFS_FREECLUS_SUPPORT            0

/* Set to 1 to enable the short file name numeric-tail cache.
   If you are creating a lot of similar long file names within a directory, 
   such as Log-2010-11-08.dat, Log-2010-11-09.dat. smxFS must create corresponding 
   short file names like LOG-20~1.DAT, LOG-20~2.DAT and verify that short file 
   name is unique in that directory. When you are creating hundreds of this kind 
   of file name, the compare process will become very time-consuming without this
   cache.
*/
#define SFS_NUMERICTAIL_CACHE_SUPPORT   0

#else /* not SFS_FULL_FEATURES_SUPPORT: */

/* You can enable the following settings, but they increase code and data size. */
#define SFS_MKDIR_SUPPORT               0
#define SFS_FAT32_SUPPORT               0
#define SFS_HANDLE_BAD_SECTOR           0
#define SFS_2NDFAT_SUPPORT              0
#define SFS_FAT_FSINFO_SUPPORT          0
#define SFS_USE_FAT32_FSINFO            0
#define SFS_SAFETY_CHECKS               0

/* Do not enable the following settings, since you may not have the source code. */
#define SFS_CWD_SUPPORT                 0
#define SFS_FINDFIRST_SUPPORT           0
#define SFS_VOLUME_SUPPORT              0
#define SFS_PROPERTY_SUPPORT            0
#define SFS_CHKDSK_SUPPORT              0
#define SFS_RENAME_SUPPORT              0
#define SFS_CLN_SHUTDOWN_SUPPORT        0
#define SFS_VFAT_SUPPORT                0
#define SFS_VFAT_ALWAYS_USE_LFN         0
#define SFS_FREECLUS_SUPPORT            0
#define SFS_NUMERICTAIL_CACHE_SUPPORT   0

#endif /* SFS_FULL_FEATURES_SUPPORT */

/* Set to 1 to write back the whole FAT/Dir cache when a cache miss happens.
   Set to 0 to write only the cache sector being swapped out.
*/
#define SFS_UPDATE_WHOLE_FATDIR_CACHE   1

/* Set to 1 to support format operation. */
#if SFS_DRV_RAMDISK || SFS_DRV_DOC || SFS_DRV_NANDFLASH || SFS_DRV_NORFLASH
#define SFS_FORMAT_SUPPORT              1    /* do not change (1) */
#else
#if SFS_READONLY
#define SFS_FORMAT_SUPPORT              0    /* do not change (0) */
#else
#define SFS_FORMAT_SUPPORT              1
#endif
#endif

/* Cache Size

   For 16-bit real mode, the C malloc() function cannot allocate memory
   greater than 64KB - 1 so smxFS cannot use a large contiguous buffer.
   In this case, we recommend you set the total cache size less than 64KB 
   to ensure all devices can work.
*/
#define SFS_DATA_CACHE_SIZE             (16*1024)
#define SFS_DIR_CACHE_SIZE              ( 2*1024)
#define SFS_FAT_CACHE_SIZE              ( 4*1024)
#define SFS_FREECLUS_CACHE_SIZE         (   1024)
#define SFS_FREECLUS_SCAN_CACHE_SIZE    ( 4*1024)
#define SFS_NUMERICTAIL_CACHE_SIZE      (      8)

/* Some disk driver may require the data buffer to be some byte aligned. */
#define SFS_CACHE_BUF_ALIGNMENT         16

/* Trigger number to force smxFS to scan for free clusters. */
#define SFS_FREECLUS_TRIGGER            (    128)

/* 
   Set to 1 to use C library (or smx) heap functions, malloc() and free().
   Set to 0 to use smxFS internal heap functions. (smx maps malloc() and
   free() onto smx heap functions.)
*/
#if defined(SB_CPU_AM18XX)
#define SFS_USE_C_HEAP                  0
#define SFS_HEAP_SIZE                   (32*1024)
#elif defined(SB_BRD_ATMEL_AT91SAM9M10G45EK) || defined(SB_CPU_IMX6UL) || defined(SB_CPU_IMX6ULL)
#define SFS_USE_C_HEAP                  0
#define SFS_HEAP_SIZE                   (64*1024)
#else
#define SFS_USE_C_HEAP                  1
#define SFS_HEAP_SIZE                   (64*1024)
#endif

/* Max devices the file system can support at the same time. This setting
   has very little impact on RAM usage (4 bytes each).
*/
#define SFS_MAX_DEV_NUM                 8

/* Max number of current working directory entries. In a multitasking
   environment, this should be set to the number of tasks that call
   sfs_chdir(). Note that each increment of this setting allocates only
   space for a pointer. Buffers for each entry (handle and path string)
   are malloced as needed.
*/
#if SB_MULTITASKING
#define SFS_CWD_MAX_ENTRIES             10
#else
#define SFS_CWD_MAX_ENTRIES             1
#endif

/* Select desired path separator. */
#if 1
#define SFS_PATHSEP                     '\\'
#define SFS_PATHSEP_STR                 "\\"
#else
#define SFS_PATHSEP                     '/'
#define SFS_PATHSEP_STR                 "/"
#endif    

#define SFS_FIRST_DRIVE                 'A'  /* case-insensitive */

/* Size of buffer allocated by sfs_copy(). */
#define SFS_COPY_BUF_SZ                 2048 /* multiple of sector size */

#define SFS_LONGFILENAME_LEN            255  /* max value 255 */
#if SFS_VFAT_SUPPORT
#define SFS_PATHFILENAME_LEN            260  /* max value 260 */
#else
#define SFS_PATHFILENAME_LEN            80   /* max value 80 */
#endif

/* Set to 1 to save a copy of full path and filename in file handle structure. 
   Only used for debug purposes. Allows smxAware to display opened filenames.
*/
#define SFS_FILENAME_IN_HANDLE          0

/* Debug Level */
/*   0  disables all debug output and debug statements are null macros */
/*   1  only output fatal error information */
/*   2  output additional warning information */
/*   3  output additional status information */
/*   4  output additional device change information */
/*   5  output additional data transfer information */
/*   6  output interrupt information */

#define SFS_DEBUG_LEVEL                 0

#endif /* SFS_FCFG_H */
