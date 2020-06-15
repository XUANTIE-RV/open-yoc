/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc/partition.h>
#include <devices/flash.h>
#include <aos/log.h>
#include <errno.h>
#include <aos/debug.h>


#ifdef CONFIG_TEE_CA
#include <drv/tee.h>
struct partion_info_t {
    char     image_name[MTB_IMAGE_NAME_SIZE];
    uint64_t part_addr;
    uint64_t part_size;
};
#else
#include <yoc/manifest_info.h>
#endif

#define TAG "partition"

static struct {
    partition_info_t *scn_list;
    uint8_t num;
} g_partion_array;

// struct partion_info_t {
//     char     image_name[MTB_IMAGE_NAME_SIZE];
//     uint64_t part_addr;
//     uint64_t part_size;
// };

int partition_init(void)
{
    int i, ret, num, flash_idx = 0, size = 512;
    char *buf;
    struct partion_info_t *part_info;
    flash_dev_info_t cur_flash_info;
    aos_dev_t *temp_flash_dev;

    buf = aos_malloc_check(size);

#ifdef CONFIG_TEE_CA
    ret = csi_tee_get_sys_partition((uint8_t *)buf, (uint32_t *)&size);
#else
    ret = get_sys_partition((uint8_t *)buf, (uint32_t *)&size);
#endif
    if (ret < 0) {
        aos_free(buf);
        return -1;
    }

    part_info = (struct partion_info_t *)buf;
    num = size / sizeof(struct partion_info_t);
    g_partion_array.scn_list = (partition_info_t *)aos_zalloc(sizeof(partition_info_t) * num);

    if (g_partion_array.scn_list == NULL) {
        aos_free(buf);
        return -ENOMEM;
    }

    while(flash_idx < MAX_FLASH_NUM) {
        temp_flash_dev = flash_open_id("eflash", flash_idx);
        if (temp_flash_dev != NULL) {
            flash_get_info(temp_flash_dev, &cur_flash_info);
            for (i = 0; i < num; i++) {
                if ((cur_flash_info.start_addr <= part_info[i].part_addr) &&
                    ((part_info[i].part_addr + part_info[i].part_size) <= (cur_flash_info.start_addr + cur_flash_info.block_size * cur_flash_info.block_count))) {
                    partition_info_t *scn = &g_partion_array.scn_list[i];
                    memcpy(scn->description, part_info[i].image_name, MTB_IMAGE_NAME_SIZE);
                    scn->base_addr = cur_flash_info.start_addr;
                    scn->start_addr = part_info[i].part_addr - cur_flash_info.start_addr;
                    scn->length = part_info[i].part_size;
                    scn->sector_size = cur_flash_info.block_size;
                    scn->idx = flash_idx;
                    aos_check_param(scn->length && scn->sector_size && strlen(scn->description) > 0);
                }
            }
            flash_close(temp_flash_dev);
        }
        flash_idx++;
    }

    g_partion_array.num = num;

    aos_free(buf);

    return num;
}

partition_info_t *hal_flash_get_info(partition_t partition)
{
    if (partition >= 0 && partition < g_partion_array.num)
        return &g_partion_array.scn_list[partition];

    return NULL;
}

partition_t partition_open(const char *name)
{
    for (int i = 0; i < g_partion_array.num; i++) {
        if (strcmp(name, g_partion_array.scn_list[i].description) == 0) {
            if (g_partion_array.scn_list[i].g_eflash_dev == NULL)
                g_partion_array.scn_list[i].g_eflash_dev = flash_open_id("eflash", g_partion_array.scn_list[i].idx);

            return i;
        }
    }

    return -EINVAL;
}

void partition_close(partition_t partition)
{
    partition_info_t *node = hal_flash_get_info(partition);
    if (node != NULL) {
        flash_close(node->g_eflash_dev);
        node->g_eflash_dev = NULL;
    }
}

int partition_read(partition_t partition, off_t off_set, void *data, size_t size)
{
    partition_info_t *node = hal_flash_get_info(partition);
    if (node != NULL && off_set + size <= node->length) {
        return flash_read(node->g_eflash_dev, node->start_addr + off_set, data, size);
    }

    return -EINVAL;
}

int partition_write_size = 0;
int partition_write(partition_t partition, off_t off_set, void *data, size_t size)
{
    partition_write_size += size;
    partition_info_t *node = hal_flash_get_info(partition);
    if (node != NULL && off_set + size <= node->length) {
        return flash_program(node->g_eflash_dev, node->start_addr + off_set, data, size);
    }
    return -EINVAL;
}

int partition_erase_size = 0;
int32_t partition_erase(partition_t partition, off_t off_set, uint32_t block_count )
{
    partition_erase_size += block_count;
    partition_info_t *node = hal_flash_get_info(partition);
    if (node != NULL && off_set / node->sector_size + block_count <= node->length / node->sector_size) {
        return flash_erase(node->g_eflash_dev, node->start_addr + off_set, block_count);
    }

    return -EINVAL;
}

int32_t get_partition_digest(partition_t partition, uint8_t *buffer, int32_t *buffer_size, digest_sch_e *digest_type)
{
#define P_MAGIC "CSKY"
#define TAIL_BUFF_SIZE (sizeof(partition_tail_head_t) + 128)
    typedef struct {
        uint32_t jump;
        uint8_t magic[4];
        uint32_t size;
    } partition_header_t;

    typedef struct {
        uint8_t magic[4];
        uint8_t digestType;
        uint8_t signatureType;
        uint16_t rsv;
        uint8_t digest_data[0];
    } partition_tail_head_t;

    partition_header_t header = {0};
    partition_tail_head_t *tail = NULL;

    CHECK_RET_WITH_RET(buffer && buffer_size && digest_type, -EINVAL);

    if (partition_read(partition, 0, &header, sizeof(partition_header_t)) == 0) {
        if (memcmp((const void *)&header.magic, (const void *)P_MAGIC, 4) == 0
            && memcmp((const void *)&header.size, (const void *)P_MAGIC, 4) != 0) {
            uint32_t img_content_size = header.size;
            uint32_t tail_offset = sizeof(partition_header_t) + img_content_size;
            tail = (partition_tail_head_t *)aos_malloc(TAIL_BUFF_SIZE);
            aos_check_mem(tail);

            if (partition_read(partition, tail_offset, (void *)tail, TAIL_BUFF_SIZE) == 0) {
                *digest_type = tail->digestType;
                switch(tail->digestType) {
                    case DIGEST_HASH_SHA1:
                        if (*buffer_size < 16) {
                            goto fail;
                        }
                        *buffer_size = 16;
                        memcpy(buffer, tail->digest_data, 16);
                        break;
                    default:
                        goto fail;
                }
                aos_free(tail);
                return 0;
            }
        }
    }

fail:
    if (tail)
        aos_free(tail);
    return -EINVAL;
}

