/****************************************************************************
 * hostfs/hostfs.c
 *
 *   Copyright (C) 2015 Ken Pettit. All rights reserved.
 *   Author: Ken Pettit <pettitkd@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <aos/kernel.h>
#include <aos/debug.h>
#include <vfs.h>
#include <vfs_inode.h>
#include <vfs_register.h>

#include <hostfs.h>
#include <hostfs_rpmsg.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TAG "HOSTFS"

#define HOSTFS_RETRY_DELAY_MS       10
#define HOSTFS_MAX_PATH             256

#ifndef TRUE
#define  TRUE (1)
#endif

#ifndef FALSE
#define  FALSE (0)
#endif

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* This structure describes the state of one open file.  This structure
 * is protected by the volume semaphore.
 */

struct hostfs_ofile_s {
    struct hostfs_ofile_s    *fnext;      /* Supports a singly linked list */
    int16_t                   crefs;      /* Reference count */
    uint32_t                  oflags;     /* Open mode */
    int                       fd;
};

/* This structure represents the overall mountpoint state.  An instance of this
 * structure is retained as inode private data on each mountpoint that is
 * mounted with a hostfs filesystem.
 */

struct hostfs_mountpt_s {
    aos_sem_t                  *fs_sem;       /* Used to assure thread-safe access */
    struct hostfs_ofile_s      *fs_head;      /* A singly-linked list of open files */
    char                        fs_root[HOSTFS_MAX_PATH];
};

