/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*/
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <yoc/partition.h>
#include <drv/porting.h>
#include "yoc/ota_ab_img.h"
#include "bootab.h"
#include "bootablog.h"

#define NORMAL_ERASE_FLAG 0xFFFFFFFF

typedef struct {
#define MAX_BOOT_CNT    (32)
#define MAX_BOOT_CNT_RE (30)
    uint32_t W[MAX_BOOT_CNT];   //bootcount计数
} bootab_bc_t;

typedef struct {
#define B_IMGS_VERSION_LEN  (64)
#define B_ROLLBACK_EN_FLAG  (0xFFFF0000)
#define B_FINISH_FLAG       (0x38)
    uint32_t active_ver;                    //分区激活版本号,以小的为准
    uint32_t rb_flag;                       //回滚开关标记，0xFFFF0000：打开回滚功能，其他：关闭回滚功能
    char img_version[B_IMGS_VERSION_LEN];   //镜像固件的版本
    uint32_t finish;                        //写完VER会打上这个标记，固定0x38
} bootab_flag_t;

typedef struct {
    bootab_bc_t bootab_bc;
    bootab_flag_t bootab_flag;
} bootab_mgr_t;

static bootab_flag_t g_bootab_flag[2];
static char g_cur_slot[4];
static uint32_t g_erase_flag = NORMAL_ERASE_FLAG;
static uint32_t g_flash_erase_size;
static partition_info_t *g_envab_info;

#define CHECK_PARAMS(ab, envinfo)                   \
    do                                              \
    {                                               \
        if ((void *)ab == NULL)                     \
        {                                           \
            return -EINVAL;                         \
        }                                           \
        if ((void *)envinfo == NULL)                \
        {                                           \
            BABLOGE("Can't find %s", B_ENVAB_NAME); \
            return -EINVAL;                         \
        }                                           \
    } while (0)

static bool check_ab_valid(bootab_flag_t *bootab_flag)
{
    if (g_erase_flag == NORMAL_ERASE_FLAG) {
        if ((bootab_flag->active_ver == NORMAL_ERASE_FLAG)
            || (bootab_flag->active_ver != NORMAL_ERASE_FLAG && bootab_flag->finish == B_FINISH_FLAG && bootab_flag->active_ver != 0)) {
            return true;
        }
    } else {
        if ((bootab_flag->active_ver == 0)
            || (bootab_flag->active_ver != 0 && bootab_flag->finish == B_FINISH_FLAG && bootab_flag->active_ver != NORMAL_ERASE_FLAG)) {
            return true;
        }
    }
    return false;
}

static int get_envab_offset(const char *ab)
{
    if (strcmp(ab, "a") == 0) {
        return 0;
    }
    return g_flash_erase_size;
}

int bootab_init(void)
{
    uint32_t erase_flag;
    partition_t partition = partition_open(B_ENVAB_NAME);
    partition_info_t *part_info = partition_info_get(partition);
    if (partition < 0) {
        return -1;
    }
    if (part_info == NULL) {
        BABLOGE("init e, get envab failed.");
        goto errout;
    }
    if (part_info->length < (OTA_AB_IMG_INFO_OFFSET_GET(part_info->erase_size) + OTA_AB_IMG_HEAD_SIZE)) {
        BABLOGE("the envab size is not enough.");
        goto errout;
    }
    erase_flag = NORMAL_ERASE_FLAG;
    if (!partition_read(partition, part_info->erase_size - 4, &erase_flag, 4)) {
        g_erase_flag = erase_flag;
    }
    BABLOGD("erase flag is 0x%x", g_erase_flag);

    g_envab_info = part_info;
    g_flash_erase_size = g_envab_info->erase_size;

    int retry = 0;
    do {
        if (bootab_get_current_ab()) {
            break;
        }
        BABLOGE("got current slot failed!!!!!!!!");
    } while (retry++ < 3);
    if (retry > 2) {
        BABLOGW("erase bootab partition.");
        partition_erase_size(partition, 0, part_info->length);
        goto errout;
    }
    partition_close(partition);
    BABLOGI("current slot:%s, env_partition:%d, sector:%d", g_cur_slot, partition, g_flash_erase_size);
    return 0;
errout:
    partition_close(partition);
    return -1;
}

