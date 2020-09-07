/*
* fapi.h                                                    Version 2.25
*
* smxFS API Defines and Prototypes.
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

#ifndef SFS_FAPI_H
#define SFS_FAPI_H

#include "bbd.h"

/* seek methods for sfs_fseek() */
#define SFS_SEEK_SET             0
#define SFS_SEEK_CUR             1
#define SFS_SEEK_END             2

/* device status for sfs_devstatus() */
#define SFS_DEVICE_NOT_FOUND     0x0000
#define SFS_DEVICE_MOUNTING      0x0001
#define SFS_DEVICE_MOUNTED       0x0002
#define SFS_DEVICE_UNFORMATTED   0x0004
#define SFS_DEVICE_FAT32_UNSUP   0x0008     /* only when you disable FAT32 but try to mount a FAT32 disk */
#define SFS_DEVICE_NOT_SHUT_DOWN 0x8000     /* formatted but not clear shutdown flag */

/* return value for sfs_getlasterror() */
#define SFS_ERR_NO_ERROR         0
#define SFS_ERR_DISK_REMOVED     -1
#define SFS_ERR_DISK_IO          -2
#define SFS_ERR_DISK_NOT_MOUNTED -3
#define SFS_ERR_INVALID_BOOTSEC  -10
#define SFS_ERR_INVALID_DIR      -11
#define SFS_ERR_INVALID_FAT      -12
#define SFS_ERR_INVALID_PAR      -13
#define SFS_ERR_FAT32_NOT_SUPP   -19
#define SFS_ERR_DIR_FULL         -20
#define SFS_ERR_DISK_FULL        -21
#define SFS_ERR_DISK_WP          -22
#define SFS_ERR_FILE_EXIST       -23
#define SFS_ERR_FILE_NOT_EXIST   -24
#define SFS_ERR_FILE_WP          -25
#define SFS_ERR_OUT_OF_MEM       -100

/* return value for sfs_chkdsk() */
#define SFS_CHKERR_NO_ERROR      0x0000
#define SFS_CHKERR_INV_FILENAME  0x0001
#define SFS_CHKERR_INV_FILETIME  0x0002
#define SFS_CHKERR_INV_FILELEN   0x0004
#define SFS_CHKERR_INV_FATNODE   0x0008
#define SFS_CHKERR_INV_FILEENTRY 0x0010
#define SFS_CHKERR_INV_DOTDIR    0x0020
#define SFS_CHKERR_INV_FIRSTCLUS 0x0040
#define SFS_CHKERR_INV_CLUSTER   0x0080
#define SFS_CHKERR_FAT_CROSSLINK 0x0100
#define SFS_CHKERR_FAT_LOSTCHAIN 0x0200
#define SFS_CHKERR_DEVICE_RD_WR  0x0400
#define SFS_CHKERR_FLUSH_FAILED  0x0800
#define SFS_CHKERR_BUF_OVERFLOW  0x2000
#define SFS_CHKERR_STILL_IN_USE  0x4000
#define SFS_CHKERR_OUT_OF_MEM    0x8000

/* fix flag for sfs_chkdsk() */
#define SFS_FIX_AUTO             0x0001
#define SFS_FIX_SAVE_LOST_CHAIN  0x0002
#define SFS_FIX_COPY_CROSS_CLUS  0x0004

/* attributes for sfs_getprop(), sfs_setprop() */
#define SFS_ATTR_READ_ONLY       0x01
#define SFS_ATTR_HIDDEN          0x02
#define SFS_ATTR_SYSTEM          0x04
#define SFS_ATTR_VOLUME_ID       0x08
#define SFS_ATTR_DIRECTORY       0x10
#define SFS_ATTR_ARCHIVE         0x20

#define SFS_MAX_PARTITION_NUM    4

#if !SFS_READONLY
/* flags for sfs_setprop() */
#define SFS_SET_ATTRIBUTE        0x01
#define SFS_SET_CREATETIME       0x02
#define SFS_SET_WRITETIME        0x04
#define SFS_SET_CHANGEMOD        0x08     /* internal used by chmod() */
#endif