struct hostfs_dir_s {
    aos_dir_t dir;
    void *fs_dir;
    aos_dirent_t cur_dirent;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int     hostfs_open(file_t *filep, const char *relpath,
                           int oflags);
static int     hostfs_close(file_t *filep);
static ssize_t hostfs_read(file_t *filep, char *buffer,
                           size_t buflen);
static ssize_t hostfs_write(file_t *filep, const char *buffer,
                            size_t buflen);
static off_t   hostfs_seek(file_t *filep, off_t offset,
                           int whence);
static int     hostfs_ioctl(file_t *filep, int cmd,
                            unsigned long arg);

static int     hostfs_sync(file_t *filep);

static aos_dir_t *hostfs_opendir(file_t *filep, const char *relpath);
static int     hostfs_closedir(file_t *filep, aos_dir_t *dir);
static aos_dirent_t *hostfs_readdir(file_t *filep, aos_dir_t *dir);

static int     hostfs_bind(const char *root, void **handle);
static int     hostfs_unbind(void *handle, unsigned int flags);

static int     hostfs_unlink(file_t *filep, const char *relpath);
static int     hostfs_mkdir(file_t *filep, const char *relpath);
static int     hostfs_rmdir(file_t *filep, const char *relpath);
static int     hostfs_rename(file_t *filep,
                             const char *oldrelpath,
                             const char *newrelpath);
static int     hostfs_stat(file_t *filep,
                           const char *relpath, struct stat *buf);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static uint8_t      g_seminitialized = FALSE;
static aos_sem_t    g_sem;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/* See fs_mount.c -- this structure is explicitly externed there.
 * We use the old-fashioned kind of initializers so that this will compile
 * with any compiler.
 */

static const fs_ops_t hostfs_operations = {
    hostfs_open,          /* open */
    hostfs_close,         /* close */
    hostfs_read,          /* read */
    hostfs_write,         /* write */
    hostfs_seek,          /* seek */

    hostfs_sync,          /* sync */
    hostfs_stat,          /* stat */
    hostfs_unlink,        /* unlinke */
    hostfs_rename,        /* rename */

    hostfs_opendir,       /* opendir */
    hostfs_readdir,       /* readdir */
    hostfs_closedir,      /* closedir */

    hostfs_mkdir,         /* mkdir */
    hostfs_rmdir,         /* rmdir */

    hostfs_ioctl,         /* ioctl */
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: hostfs_semtake
 ****************************************************************************/

void hostfs_semtake(struct hostfs_mountpt_s *fs)
{
    int ret;

    do {
        /* Take the semaphore (perhaps waiting) */

        ret = aos_sem_wait(fs->fs_sem, AOS_WAIT_FOREVER);

        /* The only case that an error should occur here is if the wait was
         * awakened by a signal.
         */

        aos_assert(ret == OK || ret == -EINTR);
    } while (ret == -EINTR);
}

/****************************************************************************
 * Name: hostfs_semgive
 ****************************************************************************/

void hostfs_semgive(struct hostfs_mountpt_s *fs)
{
    aos_sem_signal(fs->fs_sem);
}

/****************************************************************************
 * Name: hostfs_mkpath
 *
 * Description: Build absolute host path from relative NuttX path.
 *
 ****************************************************************************/

static void hostfs_mkpath(struct hostfs_mountpt_s  *fs,
                          const char *relpath,
                          char *path, int pathlen)
{
    const char *ptr = relpath;
    int depth = 0;
    int x = 0;

    /* Copy base host path to output */

    strncpy(path, fs->fs_root, pathlen);

    /* Be sure we aren't trying to use ".." to display outside of our
     * mounted path.
     */

    while (*ptr == '/') {
        ptr++;
    }

    ptr = strchr(ptr, '/');
    if (*ptr == '/') {
        ptr++;
    }

    while (ptr[x] != '\0') {
        /* Test for ".." occurance */

        if (strncmp(&ptr[x], "..", 2) == 0) {
            /* Reduce depth by 1 */

            depth--;
            x += 2;
        }

        else if (ptr[x] == '/' && ptr[x + 1] != '/' &&
                 ptr[x + 1] != '\0') {
            depth++;
            x++;
        } else {
            x++;
        }
    }

    if (depth >= 0 && ptr != NULL) {
        strncat(path, ptr, pathlen - strlen(path) - 1);
    }
}

/****************************************************************************
 * Name: hostfs_open
 ****************************************************************************/

static int hostfs_open(file_t *filep, const char *relpath,
                       int oflags)
{
    inode_t *inode;
    struct hostfs_mountpt_s *fs;
    struct hostfs_ofile_s  *hf;
    char path[HOSTFS_MAX_PATH];
    int ret;

    /* Sanity checks */

    aos_assert((filep->f_arg == NULL) && (filep->node != NULL));

    /* Get the mountpoint inode reference from the file structure and the
     * mountpoint private data from the inode structure
     */

    inode = filep->node;
    fs    = inode->i_arg;

    aos_assert(fs != NULL);

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Allocate memory for the open file */

    hf = (struct hostfs_ofile_s *)aos_malloc(sizeof *hf);
    if (hf == NULL) {
        ret = -ENOMEM;
        goto errout_with_semaphore;
    }

    /* Append to the host's root directory */

    hostfs_mkpath(fs, relpath, path, sizeof(path));

    /* Try to open the file in the host file system */

    hf->fd = host_open(path, oflags, 0666);
    if (hf->fd < 0) {
        /* Error opening file */

        ret = -EBADF;
        goto errout_with_buffer;
    }

    /* In write/append mode, we need to set the file pointer to the end of the
     * file.
     */

    if ((oflags & (O_APPEND | O_WRONLY)) == (O_APPEND | O_WRONLY)) {
        ret = host_lseek(hf->fd, 0, SEEK_END);
        if (ret >= 0) {
            filep->offset = ret;
        } else {
            goto errout_with_buffer;
        }
    }

    /* Attach the private date to the struct file instance */

    filep->f_arg = hf;

    /* Then insert the new instance into the mountpoint structure.
     * It needs to be there (1) to handle error conditions that effect
     * all files, and (2) to inform the umount logic that we are busy
     * (but a simple reference count could have done that).
     */

    hf->fnext = fs->fs_head;
    hf->crefs = 1;
    hf->oflags = oflags;
    fs->fs_head = hf;

    ret = OK;
    goto errout_with_semaphore;

errout_with_buffer:
    aos_free(hf);

errout_with_semaphore:
    hostfs_semgive(fs);
    if (ret == -EINVAL) {
        ret = -EIO;
    }

    return ret;
}

/****************************************************************************
 * Name: hostfs_close
 ****************************************************************************/

static int hostfs_close(file_t *filep)
{
    inode_t                 *inode;
    struct hostfs_mountpt_s *fs;
    struct hostfs_ofile_s   *hf;
    struct hostfs_ofile_s   *nextfile;
    struct hostfs_ofile_s   *prevfile;

    /* Sanity checks */

    aos_assert(filep->f_arg != NULL && filep->node != NULL);

    /* Recover our private data from the struct file instance */

    inode = filep->node;
    fs    = inode->i_arg;
    hf    = filep->f_arg;

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Check if we are the last one with a reference to the file and
     * only close if we are.
     */

    if (hf->crefs > 1) {
        /* The file is opened more than once.  Just decrement the
         * reference count and return.
         */

        hf->crefs--;
        goto okout;
    }

    /* Remove ourselves from the linked list */

    nextfile = fs->fs_head;
    prevfile = nextfile;
    while ((nextfile != hf) && (nextfile != NULL)) {
        /* Save the previous file pointer too */

        prevfile = nextfile;
        nextfile = nextfile->fnext;
    }

    if (nextfile != NULL) {
        /* Test if we were the first entry */

        if (nextfile == fs->fs_head) {
            /* Assign a new head */

            fs->fs_head = nextfile->fnext;
        } else {
            /* Take ourselves out of the list */

            prevfile->fnext = nextfile->fnext;
        }
    }

    /* Close the host file */

    host_close(hf->fd);

    /* Now free the pointer */

    filep->f_arg = NULL;
    aos_free(hf);

okout:
    hostfs_semgive(fs);
    return OK;
}

/****************************************************************************
 * Name: hostfs_read
 ****************************************************************************/

static ssize_t hostfs_read(file_t *filep, char *buffer,
                           size_t buflen)
{
    inode_t *inode;
    struct hostfs_mountpt_s *fs;
    struct hostfs_ofile_s *hf;
    ssize_t ret;

    /* Sanity checks */

    aos_assert(filep->f_arg != NULL && filep->node != NULL);

    /* Recover our private data from the struct file instance */

    hf    = filep->f_arg;
    inode = filep->node;
    fs    = inode->i_arg;

    aos_assert(fs != NULL);

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Call the host to perform the read */

    ret = host_read(hf->fd, buffer, buflen);
    if (ret > 0) {
        filep->offset += ret;
    }

    hostfs_semgive(fs);
    return ret;
}

/****************************************************************************
 * Name: hostfs_write
 ****************************************************************************/

static ssize_t hostfs_write(file_t *filep, const char *buffer,
                            size_t buflen)
{
    inode_t *inode;
    struct hostfs_mountpt_s *fs;
    struct hostfs_ofile_s *hf;
    ssize_t ret;

    /* Sanity checks.  I have seen the following assertion misfire if
     * CONFIG_DEBUG_MM is enabled while re-directing output to a
     * file.  In this case, the debug output can get generated while
     * the file is being opened,  FAT data structures are being allocated,
     * and things are generally in a perverse state.
     */

    aos_assert(filep->f_arg != NULL && filep->node != NULL);

    /* Recover our private data from the struct file instance */

    hf    = filep->f_arg;
    inode = filep->node;
    fs    = inode->i_arg;

    aos_assert(fs != NULL);

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Test the permissions.  Only allow write if the file was opened with
     * write flags.
     */

    if ((hf->oflags & O_ACCMODE) == O_RDONLY) {
        ret = -EACCES;
        goto errout_with_semaphore;
    }

    /* Call the host to perform the write */

    ret = host_write(hf->fd, buffer, buflen);
    if (ret > 0) {
        filep->offset += ret;
    }

errout_with_semaphore:
    hostfs_semgive(fs);
    return ret;
}

/****************************************************************************
 * Name: hostfs_seek
 ****************************************************************************/

static off_t hostfs_seek(file_t *filep, off_t offset, int whence)
{
    inode_t *inode;
    struct hostfs_mountpt_s *fs;
    struct hostfs_ofile_s *hf;
    off_t ret;

    /* Sanity checks */

    aos_assert(filep->f_arg != NULL && filep->node != NULL);

    /* Recover our private data from the struct file instance */

    hf    = filep->f_arg;
    inode = filep->node;
    fs    = inode->i_arg;

    aos_assert(fs != NULL);

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Call our internal routine to perform the seek */

    ret = host_lseek(hf->fd, offset, whence);
    if (ret >= 0) {
        filep->offset = ret;
    }

    hostfs_semgive(fs);
    return ret;
}

/****************************************************************************
 * Name: hostfs_ioctl
 ****************************************************************************/

static int hostfs_ioctl(file_t *filep, int cmd, unsigned long arg)
{
    inode_t *inode;
    struct hostfs_mountpt_s *fs;
    struct hostfs_ofile_s *hf;
    int ret;

    /* Sanity checks */

    aos_assert(filep->f_arg != NULL && filep->node != NULL);

    /* Recover our private data from the struct file instance */

    hf    = filep->f_arg;
    inode = filep->node;
    fs    = inode->i_arg;

    aos_assert(fs != NULL);

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Call our internal routine to perform the ioctl */

    ret = host_ioctl(hf->fd, cmd, arg);

    hostfs_semgive(fs);
    return ret;
}

/****************************************************************************
 * Name: hostfs_sync
 *
 * Description: Synchronize the file state on disk to match internal, in-
 *   memory state.
 *
 ****************************************************************************/

static int hostfs_sync(file_t *filep)
{
    inode_t *inode;
    struct hostfs_mountpt_s *fs;
    struct hostfs_ofile_s   *hf;

    /* Sanity checks */

    aos_assert(filep->f_arg != NULL && filep->node != NULL);

    /* Recover our private data from the struct file instance */

    hf    = filep->f_arg;
    inode = filep->node;
    fs    = inode->i_arg;

    aos_assert(fs != NULL);

    /* Take the semaphore */

    hostfs_semtake(fs);

    host_sync(hf->fd);

    hostfs_semgive(fs);
    return OK;
}

/****************************************************************************
 * Name: hostfs_opendir
 *
 * Description: Open a directory for read access
 *
 ****************************************************************************/

static aos_dir_t *hostfs_opendir(file_t *filep, const char *relpath)
{
    inode_t *inode;
    struct hostfs_mountpt_s *fs;
    struct hostfs_dir_s *hostfs_dir;
    char path[HOSTFS_MAX_PATH];

    /* Sanity checks */

    aos_assert(filep != NULL && filep->node != NULL);

    /* Recover our private data from the inode instance */

    inode = filep->node;
    fs    = inode->i_arg;

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Append to the host's root directory */

    hostfs_mkpath(fs, relpath, path, sizeof(path));

    /* Call the host's opendir function */

    hostfs_dir = (struct hostfs_dir_s *)aos_malloc(sizeof(struct hostfs_dir_s) + \
                                                   HOSTFS_NAME_MAX + 1);
    if (hostfs_dir == NULL) {
        goto errout_with_semaphore;
    }

    hostfs_dir->fs_dir = host_opendir(path);
    if (hostfs_dir->fs_dir == NULL) {
        aos_free(hostfs_dir);
        hostfs_dir = NULL;
    }

errout_with_semaphore:

    hostfs_semgive(fs);
    return (aos_dir_t *)hostfs_dir;
}

/****************************************************************************
 * Name: hostfs_closedir
 *
 * Description: Open a directory for read access
 *
 ****************************************************************************/

static int hostfs_closedir(file_t *filep, aos_dir_t *dir)
{
    inode_t *inode;
    struct hostfs_mountpt_s  *fs;
    struct hostfs_dir_s *hostfs_dir = (struct hostfs_dir_s *)dir;

    /* Sanity checks */

    aos_assert(filep != NULL && filep->node != NULL);

    /* Recover our private data from the inode instance */

    inode = filep->node;
    fs    = inode->i_arg;

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Call the host's closedir function */

    host_closedir(hostfs_dir->fs_dir);

    hostfs_semgive(fs);

    aos_free(hostfs_dir);
    return OK;
}

/****************************************************************************
 * Name: hostfs_readdir
 *
 * Description: Read the next directory entry
 *
 ****************************************************************************/

static aos_dirent_t *hostfs_readdir(file_t *filep, aos_dir_t *dir)
{
    inode_t *inode;
    struct hostfs_mountpt_s *fs;
    struct hostfs_dirent_s dirent;
    struct hostfs_dir_s *hostfs_dir = (struct hostfs_dir_s *)dir;
    aos_dirent_t *out_dirent = NULL;
    char *name;
    int ret;

    /* Sanity checks */

    aos_assert(filep != NULL && filep->node != NULL);

    /* Recover our private data from the inode instance */

    inode = filep->node;
    fs    = inode->i_arg;

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Call the host OS's readdir function */

    ret = host_readdir(hostfs_dir->fs_dir, &dirent);
    if (ret < 0) {
        goto errout_with_semaphore;
    }

    hostfs_dir->cur_dirent.d_ino = 0;
    hostfs_dir->cur_dirent.d_type = dirent.d_type;

    name = (char *)dirent.d_name;
    if (name[0] == '/') {
        name++;
    }
    strncpy(hostfs_dir->cur_dirent.d_name, name, HOSTFS_NAME_MAX);
    hostfs_dir->cur_dirent.d_name[HOSTFS_NAME_MAX] = '\0';

    out_dirent = &hostfs_dir->cur_dirent;

errout_with_semaphore:

    hostfs_semgive(fs);
    return out_dirent;
}

/****************************************************************************
 * Name: hostfs_bind
 *
 * Description: This implements a portion of the mount operation. This
 *  function allocates and initializes the mountpoint private data and
 *  binds the blockdriver inode to the filesystem private data.  The final
 *  binding of the private data (containing the blockdriver) to the
 *  mountpoint is performed by mount().
 *
 ****************************************************************************/

static int hostfs_bind(const char *root, void **handle)
{
    struct hostfs_mountpt_s  *fs;
    int len;

    /* Validate the root is NULL */

    if (!root) {
        return -ENODEV;
    }

    /* Create an instance of the mountpt state structure */

    fs = (struct hostfs_mountpt_s *)
         aos_zalloc(sizeof(struct hostfs_mountpt_s));

    if (fs == NULL) {
        return -ENOMEM;
    }

    /* The options we suppor are:
     *  "fs=whatever", remote dir
     */

    strncpy(fs->fs_root, root, sizeof(fs->fs_root));

    /* If the global semaphore hasn't been initialized, then
     * initialized it now.
     */

    fs->fs_sem = &g_sem;
    if (!g_seminitialized) {
        /* Initialize the semaphore that controls access */

        aos_sem_new(&g_sem, 0);
        g_seminitialized = TRUE;
    } else {
        /* Take the semaphore for the mount */

        hostfs_semtake(fs);
    }

    /* Initialize the allocated mountpt state structure.  The filesystem is
     * responsible for one reference ont the blkdriver inode and does not
     * have to addref() here (but does have to release in ubind().
     */

    fs->fs_head = NULL;

    /* Now perform the mount.  */

    len = strlen(fs->fs_root);
    if (len > 1 && fs->fs_root[len - 1] == '/') {
        /* Remove trailing '/' */

        fs->fs_root[len - 1] = '\0';
    }

    /* Append a '/' to the name now */

    if (fs->fs_root[len - 1] != '/') {
        strcat(fs->fs_root, "/");
    }

    *handle = (void *)fs;
    hostfs_semgive(fs);
    return OK;
}

/****************************************************************************
 * Name: hostfs_unbind
 *
 * Description: This implements the filesystem portion of the umount
 *   operation.
 *
 ****************************************************************************/

static int hostfs_unbind(void *handle, unsigned int flags)
{
    struct hostfs_mountpt_s *fs = (struct hostfs_mountpt_s *)handle;
    int ret;

    if (!fs) {
        return -EINVAL;
    }

    /* Check if there are sill any files opened on the filesystem. */

    ret = OK; /* Assume success */
    hostfs_semtake(fs);
    if (fs->fs_head != NULL) {
        /* We cannot unmount now.. there are open files */

        hostfs_semgive(fs);

        /* This implementation currently only supports unmounting if there are
         * no open file references.
         */

        return (flags != 0) ? -ENOSYS : -EBUSY;
    }

    hostfs_semgive(fs);
    aos_free(fs);
    return ret;
}

/****************************************************************************
 * Name: hostfs_unlink
 *
 * Description: Remove a file
 *
 ****************************************************************************/

static int hostfs_unlink(file_t *filep, const char *relpath)
{
    inode_t *inode;
    struct hostfs_mountpt_s *fs;
    char path[HOSTFS_MAX_PATH];
    int ret;

    /* Sanity checks */

    aos_assert(filep != NULL && filep->node != NULL);

    /* Recover our private data from the inode instance */

    inode = filep->node;
    fs    = inode->i_arg;

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Append to the host's root directory */

    hostfs_mkpath(fs, relpath, path, sizeof(path));

    /* Call the host fs to perform the unlink */

    ret = host_unlink(path);

    hostfs_semgive(fs);
    return ret;
}

/****************************************************************************
 * Name: hostfs_mkdir
 *
 * Description: Create a directory
 *
 ****************************************************************************/

static int hostfs_mkdir(file_t *filep, const char *relpath)
{
    inode_t *inode;
    struct hostfs_mountpt_s *fs;
    char path[HOSTFS_MAX_PATH];
    int ret;

    /* Sanity checks */

    aos_assert(filep != NULL && filep->node != NULL);

    /* Get the mountpoint private data from the inode structure */

    inode = filep->node;
    fs    = inode->i_arg;

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Append to the host's root directory */

    hostfs_mkpath(fs, relpath, path, sizeof(path));

    /* Call the host FS to do the mkdir */

    ret = host_mkdir(path, 0666);

    hostfs_semgive(fs);
    return ret;
}

/****************************************************************************
 * Name: hostfs_rmdir
 *
 * Description: Remove a directory
 *
 ****************************************************************************/

int hostfs_rmdir(file_t *filep, const char *relpath)
{
    inode_t *inode;
    struct hostfs_mountpt_s *fs;
    char path[HOSTFS_MAX_PATH];
    int ret;

    /* Sanity checks */

    aos_assert(filep != NULL && filep->node != NULL);

    /* Get the mountpoint private data from the inode structure */

    inode = filep->node;
    fs    = inode->i_arg;

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Append to the host's root directory */

    hostfs_mkpath(fs, relpath, path, sizeof(path));

    /* Call the host FS to do the mkdir */

    ret = host_rmdir(path);

    hostfs_semgive(fs);
    return ret;
}

/****************************************************************************
 * Name: hostfs_rename
 *
 * Description: Rename a file or directory
 *
 ****************************************************************************/

int hostfs_rename(file_t *filep,
                  const char *oldrelpath,
                  const char *newrelpath)
{
    inode_t *inode;
    struct hostfs_mountpt_s *fs;
    char oldpath[HOSTFS_MAX_PATH];
    char newpath[HOSTFS_MAX_PATH];
    int ret;

    /* Sanity checks */

    aos_assert(filep != NULL && filep->node != NULL);

    /* Get the mountpoint private data from the inode structure */

    inode = filep->node;
    fs    = inode->i_arg;

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Append to the host's root directory */

    strncpy(oldpath, fs->fs_root, sizeof(oldpath));
    strncat(oldpath, oldrelpath, sizeof(oldpath)-strlen(oldpath)-1);
    strncpy(newpath, fs->fs_root, sizeof(newpath));
    strncat(newpath, newrelpath, sizeof(newpath)-strlen(newpath)-1);

    /* Call the host FS to do the mkdir */

    ret = host_rename(oldpath, newpath);

    hostfs_semgive(fs);
    return ret;
}

/****************************************************************************
 * Name: hostfs_stat
 *
 * Description: Return information about a file or directory
 *
 ****************************************************************************/

static int hostfs_stat(file_t *filep,
                       const char *relpath, struct stat *buf)
{
    inode_t *inode;
    struct hostfs_mountpt_s *fs;
    char path[HOSTFS_MAX_PATH];
    int ret;

    /* Sanity checks */

    aos_assert(filep != NULL && filep->node != NULL);

    /* Get the mountpoint private data from the inode structure */

    inode = filep->node;
    fs    = inode->i_arg;

    /* Take the semaphore */

    hostfs_semtake(fs);

    /* Append to the host's root directory */

    hostfs_mkpath(fs, relpath, path, sizeof(path));

    /* Call the host FS to do the stat operation */

    ret = host_stat(path, buf);

    hostfs_semgive(fs);
    return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int hostfs_register(const char *root)
{
    struct hostfs_mountpt_s *hostfs = NULL;
    int ret;

    ret = hostfs_bind(root, (void **)&hostfs);
    if (ret < 0) {
        LOGE(TAG, "hostfs bind err!");
        return ret;
    }

    return aos_register_fs(root, &hostfs_operations, (void *)hostfs);
}

int hostfs_unregister(const char *root)
{
    inode_t *inode;
    void *handle;
    int ret;

    inode = inode_open(root);
    if (inode == NULL) {
        LOGE(TAG, "inode open err!");
        return -ENOENT;
    }

    handle = inode->i_arg;

    ret = aos_unregister_fs(root);
    if (ret < 0) {
        LOGE(TAG, "hostfs unregister err!");
        return ret;
    }

    return hostfs_unbind(handle, 0);
}

