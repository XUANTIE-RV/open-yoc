/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#include <dirent.h>
#include <vfs.h>
#include <vfs_inode.h>
#include <vfs_file.h>
#include <vfs_register.h>
#include <ext4.h>
#include <ext4_super.h>
#include "ext4_vfs.h"

static char *ext4_mnt_path = EXT4_MOUNTPOINT;

typedef struct {
    int dd_vfs_fd;
    int dd_rsv;
    aos_dirent_t *dir;
} ext4_dir_t;

static int _ext4_open(file_t *fp, const char *path, int flags)
{
    int rc;
    ext4_file *file;

    file = aos_malloc(sizeof(ext4_file));
    rc = ext4_fopen2(file, path, flags);
    if (rc != 0) {
        aos_free(file);
        return -rc;
    }

    fp->f_arg = file;

    return rc;
}

static int _ext4_close(file_t *fp)
{
    int rc;
    ext4_file *file;

    file = (ext4_file*)(fp->f_arg);
    rc = ext4_fclose(file);
    aos_free(file);

    return -rc;
}

static ssize_t _ext4_read(file_t *fp, char *buf, size_t len)
{
    int rc;
    ext4_file *file;
    size_t read_bytes;

    file = (ext4_file*)(fp->f_arg);
    rc = ext4_fread(file, buf, len, &read_bytes);

    return rc == 0 ? read_bytes : -rc;
}

static ssize_t _ext4_write(file_t *fp, const char *buf, size_t len)
{
    int rc;
    ext4_file *file;
    size_t write_bytes;

    file = (ext4_file*)(fp->f_arg);
    rc = ext4_fwrite(file, buf, len, &write_bytes);

    return rc == 0 ? write_bytes : -rc;
}

#if 0
static long int _ext4_tell(file_t *fp)
{
    int rc;
    FIL *file;

    file = (FIL*)(fp->f_arg);
    rc = f_tell(file);

    return rc;
}
#endif

static int _ext4_access(file_t *fp, const char *path, int amode)
{
    int rc;
    ext4_file file;

    rc = ext4_fopen(&file, path, "rb");
    if (rc == 0) {
        ext4_fclose(&file);
        return 0;
    }

    return -rc;
}

static off_t _ext4_lseek(file_t *fp, off_t off, int whence)
{
    int rc;
    ext4_file *file;
    int64_t cur_pos, new_pos, size;

    new_pos = 0;
    file = (ext4_file*)(fp->f_arg);
    if (whence == SEEK_SET) {
        new_pos = off;
    } else if (whence == SEEK_CUR) {
        cur_pos = ext4_ftell(file);
        new_pos = cur_pos + off;
    } else if (whence == SEEK_END) {
        size    = ext4_fsize(file);
        new_pos = size + off;
    } else {
        return -EINVAL;
    }

    rc = ext4_fseek(file, new_pos, SEEK_SET);

    return rc == 0 ? new_pos : -rc;
}

static int _ext4_sync(file_t *fp)
{
#if 0
    int rc;
    ext4_file *file;

    file = (ext4_file*)(fp->f_arg);
    rc = f_sync(file);

    return _ext4_ret_to_err(ret);
#else
    //TODO:
    return 0;
#endif
}

static int _ext4_stat(file_t *fp, const char *path, struct aos_stat *st)
{
    int rc;
    ext4_file file;

    rc = ext4_fopen(&file, path, "rb");
    if (rc == 0) {
        //FIXME:
        st->st_size = ext4_fsize(&file);
        uint32_t mode;
        ext4_mode_get(path, &mode);
        st->st_mode = mode;
        ext4_fclose(&file);
        return 0;
    }

    return -rc;
}

static int _ext4_unlink(file_t *fp, const char *path)
{
    int rc;

    rc = ext4_fremove(path);

    return rc ? -rc : 0;
}

static int _ext4_rename(file_t *fp, const char *oldpath, const char *newpath)
{
    int rc;

    rc = ext4_frename(oldpath, newpath);

    return rc ? -rc : 0;
}

static aos_dir_t *_ext4_opendir(file_t *fp, const char *path)
{
    int rc;
    ext4_dir *dir;
    ext4_dir_t *ret_dir;
    aos_dirent_t *dirent;

    dir     = aos_calloc(1, sizeof(ext4_dir));
    dirent  = aos_calloc(1, sizeof(aos_dirent_t) + 256);
    ret_dir = aos_calloc(1, sizeof(ext4_dir_t));

    ret_dir->dir = dirent;
    rc = ext4_dir_open(dir, path);
    if (rc == 0) {
        fp->f_arg = dir;
        return (aos_dir_t*)ret_dir;
    } else {
        aos_free(dir);
        aos_free(ret_dir);
        aos_free(dirent);
        return NULL;
    }
}

