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
    vfs_dirent_t *dir;
} ext4_dir_t;

/* path convert */
static char *path_convert(const char *path)
{
    int len;
    char *target_path, *p;

    if (path == NULL) {
        return NULL;
    }

    len = strlen(path);
    target_path =(char *)aos_zalloc(len + 1);
    if (target_path == NULL) {
        return NULL;
    }
    if (strcmp(path, ext4_mnt_path) == 0) {
        strcpy(target_path, path);
        return target_path;
    }
    p = (char *)path;
    int count = 0;
    for (int i = 0; i < len; i++) {
        if (*p == '/' && *(p + 1) == '/' && i < len - 1) {
            target_path[count++] = '/';
            p++;
            ++i;
        } else {
            target_path[count++] = *p;
        }
        p++;
    }
    if (target_path[len - 1] == '/') {
        target_path[len - 1] = 0;
    }

    return target_path;
}

static int _ext4_open(vfs_file_t *fp, const char *path, int flags)
{
    int rc;
    ext4_file *file;

    char *target_path = path_convert(path);
    if (target_path == NULL) {
        return -EINVAL;
    }

    file = aos_malloc(sizeof(ext4_file));
    rc = ext4_fopen2(file, target_path, flags);
    if (rc != 0) {
        aos_free(file);
        aos_free(target_path);
        return -rc;
    }

    fp->f_arg = file;
    aos_free(target_path);
    return rc;
}

static int _ext4_close(vfs_file_t *fp)
{
    int rc;
    ext4_file *file;

    file = (ext4_file*)(fp->f_arg);
    rc = ext4_fclose(file);
    aos_free(file);

    return -rc;
}

static ssize_t _ext4_read(vfs_file_t *fp, char *buf, size_t len)
{
    int rc;
    ext4_file *file;
    size_t read_bytes;

    file = (ext4_file*)(fp->f_arg);
    rc = ext4_fread(file, buf, len, &read_bytes);

    return rc == 0 ? read_bytes : -rc;
}

static ssize_t _ext4_write(vfs_file_t *fp, const char *buf, size_t len)
{
    int rc;
    ext4_file *file;
    size_t write_bytes;

    file = (ext4_file*)(fp->f_arg);
    rc = ext4_fwrite(file, buf, len, &write_bytes);

    return rc == 0 ? write_bytes : -rc;
}

#if 0
static long int _ext4_tell(vfs_file_t *fp)
{
    int rc;
    FIL *file;

    file = (FIL*)(fp->f_arg);
    rc = f_tell(file);

    return rc;
}
#endif

static int _ext4_access(vfs_file_t *fp, const char *path, int amode)
{
    int rc;
    ext4_file file;

    char *target_path = path_convert(path);
    if (target_path == NULL) {
        return -EINVAL;
    }

    rc = ext4_fopen(&file, target_path, "rb");
    if (rc == 0) {
        ext4_fclose(&file);
        aos_free(target_path);
        return 0;
    }

    aos_free(target_path);

    return -rc;
}

static off_t _ext4_lseek(vfs_file_t *fp, off_t off, int whence)
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

static int _ext4_sync(vfs_file_t *fp)
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

static int _ext4_stat(vfs_file_t *fp, const char *path, vfs_stat_t *st)
{
    int rc;
    ext4_file file;

    char *target_path = path_convert(path);
    if (target_path == NULL) {
        return -EINVAL;
    }

    rc = ext4_fopen(&file, target_path, "rb");
    if (rc == 0) {
        //FIXME:
        st->st_size = ext4_fsize(&file);
        uint32_t mode;
        ext4_mode_get(target_path, &mode);
        st->st_mode = mode;
        ext4_fclose(&file);
        aos_free(target_path);
        return 0;
    }

    aos_free(target_path);

    return -rc;
}

static int _ext4_unlink(vfs_file_t *fp, const char *path)
{
    int rc;

    char *target_path = path_convert(path);
    if (target_path == NULL) {
        return -EINVAL;
    }

    rc = ext4_fremove(target_path);

    aos_free(target_path);

    return rc ? -rc : 0;
}

static int _ext4_rename(vfs_file_t *fp, const char *oldpath, const char *newpath)
{
    int rc;

    char *oldname = path_convert(oldpath);
    if (!oldname) {
        return -EINVAL;
    }

    char *newname = path_convert(newpath);
    if (!newname) {
        aos_free(oldname);
        return -EINVAL;
    }

    rc = ext4_frename(oldname, newname);

    aos_free(oldname);
    aos_free(newname);

    return rc ? -rc : 0;
}