/* traditional names for bits in st_mode */
#define S_IFMT                   0xF000   /* file type mask */
#define S_IFDIR                  0x4000   /* directory */
#define S_IFIFO                  0x1000   /* FIFO special */
#define S_IFCHR                  0x2000   /* character special */
#define S_IFBLK                  0x3000   /* block special */
#define S_IFREG                  0x8000   /* or just 0x0000, regular */
#define S_IREAD                  0x0100   /* owner may read */
#define S_IWRITE                 0x0080   /* owner may write */
#define S_IEXEC                  0x0040   /* owner may execute <directory search> */
#define _S_IFMT                  S_IFMT
#define _S_IFDIR                 S_IFDIR
#define _S_IFIFO                 S_IFIFO
#define _S_IFCHR                 S_IFCHR
#define _S_IFBLK                 S_IFBLK
#define _S_IFREG                 S_IFREG
#define _S_IREAD                 S_IREAD
#define _S_IWRITE                S_IWRITE
#define _S_IEXEC                 S_IEXEC

#if SFS_FORMAT_SUPPORT
/* New Partition Information */
typedef struct
{
    uint     ActivePartition;    /* Which partition is active, from 0 - 3. */
    uint     ForceCreate;        /* Set to 1 to force creating partition table even if sector is boot sector (special case) or blank. */
    uint     SecPerTrack;        /* Used to convert the LBA sector to CHS. Set to 0 if you do not care about CHS fields. */
    uint     HeadPerCyl;         /* Used to convert the LBA sector to CHS. Set to 0 if you do not care about CHS fields. */
    uint     ReservedSectors;    /* Number of reserved sectors between partition table and the FIRST partition. */
    u8       IDNumber[SFS_MAX_PARTITION_NUM];  /* ID numbers for each of the partitions. */
    u32      Size[SFS_MAX_PARTITION_NUM];      /* Size of each partition (number of sectors). */ 
    u8 *     pMBRProg;           /* MBR boot code or NULL. It will be copied to the the beginning (1st byte) of the MBR. */
    uint     MBRProgSize;        /* The size of the MBR boot code. Must be <= 446 bytes to fit before partition table. */
} PARTITIONINFO;

/* New Boot Sector Information */
typedef struct
{
    uint     wFATNum;            /* Number of FAT tables to create. Normally 2. */
    uint     wRootDirNum;        /* Number of root directory entries to create. Normally is 512. Not used for FAT32. */
    u32      dwVolumeID;         /* The disk's volume ID, such as 1234ABCD. */
    const char *VolumeLabel;     /* Volume label such as "NO NAME". Max 11 chars (not including NUL) */
    u8       bMediaType;         /* 0xF0 removable media, 0xF8-0xFF fixed media. If 0, default value is used. */
    const u8 *pBootProg;         /* Boot sector boot code or NULL. This will be copied to the area after the BPB in the boot sector. */
    uint     BootProgSize;       /* The size of the boot sector boot code. Must be <= 510-sizeof(BPB16 or BPB32) bytes to fit in space allotted. */
} FORMATINFO;
#endif

typedef struct stat
{ 
    uint     st_mode;                 /* file mode; */
    u32      st_size;                 /* file size */
    DATETIME st_ctime;                /* create time */
    DATETIME st_mtime;                /* modify time */
    DATETIME st_atime;                /* access time */
    uint     st_dev;                  /* device number */
    u8       name[SFS_PATHFILENAME_LEN];  /* full path name */
    uint     bAttr;                   /* file attribute; if it is a dir, bAttr & SFS_ATTR_DIRECTORY is set */
    u32      dwPathCluster;           /* entry belongs to a path, this is the first cluster of the path */
    u32      dwEntryCluster;          /* entry belongs to a cluster, this is the cluster number */
    uint     dwOffset;                /* offset of dir entry relative to dwEntryCluster */
    u32      dwNextClusterInEntry;    /* entry has a next cluster, for a dir, it is dir content , for a file , it is a linked file clusters */
#if SFS_FINDFIRST_SUPPORT
    u32      dwLastFoundEntryCluster; /* used in finding file */
    u8       *pFindSpec;
#endif
} FINDDIRENTRY;

typedef FINDDIRENTRY FILEINFO;

typedef struct tFileShared
{
    u32      dwPathCluster;
    u32      dwEntryCluster;
    u32      dwFstCluster;
    u32      FileSize;
    uint     dwOffset;
    u16      bAttr;
    u16      wReferenceNum;
    struct   tFileShared *pNextShared;
    u8       *pFileName;     /* for debug purpose only */
} FILESHARED;

typedef struct tFileStruc
{
    uint     wDevice;
    uint     cFileStatus;
    u32      uFilePointer;
    u32      dwFPCluster;
    u8       *pBuf;
    FILESHARED *pFileShared;
    struct   tFileStruc *pNextFile;
} FILESTRUCT, *FILEHANDLE;

