/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_CLI) && defined(AOS_COMP_POSIX)
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
//#include <getopt.h>

#include <aos/cli.h>
#include <sys/stat.h>
#include <aos/kernel.h>
#include <dirent.h>

#include <vfs.h>
#include <vfs_cli.h>
#include <vfs_api.h>
#if defined(AOS_COMP_FATFS)
#include <ff.h>
#endif

#define LSFLAGS_SIZE          1
#define LSFLAGS_LONG          2
#define LSFLAGS_RECURSIVE     4

#define IOBUFFERSIZE 1024

const char g_fmtcmdfailed[]      = "cli: %s: %s failed: %d\n";
const char g_fmttoomanyargs[]    = "cli: %s: too many arguments\n";
const char g_fmtarginvalid[]     = "cli: %s: argument invalid\n";
const char g_fmtargrequired[]    = "cli: %s: missing required argument(s)\n";
const char g_fmtnosuch[]         = "cli: %s: no such %s: %s\n";
const char g_fmtcmdoutofmemory[] = "cli: %s: out of memory\n";
const char g_fmtargrange[]       = "cli: %s: value out of range\n";

typedef int (*direntry_handler_t)(const char *dirpath,
                                  aos_dirent_t *entryp, void *pvarg);
#ifndef __NEWLIB__
char *optarg; /* Optional argument following option */
int optind = 1;   /* Index into argv */
int optopt = '?'; /* unrecognized option character */

/****************************************************************************
 * Private Data
 ****************************************************************************/

static  char *g_optptr       = NULL;
static bool      g_binitialized = false;

int getopt(int argc, char *const argv[], const char *optstring)
{
    if (argv && optstring && argc > 1) {
        int noarg_ret = '?';
        char *optchar;

        /* The inital value of optind is 1.  If getopt() is called again in the
         * program, optind must be reset to some value <= 1.
         */

        if (optind < 1 || !g_binitialized) {
            optind         = 1;     /* Skip over the program name */
            g_optptr       = NULL;  /* Start at the beginning of the first argument */
            g_binitialized = true;  /* Now we are initialized */
        }

        /* If the first character of opstring s ':', then ':' is in the event of
         * a missing argument. Otherwise '?' is returned.
         */

        if (*optstring == ':') {
            noarg_ret = ':';
            optstring++;
        }

        /* Are we resuming in the middle, or at the end of a string of arguments?
         * g_optptr == NULL means that we are started at the beginning of argv[optind];
         * *g_optptr == \0 means that we are starting at the beginning of optind+1
         */

        while (!g_optptr || !*g_optptr) {
            /* We need to start at the beginning of the next argv. Check if we need
             * to increment optind
             */

            if (g_optptr) {
                /* Yes.. Increment it and check for the case where where we have
                 * processed everything in the argv[] array.
                 */

                optind++;
            }

            /* Check for the end of the argument list */

            g_optptr = argv[optind];

            if (!g_optptr) {
                /* There are no more arguments, we are finished */

                g_binitialized = false;
                return -1;
            }

            /* We are starting at the beginning of argv[optind].  In this case, the
             * first character must be '-'
             */

            if (*g_optptr != '-') {
                /* The argument does not start with '-', we are finished */

                g_binitialized = false;
                return -1;
            }

            /* Skip over the '-' */

            g_optptr++;
        }

        /* Special case handling of "-" and "-:" */

        if (!*g_optptr) {
            optopt = '\0'; /* We'll fix up g_optptr the next time we are called */
            return '?';
        }

        /* Handle the case of "-:" */

        if (*g_optptr == ':') {
            optopt = ':';
            g_optptr++;
            return '?';
        }

        /* g_optptr now points at the next option and it is not something crazy.
         * check if the option is in the list of valid options.
         */

        optchar = strchr(optstring, *g_optptr);

        if (!optchar) {
            /* No this character is not in the list of valid options */

            optopt = *g_optptr;
            g_optptr++;
            return '?';
        }

        /* Yes, the character is in the list of valid options.  Does it have an
         * required argument?
         */

        if (optchar[1] != ':') {
            /* No, no arguments. Just return the character that we found */

            g_optptr++;
            return *optchar;
        }

        /* Yes, it has a required argument.  Is the required argument
         * immediately after the command in this same argument?
         */

        if (g_optptr[1] != '\0') {
            /* Yes, return a pointer into the current argument */

            optarg = &g_optptr[1];
            optind++;
            g_optptr = NULL;
            return *optchar;
        }

        /* No.. is the optional argument the next argument in argv[] ? */

        if (argv[optind + 1] && *argv[optind + 1] != '-') {
            /* Yes.. return that */

            optarg = argv[optind + 1];
            optind += 2;
            g_optptr = NULL;
            return *optchar;
        }

        /* No argument was supplied */

        g_optptr = NULL;
        optarg = NULL;
        optopt = *optchar;
        optind++;
        return noarg_ret;
    }

    /* Restore the initial, uninitialized state */

    g_binitialized = false;
    optind = 1;
    optopt = '?';
    return -1;
}
#endif

