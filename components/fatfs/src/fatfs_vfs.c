/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <dirent.h>
#include <vfs.h>
#include <vfs_inode.h>
#include <vfs_file.h>
#include <vfs_register.h>

#include "fatfs_vfs.h"
#include "ff.h"

static char *fatfs_mnt_path = SD_FATFS_MOUNTPOINT;

typedef struct {
    int dd_vfs_fd;
    int dd_rsv;
    vfs_dirent_t *dir;
} fatfs_dir_t;

static char *translate_relative_path(const char *path)
{
    int len, prefix_len;
    char *relpath, *p;

    if (!path) {
        return NULL;
    }

    len = strlen(path);
    prefix_len = strlen(fatfs_mnt_path);
    if (strncmp(fatfs_mnt_path, path, prefix_len) != 0) {
        return NULL;
    }

    //len = len - prefix_len;
    relpath = (char *)aos_malloc(len + 1);
    if (!relpath) {
        return NULL;
    }

    memset(relpath, 0, len + 1);

    //p = (char *)(path + prefix_len);
    p = (char *)path;
    memcpy(relpath, p, len);
    relpath[len] = '\0';

    return relpath;
}

static int _fatfs_mode_conv(int flags)
{
    int acc_mode, res = 0;
    acc_mode = flags & O_ACCMODE;
    if (acc_mode == O_RDONLY) {
        res |= FA_READ;
    } else if (acc_mode == O_WRONLY) {
        res |= FA_WRITE;
    } else if (acc_mode == O_RDWR) {
        res |= FA_READ | FA_WRITE;
    }

    if ((flags & O_CREAT) && (flags & O_EXCL)) {
        res |= FA_CREATE_NEW;
    } else if ((flags & O_CREAT) && (flags & O_TRUNC)) {
        res |= FA_CREATE_ALWAYS;
    } else if (flags & O_APPEND) {
        res |= FA_OPEN_APPEND;
    } else if (flags & O_CREAT) {
        res |= FA_OPEN_ALWAYS;
    }
    return res;
}

static int _fatfs_ret_to_err(int ret)
{
    switch (ret) {
    case FR_OK:
        return 0;
    case FR_NO_FILESYSTEM:
    case FR_NOT_ENABLED:
    case FR_DISK_ERR:
    case FR_INVALID_DRIVE:
    case FR_NOT_READY:
        return -ENODEV;
    case FR_TOO_MANY_OPEN_FILES:
        return -EMFILE;
    case FR_EXIST:
        return -EEXIST;
    case FR_INVALID_PARAMETER:
        return -EINVAL;
    case FR_NO_FILE:
    case FR_NO_PATH:
        return -ENOENT;
    case FR_INVALID_NAME:
        return -ENAMETOOLONG;
    case FR_LOCKED:
        return -ETXTBSY;
    default:
        return -EIO;
    }
}

static int _fatfs_open(vfs_file_t *fp, const char *path, int flags)
{
    FRESULT ret;
    //char *relpath = NULL;

    FIL *file = aos_malloc(sizeof(FIL));

    if (file == NULL) {
        return -ENOMEM;
    }

    ret = f_open(file, path, _fatfs_mode_conv(flags));

    if (ret == FR_OK) {

        fp->f_arg = file;
    }

    ret = _fatfs_ret_to_err(ret);

    return ret;
}

static int _fatfs_close(vfs_file_t *fp)
{
    FRESULT ret;
    FIL *file;

    file = (FIL*)(fp->f_arg);
    ret = f_close(file);

    if (ret == FR_OK) {
        aos_free(file);
    }

    return  _fatfs_ret_to_err(ret);
}

static ssize_t _fatfs_read(vfs_file_t *fp, char *buf, size_t len)
{
    FRESULT ret;
    FIL *file;
    uint32_t read_byte;

    file = (FIL*)(fp->f_arg);
    ret = f_read(file, buf, len, &read_byte);

    if (ret == FR_OK) {
        return read_byte;
    }

    return _fatfs_ret_to_err(ret);
}

static ssize_t _fatfs_write(vfs_file_t *fp, const char *buf, size_t len)
{
    FRESULT ret;
    FIL *file;
    uint32_t write_byte;

    file = (FIL*)(fp->f_arg);
    ret = f_write(file, buf, len, &write_byte);

    if (ret == FR_OK) {
        return write_byte;
    }

    return _fatfs_ret_to_err(ret);
}

#if 0
static long int _fatfs_tell(vfs_file_t *fp)
{
    FRESULT ret;
    FIL *file;

    file = (FIL*)(fp->f_arg);
    ret = f_tell(file);

    return ret;
}
#endif

