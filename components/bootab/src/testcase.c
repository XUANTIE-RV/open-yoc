/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*/
#if defined(AOS_COMP_CLI) && (AOS_COMP_CLI > 0)
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <aos/kernel.h>
#include <aos/debug.h>
#include <aos/cli.h>
#include <yoc/ota_ab.h>
#include <yoc/partition.h>

#define BOOTAB_TEST_DBG(format, ...) printf("[%s, %d]"format"\r\n", __func__, __LINE__, ##__VA_ARGS__)

// void otaab_test_upgrade_boot2(int is_error)
// {
//     int   SLICE_SIZE = 4096;
//     char *cur_slot, *next_slot;
//     char *part_name;
//     int   img_size;
//     int   slice_count;
//     int   last_slice_size;

//     BOOTAB_TEST_DBG("come to otaab_test_upgrade_boot2");
//     // get cur slot
//     cur_slot = (char *)otaab_get_current_ab();
//     if (cur_slot == NULL) {
//         BOOTAB_TEST_DBG("no slot valid ,will panic");
//         return;
//     }
//     BOOTAB_TEST_DBG("current slot is [%s]", cur_slot);

//     part_name = "boot2b";
//     next_slot = "a";
//     if (strcmp(cur_slot, "a") == 0) {
//         part_name = "boot2a";
//         next_slot = "b";
//     }
//     if (is_error) {
//         BOOTAB_TEST_DBG("test fallback---------------------");
//     }
//     // upgrade
//     hal_logic_partition_t part_info;
//     if (get_part_info_with_name(part_name, &part_info) < 0) {
//         BOOTAB_TEST_DBG("get part %s e", part_name);
//         return;
//     }
//     BOOTAB_TEST_DBG("start_addr:0x%x", part_info.partition_start_addr);
//     img_size = get_image_size(part_info.partition_start_addr, part_info.partition_length);
//     BOOTAB_TEST_DBG("000img_size:%d", img_size);
//     slice_count = img_size / SLICE_SIZE;
//     BOOTAB_TEST_DBG("slice_count:%d", slice_count);
//     void *buffer = aos_malloc(SLICE_SIZE);
//     aos_check_mem(buffer);

//     uint32_t offset = 0;
//     uint32_t addr   = part_info.partition_start_addr;
//     for (int i = 0; i < slice_count; i++) {
//         get_data_from_faddr(addr, buffer, SLICE_SIZE);
//         if (is_error) {
//             memset(buffer + 20, 0xFF, SLICE_SIZE - 20);
//         }
//         if (otaab_upgrade_slice("boot2", "", offset, SLICE_SIZE, 0, buffer) < 0) {
//             BOOTAB_TEST_DBG("bootab_upgrade_slice failed!!!!!!");
//             aos_free(buffer);
//             return;
//         }
//         offset += SLICE_SIZE;
//         addr += SLICE_SIZE;
//     }
//     last_slice_size = img_size - slice_count * SLICE_SIZE;
//     BOOTAB_TEST_DBG("last_slice_size:%d", last_slice_size);
//     get_data_from_faddr(addr, buffer, last_slice_size);
//     if (otaab_upgrade_slice("boot2", "", offset, last_slice_size, 1, buffer) < 0) {
//         BOOTAB_TEST_DBG("222bootab_upgrade_slice failed!!!!!!");
//         aos_free(buffer);
//         return;
//     }
//     aos_free(buffer);
//     BOOTAB_TEST_DBG("ota test boot2 upgrade ok......");
// }