int nsh_catfile(const char *cmd,
                const char *filepath)
{
    char *buffer;
    int fd;
    int ret = 0;

    /* Open the file for reading */

    fd = aos_open(filepath, O_RDONLY);

    if (fd < 0) {
        printf(g_fmtcmdfailed, cmd, "open", fd);
        return -1;
    }

    buffer = aos_malloc(IOBUFFERSIZE);

    if (buffer == NULL) {
        (void)aos_close(fd);
        printf(g_fmtcmdfailed, cmd, "malloc", -1);
        return -1;
    }

    /* And just dump it byte for byte into stdout */

    for (;;) {
        int nbytesread = aos_read(fd, buffer, IOBUFFERSIZE);

        /* Check for read errors */

        if (nbytesread < 0) {
            //int errval = errno;

            /* EINTR is not an error (but will stop stop the cat) */

            printf(g_fmtcmdfailed, cmd, "read", errno);

            ret = -1;
            break;
        }

        /* Check for data successfully read */

        else if (nbytesread > 0) {

            printf("%.*s", nbytesread, buffer);
        }

        /* Otherwise, it is the end of file */

        else {
            break;
        }
    }

    /* Make sure that the following NSH prompt appears on a new line.  If the
     * file ends in a newline, then this will print an extra blank line
     * before the prompt, but that is preferable to the case where there is
     * no newline and the NSH prompt appears on the same line as the cat'ed
     * file.
     */

    printf("\n");

    /* Close the input file and return the result */

    (void)aos_close(fd);
    aos_free(buffer);
    return ret;
}

static inline int ls_specialdir(const char *dir)
{
    /* '.' and '..' directories are not listed like normal directories */

    return (strcmp(dir, ".")  == 0 || strcmp(dir, "..") == 0);
}


static int nsh_foreach_direntry(const char *cmd,
                                char *dirpath,
                                direntry_handler_t handler, void *pvarg)
{
    aos_dir_t *dirp;
    int ret = 0;

    /* Open the directory */

    dirp = aos_opendir(dirpath);

    if (dirp == NULL) {
        /* Failed to open the directory */

        printf(g_fmtnosuch, cmd, "directory", dirpath);
        return -1;
    }

    /* Read each directory entry */

    for (; ;) {
        aos_dirent_t *entryp = aos_readdir(dirp);

        if (entryp == NULL) {
            /* Finished with this directory */

            break;
        }

        /* Call the handler with this directory entry */

        if (handler(dirpath, entryp, pvarg) <  0) {
            /* The handler reported a problem */

            ret = -1;
            break;
        }
    }

    aos_closedir(dirp);
    return ret;
}

