/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*/
#include <string.h>
#include "yoc/ota_ab.h"
#include "bootab.h"
#include "bootablog.h"

const char *otaab_get_current_ab(void)
{
    return bootab_get_current_ab();
}

const char* otaab_get_next_ab(void)
{
    const char *ab = bootab_get_current_ab();
    if (ab && strcmp(ab, "a") == 0) {
        return "b";
    }
    return "a";
}

void otaab_start(void)
{
    const char *ab = bootab_get_current_ab();
    BABLOGD("===>otaab start.current slot is %s.", ab ? ab : "NULL");
}

int otaab_upgrade_slice(const char *image_name, const char *file_name, uint32_t offset,
                        uint32_t slice_size, uint32_t is_last_slice, char *buffer)
{
    const char *next_slot;
    next_slot = otaab_get_next_ab();
    return bootab_upgrade_slice(image_name, next_slot, file_name, offset, slice_size, is_last_slice, buffer);
}

int otaab_upgrade_partition_slice(const char *partition_name, const char *file_name, uint32_t offset,
                                  uint32_t slice_size, uint32_t is_last_slice, char *buffer)
{
    return bootab_upgrade_partition_slice(partition_name, file_name, offset, slice_size, is_last_slice, buffer);
}

int otaab_upgrade_end(const char *img_version)
{
    char *next_ab = "a";
    const char *ab = bootab_get_current_ab();

    BABLOGI("otaab upgrade end.[%s]", img_version ? img_version : "NULL");
    if (ab && strcmp(ab, "a") == 0) {
        next_ab = "b";
    }
    BABLOGI("start to clear&set slot[%s]", next_ab);
    if (bootab_clear_flags(next_ab) < 0) {
        return -1;
    }
    // 打开回滚功能
    if (bootab_set_fallback_enable(next_ab)) {
        return -1;
    }
    // 下载结束之后需要将另一个分区设置为有效分区
    if (bootab_set_valid(next_ab, img_version)) {
        return -1;
    }
    BABLOGI("otaab upgrade end ok.");
    return 0;
}

// app正常启动之后根据某些条件判断到此app正常了之后再调用此接口
// 当前分区一旦设置为不能回滚之后，就不能再次打开回滚功能了。
void otaab_finish(int ab_fallback)
{
    int ret;
    char *another = "a";
    const char *ab = bootab_get_current_ab();

    BABLOGI("[otaab_finish] I am startup ok.");

    if (ab && strcmp(ab, "a") == 0) {
        another = "b";
    }
    // 清理另一个slot的ab相关的FLAG
    ret = bootab_clear_flags(another);
    if (ret != 0) {
        BABLOGE("otaab init clear failed!");
        return;
    }
    if (ab_fallback == 1) {
        ret = bootab_set_fallback_enable(ab);
        if (ret != 0) {
            BABLOGE("otaab set fallback failed!!!");
            return;
        }
    } else {
        ret = bootab_set_fallback_disable(ab);
        if (ret != 0) {
            BABLOGE("otaab set fallback failed!!!");
            return;
        }
    }
    BABLOGI("[otaab_finish] I am startup init ok.");
}