static int _fatfs_access(vfs_file_t *fp, const char *path, int amode)
{
    char *relpath = NULL;
    FILINFO info;
    int32_t len, ret;

#if FF_FS_READONLY
    if (amode == W_OK) {
        return -EACCES;
    }
#endif

    relpath = translate_relative_path(path);
    if (!relpath) {
        return -EINVAL;
    }

    len = strlen(relpath);
    if (relpath[len - 1] == ':' && relpath[len] == '\0') {
        ret = FR_OK;
    } else {
        ret = f_stat(relpath, &info);
    }

    aos_free(relpath);
    return ret;
}

static off_t _fatfs_lseek(vfs_file_t *fp, off_t off, int whence)
{
    int64_t cur_pos, new_pos, size;

    int32_t ret = -EPERM;

    FIL *f = (FIL *)(fp->f_arg);

    new_pos = 0;

    if (f) {
        if (whence == SEEK_SET) {
            new_pos = off;
        } else if (whence == SEEK_CUR) {
            cur_pos = f_tell(f);
            new_pos = cur_pos + off;
        } else if (whence == SEEK_END) {
            size    = f_size(f);
            new_pos = size + off;
        } else {
            return -EINVAL;
        }

        if ((ret = f_lseek(f, new_pos)) != FR_OK) {
            return ret;
        }
    }

    return new_pos;
}

static int _fatfs_sync(vfs_file_t *fp)
{
    FRESULT ret;
    FIL *file;

    file = (FIL*)(fp->f_arg);
    ret = f_sync(file);

    return _fatfs_ret_to_err(ret);
}

static int _fatfs_fstat(vfs_file_t *fp, vfs_stat_t *st)
{
    FIL *file = (FIL*)(fp->f_arg);

    if (file) {
        st->st_mode = S_IFREG;
        st->st_size = file->obj.objsize;
        return 0;
    }

    return -1;
}

static int _fatfs_stat(vfs_file_t *fp, const char *path, vfs_stat_t *st)
{
    FRESULT ret;
    FILINFO fno = {0};

    ret = f_stat(path, &fno);

    if (ret == FR_OK) {
        st->st_size = fno.fsize;
        //st->st_mtime = fno.ftime;
        if (fno.fattrib & AM_DIR) {
            st->st_mode  = S_IFDIR;
        } else {
            st->st_mode  = S_IFREG;
        }
    }

    return  _fatfs_ret_to_err(ret);
}

static int _fatfs_unlink(vfs_file_t *fp, const char *path)
{
    FRESULT ret;

    ret = f_unlink(path);

    return _fatfs_ret_to_err(ret);
}

static int _fatfs_rename(vfs_file_t *fp, const char *oldpath, const char *newpath)
{
    FRESULT ret;

    ret = f_rename(oldpath, newpath);

    return _fatfs_ret_to_err(ret);
}

static int _fatfs_truncate(vfs_file_t *fp, off_t len)
{
    FRESULT ret;
    FIL *file;

    file = (FIL*)(fp->f_arg);
    if (!(len >= 0 && len < file->obj.objsize)) {
        return -EINVAL;
    }
    file->fptr = len;
    ret = f_truncate(file);

    return _fatfs_ret_to_err(ret);
}

