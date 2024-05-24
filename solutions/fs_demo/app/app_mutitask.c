#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>
#include <aos/aos.h>

int g_task_quit = true;
int g_task_debug = false;

char g_content[] = "This is a test.";

void file_read_task(void* filename)
{
    int fd = -1;
    int ret = -1;
    char buffer[20] = {0};
    int len = sizeof(g_content);
    char* file = (char*)filename;

    printf("file_read_task start....\n");

    fd = open(file, O_RDWR);
    if (fd < 0) {
        printf("open file fail: %s!!!\n", file);
        return;
    }

    while (1) {
        ret = read(fd, buffer, len - 1);
        if (ret < 0) {
            printf("read file fail !!!!\n");
            break;
        }
        ret = lseek(fd, 0, SEEK_SET);
        if (ret < 0) {
            printf("lseek file fail !!!!\n");
            break;
        }
        buffer[len - 1] = '\0';
        if (g_task_debug) {
            printf("read content:%s\n", buffer);
        }
        if (memcmp(buffer, g_content, len) != 0) {
            printf("read file content fail !!!!\n");
            break;
        }
        if (g_task_quit) {
            break;
        }
        memset(buffer, 0, len);
        aos_msleep(1000);
    }

    printf("\n");
    printf("read from file:%s\n", file);
    printf("need read content:%s\n", g_content);
    printf("read content:%s\n", buffer);

    if (g_task_quit) {
        printf("task read file successfully....\n");
    } else {
        printf("task read file fail....\n");
    }

    close(fd);
}

void file_write_task(void* filename)
{
    int fd = -1;
    int len = 0;
    int ret = -1;
    char* file = (char*)filename;

    printf("file_write_task start....\n");

    fd = open(file, O_RDWR);
    if (fd < 0) {
        printf("open file fail: %s!!!!\n", file);
        return;
    }

    while (1) {
        len = write(fd, g_content, sizeof(g_content));
        if (len < 0) {
            printf("write file fail!!!!\n");
            break;
        }
        ret = lseek(fd, 0, SEEK_SET);
        if (ret < 0) {
            printf("lseek  file fail !!!!\n");
            break;
        }
        if (g_task_quit) {
            break;
        }
        aos_msleep(1000);
    }

    printf("\n");
    printf("write from file:%s\n", file);
    printf("need write content:%s\n", g_content);

    if (g_task_quit) {
        printf("please check result in file2.txt: This is a test.\n");
    } else {
        printf("task file write  fail !!!!\n");
    }

    close(fd);
}

int creat_files(char** filenames)
{
    int ret = -1;
    printf("creat_files start....\n");

    int fd_read = open(filenames[0], O_RDWR | O_CREAT);
    if (fd_read < 0) {
        printf("open file fail: %s !!!!\n", filenames[0]);
        return -1;
    }
    printf("open read_file:%s successfully....\n", filenames[0]);

    ret = write(fd_read, g_content, sizeof(g_content));
    if (ret < 0) {
        close(fd_read);
        printf("write file fail!!!!\n");
        return ret;
    }

    int fd_write = open(filenames[1], O_RDWR | O_CREAT);
    if (fd_write < 0) {
        close(fd_read);
        printf("open file fail!!!!: %s\n", filenames[1]);
        return -1;
    }
    printf("open write file:%s successfully....\n", filenames[1]);

    close(fd_read);
    close(fd_write);

    printf("creat_files succeed....\n");
    return 0;
}

void creat_fs_task(char** filenames)
{
    int ret = -1;
    aos_task_t read_task;
    aos_task_t write_task;

    if (!g_task_quit) {
        printf("task is running....\n");
        return;
    }

    printf("creat_fs_task run....\n");
    ret = creat_files(filenames);
    if (ret < 0) {
        printf("creat_files fial !!!!\n");
        return;
    }

    g_task_quit = false;

    aos_task_new_ext(&read_task, "file_read", file_read_task, (void*)filenames[0], 4096, 32);
    aos_task_new_ext(&write_task, "file_write", file_write_task, (void*)filenames[1], 4096, 32);
    printf("creat_fs_task end...\n");
    aos_msleep(100);
}

const char* help_str =
    "fs_task test\n"
    "please check whether the board supports this file systems!!!!\n"
    "fs_task support command:\n"
    "fs_task ext4\n"
    "fs_task sd\n"
    "fs_task lfs\n"
    "fs_task stop\n"
    "for more information see the README\n\n>";

void creat_fs_task_cmd(char* wbuf, int wbuf_len, int argc, char** argv)
{
    if (argc < 2) {
        goto help;
    }

    if (strcmp(argv[1], "ext4") == 0) {
#ifdef CONFIG_EXT4_MOUNTPOINT
        char* filenames[2] = {CONFIG_EXT4_MOUNTPOINT "/file1.txt", CONFIG_EXT4_MOUNTPOINT "/file2.txt"};
        creat_fs_task(filenames);
#else
        goto help;
#endif
    } else if (strcmp(argv[1], "sd") == 0) {
#ifdef CONFIG_SD_FATFS_MOUNTPOINT
        char* filenames[2] = {CONFIG_SD_FATFS_MOUNTPOINT "/file1.txt", CONFIG_SD_FATFS_MOUNTPOINT "/file2.txt"};
        creat_fs_task(filenames);
#else
        goto help;
#endif
    } else if (strcmp(argv[1], "lfs") == 0) {
#ifdef CONFIG_LFS_MOUNTPOINT
        char* filenames[2] = {CONFIG_LFS_MOUNTPOINT "file1.txt", CONFIG_LFS_MOUNTPOINT "file2.txt"};
        creat_fs_task(filenames);
#else
        goto help;
#endif
    } else if (strcmp(argv[1], "stop") == 0) {
        if (g_task_quit) {
            printf("task not start!!!\n");
            goto help;
        }
        g_task_quit = true;
    } else {
        goto help;
    }
    return;
help:
    printf("Argument failed\n%s", help_str);
}

void fs_multi_task_cmd(void)
{
    static const struct cli_command cmd_info = {"fs_task", "fs multi-task test", creat_fs_task_cmd};
    aos_cli_register_command(&cmd_info);
}