void otaab_test_upgrade_prim(int is_error)
{
    int ret;
    int SLICE_SIZE = (32 * 1024);
    char *cur_slot;
    char *part_name;
    int img_size;
    int slice_count;
    int last_slice_size;

    BOOTAB_TEST_DBG("come to otaab_test_upgrade_prim");

    // get cur slot
    cur_slot = (char *)otaab_get_current_ab();
    if (cur_slot == NULL) {
        BOOTAB_TEST_DBG("no slot valid ,will panic");
        return;
    }
    BOOTAB_TEST_DBG("current slot is [%s]", cur_slot);

    part_name = "primb";
    if (strcmp(cur_slot, "a") == 0) {
        part_name = "prima";
    }
    BOOTAB_TEST_DBG("++++++++++++++++++++++++++cur_slot : %s", cur_slot);
    if (is_error) {
        BOOTAB_TEST_DBG("test fallback---------------------");
    }

    // upgrade
    partition_t partition;
    partition_info_t *part_info;
    partition = partition_open(part_name);
    part_info = partition_info_get(partition);
    if (part_info == NULL) {
        BOOTAB_TEST_DBG("get part %s e", part_name);
        return;
    }
    BOOTAB_TEST_DBG("start_addr:0x%" PRIX64, part_info->start_addr + part_info->base_addr);
    img_size = part_info->length; // FIXME: just use partition length instead
    BOOTAB_TEST_DBG("111img_size:%d", img_size);
    slice_count = img_size / SLICE_SIZE;
    BOOTAB_TEST_DBG("slice_count:%d", slice_count);
    char *buffer = aos_malloc(SLICE_SIZE);
    aos_check_mem(buffer);

    off_t offset = 0;
    for (int i = 0; i < slice_count; i++) {
        ret = partition_read(partition, offset, buffer, SLICE_SIZE);
        if (ret < 0) {
            BOOTAB_TEST_DBG("read flash error.");
            aos_free(buffer);
            return;
        }
        if (is_error) {
            memset(buffer + 20, 0xFF, SLICE_SIZE - 20);
        }
        if (otaab_upgrade_slice("prim", "", offset, SLICE_SIZE, 0, buffer) < 0) {
            BOOTAB_TEST_DBG("bootab_upgrade_slice failed!!!!!!");
            aos_free(buffer);
            return;
        }
        offset += SLICE_SIZE;
        aos_msleep(50);
    }
    last_slice_size = img_size - slice_count * SLICE_SIZE;
    BOOTAB_TEST_DBG("last_slice_size:%d", last_slice_size);
    ret = partition_read(partition, offset, buffer, last_slice_size);
    if (ret < 0) {
        BOOTAB_TEST_DBG("read flash error.");
        aos_free(buffer);
        return;
    }
    if (otaab_upgrade_slice("prim", "", offset, last_slice_size, 1, buffer) < 0) {
        BOOTAB_TEST_DBG("222bootab_upgrade_slice failed!!!!!!");
        aos_free(buffer);
        return;
    }
    aos_free(buffer);

    BOOTAB_TEST_DBG("ota test prim upgrade ok......");
}

static void show_help(void)
{
    aos_cli_printf("usage:\r\n"
            "  otaab start\r\n"
            "  otaab up prim [1]\r\n"
            "  otaab end\r\n"
            "  otaab get info\r\n"
            "  otaab select A[B]\r\n");
}

static void cmd_ota_offline_setup(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int is_error = 0;
    // otaab start
    // otaab up prim
    // otaab up prim 1
    // otaab end
    // TODO: need reboot device
    // otaab finish 0
    // otaab finish 1

    if (argc >= 2) {
        //printf("argv[0]:%s, argv[1]:%s\n", argv[0], argv[1]);
        if (strcmp(argv[1], "start") == 0) {
            printf("otaab_start\n");
            otaab_start();
            return;
        } else if (strcmp(argv[1], "end") == 0) {
            printf("otaab_upgrade_end\n");
            otaab_upgrade_end("v1.1");
            aos_msleep(1000);
            aos_reboot();
            return;
        } else if (strcmp(argv[1], "finish") == 0) {
            int fallback;
            if (argc > 2) {
                fallback = atoi(argv[2]);
                printf("otaab_finish,fallback:%d\n", fallback);
                otaab_finish(fallback);
                return;
            }
        } else if (strcmp(argv[1], "up") == 0) {
            if (argc > 2) {
                if (strcmp(argv[2], "boot2") == 0) {
                    if (argc > 3)
                        is_error = atoi(argv[3]);
                    // otaab_test_upgrade_boot2(is_error);
                    return;
                } else if (strcmp(argv[2], "prim") == 0) {
                    if (argc > 3)
                        is_error = atoi(argv[3]);
                    otaab_test_upgrade_prim(is_error);
                    return;
                }
            }
        } else if (strcmp(argv[1], "get") == 0) {
            if (strcmp(argv[2], "info") == 0) {
                const char * ab = otaab_get_current_ab();
                if (ab[0] == 'a') {
                    printf("slot A\r\n");
                } else if (ab[0] == 'b') {
                    printf("slot B\r\n");
                } else {
                    printf("slot ERR\r\n");
                }
            }
            return;
        } else if (strcmp(argv[1], "select") == 0) {
            const char * ab = otaab_get_current_ab();
            if (strcmp(argv[2], "A") == 0) {
                if (ab[0] != 'a') {
                    otaab_upgrade_end(NULL);
                }
                printf("Select A\r\n");
            } else if(strcmp(argv[2], "B") == 0) {
                if (ab[0] != 'b') {
                    otaab_upgrade_end(NULL);
                }
                printf("Select B\r\n");
            }
            return;
        }
    }
    show_help();
}

void cli_reg_cmd_otaab(void)
{
    static const struct cli_command cmd_info = {
        "otaab",
        "ota ab test",
        cmd_ota_offline_setup
    };

    aos_cli_register_command(&cmd_info);
}

#endif