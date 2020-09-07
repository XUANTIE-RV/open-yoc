/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include <vfs.h>
#include <vfs_inode.h>
#include <vfs_file.h>
#include <vfs_register.h>

#include "ff.h"

static const char *fatfs_mnt_path = "/fatfs0";

typedef struct {
    int dd_vfs_fd;
    int dd_rsv;
    aos_dirent_t *dir;
} fatfs_dir_t;

#if 0
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

    len = len - prefix_len;
    relpath = (char *)aos_malloc(len + 1);
    if (!relpath) {
        return NULL;
    }

    memset(relpath, 0, len + 1);

    p = (char *)(path + prefix_len);
    memcpy(relpath, p, len);
    relpath[len] = '\0';

    return relpath;
}
#endif

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

static int _fatfs_open(file_t *fp, const char *path, int flags)
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

static int _fatfs_close(file_t *fp)
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

static ssize_t _fatfs_read(file_t *fp, char *buf, size_t len)
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

static ssize_t _fatfs_write(file_t *fp, const char *buf, size_t len)
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

static off_t _fatfs_lseek(file_t *fp, off_t off, int whence)
{
    FRESULT ret;
    FIL *file;

    if (whence == SEEK_CUR || whence == SEEK_END) {
        return -EPERM;
    }

    file = (FIL*)(fp->f_arg);
    ret = f_lseek(file, off);

    return _fatfs_ret_to_err(ret);
}

static int _fatfs_sync(file_t *fp)
{
    FRESULT ret;
    FIL *file;

    file = (FIL*)(fp->f_arg);
    ret = f_sync(file);

    return _fatfs_ret_to_err(ret);
}

static int _fatfs_stat(file_t *fp, const char *path, struct stat *st)
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

static int _fatfs_unlink(file_t *fp, const char *path)
{
    FRESULT ret;
 
    ret = f_unlink(path);

    return _fatfs_ret_to_err(ret);
}

static int _fatfs_rename(file_t *fp, const char *oldpath, const char *newpath)
{
    FRESULT ret;

    ret = f_rename(oldpath, newpath);

    return _fatfs_ret_to_err(ret);
}

static aos_dir_t *_fatfs_opendir(file_t *fp, const char *path)
{
    FRESULT ret;

    FF_DIR *dir = aos_malloc(sizeof(FF_DIR));

    if (dir == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    aos_dirent_t *dirnet = aos_malloc(sizeof(aos_dirent_t) + 256);

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
        return (aos_dir_t*)ret_dir;
    } else {
        aos_free(dir);
        aos_free(ret_dir);
        aos_free(dirnet);
        errno = _fatfs_ret_to_err(ret);
        return NULL;
    }
}

static aos_dirent_t *_fatfs_readdir(file_t *fp, aos_dir_t *dir)
{
    FRESULT ret;
    FF_DIR *dirp;
    FILINFO fno;
    fatfs_dir_t *dp = (fatfs_dir_t*)dir;
    aos_dirent_t *dirent = dp->dir;

    dirp = (FF_DIR*)(fp->f_arg);
    ret = f_readdir(dirp, &fno);

    if (ret == FR_OK && fno.fname[0] != 0) {
        strncpy(dirent->d_name, fno.fname, 255);

        if (fno.fattrib & AM_DIR) {
            dirent->d_type = DT_DIR;
        } else {
            dirent->d_type = DT_REG;
        }

        return dirent;
    }

    return NULL;
}

static int _fatfs_closedir(file_t *fp, aos_dir_t *dir)
{
    FRESULT ret;
    FF_DIR *dirp;
    fatfs_dir_t *dp = (fatfs_dir_t*)dir;
    aos_dirent_t *dirent = dp->dir;

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

static int _fatfs_mkdir(file_t *fp, const char *path)
{
    FRESULT ret;

    ret = f_mkdir(path);

    ret = _fatfs_ret_to_err(ret);

    return ret;
}

static int _fatfs_rmdir(file_t *fp, const char *path)
{
    FRESULT ret;

    ret = f_rmdir(path);

    ret = _fatfs_ret_to_err(ret);

    return ret;
}

static const fs_ops_t fatfs_ops = {
    .open       = &_fatfs_open,
    .close      = &_fatfs_close,
    .read       = &_fatfs_read,
    .write      = &_fatfs_write,
    .lseek      = &_fatfs_lseek,
    .sync       = &_fatfs_sync,
    .stat       = &_fatfs_stat,
    .unlink     = &_fatfs_unlink,
    .rename     = &_fatfs_rename,
    .opendir    = &_fatfs_opendir,
    .readdir    = &_fatfs_readdir,
    .closedir   = &_fatfs_closedir,
    .mkdir      = &_fatfs_mkdir,
    .rmdir      = &_fatfs_rmdir,
    .ioctl      = NULL
};

int vfs_fatfs_register(void)
{
    FRESULT ret;
    FATFS *fs;

    fs = aos_malloc(sizeof(FATFS));

    if (fs == NULL) {
        return -1;
    }

    ret = f_mount(fs, "", 1);

    if (ret != FR_OK) {
        return -1;
    }

    return aos_register_fs(fatfs_mnt_path, &fatfs_ops, fs);
}


#if 0
int vfs_fatfs_unregister(void)
{
	FRESULT ret;

    SPIFFS_unmount(g_spiffs_mgr->fs);
    ret = f_mount(NULL, "", 1);
	if (ret != FR_OK){
		return -1;
	}
    return aos_unregister_fs(fatfs_mnt_path);
}
#endif