static int ls_handler(const char *dirpath,
                      aos_dirent_t *entryp, void *pvarg)
{
    unsigned int lsflags = (unsigned int)((uintptr_t)pvarg);

    int ret;

    /* Check if any options will require that we stat the file */

    if ((lsflags & (LSFLAGS_SIZE | LSFLAGS_LONG)) != 0) {
        aos_stat_t buf = {0};

        /* stat the file */

        if (entryp != NULL) {
            char *fullpath = aos_malloc(strlen(dirpath) + strlen(entryp->d_name) + 1);

            if (fullpath == NULL) {
                return -1;
            }

            sprintf(fullpath, "%s/%s", dirpath, entryp->d_name);
            ret = aos_stat(fullpath, &buf);

            aos_free(fullpath);
        } else {
            /* A NULL entryp signifies that we are running ls on a single file */
            printf("%s,%d\n", __func__, __LINE__);
            ret = aos_stat(dirpath, &buf);
        }

        if (ret != 0) {
            printf(g_fmtcmdfailed, "ls", "stat", ret);
            return -1;
        }

        if ((lsflags & LSFLAGS_LONG) != 0) {
            char details[] = "----------";

            if (S_ISDIR(buf.st_mode)) {
                details[0] = 'd';
            } else if (S_ISCHR(buf.st_mode)) {
                details[0] = 'c';
            } else if (S_ISBLK(buf.st_mode)) {
                details[0] = 'b';
            } else if (!S_ISREG(buf.st_mode)) {
                details[0] = '?';
            }

            if ((buf.st_mode & S_IRUSR) != 0) {
                details[1] = 'r';
            }

            if ((buf.st_mode & S_IWUSR) != 0) {
                details[2] = 'w';
            }

            if ((buf.st_mode & S_IXUSR) != 0) {
                details[3] = 'x';
            }

            if ((buf.st_mode & S_IRGRP) != 0) {
                details[4] = 'r';
            }

            if ((buf.st_mode & S_IWGRP) != 0) {
                details[5] = 'w';
            }

            if ((buf.st_mode & S_IXGRP) != 0) {
                details[6] = 'x';
            }

            if ((buf.st_mode & S_IROTH) != 0) {
                details[7] = 'r';
            }

            if ((buf.st_mode & S_IWOTH) != 0) {
                details[8] = 'w';
            }

            if ((buf.st_mode & S_IXOTH) != 0) {
                details[9] = 'x';
            }

            printf("%s ", details);
        }

        if ((lsflags & LSFLAGS_SIZE) != 0) {
            printf("%14d", buf.st_size);
        }
    }

    /* Then provide the filename that is common to normal and verbose output */

    if (entryp != NULL) {
        printf(" %s", entryp->d_name);

        if ((entryp->d_type) == DT_DIR &&
            !ls_specialdir(entryp->d_name)) {
            printf("/\n");
        } else {
            printf("\n");
        }
    } else {
        /* A single file */

        printf(" %s\n", dirpath);
    }

    return 0;
}

static void cmd_ls(char *wbuf, int wbuf_len, int argc, char **argv)
{
    aos_stat_t st = {0};
    char *path;
    unsigned long lsflags = 0;
    uint8_t badarg = 0;
    int len;
    int ret;

    /* Get the ls options */

    int option;
    optind = 0;
    while ((option = getopt(argc, argv, "ls")) != -1) {
        switch (option) {
        case 'l':
            lsflags |= (LSFLAGS_SIZE | LSFLAGS_LONG);
            break;

        case 's':
            lsflags |= LSFLAGS_SIZE;
            break;

        case '?':
        default:
            printf(g_fmtarginvalid, argv[0]);
            badarg = 1;
            break;
        }
    }

    /* If a bad argument was encountered, then return without processing the command */

    if (badarg) {
        return;
    }

    /* There may be one argument after the options */
#if 0

    if (optind + 1 < argc) {
        printf(g_fmttoomanyargs, argv[0]);
        return -1;
    } else if (optind >= argc) {
        printf(g_fmtargrequired, argv[0]);
        return -1;
    } else {
        path = argv[optind];
    }

#endif
    path = argv[argc - 1];

    /* Trim any trailing '/' characters */

    len = strlen(path) - 1;

    while (len > 0 && path[len] == '/') {
        path[len] = '\0';
        len--;
    }

    /* See if it is a single file */

    if ((ret = aos_stat(path, &st)) < 0) {
        printf(g_fmtcmdfailed, argv[0], "stat", ret);
    } else if (!S_ISDIR(st.st_mode)) {
        /* Pass a null dirent to ls_handler to signify that this is a single
         * file
         */

        ls_handler(path, NULL, (void *)lsflags);
    } else {
        /* List the directory contents */
        nsh_foreach_direntry("ls", path, ls_handler,
                             (void *)((uintptr_t)lsflags));
    }

    return;
}

