/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*/
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <yoc/partition.h>
#include "yoc/ota_ab.h"
#include "yoc/ota_ab_img.h"
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

int otaab_env_img_info_init(const char *ab, download_img_info_t *dl_img_info, uint8_t *header_buf)
{
    int i;
    int ret = 0;
    off_t offset;
    pack_header_v2_t *header = (pack_header_v2_t *)header_buf;
    pack_header_imginfo_v2_t *imginfo;

    BABLOGD("come to images info init.");
    if (dl_img_info == NULL || ab == NULL) {
        return -EINVAL;
    }
    if (header_buf == NULL) {
        header = malloc(sizeof(pack_header_v2_t));
        if (header == NULL) {
            BABLOGE("malloc failed.");
            return -ENOMEM;
        }
        memset(header, 0, sizeof(pack_header_v2_t));
        partition_t partition = partition_open(B_ENVAB_NAME);
        partition_info_t *part_info = partition_info_get(partition);
        if (part_info == NULL) {
            BABLOGE("get envab partition error.");
            ret = -1;
            goto out;
        }
        offset = OTA_AB_IMG_INFO_OFFSET_GET(part_info->erase_size);
        ret = partition_read(partition, offset, (uint8_t *)header, sizeof(pack_header_v2_t));
        if (ret < 0) {
            BABLOGE("read env partition error.");
            partition_close(partition);
            ret = -1;
            goto out;
        }
        partition_close(partition);
    }
    imginfo = (pack_header_imginfo_v2_t *)&header->image_info;
    if (header->magic != PACK_HEAD_MAGIC) {
        BABLOGE("the image header is wrong.[0x%x]", header->magic);
        ret = -1;
        goto out;
    }
    if (header->image_count > IMG_MAX_COUNT) {
        BABLOGE("the image count is overflow.");
        ret = -1;
        goto out;
    }
    memset(dl_img_info, 0, sizeof(download_img_info_t));
    dl_img_info->image_count = header->image_count;
    dl_img_info->head_size = header->head_size;
    dl_img_info->digest_type = header->digest_type;
    dl_img_info->signature_type = header->signature_type;
    memcpy(dl_img_info->md5sum, header->md5sum, sizeof(dl_img_info->md5sum));
    memcpy(dl_img_info->signature, header->signature, sizeof(dl_img_info->signature));
    BABLOGD("image count is :%d", header->image_count);
    for (i = 0; i < header->image_count; i++) {
        BABLOGD("-------> %s", imginfo->img_name);
        BABLOGD("offset:%d", imginfo->offset);
        BABLOGD("size:%d", imginfo->size);
        BABLOGD("preload_size:%d", imginfo->preload_size);
        // snprintf(dl_img_info->img_info[i].partition_name, sizeof(dl_img_info->img_info[i].partition_name), "%s%s", imginfo->img_name, otaab_get_next_ab());
        strlcpy(dl_img_info->img_info[i].partition_name, imginfo->img_name, sizeof(dl_img_info->img_info[i].partition_name) - 1);
        strcat(dl_img_info->img_info[i].partition_name, ab);
        partition_t hdl = partition_open(dl_img_info->img_info[i].partition_name);
        if (hdl < 0) {
            BABLOGE("open partition %s failed. maybe it is not existed", dl_img_info->img_info[i].partition_name);
            ret = -1;
            goto out;
        }
        partition_info_t *part_info = partition_info_get(hdl);
        if (part_info == NULL) {
            BABLOGE("get %s info failed.", dl_img_info->img_info[i].partition_name);
            partition_close(hdl);
            ret = -1;
            goto out;
        }
        if (imginfo->size > part_info->length) {
            BABLOGE("imgsize overflow, [%s, %d > %"PRId64"]", imginfo->img_name, imginfo->size, part_info->length);
            partition_close(hdl);
            ret = -1;
            goto out;
        }
        dl_img_info->img_info[i].partition_handle = hdl;
        dl_img_info->img_info[i].partition_abs_addr = part_info->base_addr + part_info->start_addr;
        dl_img_info->img_info[i].partition_size = part_info->length;
        dl_img_info->img_info[i].write_size = 0;
        dl_img_info->img_info[i].read_size = 0;
        dl_img_info->img_info[i].img_offset = imginfo->offset;
        dl_img_info->img_info[i].img_size = imginfo->size;
        dl_img_info->img_info[i].preload_size = imginfo->preload_size;
        partition_close(hdl);
        imginfo++;
    }
out:
    if (header_buf == NULL)
        free(header);
    return ret;
}