const char *bootab_get_current_ab(void)
{
    char *ab = NULL;
    bool valid[2];

    if (g_cur_slot[0] != 0) {
        return g_cur_slot;
    }

    if (g_envab_info == NULL) {
        return NULL;
    }

    partition_t partition = partition_open(B_ENVAB_NAME);
    // check a
    partition_read(partition, sizeof(bootab_bc_t), (uint8_t *)&g_bootab_flag[0], sizeof(bootab_flag_t));
    valid[0] = check_ab_valid(&g_bootab_flag[0]);

    // check b
    partition_read(partition, g_flash_erase_size + sizeof(bootab_bc_t), (uint8_t *)&g_bootab_flag[1], sizeof(bootab_flag_t));
    valid[1] = check_ab_valid(&g_bootab_flag[1]);

    // select a/b
    if (valid[0] && valid[1]) {
        BABLOGD("both parts are valid,[0x%x, 0x%x]", g_bootab_flag[0].active_ver, g_bootab_flag[1].active_ver);
        if (g_erase_flag == NORMAL_ERASE_FLAG) {
            if (g_bootab_flag[0].active_ver <= g_bootab_flag[1].active_ver) {
                BABLOGD("%s is valid", "a");
                ab = "a";
                goto exit;
            }
        } else {
            if (g_bootab_flag[0].active_ver >= g_bootab_flag[1].active_ver) {
                BABLOGD("%s is valid", "a");
                ab = "a";
                goto exit;
            }
        }
        BABLOGD("%s is valid", "b");
        ab = "b";
        goto exit;
    } else if (valid[0]) {
        BABLOGD("%s is valid", "a");
        ab = "a";
        goto exit;
    } else if (valid[1]) {
        BABLOGD("%s are valid", "b");
        ab = "b";
        goto exit;
    }
exit:
    partition_close(partition);
    if (ab != NULL) {
        strcpy(g_cur_slot, ab);
        return ab;
    }
    BABLOGE("both are invalid!!!");
    return NULL;
}

int bootab_get_bootcount(const char *ab)
{
    int ret;
    int cnt;
    bootab_bc_t bootab_bc;

    CHECK_PARAMS(ab, g_envab_info);
    BABLOGD("get bootcount flag from [%s]", ab);
    partition_t partition = partition_open(B_ENVAB_NAME);
    ret = partition_read(partition, get_envab_offset(ab), (uint8_t *)&bootab_bc, sizeof(bootab_bc_t));
    partition_close(partition);
    if (ret < 0) {
        BABLOGE("read flash error.");
        return ret;
    }
    cnt = 0;
    for (int i = 0; i < MAX_BOOT_CNT; i++) {
        if (bootab_bc.W[i] != g_erase_flag && i < MAX_BOOT_CNT_RE) {
            cnt++;
        }
        BABLOGD("bootab_bc.W[%02d]:0x%x", i, bootab_bc.W[i]);
    }
    BABLOGD("%s , cnt:%d", ab, cnt);
    return cnt;
}

int bootab_set_bootcount(const char *ab, int old_cnt)
{
    off_t offset;
    boot_reason_t breason;
    bootab_bc_t bootab_bc;

    CHECK_PARAMS(ab, g_envab_info);

    BABLOGD("set bootcount, %s, old_cnt:%d", ab, old_cnt);

    if (old_cnt > MAX_BOOT_CNT_RE - 1) {
        BABLOGE("######old_cnt is :%d", old_cnt);
        return -EINVAL;
    }

    breason = soc_get_boot_reason();
    BABLOGI("===========breason:%d", breason);
    if (breason != BOOTREASON_SOFT && breason != BOOTREASON_WDT) {
        BABLOGE("is not a soft or wdt reset!!!");
        return -1;
    } else {
        BABLOGD("is a soft reset!!!, bootcount++");
    }

    bootab_bc.W[old_cnt] = ~g_erase_flag;

    offset = get_envab_offset(ab) + 4 * old_cnt;
    partition_t partition = partition_open(B_ENVAB_NAME);
    BABLOGD("@@write to [%d], offset:[0x%lx], size: %d", partition, offset, 4);
    if (partition_write(partition, offset, (uint8_t *)&bootab_bc.W[old_cnt], 4) < 0) {
        BABLOGE("write bootcount to flash failed.");
        partition_close(partition);
        return -1;
    }
    partition_close(partition);
    BABLOGD("set bootcount sucess");
    return 0;
}

