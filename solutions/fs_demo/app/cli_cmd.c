/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <aos/cli.h>
#include <vfs.h>

static void cmd_fs_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 2) {
#ifdef CONFIG_FS_FAT
        int ret;
        extern int vfs_fatfs_register(void);
        extern int vfs_fatfs_unregister(void);
        if (strcmp(argv[1], "mountfatfs") == 0) {
            if ((ret = vfs_fatfs_register())) {
                printf("fatfs register failed(%d).\n", ret);
            } else {
                printf("fatfs register ok.\n");
            }
        } else if (strcmp(argv[1], "unmountfatfs") == 0) {
            if (vfs_fatfs_unregister()) {
                printf("fatfs unregister failed.\n");
            } else {
                printf("fatfs unregister ok.\n");
            }
        }
#endif
    } else {
        printf("fs: invaild argv");
    }
}

int cli_reg_cmd_fs(void)
{
    static const struct cli_command cmd_info = {
        "fs",
        "fs test",
        cmd_fs_func,
    };

    aos_cli_register_command(&cmd_info);

    return 0;
}

extern void fs_multi_task_cmd(void);
void board_cli_init()
{
    aos_cli_init();
    cli_reg_cmd_fs();
    fs_multi_task_cmd();
}