#ifdef __cplusplus
extern "C" {
#endif

int           sfs_init(void);
int           sfs_exit(void);
int           sfs_devreg(const SB_BD_IF *dev_if, uint nID);
int           sfs_devunreg(uint nID);
int           sfs_devstatus(uint nID);
const SB_BD_IF *sfs_getdev(uint nID);

FILEHANDLE    sfs_fopen(const char *filename, const char *mode);
int           sfs_fclose(FILEHANDLE filehandle);
size_t        sfs_fread(void * buf, size_t size, size_t items, FILEHANDLE filehandle);
#if !SFS_READONLY
size_t        sfs_fwrite(void * buf, size_t size, size_t items, FILEHANDLE filehandle);
#endif
int           sfs_fseek(FILEHANDLE filehandle, long lOffset, int nMethod);
#if !SFS_READONLY
int           sfs_fdelete(const char * filename);
int           sfs_fflush(FILEHANDLE filehandle);
#endif
unsigned long sfs_filelength(const char *filename);
int           sfs_findfile(const char *filename);

#if !SFS_READONLY
#if SFS_MKDIR_SUPPORT
int           sfs_mkdir(const char *path);
int           sfs_rmdir(const char *path);
#endif
#endif

#if SFS_CWD_SUPPORT
#define       sfs_chdir sfs_setcwd
int           sfs_setcwd(const char *path);
char *        sfs_getcwd(char * buffer, int maxlen);
#endif

unsigned long sfs_freekb(uint nID);
unsigned long sfs_totalkb(uint nID);
int           sfs_ioctl(uint nID, uint command, void * par);
int           sfs_writeprotect(uint nID);
int           sfs_flushall(uint nID);
int           sfs_getlasterror(uint nID);

#if SFS_CHKDSK_SUPPORT
int           sfs_chkdsk(uint nID, uint iFixFlag, char *pResultBuf, uint iBufLen);
#endif

#if !SFS_READONLY
#if SFS_PROPERTY_SUPPORT
int           sfs_chmod(const char * filename, uint pmode);
#endif
#if SFS_FULL_FEATURES_SUPPORT
int           sfs_copy(const char * src, const char * dest);
int           sfs_delmany(const char * filelist, int num);
#endif
#endif

int           sfs_feof(FILEHANDLE filehandle);

#if SFS_FINDFIRST_SUPPORT
int           sfs_findfirst(const char * filespec, FILEINFO* fileinfo);
int           sfs_findnext(int id, FILEINFO* fileinfo);
int           sfs_findclose(FILEINFO* fileinfo);
FILEHANDLE    sfs_fastfopen(FILEINFO* fileinfo, BOOLEAN bReadOnly);
#endif

#if !SFS_READONLY
#if SFS_FORMAT_SUPPORT
int           sfs_format(uint nID, FORMATINFO * formatinfo);
#endif
#endif

long          sfs_ftell(FILEHANDLE filehandle);

#if !SFS_READONLY
#if SFS_FULL_FEATURES_SUPPORT
int           sfs_ftruncate(FILEHANDLE filehandle);
#endif
#endif

long          sfs_getclustersize(uint nID);

#if SFS_PROPERTY_SUPPORT
int           sfs_getprop(const char * filename, FILEINFO * fileinfo);
#endif

#if SFS_VOLUME_SUPPORT
int           sfs_getvolname(uint nID, char * name);
#endif

#if !SFS_READONLY
#define       sfs_move sfs_rename
#if SFS_FORMAT_SUPPORT
int           sfs_partition(uint nID, PARTITIONINFO * partitioninfo);
#endif
#define       sfs_remove sfs_fdelete
#if SFS_RENAME_SUPPORT
int           sfs_rename(const char * oldname, const char * newname);
#endif
#endif

void          sfs_rewind(FILEHANDLE filehandle);

#if SFS_FREECLUS_SUPPORT
unsigned long sfs_scanfreeclus(uint nID, uint iScanSectorNum);
#endif

int           sfs_setpassthrough(FILEHANDLE filehandle, BOOLEAN bEnable);

#if !SFS_READONLY
#if SFS_PROPERTY_SUPPORT
int           sfs_setprop(const char * filename, FILEINFO* fileinfo, uint flag);
#endif
#if SFS_VOLUME_SUPPORT
int           sfs_setvolname(uint nID, const char * name);
#endif
#endif

#if SFS_PROPERTY_SUPPORT
int           sfs_stat(const char * filename, FILEINFO * fileinfo);
#if !SFS_READONLY
int           sfs_timestamp(const char * filename, DATETIME* datetime);
#endif
#endif


#if defined(SMXAWARE)
int           sfs_getdevhandlesize(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SFS_FAPI_H */