int bootab_get_bootlimit(const char *ab)
{
    return (int)BOOT_LIMIT;
}

bool bootab_get_fallback_is_enable(const char *ab)
{
    bool ret = false;
    bootab_flag_t boot_flag;

    if (ab == NULL) {
        return false;
    }

    if (g_envab_info == NULL) {
        BABLOGE("cant find %s", B_ENVAB_NAME);
        return false;
    }
    partition_t partition = partition_open(B_ENVAB_NAME);
    partition_read(partition, get_envab_offset(ab) + sizeof(bootab_bc_t), (uint8_t *)&boot_flag, sizeof(bootab_flag_t));
    partition_close(partition);
    ret = boot_flag.rb_flag != B_ROLLBACK_EN_FLAG ? false : true;
    BABLOGD("check fallback enable:%d", ret);
    return ret;
}

int bootab_clear_flags(const char *ab)
{
    CHECK_PARAMS(ab, g_envab_info);
    BABLOGD("clear flags on [%s]", ab);
    partition_t partition = partition_open(B_ENVAB_NAME);
    BABLOGD("@@erase to [%d], offset:[0x%x], size: %d", partition, get_envab_offset(ab), g_flash_erase_size);
    int ret = partition_erase_size(partition, get_envab_offset(ab), g_flash_erase_size);
    partition_close(partition);
    if (ret < 0) {
        BABLOGE("clear flags failed.ret:%d", ret);
    }
    return ret;
}

int bootab_set_valid(const char *ab, const char *img_version)
{
    int ret;
    char *another;
    off_t offset;
    bootab_flag_t boot_flag;

    CHECK_PARAMS(ab, g_envab_info);

    another = "a";
    if (strcmp(ab, "a") == 0) {
        another = "b";
    }
    offset = get_envab_offset(another) + sizeof(bootab_bc_t);
    partition_t partition = partition_open(B_ENVAB_NAME);
    ret = partition_read(partition, offset, &boot_flag, sizeof(bootab_flag_t));
    if (ret < 0) {
        BABLOGE("read flash error.");
        goto errout;
    }
    offset = get_envab_offset(ab) + sizeof(bootab_bc_t);
    if (g_erase_flag == NORMAL_ERASE_FLAG) {
        BABLOGD("offset:0x%lx, 0x%x, 0x%x", offset, boot_flag.active_ver, boot_flag.active_ver - 1);
        boot_flag.active_ver = boot_flag.active_ver - 1;
    } else {
        BABLOGD("offset:0x%lx, 0x%x, 0x%x", offset, boot_flag.active_ver, boot_flag.active_ver + 1);
        boot_flag.active_ver = boot_flag.active_ver + 1;
    }
    boot_flag.rb_flag = B_ROLLBACK_EN_FLAG;
    boot_flag.finish = B_FINISH_FLAG;
    if (img_version) {
        strlcpy(boot_flag.img_version, img_version, sizeof(boot_flag.img_version) - 1);
    } else {
        memset(&boot_flag.img_version, 0xFF, sizeof(boot_flag.img_version));
    }
    BABLOGD("@@write to [%d], offset:[0x%lx], size: %d", partition, offset, (int)sizeof(bootab_flag_t));
    ret = partition_write(partition, offset, &boot_flag, sizeof(bootab_flag_t));
    if (ret < 0) {
        BABLOGE("write flash error.");
        goto errout;
    }

    // check
    bootab_flag_t new_flag;
    ret = partition_read(partition, offset, &new_flag, sizeof(bootab_flag_t));
    if (ret < 0) {
        BABLOGE("read flash error.");
        goto errout;
    }
    partition_close(partition);
    if (boot_flag.active_ver != new_flag.active_ver || boot_flag.finish != new_flag.finish) {
        BABLOGE("oldver:0x%x, newver:0x%x", boot_flag.active_ver, new_flag.active_ver);
        BABLOGE("oldfin:0x%x, newfin:0x%x", boot_flag.finish, new_flag.finish);
        BABLOGE("check failed.");
        return -1;
    }
    BABLOGD("oldver:0x%x, newver:0x%x", boot_flag.active_ver, new_flag.active_ver);
    BABLOGD("oldfin:0x%x, newfin:0x%x", boot_flag.finish, new_flag.finish);
    return 0;
errout:
    partition_close(partition);
    return ret;
}