void cli_reg_cmd_ls(void)
{
    static const struct cli_command cmd_info = {
        "ls",
        "ls -ls",
        cmd_ls
    };

    aos_cli_register_command(&cmd_info);
}

static void cmd_rm(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ret = -1;
    char *path;
    int len;
    aos_stat_t st;

    if (argc != 2) {
        printf(g_fmtarginvalid, argv[0]);
        return;
    }

    path = argv[1];

    /* Trim any trailing '/' characters */

    len = strlen(path) - 1;

    while (len > 0 && path[len] == '/') {
        path[len] = '\0';
        len--;
    }

    /* See if it is a single file */
    if ((ret = aos_stat(path, &st)) < 0) {
        printf(g_fmtcmdfailed, argv[0], "stat", ret);
    } else if (!S_ISDIR(st.st_mode)) {
        ret = aos_unlink(path);
        if (ret < 0) {
            printf(g_fmtcmdfailed, argv[0], "unlink", ret);
        }
    } else {
        ret = aos_rmdir(path);
        if (ret < 0) {
            printf(g_fmtcmdfailed, argv[0], "rmdir", ret);
        }
    }
}

void cli_reg_cmd_rm(void)
{
    static const struct cli_command cmd_info = {
        "rm",
        "rm path",
        cmd_rm
    };

    aos_cli_register_command(&cmd_info);
}

static void cmd_cat(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int i;
    int ret = 0;

    /* Loop for each file name on the command line */

    for (i = 1; i < argc && ret == 0; i++) {
        /* Dump the file to the console */

        ret = nsh_catfile(argv[0], argv[1]);
    }
}

void cli_reg_cmd_cat(void)
{
    static const struct cli_command cmd_info = {
        "cat",
        "cat path",
        cmd_cat
    };

    aos_cli_register_command(&cmd_info);
}

static void cmd_echo(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int fd;
    ssize_t ret = 0;

    if (argc != 3) {
        printf(g_fmtcmdfailed, argv[0], "Invalid argument", argc);
        return;
    }

    fd = aos_open(argv[2], O_WRONLY|O_APPEND);
    if (fd < 0) {
        printf(g_fmtcmdfailed, argv[0], "open", fd);
        return;
    }

    ret = aos_write(fd, argv[1], strlen(argv[1]));
    if (ret != strlen(argv[1])) {
        printf(g_fmtcmdfailed, argv[0], "write", (int)ret);
    }

    aos_close(fd);
}

void cli_reg_cmd_echo(void)
{
    static const struct cli_command cmd_info = {
        "echo",
        "echo string file",
        cmd_echo
    };

    aos_cli_register_command(&cmd_info);
}

static void cmd_mkdir(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ret = -1;

    if (argc == 2) {
        ret = aos_mkdir(argv[1]);

        if (ret < 0) {
            printf(g_fmtcmdfailed, argv[0], "mkdir", ret);
        }
    }

    return;
}

void cli_reg_cmd_mkdir(void)
{
    static const struct cli_command cmd_info = {
        "mkdir",
        "mkdir path",
        cmd_mkdir
    };

    aos_cli_register_command(&cmd_info);
}

static void cmd_mv(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ret;

    /* Perform the mount */
    if (argc != 3) {
        printf(g_fmtcmdfailed, argv[0], "rename", -1);
        return;
    }

    ret = aos_rename(argv[1], argv[2]);

    if (ret < 0) {
        printf(g_fmtcmdfailed, argv[0], "rename", ret);
    }
}

void cli_reg_cmd_mv(void)
{
    static const struct cli_command cmd_info = {
        "mv",
        "mv oldpath newpath",
        cmd_mv
    };

    aos_cli_register_command(&cmd_info);
}

#define BUFF_SIZE       (1024 * 64)
#define NS_PER_SEC      1000000000

