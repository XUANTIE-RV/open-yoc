/*
* fconst.h                                                  Version 2.24
*
* smxFS Internal Constants.
*
* Copyright (c) 2004-2014 Micro Digital Inc.
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

#ifndef SFS_FCONST_H
#define SFS_FCONST_H

/* Flag for not a valid device */
#define INVALID_DEVICE          0xFFFF

/* FAT Types */
#define FAT_12                  12
#define FAT_16                  16
#define FAT_32                  32

/* File attribute */
#define SFS_ATTR_LONG_NAME      (SFS_ATTR_READ_ONLY|SFS_ATTR_HIDDEN|SFS_ATTR_SYSTEM|SFS_ATTR_VOLUME_ID)
#define SFS_ATTR_LONG_NAME_MASK (SFS_ATTR_READ_ONLY|SFS_ATTR_HIDDEN|SFS_ATTR_SYSTEM|SFS_ATTR_VOLUME_ID|SFS_ATTR_DIRECTORY|SFS_ATTR_ARCHIVE)

/* . and .. dir enty name */
#define DOT_DIR_NAME            ".          "
#define DOT_DOT_DIR_NAME        "..         "

/* Dir Entry flag */
#define FREE_DIR_ENTRY_THIS     0xE5    
#define FREE_DIR_ENTRY_BELOW    0x00    
#define FREE_DIR_ENTRY_NONE     0xFFFF 

#define ROOT_PATH_CLUSTER_NUM   0

/* Long File Name flag */
#define LAST_LONG_ENTRY         0x40
#define LFN_CHAR_PER_ENTRY      13

/* File Handle FileStatus flags */
#define READONLY                0x01
#define READWRITE               0x02
#define FILE_UPDATED            0x04
#define FILE_CACHE_EMPTY        0x08
#define CACHE_UPDATED           0x10
#define PASSTHROUGH             0x80

/* Return codes for internal functions for finding files in directories */
#define FILE_FOUND              1
#define BAD_PATH_NAME           2
#define BAD_FILE_NAME           3
#define PATH_NOT_FOUND          4
#define FILE_NOT_FOUND          5

/* Status for CheckDevice function */
#define DISK_FORMATTED          0
#define DISK_CHECK_ERROR        1
#define DISK_UNFORMATTED        2

/* Short file name lengths. Do not change these values. */
#define FILENAME_LEN            8
#define FILEEXT_LEN             3
#define PUREFILENAME_LEN        (FILENAME_LEN + FILEEXT_LEN) /* 11, not including path and dot */
#define FULLFILENAME_LEN        (PUREFILENAME_LEN + 1)       /* 12, including dot */

#define CLR_SHUTDOWN_FAT16      0x8000
#define CLR_SHUTDOWN_FAT32      0x08000000UL

#define SIZEOFBPB16             62
#define SIZEOFBPB32             90
#define SIZEOFPARTITIONTABLE    16

#if SB_PACKED_STRUCT_SUPPORT
#define SIZEOFDIRENTY           sizeof(DIRENTRY)
#else
#define SIZEOFDIRENTY           32
#endif

/* Active Partition Table Flag */
#define PARTITON_OFFSET         446
#define ACTIVE_PARTITION_FLAG   0x80

/* mount and format related constants*/
#define BACK_BOOT_SECTOR        6  /* backup boot sector is 6 */

#define MAX_CLUSTERS_OF_FAT12   4084L
#define MAX_CLUSTERS_OF_FAT16   65524L

#define BOOTSEC_SIGNATURE_OFFSET 255
#define BOOTSEC_SIGNATURE        0xAA55

/* FSI constants */
#define FSI_LEADSIG_OFFSET      0
#define FSI_LEADSIG             0x41615252L
#define FSI_STRUCSIG_OFFSET     484
#define FSI_STRUCSIG            0x61417272L
#define FSI_FREECOUNT_OFFSET    488
#define FSI_FREECOUNT_UNKNOWN   0xFFFFFFFFL
#define FSI_NEXTFREE_OFFSET     492
#define FSI_NEXTFREE_DEFAULT    0x03
#define FSI_NEXTFREE_UNKNOWN    0xFFFFFFFFL
#define FSI_TRAILSIG_OFFSET     508
#define FSI_TRAILSIG            0xAA550000L

/* Macros */

#define ISFAT12(pDevHandle)     (pDevHandle->wFatType == FAT_12)
#define ISFAT16(pDevHandle)     (pDevHandle->wFatType == FAT_16)

#define ISFAT32(pDevHandle)     (pDevHandle->wFatType == FAT_32)

#define ISEOC(dwCluster)        (((dwCluster)|0x7) == pDevHandle->pFD->FAT_NODE_EOC)
#define ISFREE(dwCluster)       ((dwCluster) == pDevHandle->pFD->FAT_NODE_FREE)
#define ISBAD(dwCluster)        ((dwCluster) == pDevHandle->pFD->FAT_NODE_BAD)
#define ISINVALID(dwCluster)    ((dwCluster) == pDevHandle->pFD->FAT_NODE_INVALID)

#define CLUSTOSEC(dwCluster)    ((dwCluster-2) * pDevHandle->wSecPerClu + pDevHandle->dwFirstDataSector)
#define SECTOCLUS(dwSector)     (2 + ((dwSector - pDevHandle->dwFirstDataSector)/pDevHandle->wSecPerClu))

#if SFS_FAT32_SUPPORT
#define GETROOTCLUS(pDevHandle) (ISFAT32(pDevHandle) ? pDevHandle->dwRootClus : ROOT_PATH_CLUSTER_NUM)
#else
#define GETROOTCLUS(pDevHandle) ROOT_PATH_CLUSTER_NUM
#endif /* SFS_FAT32_SUPPORT */

#define GETLONGFILENAMENUM(iLength) (iLength/LFN_CHAR_PER_ENTRY + (0 == (iLength%LFN_CHAR_PER_ENTRY) ? 0 : 1))

#endif /* SFS_FCONST_H */