int bootab_set_invalid(const char *ab)
{
    int ret;
    int val;
    off_t offset, ver_flag_offset;
    bootab_flag_t boot_flag;

    CHECK_PARAMS(ab, g_envab_info);

    offset = get_envab_offset(ab) + sizeof(bootab_bc_t);
    ver_flag_offset = offset;
    val = 0;
    partition_t partition = partition_open(B_ENVAB_NAME);
    BABLOGD("@@write to [%d], offset:[0x%lx], size: %d", partition, ver_flag_offset, (int)sizeof(boot_flag.active_ver));
    ret = partition_write(partition, ver_flag_offset, (uint8_t *)&val, sizeof(boot_flag.active_ver));
    partition_close(partition);
    if (ret < 0) {
        BABLOGE("write flash error.");
    }
    return ret;
}

int bootab_set_fallback_enable(const char *ab)
{
    int ret;
    off_t offset;
    off_t rb_flag_offset;
    bootab_flag_t boot_flag;

    CHECK_PARAMS(ab, g_envab_info);

    offset = get_envab_offset(ab) + sizeof(bootab_bc_t);
    partition_t partition = partition_open(B_ENVAB_NAME);
    ret = partition_read(partition, offset, &boot_flag, sizeof(bootab_flag_t));
    if (ret < 0) {
        BABLOGE("read flash error.");
        goto errout;
    }
    if (boot_flag.rb_flag != B_ROLLBACK_EN_FLAG) {
        if (boot_flag.active_ver == g_erase_flag) {
            BABLOGI("init version, do not need set fallback enable.");
        } else {
            boot_flag.rb_flag = B_ROLLBACK_EN_FLAG;
            rb_flag_offset = offset + sizeof(boot_flag.active_ver);
            BABLOGD("@@write to [%d], offset:[0x%lx], size: %d", partition, rb_flag_offset, (int)sizeof(boot_flag.rb_flag));
            ret = partition_write(partition, rb_flag_offset, (void *)&boot_flag.rb_flag, sizeof(boot_flag.rb_flag));
            if (ret < 0) {
                BABLOGE("write flash error.");
                goto errout;
            }
        }
    }
    partition_close(partition);
    return 0;
errout:
    partition_close(partition);
    return ret;
}

int bootab_set_fallback_disable(const char *ab)
{
    int ret;
    off_t offset;
    off_t rb_flag_offset;
    bootab_flag_t boot_flag;

    CHECK_PARAMS(ab, g_envab_info);

    offset = get_envab_offset(ab) + sizeof(bootab_bc_t);
    partition_t partition = partition_open(B_ENVAB_NAME);
    ret = partition_read(partition, offset, &boot_flag, sizeof(bootab_flag_t));
    if (ret < 0) {
        BABLOGE("read flash error.");
        goto errout;
    }
    if (boot_flag.rb_flag == B_ROLLBACK_EN_FLAG) {
        boot_flag.rb_flag = !g_erase_flag;
        rb_flag_offset = offset + sizeof(boot_flag.active_ver);
        BABLOGD("@@write to [%d], offset:[0x%lx], size: %d", partition, rb_flag_offset, (int)sizeof(boot_flag.rb_flag));
        ret = partition_write(partition, rb_flag_offset, (void *)&boot_flag.rb_flag, sizeof(boot_flag.rb_flag));
        if (ret < 0) {
            BABLOGE("write flash error.");
            goto errout;
        }
    }
    partition_close(partition);
    return 0;
errout:
    partition_close(partition);
    return ret;
}

static const char *_get_pre_slot_name(const char *ab)
{
    if (strcmp(ab, "a") == 0) {
        return "b";
    }
    return "a";
}

