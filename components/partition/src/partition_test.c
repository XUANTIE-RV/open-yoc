/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if AOS_COMP_CLI
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <aos/cli.h>
#include <aos/kernel.h>
#include <yoc/partition.h>

#define HELP_INFO "Usage:\tpart test <name>\n"

static void dump_data(uint8_t *data, int32_t len)
{
    int32_t i;

    for (i = 0; i < len; i++) {
        if (i % 16 == 0 && i > 0) {
            printf("\n");
        }

        printf("%02x ", data[i]);
    }

    printf("\n");
}

static int g_test_runing;
static void test_task(void *arg)
{
    char *name = arg;
    int ret;
    int part_offset;
    int read_size;
    uint8_t value;
    uint8_t *tmpbuf = NULL;

    part_offset = 0;
    read_size = 4096;
    value = 1;

    printf("test partition:%s, start..Please wait...\n", name);
    partition_t partition = partition_open(name);
    if (partition < 0) {
        printf("open %s failed.\n", name);
        goto failure;
    }
    printf("partition:%d\n", partition);
    partition_info_t *part_info = partition_info_get(partition);
    if (!part_info) {
        printf("get %s information failed.\n", name);
        goto failure;
    }
    printf("part_info->base_addr:   0x%"PRIX64"\n", part_info->base_addr);
    printf("part_info->start_addr:  0x%"PRIX64"\n", part_info->start_addr);
    printf("part_info->length:      0x%"PRIX64"\n", part_info->length);
    while (part_offset + read_size < part_info->length) {
        if (g_test_runing == 0) break;
        printf("test part_offset:0x%x, size:%d\n", part_offset, read_size);
        tmpbuf = calloc(1, read_size);
        if (!tmpbuf) {
            printf("malloc mem e.\n");
            goto failure;
        }
        ret = partition_read(partition, part_offset, tmpbuf, read_size);
        if (ret < 0) {
            printf("read e.\n");
            goto failure;
        }
        // dump_data(tmpbuf, 32);
        uint32_t erase_offset = part_offset;
        uint32_t erase_size = read_size;
        if (part_offset % part_info->erase_size) {
            erase_offset = (part_offset / part_info->erase_size) * part_info->erase_size;
            erase_size = read_size + part_info->erase_size;
        }
        if (erase_offset + erase_size >= part_info->length) {
            free(tmpbuf);
            break;
        }
        ret = partition_erase_size(partition, erase_offset, erase_size);
        if (ret < 0) {
            printf("erase e.\n");
            goto failure;
        }
        ret = partition_read(partition, part_offset, tmpbuf, read_size);
        if (ret < 0) {
            printf("read2 e.\n");
            goto failure;
        }
        for (int i = 0; i < read_size; i++) {
            if (tmpbuf[i] != 0xFF) {
                printf("not erase ok .\n");
                dump_data(&tmpbuf[i], 32);
                goto failure;
            }
        }
        for (int i = 0; i < read_size; i++) {
            tmpbuf[i] = value;
        }
        // dump_data(tmpbuf, 32);
        ret = partition_write(partition, part_offset, tmpbuf, read_size);
        if (ret < 0) {
            printf("write e.\n");
            goto failure;
        }
        ret = partition_read(partition, part_offset, tmpbuf, read_size);
        if (ret < 0) {
            printf("read3 e.\n");
            goto failure;
        }
        for (int i = 0; i < read_size; i++) {
            if (tmpbuf[i] != value) {
                printf("write data wrong.\n");
                dump_data(&tmpbuf[i], 32);
                goto failure;
            }
        }
#if 1
        part_offset += 277 * value;
        read_size += 359 * value;
#else
        part_offset += 4096;
        read_size = 4096;
#endif
        value += 1;
        free(tmpbuf);
    }
    printf("...........test ok..........\n");
    partition_close(partition);
    g_test_runing = 0;
    return;
failure:
    if (tmpbuf) free(tmpbuf);
    partition_close(partition);
    printf("!!!test partition failed. part_offset:0x%x, size:%d\n", part_offset, read_size);
    g_test_runing = 0;
}

static void cmd_partition_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    static char part_name[12];
    if (argc < 3) {
        printf("%s\n", HELP_INFO);
        return;
    }
    printf("========>%s\n", argv[2]);
    strlcpy(part_name, argv[2], sizeof(part_name));
    if (strcmp(argv[2], "stop") == 0) {
        g_test_runing = 0;
    } else if (g_test_runing == 0) {
        g_test_runing = 1;
        aos_task_new("test-part", test_task, part_name, 8192);
    } else if (g_test_runing) {
        printf("test is runing...Please use `part test stop` to stop first.\n");
    }
}

void cli_reg_cmd_partition_test(void)
{
    static const struct cli_command cmd_info = {
        "part",
        "part test",
        cmd_partition_func
    };

    aos_cli_register_command(&cmd_info);
}
#endif