static double time_diff(struct timespec *start, struct timespec *end)
{
    double diff;

    if (end->tv_nsec < start->tv_nsec) {
        diff = (double)(NS_PER_SEC + end->tv_nsec - start->tv_nsec)/NS_PER_SEC;
        diff += end->tv_sec - 1 - start->tv_sec;
    } else {
        diff = (double)(end->tv_nsec - start->tv_nsec)/NS_PER_SEC;
        diff += end->tv_sec - start->tv_sec;
    }

    return diff;
}

static void cmd_cp(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ret, src_fd = 0, dst_fd = 0, rd_data = 0, wr_data = 0;
    uint8_t *buf = NULL;
    struct timespec begin, end;
    double rd_time = 0.0, wr_time = 0.0;

    /* Perform the mount */
    if (argc != 3) {
        printf(g_fmtarginvalid, argv[0]);
        return;
    }

    src_fd = aos_open(argv[1], O_RDONLY);
    if (src_fd < 0) {
        printf(g_fmtcmdfailed, argv[0], "open src file", src_fd);
        return;
    }

    dst_fd = aos_open(argv[2], O_WRONLY | O_CREAT);
    if (dst_fd < 0) {
        printf(g_fmtcmdfailed, argv[0], "open dst file", dst_fd);
        goto out;
    }

    buf = aos_malloc(BUFF_SIZE);
    if (!buf) {
        printf(g_fmtcmdfailed, argv[0], "malloc", BUFF_SIZE);
        goto out;
    }

    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &begin);
        ret = aos_read(src_fd, buf, BUFF_SIZE);
        if (ret < 0) {
            printf(g_fmtcmdfailed, argv[0], "read()", ret);
            break;
        }
        else if (ret == 0) {
            /* It's the end of file */
            break;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        rd_time += time_diff(&begin, &end);
        rd_data += ret;

        clock_gettime(CLOCK_MONOTONIC, &begin);
        ret = aos_write(dst_fd, buf, ret);
        if (ret < 0) {
            printf(g_fmtcmdfailed, argv[0], "write()", ret);
            break;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        wr_time += time_diff(&begin, &end);
        wr_data += ret;
    }

out:
    if (rd_time > 0)
        printf("Read  data %d B, speed %.2f MB/s\n", rd_data,
               (double)rd_data / 1024 / 1024 / rd_time);
    if (wr_time > 0)
        printf("Write data %d B, speed %.2f MB/s\n", wr_data,
               (double)wr_data / 1024 / 1024 / wr_time);

    if (buf)
        aos_free(buf);
    if (dst_fd)
        aos_close(dst_fd);
    if (src_fd)
        aos_close(src_fd);
}

void cli_reg_cmd_cp(void)
{
    static const struct cli_command cmd_info = {
        "cp",
        "cp srcfile dstfile",
        cmd_cp
    };

    aos_cli_register_command(&cmd_info);
}

static void cmd_diff(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ret1, ret2, diff = 0, fd1 = 0, fd2 = 0, size = BUFF_SIZE >> 1;
    uint8_t *buf1 = NULL, *buf2 = NULL;

    /* Perform the mount */
    if (argc != 3) {
        printf(g_fmtarginvalid, argv[0]);
        return;
    }

    fd1 = aos_open(argv[1], O_RDONLY);
    if (fd1 < 0) {
        printf(g_fmtcmdfailed, argv[0], "open file1", fd1);
        return;
    }
    fd2 = aos_open(argv[2], O_RDONLY);
    if (fd2 < 0) {
        printf(g_fmtcmdfailed, argv[0], "open file2", fd2);
        goto out;
    }

    buf1 = aos_malloc(size);
    if (!buf1) {
        printf(g_fmtcmdfailed, argv[0], "malloc buf1", size);
        goto out;
    }
    buf2 = aos_malloc(size);
    if (!buf2) {
        printf(g_fmtcmdfailed, argv[0], "malloc buf2", size);
        goto out;
    }

    while (1) {
        ret1 = aos_read(fd1, buf1, size);
        if (ret1 < 0) {
            printf(g_fmtcmdfailed, argv[0], "read() file1", ret1);
            goto out;
        }

        ret2 = aos_read(fd2, buf2, size);
        if (ret2 < 0) {
            printf(g_fmtcmdfailed, argv[0], "read() file2", ret2);
            goto out;
        }

        if (ret1 != ret2) {
            diff = 1;
            break;
        }
        if (ret1 == 0) /* Reach the end of file */
            break;
        if (memcmp(buf1, buf2, ret1)) {
            diff = 1;
            break;
        }
    }

    if (diff)
        printf("They are different\n");
    else
        printf("They are identical\n");
out:
    if (buf1)
        aos_free(buf1);
    if (buf2)
        aos_free(buf2);
    if (fd1)
        aos_close(fd1);
    if (fd2)
        aos_close(fd2);
}

