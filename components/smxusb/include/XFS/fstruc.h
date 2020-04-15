/*
* fstruc.h                                                  Version 2.21
*
* smxFS Internal Structure Definitions.
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

#ifndef SFS_FSTRUC_H
#define SFS_FSTRUC_H

/*==============================================================================*/
#if __packed_pragma
#if defined(__RENESAS__)
#pragma pack 1  /* pack structures (byte alignment) */
#else
#pragma pack(1) /* pack structures (byte alignment) */
#endif
#endif

/***** PUT ALL PACKED STRUCTURES IN THIS SECTION *****/

/* Short File Name Dir Entry */
typedef __packed struct
{
    char DIR_Name[PUREFILENAME_LEN]; /* short name - name and extension */
    u8   DIR_Attr;              /* attribute bits */
    u8   NTRes;                 /* 0 */
    u8   CrtTimeTenth;          /* optional */
    u16  CrtTime;               /* optional */
    u16  CrtDate;               /* optional */
    u16  LstAccDate;            /* optional */
    u16  FstClusHI;             /* 0 in FAT16, High 16 bits of cluster in FAT32 */
    u16  WrtTime;               /* write time Second bit  0-4, 2-second count, range(0-29)
                                              Minute bit  5-10
                                              Hour   bit 11-15              */
    u16  WrtDate;               /* write date Day    bit  0-4
                                              Month  bit  5-8
                                              Year   bit  9-15              */
    u16  FstClusLO;
    u32  FileSize;              /* file size (in bytes) */
} __packed_gnu DIRENTRY;

#if SFS_VFAT_SUPPORT
/* LONG File Name Dir Entry */
/* Notice the long file name is UNICODE */
typedef __packed struct
{
    u8   LDIR_Ord;              /* The order of the Entry */
    u8   LDIR_Name1[10];        /* char 1-5 for the long name */
    u8   LDIR_Attr;             /* attribute bits must be SFS_ATTR_LONG_NAME */
    u8   LDIR_Type;             /* 0 */
    u8   LDIR_Chksum;           /* short name's checksume */
    u16  LDIR_Name2[6];         /* char 6-11 for the long name */
    u16  FstClusLO;             /* 0 */
    u32  LDIR_Name3;            /* char 12-13 for the long name */
} __packed_gnu LONGDIRENTRY;
#endif /* SFS_VFAT_SUPPORT */

/* Partition Table structure */
typedef __packed struct
{
    u8   Active;
    u8   StartHead;
    u16  StartCyl;
    u8   IDNumber;
    u8   EndHead;
    u16  EndCyl;
    u32  StartLBA;
    u32  Size;
} __packed_gnu PARTITIONTABLE;

/* BPB for common part of FAT */
typedef __packed struct
{
    u8   JmpBoot[3];
    char OEMName[8];
    u16  BytesPerSec;
    u8   SecPerClu;
    u16  RsvdSecCnt;
    u8   NumFATs;
    u16  RootEntCnt;
    u16  TotSec16;
    u8   Media;
    u16  FATSz16;
    u16  SecPerTrk;
    u16  NumHeads;
    u32  HiddSec;
    u32  TotSec32;

} __packed_gnu BPB;

/* BPB for common part of FAT */
typedef __packed struct
{
    u8   DrvNum;
    u8   Reserved1;
    u8   BootSig;
    u32  VolID;
    char VolLab[11];
    u8   FilSysType[8];

} __packed_gnu BPB_2;

/* BPB for FAT12/16 */
typedef __packed struct
{
    u8   JmpBoot[3];
    char OEMName[8];
    u16  BytesPerSec;
    u8   SecPerClu;
    u16  RsvdSecCnt;
    u8   NumFATs;
    u16  RootEntCnt;
    u16  TotSec16;
    u8   Media;
    u16  FATSz16;
    u16  SecPerTrk;
    u16  NumHeads;
    u32  HiddSec;
    u32  TotSec32;

    BPB_2 BPB2;
} __packed_gnu BPB16;

#if SFS_FAT32_SUPPORT
/* BPB for FAT32 */
typedef __packed struct
{
    u8   JmpBoot[3];
    char OEMName[8];   /* Don't care */
    u16  BytesPerSec;  /* Not must be 512 */
    u8   SecPerClu;    /* 16 - should be set dynamically */
    u16  RsvdSecCnt;   /* 32 */
    u8   NumFATs;      /* 2 */
    u16  RootEntCnt;   /* 0 */
    u16  TotSec16;     /* 0 */
    u8   Media;        /* 0xF8 */
    u16  FATSz16;      /* 0 */
    u16  SecPerTrk;    /* 0 */
    u16  NumHeads;     /* 0 */
    u32  HiddSec;      /* 0 */
    u32  TotSec32;     /* Determined by media size / BytesPerSec */

    u32  FATSz32;      /* by computation */
    u16  ExtFlags;     /* 0 */
    u16  FSVer;        /* 0 */
    u32  RootClus;     /* 2 - logic cluster number */
    u16  FSInfo;       /* 1 */
    u16  BkBootSec;    /* 6 */
    u16  Reserved[6];  /* 0 */

    BPB_2 BPB2;
} __packed_gnu BPB32;
#endif /* SFS_FAT32_SUPPORT */

#if __packed_pragma
#if defined(__RENESAS__)
#pragma unpack /* restore default field alignment */
#else
#pragma pack() /* no par means restore default field alignment */
#endif
#endif
/*==============================================================================*/