const char *bootab_fallback(const char *ab)
{
    int   cnt;
    char *pre_slot;

    if (ab == NULL) {
        return NULL;
    }
    if (g_envab_info == NULL) {
        BABLOGE("cant find %s", B_ENVAB_NAME);
        return NULL;
    }
    BABLOGD("fallback check... [%s]", ab);
    if (bootab_get_fallback_is_enable(ab)) {
// #ifdef CONFIG_FLASH_WP_CHECK
//         com_flash_wp_enable(0);
// #endif
        BABLOGI("fallback is enabled...");
        cnt = bootab_get_bootcount(ab);
        BABLOGD("bootab fallback bootcount: %d", cnt);
        if (cnt >= bootab_get_bootlimit(ab)) {
            // need fallback
            pre_slot = (char *)_get_pre_slot_name(ab);
            BABLOGI("bootab, i will fallback to %s", pre_slot);
            bootab_set_invalid(ab);
            return pre_slot;
        }
        bootab_set_bootcount(ab, cnt);
    } else {
        BABLOGI("fallback is disable...");
    }

    return NULL;
}

const char *bootab_get_imgs_version(const char *ab)
{
    static char imgs_version[B_IMGS_VERSION_LEN];
    bootab_flag_t boot_flag;

    if (ab == NULL) {
        return "";
    }
    BABLOGD("get slot[%s] version.", ab);
    partition_t partition = partition_open(B_ENVAB_NAME);
    int ret = partition_read(partition, get_envab_offset(ab) + sizeof(bootab_bc_t), (uint8_t *)&boot_flag, sizeof(bootab_flag_t));
    partition_close(partition);
    if (ret < 0) {
        BABLOGE("read flash error");
        return "";
    }
#if 0
    for (int i = 0; i < sizeof(boot_flag.img_version); i++) {
        if (i % 16 == 0) {
            printf("\n");
        }
        printf("0x%02x ", boot_flag.img_version[i]);
    }
    printf("\n");
#endif
    if (boot_flag.img_version[0] == 0xFF || boot_flag.img_version[0] == 0) {
        return BOOTAB_INIT_VER;
    }
    strlcpy(imgs_version, boot_flag.img_version, sizeof(boot_flag.img_version));
    return imgs_version;
}

const char *bootab_get_cur_imgs_version(void)
{
    const char *ab = bootab_get_current_ab();

    return bootab_get_imgs_version(ab);
}

#ifndef CONFIG_KERNEL_NONE
#include <aos/debug.h>
#include <aos/kernel.h>

#define _BOOTAB_DUMP_DATA_ 0

static bool check_mem_is_same(const uint8_t *mem1, const uint8_t *mem2, size_t size)
{
    aos_check_return_val(mem1 && mem2, false);

    for (int i = 0; i < size; i++) {
        if (mem1[i] != mem2[i]) {
            BABLOGE("cmp e i:%d, 0x%x, origin: 0x%x", i, mem1[i], mem2[i]);
            return false;
        }
    }
    return true;
}

static uint8_t g_check_buffer[AB_WRITE_CHECK_BUF_SIZE] __attribute__((__aligned__(4)));
static int _slice_data_rd_check(partition_t partition, off_t offset, void *buffer, uint32_t slice_size)
{
    bool ret;
    int  i;
    int  count = slice_size / AB_WRITE_CHECK_BUF_SIZE;
    int  mod   = slice_size % AB_WRITE_CHECK_BUF_SIZE;

    BABLOGD("start to check flash data. partition:%d, offset:0x%lx, slice_size:%d", partition, offset, slice_size);
    // soc_dcache_clean_invalid();
    i = 0;
    for (; i < count; i++) {
        if (partition_read(partition, offset + i * AB_WRITE_CHECK_BUF_SIZE, g_check_buffer, AB_WRITE_CHECK_BUF_SIZE) < 0) {
            BABLOGE("read flash failed.");
            return -1;
        }
        ret = check_mem_is_same((uint8_t *)g_check_buffer,
                                (uint8_t *)((unsigned long)buffer + i * AB_WRITE_CHECK_BUF_SIZE),
                                AB_WRITE_CHECK_BUF_SIZE);
        if (!ret) {
            BABLOGE("check same failed!!!");
            return -1;
        }
    }
    if (mod) {
        if (partition_read(partition, offset + i * AB_WRITE_CHECK_BUF_SIZE, g_check_buffer, mod) < 0) {
            BABLOGE("read flash failed.");
            return -1;
        }
        ret = check_mem_is_same((uint8_t *)g_check_buffer,
                                (uint8_t *)((unsigned long)buffer + i * AB_WRITE_CHECK_BUF_SIZE),
                                mod);
        if (!ret) {
            BABLOGE("check same failed!!!");
            return -1;
        }
    }
    return 0;
}