void cli_reg_cmd_diff(void)
{
    static const struct cli_command cmd_info = {
        "diff",
        "diff file1 file2",
        cmd_diff
    };

    aos_cli_register_command(&cmd_info);
}


static int up_one_level(char *s)
{
    char *tail;

    if (!s)
        return -1;

    tail = s + strlen(s) - 1;
    if (*tail == '/')
        tail--;

    while (*tail != '\0' && *tail != '/')
        tail--;

    if (*tail == '\0') {
        return -1;
    } else {
        *(tail + 1) = '\0';
        return 0;
    }
}

static char *get_realpath(const char *path, char *resolved_path, unsigned int len)
{
    char *ret, *p = (char *)path, *r = resolved_path;

    if (!path || !r || len < 1)
        return NULL;

    memset(r, 0, len);

    // deal with heading char
    if (p[0] != '/') {
        // relative path
        ret = getcwd(r, len);
        if (!ret)
            return NULL;

        // add tailing '/' if no
        if (r[strlen(r) - 1] != '/') {
            r[strlen(r)] = '/';
        }

        r += strlen(r);
    } else {
        // absolute path
        r[0] = '/';
        r++;
    }

    // iterate to exclude '.', '..'. '/'
    while (*p != '\0') {
        while (*p == '/')
            p++;
        if (*p == '\0')
            break;

        if (*p == '.') {
            p++;
            // end with '.'
            if (*p == '\0')
                break;

            if (*p == '.') {
                // '..' or '../'
                if ((*(p + 1) != '/') && (*(p + 1) != '\0')) {
                    printf("Invalid path %s\r\n", path);
                    return NULL;
                } else {
                    // '..' case
                    p++;
                    // if (*p == '/') {
                    if (up_one_level(resolved_path) != 0) {
                        printf("Failed to go up now. Invalid path %s\r\n", path);
                        return NULL;
                    }

                    r = resolved_path + strlen(resolved_path);
                    // }

                    // end with '.'
                    if (*p == '\0') {
                        break;
                    }
                }
            } else {
                if (*p == '/' || *p == '\0') {
                    p++;
                } else {
                    // '.xxx' might be hidden file or dir
                    p--;
                    goto copy_valid;
                }
            }
        }

        while (*p == '/')
            p++;
        if (*p == '\0')
            break;

        // if another round of ./.., just continue
        if (*p == '.')
            continue;

copy_valid:
        // path string may be found now, save to r
        while ((*p != '/') && (*p != '\0'))
            *r++ = *p++;

        // add taling '/' if necessary
        if (*(r - 1) != '/') {
            *r++ = '/';
        }
    }

    /**
     * considering "cd ../config" for tab key case,
     * we need set string EOF avoid out of control.
     */
    *r = '\0';

    // exclude the tailing '/', just in case it is a file
    if ((resolved_path[strlen(resolved_path) - 1] == '/') &&
        (strlen(resolved_path) != 1)) {
        resolved_path[strlen(resolved_path) - 1] = '\0';
    }

    return resolved_path;
}

