/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <ext4_vfs.h>
#include <vfs.h>
#include <aos/cli.h>

static char buff[512] = {0};

static void read_write(void)
{
	int fd1 = aos_open("/mnt/emmc/test5.txt", O_CREAT | O_RDWR | O_APPEND);
    printf("fd1:%d\n", fd1);
	int ret = aos_write(fd1, "hello world1\n", 13);
    if (ret < 0) {
        printf("write failed. ret:%d\n", ret);
    }
	aos_sync(fd1);
	aos_close(fd1);

	int fd2 = aos_open("/mnt/emmc/test5.txt", O_RDWR);
	//aos_lseek(fd2, 4, 0);
	ret = aos_read(fd2, buff, 512);
	printf("read ret %d\n", ret);
	if (ret > 0) {
		printf("read: %s\n", buff);
	}
	aos_close(fd2);
}

static void write_dir(void)
{
	int rc;

	rc = aos_mkdir("/mnt/emmc/dir_0");
	printf("create dir_0 rc = %d\n", rc);

	rc = aos_mkdir("/mnt/emmc/dir_1");
	printf("create dir_1 rc = %d\n", rc);

	rc = aos_rmdir("/mnt/emmc/dir_1");
	printf("rm dir_1 rc = %d\n", rc);
}

static void read_dir(void)
{
	aos_dir_t *dir = aos_opendir("/mnt/emmc");
	aos_dirent_t *dp = NULL;

	do {
		dp = aos_readdir(dir);
		if (dp)
			printf("readdir: %s\n", dp->d_name);
	} while (dp != NULL);

	aos_closedir(dir);
}

static void cmd_ext4_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
	if (argc == 2) {
		if (strcmp(argv[1], "file") == 0) {
			read_write();
		} else if (strcmp(argv[1], "dir") == 0) {
			write_dir();
			read_dir();
		}
		else if (strcmp(argv[1], "mount") == 0) {
			if (vfs_ext4_register()) {
                printf("ext4 register failed.\n");
            }
		} else if (strcmp(argv[1], "umount") == 0) {
			if (vfs_ext4_unregister()) {
                printf("ext4 unregister failed.\n");
            }
		}
		else if (strcmp(argv[1], "mkfs") == 0) {
			extern void dfs_ext_mkfs();
			dfs_ext_mkfs();
		}
	} else {
		printf("ext4: invaild argv");
	}
}

int cli_reg_cmd_lext4(void)
{
	static const struct cli_command cmd_info = {
        "ext4",
        "ext4 test",
        cmd_ext4_func,
	};

	aos_cli_register_command(&cmd_info);

	return 0;
}