#if _BOOTAB_DUMP_DATA_
static void dump_data(uint8_t *data, int32_t len)
{
    int32_t i;

    for (i = 0; i < len; i++) {
        if (i % 16 == 0) {
            printf("\n");
        }

        printf("%02x ", data[i]);
    }

    printf("\n");
}
#endif

int bootab_upgrade_slice(const char *image_name, const char *ab, const char *file_name,
                         uint32_t offset, uint32_t slice_size, uint32_t is_last_slice, char *buffer)
{
    char part_name[16];

    aos_check_return_einval(image_name && ab);
    snprintf(part_name, sizeof(part_name), "%s%s", image_name, ab);
    return bootab_upgrade_partition_slice(part_name, file_name, offset, slice_size, is_last_slice, buffer);
}

int bootab_upgrade_partition_slice(const char *partition_name, const char *file_name,
                         uint32_t offset, uint32_t slice_size, uint32_t is_last_slice, char *buffer)
{
    int ret = -1;
    char *pbuf = NULL;
    char *tmpBuf = NULL;
    uint32_t wsize;
    partition_t partition;
    partition_info_t *part_info;

    if (slice_size == 0) {
        return 0;
    }

    aos_check_return_einval(partition_name);

    partition = partition_open(partition_name);
    part_info = partition_info_get(partition);
    if (part_info == NULL) {
        BABLOGE("get part %s e", partition_name);
        return -EINVAL;
    }

    if (offset + slice_size > part_info->length) {
        BABLOGE("offset is too large:%d, slice_size:%d, length:%d", offset, slice_size, part_info->length);
        return -EINVAL;
    }
    if (buffer) {
        pbuf = buffer;
        wsize = slice_size;
    } else if (file_name) {
        long size;
        FILE *fp = fopen(file_name, "r");
        if (fp == NULL) {
            BABLOGE("open file %s failed!", file_name);
            return -EIO;
        }
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        tmpBuf = aos_malloc(size + 1);
        if (tmpBuf == NULL) {
            fclose(fp);
            return -ENOMEM;
        }
        rewind(fp);
        fread(tmpBuf, size, 1, fp);
        fclose(fp);
        pbuf = tmpBuf;
        wsize = size;
    } else {
        BABLOGE("buffer and file_name is null");
        return -EINVAL;
    }
    // erase sectors
    uint32_t erase_offset = ((offset + part_info->erase_size -1) / part_info->erase_size) * part_info->erase_size;
    if (erase_offset + part_info->erase_size <= part_info->length) {
        BABLOGD("##erase part offset:0x%x erase_size:%d", erase_offset, wsize);
        ret = partition_erase_size(partition, erase_offset, wsize);
        if (ret < 0) {
            BABLOGE("erase error");
            goto fail;
        }
    }
    BABLOGD("@@write to partition[%d], offset:[0x%x], size: %d", partition, offset, wsize);
#if _BOOTAB_DUMP_DATA_
    dump_data((uint8_t *)pbuf, wsize);
#endif
    ret = partition_write(partition, offset, pbuf, wsize);
    if (ret < 0) {
        BABLOGE("write offset:0x%x wsize:0x%x", offset, wsize);
        goto fail;
    }
    for (int i = 0; i < AB_WRITE_CHECK_COUNT; i++) {
        // 回读校验
        if (_slice_data_rd_check(partition, offset, pbuf, wsize) < 0) {
            BABLOGE("w data verify failed!!!");
            ret = -1;
            goto fail;
        }
    }
    if (tmpBuf) {
        aos_free(tmpBuf);
    }
    partition_close(partition);
    return 0;
fail:
    if (tmpBuf) {
        aos_free(tmpBuf);
    }
    partition_close(partition);
    return ret;
}
#endif