static vfs_dir_t *_ext4_opendir(vfs_file_t *fp, const char *path)
{
    int rc;
    ext4_dir *dir;
    ext4_dir_t *ret_dir;
    vfs_dirent_t *dirent;

    char *relpath = path_convert(path);
    if (!relpath) {
        return NULL;
    }

    dir     = aos_calloc(1, sizeof(ext4_dir));
    dirent  = aos_calloc(1, sizeof(vfs_dirent_t) + 256);
    ret_dir = aos_calloc(1, sizeof(ext4_dir_t));

    ret_dir->dir = dirent;
    rc = ext4_dir_open(dir, relpath);
    if (rc == 0) {
        fp->f_arg = dir;
        aos_free(relpath);
        return (vfs_dir_t*)ret_dir;
    } else {
        aos_free(dir);
        aos_free(ret_dir);
        aos_free(dirent);
        aos_free(relpath);
        return NULL;
    }
}

static vfs_dirent_t *_ext4_readdir(vfs_file_t *fp, vfs_dir_t *dir)
{
    const ext4_direntry *rentry;
    ext4_dir *dirp = (ext4_dir*)(fp->f_arg);
    ext4_dir_t *dp = (ext4_dir_t*)dir;
    vfs_dirent_t *dirent = dp->dir;

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

static int _ext4_closedir(vfs_file_t *fp, vfs_dir_t *dir)
{
    int rc;
    ext4_dir *dirp;
    ext4_dir_t *dp = (ext4_dir_t*)dir;
    vfs_dirent_t *dirent = dp->dir;

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

static int _ext4_mkdir(vfs_file_t *fp, const char *path)
{
    int rc;

    char *pathname = path_convert(path);
    if (!pathname) {
        return -EINVAL;
    }

    rc = ext4_dir_mk(pathname);

    aos_free(pathname);

    return rc == 0 ? rc : -rc;
}

static int _ext4_rmdir(vfs_file_t *fp, const char *path)
{
    int rc;

    char *pathname = path_convert(path);
    if (!pathname) {
        return -EINVAL;
    }

    rc = ext4_dir_rm(pathname);

    aos_free(pathname);

    return rc == 0 ? rc : -rc;
}

static void _ext4_rewinddir(vfs_file_t *fp, vfs_dir_t *dir)
{
    ext4_dir *dirp;
    ext4_dir_t *dp = (ext4_dir_t *)dir;

    if (dp) {
        dirp = (ext4_dir*)(fp->f_arg);
        ext4_dir_entry_rewind(dirp);
    }

    return;
}

static long _ext4_telldir(vfs_file_t *fp, vfs_dir_t *dir)
{
    ext4_dir *dirp;
    ext4_dir_t *dp = (ext4_dir_t *)dir;

    if (dp) {
        dirp = (ext4_dir*)(fp->f_arg);

        return (long)(dirp->next_off);
    }

    return -1;
}

static void _ext4_seekdir(vfs_file_t *fp, vfs_dir_t *dir, long loc)
{
    ext4_dir *dirp;
    ext4_dir_t *dp = (ext4_dir_t *)dir;

    if (dp) {
        dirp = (ext4_dir*)(fp->f_arg);
        dirp->next_off = loc;
    }

    return;
}

static int _ext4_statfs(vfs_file_t *fp, const char *path, vfs_statfs_t *suf)
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

static const vfs_fs_ops_t ext4_ops = {
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
static const char *s_dev_name = "mmc";

int vfs_ext4_register(void)
{
    int rc;
    struct ext4_blockdev *bd = NULL;

    bd = ext4_blockdev_mmc_get();
    if (!bd)
        printf("ext4 bd is NULL\n");
    rc = ext4_device_register(bd, s_dev_name);
    if (rc != 0) {
        printf("ext4 dev register err, rc = %d\n", rc);
        return -1;
    } else {
        rc = ext4_mount(s_dev_name, ext4_mnt_path, false);
        if (rc != 0) {
            printf("ext4 mount err, rc = %d\n", rc);
            ext4_device_unregister(s_dev_name);
            return -1;
        } else {
            printf("ext4 mount success!!\n");
        }
    }
    rc = vfs_register_fs(ext4_mnt_path, &ext4_ops, bd);
    if (rc) {
        printf("aos register ext4 failed. ret:%d\n", rc);
    }
    return rc;
}

int vfs_ext4_unregister(void)
{
    int rc;

    ext4_device_unregister(s_dev_name);
    rc = ext4_umount(ext4_mnt_path);
    if (rc != 0) {
        printf("ext4 umount err, rc = %d\n", rc);
    }

    return vfs_unregister_fs(ext4_mnt_path);
}