static vfs_dir_t *_fatfs_opendir(vfs_file_t *fp, const char *path)
{
    FRESULT ret;

    FF_DIR *dir = aos_malloc(sizeof(FF_DIR));

    if (dir == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    vfs_dirent_t *dirnet = aos_malloc(sizeof(vfs_dirent_t) + FF_LFN_BUF + 1);

    if (dirnet == NULL) {
        aos_free(dir);
        errno = ENOMEM;
        return NULL;
    }

    fatfs_dir_t *ret_dir = aos_malloc(sizeof(fatfs_dir_t));

    if (ret_dir == NULL) {
        aos_free(dir);
        aos_free(dirnet);
        errno = ENOMEM;
        return NULL;
    }

    ret_dir->dir = dirnet;

    ret = f_opendir(dir, path);

    if (ret == FR_OK) {
        fp->f_arg = dir;
        return (vfs_dir_t*)ret_dir;
    } else {
        aos_free(dir);
        aos_free(ret_dir);
        aos_free(dirnet);
        errno = _fatfs_ret_to_err(ret);
        return NULL;
    }
}

static vfs_dirent_t *_fatfs_readdir(vfs_file_t *fp, vfs_dir_t *dir)
{
    FRESULT ret;
    FF_DIR *dirp;
    FILINFO fno;
    fatfs_dir_t *dp = (fatfs_dir_t*)dir;
    vfs_dirent_t *dirent = dp->dir;

    dirp = (FF_DIR*)(fp->f_arg);
    ret = f_readdir(dirp, &fno);

    if (ret == FR_OK && fno.fname[0] != 0) {
        strncpy(dirent->d_name, fno.fname, FF_LFN_BUF);
        dirent->d_name[FF_LFN_BUF] = 0;

        if (fno.fattrib & AM_DIR) {
            dirent->d_type = DT_DIR;
        } else {
            dirent->d_type = DT_REG;
        }

        return dirent;
    }

    return NULL;
}

static int _fatfs_closedir(vfs_file_t *fp, vfs_dir_t *dir)
{
    FRESULT ret;
    FF_DIR *dirp;
    fatfs_dir_t *dp = (fatfs_dir_t*)dir;
    vfs_dirent_t *dirent = dp->dir;

    dirp = (FF_DIR*)(fp->f_arg);
    ret = f_closedir(dirp);

    if (ret == FR_OK) {
        aos_free(dirp);
        aos_free(dirent);
        aos_free(dp);
    }

    ret = _fatfs_ret_to_err(ret);

    return ret;
}

static int _fatfs_mkdir(vfs_file_t *fp, const char *path)
{
    FRESULT ret;

    ret = f_mkdir(path);

    ret = _fatfs_ret_to_err(ret);

    return ret;
}

static int _fatfs_rmdir(vfs_file_t *fp, const char *path)
{
    FRESULT ret;

    ret = f_rmdir(path);

    ret = _fatfs_ret_to_err(ret);

    return ret;
}

static void _fatfs_rewinddir(vfs_file_t *fp, vfs_dir_t *dir)
{
    FF_DIR *dirp;
    fatfs_dir_t *dp = (fatfs_dir_t *)dir;

    if (!dp) {
        return;
    }

    dirp = (FF_DIR*)(fp->f_arg);
    f_rewinddir(dirp);

    return;
}

static long _fatfs_telldir(vfs_file_t *fp, vfs_dir_t *dir)
{
    FF_DIR *dirp;
    fatfs_dir_t *dp = (fatfs_dir_t *)dir;

    if (!dp) {
        return -1;
    }

    dirp = (FF_DIR*)(fp->f_arg);

    return (long)(dirp->dptr);
}

static void _fatfs_seekdir(vfs_file_t *fp, vfs_dir_t *dir, long loc)
{
    FF_DIR *dirp;
    fatfs_dir_t *dp = (fatfs_dir_t *)dir;

    if (!dp) {
        return;
    }

    dirp = (FF_DIR*)(fp->f_arg);
    dirp->dptr = loc;

    return;
}

static int _fatfs_statfs (vfs_file_t *fp, const char *path, vfs_statfs_t *suf)
{
    FATFS *fs;
    int32_t ret = -EPERM;
    char *relpath = NULL;
    DWORD fre_clust, fre_sect, tot_sect;

    relpath = translate_relative_path(path);
    if (!relpath) {
        return -EINVAL;
    }
    /* Get volume information and free clusters*/
    ret = f_getfree(relpath, &fre_clust, &fs);
    if (ret != FR_OK) {
        aos_free(relpath);
        return ret;
    }

    /* Get total sectors and free sectors */
    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;
    suf->f_blocks = tot_sect;
    suf->f_bfree = fre_sect;
#if FF_MAX_SS != FF_MIN_SS
    suf->f_bsize = fs->ssize;
#else
    suf->f_bsize = FF_MIN_SS;
#endif

    aos_free(relpath);

    return ret;
}

static const vfs_fs_ops_t fatfs_ops = {
    .open       = &_fatfs_open,
    .close      = &_fatfs_close,
    .read       = &_fatfs_read,
    .write      = &_fatfs_write,
    .access     = &_fatfs_access,
    .lseek      = &_fatfs_lseek,
    .sync       = &_fatfs_sync,
    .stat       = &_fatfs_stat,
    .fstat      = &_fatfs_fstat,
    .unlink     = &_fatfs_unlink,
    .remove     = &_fatfs_unlink,
    .rename     = &_fatfs_rename,
    .truncate   = &_fatfs_truncate,
    .opendir    = &_fatfs_opendir,
    .readdir    = &_fatfs_readdir,
    .closedir   = &_fatfs_closedir,
    .mkdir      = &_fatfs_mkdir,
    .rmdir      = &_fatfs_rmdir,
    .rewinddir  = &_fatfs_rewinddir,
    .telldir    = &_fatfs_telldir,
    .seekdir    = &_fatfs_seekdir,
    .statfs     = &_fatfs_statfs,
    .ioctl      = NULL
};

int vfs_fatfs_register(void)
{
    FRESULT ret;
    static FATFS *fs = NULL;

    if (fs == NULL) {
        fs = aos_malloc(sizeof(FATFS));
        if (fs == NULL)
            return -1;
    }

    ret = f_mount(fs, fatfs_mnt_path, 1);

    if (ret != FR_OK) {
        return -1;
    }

    return vfs_register_fs(fatfs_mnt_path, &fatfs_ops, fs);
}

int vfs_fatfs_unregister(void)
{
    f_mount(NULL, "", 1);
    return vfs_unregister_fs(fatfs_mnt_path);
}