static aos_dirent_t *_ext4_readdir(file_t *fp, aos_dir_t *dir)
{
    const ext4_direntry *rentry;
    ext4_dir *dirp = (ext4_dir*)(fp->f_arg);
    ext4_dir_t *dp = (ext4_dir_t*)dir;
    aos_dirent_t *dirent = dp->dir;

    rentry = ext4_dir_entry_next(dirp);
    if (rentry && rentry->name[0] != 0) {
        strncpy(dirent->d_name, (const char*)rentry->name, 255);
        if(EXT4_DE_DIR == rentry->inode_type) {
            dirent->d_type = DT_DIR;
        } else {
            dirent->d_type = DT_REG;
        }

        return dirent;
    }

    return NULL;
}

static int _ext4_closedir(file_t *fp, aos_dir_t *dir)
{
    int rc;
    ext4_dir *dirp;
    ext4_dir_t *dp = (ext4_dir_t*)dir;
    aos_dirent_t *dirent = dp->dir;

    dirp = (ext4_dir*)(fp->f_arg);
    rc = ext4_dir_close(dirp);
    if (rc == 0) {
        aos_free(dirp);
        aos_free(dirent);
        aos_free(dp);
        return rc;
    }

    return -rc;
}

static int _ext4_mkdir(file_t *fp, const char *path)
{
    int rc;

    rc = ext4_dir_mk(path);

    return rc == 0 ? rc : -rc;
}

static int _ext4_rmdir(file_t *fp, const char *path)
{
    int rc;

    rc = ext4_dir_rm(path);

    return rc == 0 ? rc : -rc;
}

static void _ext4_rewinddir(file_t *fp, aos_dir_t *dir)
{
    ext4_dir *dirp;
    ext4_dir_t *dp = (ext4_dir_t *)dir;

    if (dp) {
        dirp = (ext4_dir*)(fp->f_arg);
        ext4_dir_entry_rewind(dirp);
    }

    return;
}

static long _ext4_telldir(file_t *fp, aos_dir_t *dir)
{
    ext4_dir *dirp;
    ext4_dir_t *dp = (ext4_dir_t *)dir;

    if (dp) {
        dirp = (ext4_dir*)(fp->f_arg);

        return (long)(dirp->next_off);
    }

    return -1;
}

static void _ext4_seekdir(file_t *fp, aos_dir_t *dir, long loc)
{
    ext4_dir *dirp;
    ext4_dir_t *dp = (ext4_dir_t *)dir;

    if (dp) {
        dirp = (ext4_dir*)(fp->f_arg);
        dirp->next_off = loc;
    }

    return;
}

static int _ext4_statfs(file_t *fp, const char *path, struct aos_statfs *suf)
{
    int rc;
    struct ext4_sblock *sb = NULL;

    rc = ext4_get_sblock(path, &sb);
    if(rc == 0) {
        suf->f_bsize  = ext4_sb_get_block_size(sb);
        suf->f_blocks = ext4_sb_get_blocks_cnt(sb);
        suf->f_bfree  = ext4_sb_get_free_blocks_cnt(sb);
    }

    return -rc;
}

static const fs_ops_t ext4_ops = {
    .open       = &_ext4_open,
    .close      = &_ext4_close,
    .read       = &_ext4_read,
    .write      = &_ext4_write,
    .access     = &_ext4_access,
    .lseek      = &_ext4_lseek,
    .sync       = &_ext4_sync,
    .stat       = &_ext4_stat,
    .unlink     = &_ext4_unlink,
    .remove     = &_ext4_unlink,
    .rename     = &_ext4_rename,
    .opendir    = &_ext4_opendir,
    .readdir    = &_ext4_readdir,
    .closedir   = &_ext4_closedir,
    .mkdir      = &_ext4_mkdir,
    .rmdir      = &_ext4_rmdir,
    .rewinddir  = &_ext4_rewinddir,
    .telldir    = &_ext4_telldir,
    .seekdir    = &_ext4_seekdir,
    .statfs     = &_ext4_statfs,
    .ioctl      = NULL
};

extern struct ext4_blockdev *ext4_blockdev_mmc_get(void);

int vfs_ext4_register(void)
{
    int rc;
    char *dev_name = "mmc";
    struct ext4_blockdev *bd = NULL;

    bd = ext4_blockdev_mmc_get();
    if (!bd)
        printf("ext4 bd is NULL\n");
    rc = ext4_device_register(bd, dev_name);
    if (rc != 0) {
        printf("ext4 dev register err, rc = %d\n", rc);
        return -1;
    } else {
        rc = ext4_mount(dev_name, ext4_mnt_path, false);
        if (rc != 0) {
            printf("ext4 mount err, rc = %d\n", rc);
            ext4_device_unregister(dev_name);
            return -1;
        } else {
            printf("ext4 mount success!!\n");
        }
    }
    rc = aos_register_fs(ext4_mnt_path, &ext4_ops, bd);
    if (rc) {
        printf("aos register ext4 failed. ret:%d\n", rc);
    }
    return rc;
}

int vfs_ext4_unregister(void)
{
    int rc;

    rc = ext4_umount(ext4_mnt_path);
    if (rc != 0) {
        printf("ext4 umount err, rc = %d\n", rc);
    }

    return aos_unregister_fs(ext4_mnt_path);
}