/* FAT NODE const structure */
typedef struct
{
    u32  FAT0;
    u32  FAT1;
    u32  FAT_NODE_FREE;
    u32  FAT_NODE_EOC;
    u32  FAT_NODE_BAD;
    u32  FAT_NODE_INVALID;
    const char* FILE_SYSTEM_TYPE;
} FATCONSDEF;

/* Device handle structure */
typedef struct 
{
    uint          iOpenedFileCount;           
    FILESHARED    *pFirstOpenedFile;
    FILEHANDLE    pFirstOpenedFileHandle;
} FILEHANDLELIST;

/* current working directory table */
typedef struct 
{
    SB_OS_TASK_ID TaskID;           
    char szWorkDir[SFS_PATHFILENAME_LEN + 1];
} CWDTABLE, *PCWDTABLE;

#if SFS_FREECLUS_SUPPORT
typedef struct 
{
    u32     dwLastScanned;
    u32     dwTotalScanned;
    u32     dwTotalFreeClus;
    uint    wHead;      /* First Valid item */
    uint    wTail;      /* Last Valid item */
    uint    wSize;      /* Total items */
    uint    wItem;      /* valid item number */
    u8      pContent[SFS_FREECLUS_CACHE_SIZE];
} FREECLUSCACHE, *PFREECLUSCACHE; /* it is actually a ring buffer */

typedef struct 
{
    u32     dwStartCluster;
    u32     dwSize;
} FREECLUSITEM32, *PFREECLUSITEM32;

typedef struct 
{
    u16     wStartCluster;
    u16     wSize;
} FREECLUSITEM16, *PFREECLUSITEM16;
#endif

#if SFS_NUMERICTAIL_CACHE_SUPPORT
typedef struct 
{
    u32     dwPathCluster;
    u32     dwTail;
    uint    iScaned;
    uint    LRU;
} NUMERICTAILCACHE;
#endif

typedef struct 
{
    uint    wLRUCount;  /* LRU */
    uint    wChanged;   /* if Changed flag */
    u32     dwAddress;  /* cluster logic address */
    u8      *pContent;  /* cache , point to the memory */
} CACHEITEM;

typedef struct 
{
    uint      wFatLRU;    /* which fat cache is active currently */
    uint      wDirLRU;    /* which dir cache is active currently */
    uint      wFatItemNum;
    uint      wDirItemNum;
    uint      wDataItemNum;
    CACHEITEM *pFatItem;
    CACHEITEM *pDirItem;
    CACHEITEM *pDataItem;
#if SFS_FREECLUS_SUPPORT
    FREECLUSCACHE FreeCluster;
#endif    
} DEVICECACHE;

typedef struct 
{
    uint iDeviceID;             /* device id */
    uint wFatType;              /* FAT 12 or 16 or 32 */
    uint wStatus;               /* is the file system correctly mounted? */
    uint wFSInfoSector;         /* if we have FSInfo sector */
    int  iLastError;            /* last operation error code */
    u32  dwReservedSecCount;    /* sectors between the first sector of this device and boot sector */
    u32  dwEndSector;           /* End Sector of this partition */  
    u32  dwFirstDataSector;     /* first data sector, relative to the first sector of the volume */
    u32  dwRootDirSector;       /* first root sector, relative to the first sector of the volume */  
    u32  dwRootDirSectors;      /* how many sectors the root dir entry occupy */
    u32  dwCountOfClusters;     /* how many data clusters there */
#if SFS_FREECLUS_SUPPORT    
    uint iFreeClusScanning;     /* Flag if FS is scaning free cluster */
    uint iFreeClusScanNum;      /* sector number each time FS will scan */
#endif    
    /* BPB Values */
    uint dwDirEntryPerClus;     /* dir entry in a cluster */
    uint dwBytesPerCluster;     /* bytes per cluster */
    uint wBytesPerSec;          /* bytes per sector */
    uint wSecPerClu;            /* sector per cluster */
    uint wNumFATs;              /* number of FATs */
    uint wRootEntCnt;           /* FAT12/16 Root directory entry number */
#if SFS_FAT_FSINFO_SUPPORT || SFS_USE_FAT32_FSINFO
    uint iFSInfoModified;       /* Flag if free cluster number need to be updated in FSInfo sector */
#endif
    u32  wRsvdSecCnt;           /* reserved sectors between the first sector of this partition and boot sector */
    u32  dwRootClus;            /* FAT32 Root Cluster number */
    u32  dwLastAllocated;       /* the cluster last allocated */
    u32  dwFreeClus;            /* total free clusters in this volume */
    u32  dwSecondFATOffset;
    
    DIRENTRY   DirEntryBuf;

    FATCONSDEF       *pFD;
    
    const SB_BD_IF   *pDevInterface;  /* device interface */
    SFS_MUTEX_HANDLE pMutex;          /* file operation mutex */

    FILEHANDLELIST   FileHandleList;  /* head of opened file handle linked list */
    DEVICECACHE      *pDeviceCache;   /* device cache */
    u8               *pSectorBuf;     /* sector buffer used before cache is allocated and other purpose */
#if SFS_NUMERICTAIL_CACHE_SUPPORT    
    NUMERICTAILCACHE NumericTailCache[SFS_NUMERICTAIL_CACHE_SIZE];
#endif
} DEVICEHANDLE, *PDEVICEHANDLE;

#endif /* SFS_FSTRUC_H */