static void df_do_dir(const char *dir)
{
    aos_statfs_t sfs;
    unsigned long long total, used, free;
    char abspath[256] = {0}, *dir1;

    dir1 = get_realpath(dir, abspath, sizeof(abspath));
    if (!dir1) {
        aos_cli_printf("Failed to get real path!\r\n");
        return;
    }

    if (access(dir1, F_OK) != 0) {
        aos_cli_printf("Failed to access path:%s\r\n", dir1);
        return;
    }

    memset(&sfs, 0, sizeof(aos_statfs_t));
    if (aos_statfs(dir1, &sfs) < 0) {
        aos_cli_printf("statfs %s failed\n", dir);
        return;
    }

    total = ((unsigned long long)sfs.f_bsize * (unsigned long long)sfs.f_blocks) >> 10;
    if (total == 0) {
        aos_cli_printf("total size error!\r\n");
        return;
    }
    free = ((unsigned long long)sfs.f_bsize * (unsigned long long)sfs.f_bfree) >> 10;
    used = total - free;

    aos_cli_printf("%10llu%10llu%10llu%6llu%%    %s\n", total, used, free, used * 100 / total, dir);
}

static void print_help()
{
    aos_cli_printf(
        "usage:\r\n"
        "  df <dir>\r\n"
        "eg:\r\n"
        "  1.show /etc/config disk space info:\r\n"
        "    df /etc/cofig\r\n");
}

static void cmd_df(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int i;
    char node_names[8][64];
    uint32_t count = 0;
    uint32_t index;


    if (argc >= 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        print_help();
        return;
    }

    aos_cli_printf("%10s%10s%10s%7s    %s\n", "Total(KB)", "Used(KB)", "Free(KB)", "Use%", "Mount");

    if (argc <= 1) {
        vfs_get_node_name("/", node_names, &count);
        for (index = 0; index < count; index++) {
            df_do_dir(node_names[index]);
        }

        return;
    }

    for (i = 1; i < argc; i++)
        df_do_dir(argv[i]);
    return;
}

void cli_reg_cmd_df(void)
{
    static const struct cli_command cmd_info = {
        "df",
        "df path",
        cmd_df
    };

    aos_cli_register_command(&cmd_info);
}

#if defined(AOS_COMP_FATFS)
void cmd_mkfatfs(char *wbuf, int wbuf_len, int argc, char **argv)
{
    uint8_t badarg;
    int option;
    int ret = -1;
    uint8_t opt = FM_ANY;
    uint32_t fattype = 0;

    /* mkfatfs [-F <fatsize>] <block-driver> */

    badarg = 0;

    while ((option = getopt(argc, argv, ":F:")) != -1) {
        switch (option) {
        case 'F':
            fattype = atoi(optarg);

            switch (fattype) {
            case 32:
                opt = FM_FAT32;
            case 64:
                opt = FM_EXFAT;
            case 16:
            case 12:
                opt = FM_FAT;
            case 0:
                opt = FM_ANY;
            default:
                printf(g_fmtargrange, argv[0]);
                badarg = true;
            }

            break;

        case ':':
            printf(g_fmtargrequired, argv[0]);
            badarg = true;
            break;

        case '?':
        default:
            printf(g_fmtarginvalid, argv[0]);
            badarg = true;
            break;
        }
    }

    /* If a bad argument was encountered, then return without processing the command */

    if (badarg) {
        return;
    }

    /* There should be exactly one parameter left on the command-line */

#if 0
    if (optind >= argc) {
        printf(g_fmttoomanyargs, argv[0]);
        return;
    } else {
        printf(g_fmtargrequired, argv[0]);
        return;
    }
#endif

    /* Now format the FAT file system */

    char *buffer = aos_malloc(FF_MAX_SS);

    if (buffer == NULL) {
        return;
    }

    ret = f_mkfs("", opt, 0, buffer, FF_MAX_SS);

    if (ret != FR_OK) {
        printf(g_fmtcmdfailed, argv[0], "mkfatfs", ret);
    }

    aos_free(buffer);
}

void cli_reg_cmd_mkfatfs(void)
{
    static const struct cli_command cmd_info = {
        "mkfatfs",
        "mkfatfs [-F fattype]",
        cmd_mkfatfs
    };

    aos_cli_register_command(&cmd_info);
}
#endif

#endif /* AOS_COMP_CLI */
